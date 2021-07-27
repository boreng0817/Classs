# Operating System [ELE3021-12962]

___Prof. HyungSoo Jung___ 

___2018008004 Kim SiWoo___





## Project 1 [MLFQ & Stride scheduling]



# Implementation & Design

To implement MLFQ & Stride scheduling for xv6, I mainly used queue to manage process.

Basically queue for MLFQ and Stride is doubly linked list, supporting push(enqueue), delete, and obtain highest priority of process to be scheduled.

In addition, Stride queue should be sorted to use pass properly, so I made Stride queue push to insert appropriate place.

Picture below is an example for my design.
![fig1](/uploads/4160e79449dbe1bb130b997229b4e736/fig1.png)

In schedule function, first determine which algorithm to be used to pick a process by stride algorithm.

So I made some variables for doing this.

~~~c
int mlfq_tickets = 100;
int mlfq_pass = 0;
int stride_tickes = 0;
int stride_pass = 0;
enum sched_method{MLFQ0, MLFQ1, STRIDE} sched_decision;

int mlfq_ticks = 0;  // Needs to boost mlfq queue
int total_ticks = 0; // Prevent overflow of p->stride_pass by using total_ticks
~~~



Unless system call set_cpu_share(share), scheduling algorithm works as MLFQ0 which is pure MLFQ algorithm.

If any process that called set_cpu_share, scheduler determines which algorithm to use.

~~~c
void
scheduler(void)
{
    ...
    if(stride_tickets == 0)
        sched_decision = MLFQ0;
    else
        sched_decision = mlfq_pass < stride_pass ? MLFQ1 : STRIDE;
    ...
}
~~~

After deciding which schedule method to use, scheduler tries to find process by the method.



### 0) struct proc & struct queue

First things first, I needed to modify proc.

~~~c
struct proc {
    ~~~
    //mlfq
    int mlfq_level;              // Level of MLFQ scheduling
    int passed_ticks;            // Total number of ticks proccess had 
    int possesed_ticks;          // The number of ticks since scheduled
    int syscall_yield;			 // Is proc give up cpu without timer interrupt
    //stride
    int is_stride;               // = 1 when stride; = 0 when mlfq
    int stride_ticket;           // Tickets possessed by this proc
    int stride_pass;             // Pass of this proc
    //mlfq && stride
    struct proc *next;           // Link for doubly linked list
    struct proc *prev;           // Link for doubly linked lsit (queue)
}
~~~



And I made queue structure, and some function for modifying queue.

~~~c
struct queue {
  struct proc *next;
  struct proc *prev;
  int size;
};

//mlfq
void mlfq_push(struct queue*, struct proc*);  //wrapper of util_push. use for mlfq queue
void mlfq_delete(struct queue*, struct proc*);  //wrapper of util_delete. use for mlfq queue
struct proc *mlfq_get_proc(void); //get process that sould be scheduled due to priority. (RUNNABLE & in the highest priority queue)
void mlfq_replace(struct proc*); //When proc is done with time quantum, yield CPU for next process.
void mlfq_degrade_level(struct proc*); //When proc uses time allotment, move to lower queue.
void mlfq_boost(void); //For every 100 mlfq_ticks, merge lower queue into highest queue.
void mlfq_merge(struct queue*, struct queue*); //Helper function for mlfq_boost.
//stride
void stride_stand(void); //To avoid overflow, decrease each pass of proc by minimum value of pass. Also mlfq_pass and stride_pass.
void stride_push(struct proc*); //Wrapper of util_push, and put proc in sorted order.
void stride_pop(struct proc*); //Wrapper of util_delete.
struct proc *stride_get_proc(void); //Get minimum pass process which is RUNNABLE
//util
void util_delete(struct queue*, struct proc*); // Enqueue proc in doubly linked list(queue)
void util_push(struct queue*, struct proc*); // remove proc from double linked list(queue)
~~~



### 1) MLFQ

I made MLFQ queue in proc.c

~~~c
struct queue mlfq[3];
unsigned int time_allotment[3] = {5, 10, (unsigned int)-1}; // check for degrading proc
int time_quantum[3] = {1, 2, 4};  // check for RR
~~~

Every proc that created got into mlfq[0].

In function allocproc, I set some variables of process then push into queue.

~~~c
static struct proc*
allocproc(void) {
    ...
  //* Proc initially queued in 0-level,
  //* with 0 passed tick.
  p->mlfq_level = 0;
  p->passed_ticks = 0;
  p->possesed_ticks = 0;
  p->is_stride = 0;
  p->next = p->prev = 0;
  //* insert proc into queue[0]
  mlfq_push(&mlfq[0], p);
    ...
}
~~~

When OS is booted, init process is made with userinit(), which uses allocproc(). Then init fork & exec sh process. So if OS booted successfully, 2 processes (init, sh) is in mlfq_0.



If there is no process scheduled by Stride, which means mlfq_tickets == 100, scheduler uses MLFQ0 as scheduler mod.

MLFQ0 doesn't increase any pass, just do MLFQ scheduling.

~~~c
in function schedule(void)..
      if(sched_decision == STRIDE){
        ...
      }
      else{
        if(sched_decision == MLFQ1)
          mlfq_pass += LARGE_NUM/mlfq_tickets;
        p = mlfq_get_proc();
      }
~~~

After pick a process from mlfq_queues, check wheter process is null. If null, break and try another process.

If not null, Check if process' time_quantum and time_allotment. If so, schedule another process, move process end of queue or lower queue.



MLFQ1 is chosen if stride_tickets is not zero. Only difference between MLFQ0 is MLFQ1 increases mlfq_pass.

~~~c
in function schedule(void)
    ...
      if(sched_decision == STRIDE){
        ...
      }
      else{
        if(sched_decision == MLFQ1)
          mlfq_pass += LARGE_NUM/mlfq_tickets;
        p = mlfq_get_proc();
      }    
~~~

MLFQ scheduling check time_quantum and allotment when process is scheduled.

~~~c
      if(sched_decision != STRIDE){
        if(p->passed_ticks >= time_allotment[p->mlfq_level]){  // lev0 : 5, lev1 : 10, lev2 : int_max
          mlfq_degrade_level(p); // move to lower queue
          p = mlfq_get_proc(); // pick process from MLFQ
        }
        if(p->possesed_ticks >= time_quantum[p->mlfq_level]){  // lev0 : 1, lev1 : 2, lev4 : 4
          mlfq_replace(p); //delete process, then enqueue to queue again.
          p = mlfq_get_proc();
        }
      }
~~~

After picking another process, scheduler don't check whether its null. If there's only one process that is RUNNABLE, and chosen at the start of process picked again, which is not null.



### 2) Stride

Stride scheduling enable if and if only there exists any process that called set_cpu_share and still runnable.

If set_cpu_share is called, check for total share of stride, then set some variables of process then delete from mlfq. 

Finally enqueue process into stride queue.

~~~c
struct queue stride;  // stride queue
const int LARGE_NUM = 10000;  // use for LARGE_NUM/tickets to increase pass.
~~~

When Stride is chosen by scheduler, it get process form stride queue.



~~~c
in function scheduler(void)...
      if(sched_decision == STRIDE){
        stride_pass += LARGE_NUM/stride_tickets;
        p = stride_get_proc();
      }
~~~



After checking p is null, made p RUNNING.



### 3) trap.c & yield & summary

In trap.c, I modify some part of trap to mark a process got a round from CPU. Because it yields after tick is increased, modify some variables of process.

~~~c
in trap(..)...
if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER){
    struct proc *p = myproc();
    if(p->is_stride)
      p->stride_pass += 10000/(p->stride_ticket); // increase pass if process is stride
    else
      mlfq_ticks++; // or increase mlfq_ticks
    
    total_ticks++; // prevent an overflow of pass
    p->syscall_yield = 0; // Use for MLFQ to implement RR.
    yield(); // yield.
  }
~~~

If process got an timer interrupt, it means process fully had a round (tick) of CPU. 

p->syscall_yield is a variable that help implement RR for MLFQ.

~~~c
in yield()...
  acquire(&ptable.lock);  //DOC: yieldlock
  struct proc *p = myproc();
  p->state = RUNNABLE;
  
  p->possesed_ticks++; // Design choice for gaming process.
  p->passed_ticks++; // Design choice for gaming process.
  if(p->is_stride){
    stride_pop(p); // sort stride queue
    stride_push(p);
  }
  else if(p->syscall_yield){ // for MLFQ, yield CPU time_quantum is not fulled.
    mlfq_replace(p);
    p->possesed_ticks = 0;
  }
  sched();
  release(&ptable.lock);
~~~

Yield function is called from trap, but yield can be called from a user's program. If process requested to yield CPU, 

then it should append to same level of queue regardless how many ticks this process had.

If I increase ticks in trap, which allow gaming process, process that calls yield has no big difference with other processes that don't call yield.

![fig2](/uploads/fa1d7042a3658763d5a00b0a38431c45/fig2.PNG)

__figure 2. tick increases at trap__

![fig3](/uploads/1337f78655a7e02aa7e5fe3e869c1506/fig3.PNG)

__figure 3. tick increases at yield__

___num1 num2 under MLFQ test is start time and end time by uptime()___ 



In case tick increases at trap, yield and compute don't have much difference.

In case tick increases at yield, they show huge difference in lev count.

I choose case yield, since meaning of giving up CPU share is more fitting to yield case.

+) I consider about some compensation method for yielding without timer interrupt may decrease 

half or thirds of tick, but I don't have enough time to implement it.



So, to sum up this schedule's implementation, I will briefly summary schedule function.

~~~c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();
    // Loop over process table looking for process to run.
    //* find right proc via policy
    acquire(&ptable.lock);
    for(;;){
      //* prevent overflow
      if(total_ticks%1000 == 0)
        stride_stand();
      if(mlfq_ticks%100 == 0)
        mlfq_boost();
      
      if(stride_tickets == 0)
        sched_decision = MLFQ0;
      else
        sched_decision = mlfq_pass < stride_pass ? MLFQ1 : STRIDE;
      //*get runnable process in the highest priority queue
      if(sched_decision == STRIDE){
        stride_pass += LARGE_NUM/stride_tickets;
        p = stride_get_proc();
      }
      else{
        if(sched_decision == MLFQ1)
          mlfq_pass += LARGE_NUM/mlfq_tickets;
        p = mlfq_get_proc();
      }
      
      if(!p)
        break;
        
      if(sched_decision != STRIDE){
        if(p->passed_ticks >= time_allotment[p->mlfq_level]){
          mlfq_degrade_level(p);
          p = mlfq_get_proc();
        }
        if(p->possesed_ticks >= time_quantum[p->mlfq_level]){
          mlfq_replace(p);
          p = mlfq_get_proc();
        }
      }
      p->syscall_yield = 1; 
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;
      swtch(&(c->scheduler), p->context);
      switchkvm();
      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      break;
    }
    release(&ptable.lock);
  }
}
~~~

This code can be explained simple flow chart below.

![fig4](/uploads/e8d762784bbda1338a07dfd04b168f7e/fig4.png)

Red arrow denotes for false, blue arrow denotes for true.



In exit function, process delete from their queue. If process was in stride, I reallocate tickets to mlfq_tickets and stride_tickets.

~~~c
in exit(void)...
      //* pop from queue
  if(curproc->is_stride){
    stride_tickets -= curproc->stride_ticket; // reallocate tickets
    mlfq_tickets += curproc->stride_ticket;
    stride_pop(curproc); // remove from queue
  }
  else
    mlfq_delete(&mlfq[curproc->mlfq_level], curproc); // mlfq simply remove from queue
~~~



+) I didn't modify queue when process call sleep of wait. This process get compensation by stay in their position. 

Whenever they become RUNNABLE, they can get higher priority then other waken process.

This method may cause performance issue, such as keeping sh, init in queue whether they are almost

sleep.



### 4) system call

a) yield

~~~c
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  struct proc *p = myproc();
  p->state = RUNNABLE;
  p->possesed_ticks++;
  p->passed_ticks++;
  if(p->is_stride){
    stride_pop(p);
    stride_push(p);
  }
  else if(p->syscall_yield){
    mlfq_replace(p);
    p->possesed_ticks = 0;
  }
  sched();
  release(&ptable.lock);
}
~~~

If yield is called from user, give up CPU.

If yield is called from trap, use RR for mlfq or stride scheduling.



b) getlev

~~~c
int
getlev(void)
{
  return myproc()->mlfq_level;
}
~~~

Simply return process' mlfq_level.



c) set_cpu_share

~~~c
int
set_cpu_share(int share)
{
  struct proc *p;
  //* check for non-valide share rate
  if(share <= 0)
    return -1;
  //* check if stride sched posses more than
  //* 80% of cpu if this proc enter
  if(share + stride_tickets > 80)
    return -1;
  acquire(&ptable.lock);
  //pull out from mlfq
  p = myproc();
  mlfq_delete(&mlfq[p->mlfq_level], p);
  p->is_stride = 1;
  p->stride_ticket = share;
  p->stride_pass = 0;
  //push into stride queue
  stride_push(p);
  mlfq_tickets -= share;
  stride_tickets += share;
  release(&ptable.lock);
  return 0;
}
~~~

Check share is valid, then set variables and push into stride queue. Finally, adjust tickets of mlfq and stride.



### 5) MLFQ stride scheduling scenario

__a)__ 5 strides(5, 10, 15, 20, 25 => 75)

test for stride scheduling works properly.


![fig5](/uploads/fd09ccfa921a2c3d1cf2c3acd01a5ae0/fig5.png)



In simulation a, I did 4 trials and check Stride give right CPU shares. 

I made 100% count by dividing 0.75 to sum of Avg.

It seems properly function.



__b)__ Given test_master.c

I needed to compare stride and mlfq. So I made a 2 compute mlfq, and 10%, 40% strides.
In 300 ticks, 2 mlfq_process terminated. Assume mlfq_queue had 50% CPU share during mlfq_process are executing.
Then each process had total 75ticks to terminates. With simple experiment;test_mlfq 0, its quite right.

![fig6](/uploads/b8213adaf357741222ce62cf62cee6ff/fig6.png)


Picture below is result of given test_master.c

It seems there's a problem with yield.

I'm so sad that I can't find the problem. But I'm guessing that yield makes a process gain portion of ticks.
Which makes process much more ticks to end its process. 

![fig7](/uploads/e966669db3b71ff988b14d807327e404/fig7.png)

__c)__ Some more stride.

I played with some stride tester, I found something to be correct.

![fig8](/uploads/bd0aed53d421b29c69004b7a682fad80/fig8.png)

Shown in picture, stride get about 4k count for 80% cpu share. 
But, in case single process occupy stride queue, and there's no runnable process in mlfq,
that single process scheduled as if it has 80% share.
+) It can be solved by adding some idle runnable process for mlfq queue.

부족한 위키 읽어주셔서 감사드립니다!

___end of project1___

## 







> Milestone 1, design of new scheduler with MLFQ and stride.



 In my opinion, main task to modify xv6's basic scheduler which is Round Robin policy is to modify [void scheduler(void)] 

 function to choose adequate victim to accomplish this programming project. 



 In milestone 1, I will explain about how I designed new scheduler and how I would implement briefly.

 



#### _a) First step_ : MLFQ



i) Use 3-level feedback queue

~~~ c
/* 1. Modify ptable in proc.c */
    =>
    struct {
  struct spinlock lock;
  struct proc proc_0[NPROC];
  struct proc proc_1[NPROC];
  struct proc proc_2[NPROC];
  //int current_queue;
} ptable;
/* saving index of current_queue may replaced by implementing function that finds adequate queue. 
For differnet time quantum, this kind of variable may help better implementation 
-> adding new variable for saving a level of queue to struct proc is better I think*/

/* 2. If 1) does not work, Make new structure for MLFQ's 3-level queue */
~~~



ii) Each level of queue adopts RR policy with different time quantum

~~~c
/* 1. Add new variable to [struct proc] to determine which queue is having this process. */
Highest priority queue : 1 tick
Middle priority queue  : 2 ticks
Lowest priority queue  : 4 ticks
    
/* Need a function to check such queue is empty */
int is_MLFQ_empty(int index) {
    return MLFQ[index] is empty
}

/* In [proc.c] in [void scheduler(void)] needs to determine which queue should be used */
~~~



iii) Each queue has different time allotment

~~~c
/* Also, add allotment variable to [struct proc] to check how many ticks that this process possessed before. */
/* allotment count should be added in timer interupt, in [trap.c] */
~~~



iv) To prevent starvation, priority boost is required

~~~c 
/* implement priority_boost() to prevent starvation. */
In [trap.c] in [case T_IRQ0 + IRQ_TIMER:]
    checking ticks%100 == 0 to call priority_boost().
    //Initial tick count increment won't call priority_boost().
~~~



I thought this new scheduler uses MLFQ "or" stride, but it should use both. 

So I designed a scheduler to determine which scheduler to use.

When and __[set_cpu_share(int)]__ called, scheduler use stride scheduling to determine MLFQ or stride. 



~~~c 
/* Assume MLFQ's net ticket as A, stride scheduling as B, where A + B = 100 */
/* Simply apply scheduling method to determine which method to use for this round. */
1. if MLFQ gets the walk,
	-> tick for process in MLFQ once and go to next round
2. if stride scheduling gets the walk,
	-> do stride scheduling among process called set_cpu_share(int) and go to next round.
~~~



#### b) stride scheduling algorithm with MLFQ



i) Make a system call that request the portion of CPU and guarantees the calling process to be allocated that CPU time

~~~c
/* Using nested stride scheduling to choose which kind of method to use guarantee CPU share for such process that called set_cpu_share */
probability stride sched gets walk -> B/(A+B) = B/100
probability such process gets walk -> [int]/B * B/100 = [int]/100
~~~



ii) Total stride processes are able to get at most 80% of CPU time. Exception handling is needed for exceeding request.

~~~c
/* Main scheduler should have CPU share for each scheduling method. */
In int set_cpu_share(int share) {
    ...
    if (stride_share exceeds 80% of CPU share)
        exception handles { abort cpu_share request }
}
~~~



iii) The rest 20% of CPU time should run for the MLFQ scheduling which is the default scheduling in this project.

~~~c 
/* Main scheduler will choose MLFQ scheduler at least 20%. */
~~~



---

## Required system calls

~~~c
int sys_yield(void); // yield the cpu to the next process
~~~

I am considering modification of yield function or scheduler function to achieve this project's goal. At last, I decided to modify scheduler 
to find adequate victim for next process.





~~~c
int sys_getlev(void); // get the level of current process ready queue of 					   
                      // MLFQ. Returns one of the level of MLFQ (0,1,2)
~~~

In my design, [ int is_MLFQ_empty(int index) ] could be inner function of getlev, or just implement getlev function can be another good way.





~~~c
int sys_set_cpu_share(void) // wrapper
set_cpu_share(int) // inquires to obtain cpu share
~~~

Function that triggers stride scheduling. It should be handled when stride scheduling occupies more than 80% of CPU share.





_end of milestone1_

---

2018008004 Kim SiWoo


