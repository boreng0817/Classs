# Operating System [ELE3021-12962]

___Prof. HyungSoo Jung___ 

___2018008004 Kim SiWoo___





## Project 2 [Light-weight Process]

# Milestone1 

a) **Process / Thread**

Process is a running program. It has CPU state(PC, stack pointer, GPRs) and memory(Instructions, data section). 

Thread is a kind of element of process for achieving parallel programming for better performance.

It mostly uses shared memory of the process's but user stack and it is managed independently by scheduler.

The point is, thread can be thought as light weight process.



When process is scheduled by scheduler, OS needs to perform context switch.
Saving and restoring processes' register and changing memory part need time.

But thread doesn't have to change every part related with memory since they share
with process, so it makes less overhead.



b) **POSIX thread**

POSIX threads are LWP implemented by UNIX. Using ptread and understanding its concept will help for implementing LWP for xv6.

~~~c
int pthread_create(ptread_t *thread, const pthread_attr_t *attr,
                   void *(*strat_routine)(void *), void *arg);
~~~

This pthread_create function creates LWP associated with process which calls pthread_create.

I think this function can be thought as fork() - execv() for thread.

It creates thread with name thread, with work load start_routine. 
We can pass arguments and attribute configuration for function argument.

It returns 0 for success, or error number for failure.



~~~c
int pthread_join(pthread_t thread, void **ret_val);
~~~

This pthread_join works similar wait system call for parent process.
A process can be thought as parent process of thread.

It waits for thread to exit, and get ret_val by passing it's pointer.



~~~c
void pthread_exit(void *ret_val);
~~~

This pthread_exit works similar with exit system call for a process.

It's called by thread for terminates the thread. And ret_val for return value of thread.
Master thread(process) collects ret_val with pthread_join.





c) **Design basic LWP Operations for xv6**

In milestone 2, there are two steps for implementing LWP.

First implementing pthread-like functions, which are thread_create, thread_exit, and thread_join.
Secondly, we need to make LWP compatible for xv6's services like scheduler and system calls.



Create would be implemented by understanding fork and exev (flow of creating process) 
and modify to create LWP.

Join would be implemented by understanding wait system call.
Getting return from thread could be made with some additional system call.

Exit would be implemented by understanding exit system call.

I will treat main thread(process) as a parent and worker thread as child process.



Also I need to modify MLFQ & stride scheduler to be compatible with LWP.

~~~c
struct proc {  // or struct thread can be added.
    ...
        struct proc *worker_thread[MAX_THREAD]; // max number of thread
    											// should be considered.
    	int total_ticks; // If worker thread use one clock
    					 // tick increases in main thread as well.
}
~~~



I think adding proc pointer (or thread pointer) to worker thread may help

modify scheduler easier. Scheduler basically schedules main thread, and when main thread is scheduled, it should share their time allotment with worker and main threads.

Passed ticks will be managed by main thread.



exit, sbrk, kill, pipe => When single LWP use these system calls, all LWP in their group
										needs to behave the same.

fork, exec, sleep=> I think this should behave the same with process and thread.









_end of milestone1_

---

# Milestone 2 _implementation_

### Basic Operation && interaction w/ other services in xv6



In milestone 2, I needed to implement a simple thread compatible with xv6.  



First things first, I modified proc structure.

~~~c
[proc.c]
struct proc {
    int pid; // negative value for thread.
    ...
    struct run *sp_free_list; // for managing user stack for threads
    int thread_cnt; // for master thread, to count worker thread
    int is_therad; // indicate this proc is worker(= 1) or master(= 0)
    thread_t tid; // to classify thread.
    void *retval; // for exit, join
    uint stack_base; // for mananging user stack for threads
    
    struct queue LWP_group; // contains every threads in the same LWP_group (also master thread)
    struct proc *next_LWP; // next LWP
    struct proc *prev_LWP; // prev LWP
}
[types.h]
typedef uint thread_t;
~~~

![3](/uploads/ffd604513f5baf15b59ec78334653948/3.png)

The picture above is abstract design of my LWP.

Key implementations for my LWP are __LWP_group managing__ and **sp_free_list managing**.  

I got idea form free_list [struct run] to implement sp_free_list. In case of reusing stack space for thread, the size of process can grow constantly, and end in PHYSTOP.

So, use sp_free_list to manage stack frames. If there is reusable stack frame, just get it or expand proc to make stack for new LWP.

~~~c
[in thread_create; allocation stackbase] 
if(curproc->sp_free_list){
    struct run *r;
    r = curproc->sp_free_list;
    curproc->sp_free_list = r->next;
    sp = (uint)r + 4;
}
else{
    sz = PGROUNDUP(curproc->sz);
    if((sz = allocuvm(curproc->pgdir, sz, sz + 2*PGSIZE)) == 0){
      // Handle exception
      return THREAD_CREATE_FAIL;
    }
    clearpteu(curproc->pgdir, (char*)(sz - 2*PGSIZE));

    curproc->sz=sz;
    sp = sz;
}

[in thread_join; deallocation stackbase, return it to sp_free_list]

struct run *r = (struct run*)((uint)p->stack_base - 4);
r->next = curproc->sp_free_list;
curproc->sp_free_list = r;

~~~



LWP_group management is key factor for LWP scheduling. For implementing inter-scheduling

among the same LWP_group, manaing LWP group is essential point.  



So, I added some LWP class function for manipulating LWP group.

~~~c
void LWP_push(struct queue*, struct proc*);
void LWP_delete(struct queue*, struct proc*);
struct proc *util_get_runnable_LWP(struct proc*);
~~~

Basically, LWP group is a queue. So we need enqueue(= push) and dequeue(= delete)  

**util_get_runnable_LWP(p)** is a utility function for scheduling. It simply finds a runnable thread in LWP group, and it follows Round Robin policy.

---



My report for project 2 will consist of 4 parts

1. LWP basic implementation
2. scheduling for LWP
3. system call
4. result, extra.

---



#### 1. LWP basic implementation

~~~c
int thread_create(thread_t *thread, void *(*start_routine)(void*), void *args);
void thread_exit(void *retval);
int thread_join(thread_t thread, void **retval);
~~~

For thread_create, I brought many ideas from fork and exec.

~~~c
[thread_create]
1. Create proc by calling allocproc;
2. Check sp_free_list for making stack frame to thread;
	If true, get one from sp_free_list.
    If false, grow proc size for stack frame to thread.
3. Assign essential value for thread like trapframe, pgdir(shared), ...;
4. Set initial stack frame with fake return address, and given argument;
5. Set eip to start routine, then set thread RUNNABLE;
~~~

After thread got sched, it will start its job from start_routine.



For thread_exit, most of the codes are from exit

~~~C
[thread_exit]
1. Close all open files;
2. log some data; // iput. I'm not sure what this function does, but I think it logs some data.
3. acquire ptable lock, and wakeup parent to clean up;
4. call sched at last, and this thread never be sched again.
~~~



For thread_join, most of the codes are from wait

~~~c
[thread_join]
1. Check for any proccess in ptable that belongs to proccess. (Same LWP group);
2. If it has, check it is ZOMBIE.
    If not, wait for worker thread to wake up.
    If does, clean up threads kstack, stack frame for later usage;
~~~

---



#### 2. Scheduling.

In project2's specification, scheduling have given policy.

~~~c
* MLFQ { highest lev => time quantum : 5 ticks / time allotment : 20 ticks  }
  	   { Middle lev  => time quantum : 10 ticks/ time allotment : 40 ticks  }
	   { lowest lev  => time quantum : 20 ticks/ time allotment : MAX_INT   }
* For every 200ticks, do priority boost.
* Stride scheduling => default time quantum : 5 ticks
* Switching between the same LWP group should not pass scheduler()
~~~

Actually, top most three policy are not that hard. We can do it by editing number in code.  

But though part is the last policy that switching between the same LWP group must be handled in different way.

In this project2, we need to re-implement project 1's scheduler to make compatible with LWP.  

Also, with same LWP, it doesn't need to context switch between scheduler to be sched.  

So, I made a minisched() that does scheduling instead of scheduler() among threads in the same LWP group.  

The picture below is the big picture of Scheduling.
![4](/uploads/9c9b1f1933856a3b108e8536cd2721bf/4.png)

In minisched, do swtch() between current thread and next selected thread.  (Both threads in same group)

Do all context switch except changing cr3 register.  

~~~c
[proc.c]
void
minisched(struct proc *prev, struct proc *next)
{

  int intena;
  if(!holding(&ptable.lock))
    panic("minisched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("minisched locks");
  if(prev->state == RUNNING)
    panic("minisched running");
  if(readeflags()&FL_IF)
    panic("minisched interruptible");
  intena = mycpu()->intena;
  mycpu()->proc = next;
  next->state = RUNNING;
  if(prev != next){
    /* simplified switchuvm;change kstack*/
    if(next == 0)
      panic("switchuvm: no process");
    if(next->kstack == 0)
      panic("switchuvm: no kstack");

    pushcli();
    mycpu()->gdt[SEG_TSS] = SEG16(STS_T32A, &mycpu()->ts,
                                  sizeof(mycpu()->ts)-1, 0); 
    mycpu()->gdt[SEG_TSS].s = 0; // 0 : system
    mycpu()->ts.ss0 = SEG_KDATA << 3;
    mycpu()->ts.esp0 = (uint)next->kstack + KSTACKSIZE;
    // setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
    // forbids I/O instructions (e.g., inb and outb) from user space
    mycpu()->ts.iomb = (ushort) 0xFFFF;
    ltr(SEG_TSS << 3);
    popcli();

    swtch(&prev->context, next->context);
  }
  mycpu()->intena = intena;
}
~~~

Simply, I mixed sched() and switchuvm() code to implement minisched.  

So, this minisched makes LWP to be switched among in the same LWP group.



Also, yield is very important function for scheduling. I made big change in function yield().  

yield determines not only which scheduling method to use (MLFQ & stride) but also choose sched or minisched. Picture below is flow of yield().

![6](/uploads/324a4e7d4979d7774743c7cb13ada308/6.png)

~~~C
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  struct proc *p = myproc();
  struct proc *tmp, *next;
  uint possesed_ticks;
  int is_sched, has_runnable_thread = 1;

  //check time priority boost
  if((++total_ticks) == 200)
    mlfq_boost();
  // tick counts are managed in master thread.
  if(p->is_thread)
    tmp = p->parent;
  else
    tmp = p;
  if(tmp->is_thread)
    panic("worker thread can't be master thread");
  tmp->passed_ticks++;
  possesed_ticks = ++tmp->possesed_ticks;

  //put into end of LWP group (for RR)
  LWP_delete(&tmp->LWP_group, p);
  LWP_push(&tmp->LWP_group, p);

  // stride
  if(p->is_stride){
    if(possesed_ticks == 5){
      is_sched = 1;
      tmp->stride_pass += 10000/(tmp->stride_ticket);
      stride_pop(tmp);
      stride_push(tmp);
    }
    else
      is_sched = 0;
  }
  // mlfq
  else{
    if(tmp->passed_ticks >= time_allotment[tmp->mlfq_level]){
      mlfq_degrade_level(tmp);
      is_sched = 1;
    }
    else if(possesed_ticks >= time_quantum[tmp->mlfq_level]){
      tmp->possesed_ticks = 0;
      is_sched = 1;
    }
    else
      is_sched = 0;
  }

  // simple scheduling
  p->state = RUNNABLE;
  next = util_get_runnable_LWP(tmp);
  // Can't find runnable LWP in LWP_group.
  if(!next)
    has_runnable_thread = 0;

  if(!is_sched && has_runnable_thread){
    minisched(p, next);
    release(&ptable.lock);
  }
  else {
    sched();
    release(&ptable.lock);
  }
}

~~~

Most mlfq, stride policies are handled in yield function. It manages tick, and maintain scheduling method until LWP group uses all time quantum.  

In case of **is_sched** flag is on, yield jumps to sched(). **is_sched** is 1 when LWP group used all of time quantum. And **has_runnable_thread** should be 1 to make sure yield jumps to minisched.



Lastly, scheduler choose mlfq or stride based on their passes, and pick a LWP group to get CPU for their time quantum.

~~~C
[in proc.c : part of scheduler]

    for(;;){
      if(stride_tickets == 0)
        sched_decision = MLFQ0;
      else
        sched_decision = mlfq_pass < stride_pass ? MLFQ1 : STRIDE;
      //get runnable process in the highest priority queue
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

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      last_stand = p;
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

~~~

When we pick a runnable process or thread from LWP group, we use **mlfq_get_proc()** and **stride_get_proc()**.  

In project1, they just search from each queue, but in project2 *_get_proc() function finds master thread, and search runnable thread (or process) in group. 

By this way, each level of mlfq can perform RR.

Below picture is abstraction of mlfq & stride queue for project2.

![7](/uploads/6f4d1d4513905e105c1698adc151096a/7.png)

To wrap up scheduling, I made minisched and improved yield. Also managing LWP group made scheduling efficiently. So, this how I made simpler scheduling for LWP.

---



#### 3.System Call

In case of LWP calls system call, I need to modify some of them for right operation.

#### Exit

For normal exit of process, It partly clean process' resource then leave parent to collect child's resources.

But for thread call exit instead of thread_exit, all threads in LWP_group should be exited as well.

So, I made exit to handle thread case and process case. For process case, exit will do its job.

~~~c
[proc.c:void exit()]
1. set all threadss killed flag 1 in LWP_group by calling void LWP_set_exit(struct proc*)
    After thread in LWP_group enters trap, it will enters to exit.
case: thread => do simplified thread_exit in exit (close file, set state ZOMBIE, sched())

case: process without thread (p->thread_cnt == 0)
    => do normal exit system call.
case: process with thread (p->thread_cnt != 0)
    => collect all child thread's resources (simplified thread_join) and do exit.
~~~

I tried to implement exit with therad_join and thread_exit, but I had some deadlock problem. (log lock).

So I made a long exit system call.



#### Fork

For fork, I didn't do much things but it worked well. I just modify np->pgdir's copyuvm size.  

Memory space of forked children should follow master thread since it manage all of memory spaces.



#### Exec 

When exec is called by thread, other threads should be exit as well. So thread that called exec send killed signal to all threads in LWP_group.

I implemented exit as master threads exit at last. So, program like echo, ls.. created by exec also belongs to LWP_group. When process created by exec ends, it will be collected by master thread, and master thread will terminate as well.

~~~c
[exec.c : int exec(char*, char**)]
{
 // at first, check for thread called exec
 if(curproc->is_thread){
		struct proc *p;
		for(p = curproc->parent->LWP_group.next; p;p = p->next_LWP)
			if(p != curproc)
				p->killed = 1;
	}
    ...
 // at last, if thread called exec, leave freevm(oldpgdir) for master thread.
 // if it is not thread, just do freevm for oldpgdir.
 if(!curproc->is_thread)
		freevm(oldpgdir);
	return 0;
}
~~~



#### sbrk

At first, I tried to modify sys_sbrk.

~~~c
int
sys_sbrk(void)
{
  int addr;
  int n;
    
  acquire(&ptable.lock);
  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0) {
      release(&ptable.lock);
    return -1;
  }
  release(&ptable.lock);
  return addr;
}
~~~

Oh NO! in sysproc.c, ptable is not extern variable so I can't acquire or release lock in sys_sbrk.

So, I modified both sys_sbrk and growproc.

~~~c
[proc.c : int growproc(int)]
growproc(int n)
{
  uint sz, addr;
  struct proc *curproc = myproc();
	struct proc *p;
	
	// prevent race
	acquire(&ptable.lock);
	if(curproc->is_thread)
		p = curproc->parent;
	else
		p = curproc;
	sz = addr = p->sz;
...
	p->sz = sz;
  switchuvm(curproc);
  release(&ptable.lock);
	return addr;
}
[sysproc.c : int sys_sbrk(void)]
sys_sbrk(void)
{
  int addr;
  int n;
	// can't acquire ptable lock in sys_brk
  if(argint(0, &n) < 0)
    return -1;
  if((addr = growproc(n)) < 1) 
    return -1;
	
  return addr;
}

~~~

#### Kill

I didn't change any code for kill. If any thread in LWP_group exits, (kill as well) all threads need to be exit as well.  

By implementation of exit(), I worked well.



#### Pipe

~~~c
[proc.c : int thread_create(thread_t*. void*(*)(void*), void*)]
thread_create(...){
    ...
  for(i = 0; i < NOFILE; ++i)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
	...
}
~~~

Code above makes pipe work properly in thread as well.



#### Sleep

Since i implement thread on proc structure, sleep is very natural for right operation.



#### 4. Result, ect

The picture below is the result of test_thread2.c

For stress test, it needs 35 lines for result, so I reduced nstress = 5000. It works well for nstress = 35000.

![5](/uploads/41b0cbbb835badfcf9b8948579b2a828/5.png)

+exec test may not show result in full test. But in single test or ranged test show echo execution.

+Lack of time, I submitted wiki about 3 hours late. I'm very sorry.

+For corner case test, I thought about some cases, but I couldn't implemented.

- thread_create in thread => I would promote created thread to child of master thread.
- exec on master thread => I think all worker thread should be cleaned, then do exec. 

+locking was a devil for implementing minisched. I saw tons of acquire and release panic.





2018008004 Kim SiWoo

_end of milestone2_

---