# Operating System [ELE3021-12962]

___Prof. HyungSoo Jung___ 

___2018008004 Kim SiWoo___





## Project 1 [MLFQ & Stride scheduling] 

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
-> adding new variable to struct proc is better I think*/

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
probability such process gets walk -> [int]/B * B/100 = [int]/B
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
int sys_getlev(void); // get the level of current process ready queue of 					   // MLFQ. Returns one of the level of MLFQ (0,1,2)
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

