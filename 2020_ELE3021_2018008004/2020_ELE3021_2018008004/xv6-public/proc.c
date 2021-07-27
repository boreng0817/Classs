#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

int total_ticks = 0;
//mlfq 
struct queue mlfq[3];
unsigned int time_allotment[3] = {20, 40, (unsigned int)-1};
int time_quantum[3] = {5, 10, 20};

//stride
struct queue stride;
int mlfq_tickets = 100;
int mlfq_pass = 0;
int stride_tickets = 0;
int stride_pass = 0;
const int LARGE_NUM = 10000;
enum sched_method{MLFQ0, MLFQ1, STRIDE} sched_decision;

//LWP
uint thread_count = 0;
const struct proc empty;
struct proc *last_stand;

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE; // KSTACKSIZE : 4K

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  //* Proc initially queued in 0-level,
  //* with 0 passed tick.
  p->mlfq_level = 0;
  p->passed_ticks = 0;
  p->possesed_ticks = 0;
  p->is_stride = 0;
  p->next = p->prev = 0;
  p->next_LWP = p->prev_LWP = 0;
  p->LWP_group.next = p->LWP_group.prev = (struct proc*)0;
	p->LWP_group.size = 0;

  p->is_thread = 0;
	p->sp_free_list = (struct run*)0;
  //* insert proc into queue[0]
  mlfq_push(&mlfq[0], p);
  LWP_push(&p->LWP_group, p);
  
	return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
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

	if(n > 0){
    if((sz = allocuvm(p->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(p->pgdir, sz, sz + n)) == 0)
      return -1;
  }
	p->sz = sz;
  switchuvm(curproc);
  release(&ptable.lock);
	return addr;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid, sz;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }


	sz = curproc->is_thread ? curproc->parent->sz : curproc->sz;
  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0; // child process returns 0 for fork().

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

void
LWP_set_exit(struct proc *p)
{
	struct proc *it;
	if(p->is_thread)
		it = p->parent->LWP_group.next;
	else
		it = p->LWP_group.next;
	for(;it;it = it->next_LWP){
		it->killed = 1;
	}
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd, cnt = 0, tcnt;

  if(curproc == initproc)
    panic("init exiting");

	acquire(&ptable.lock);
	LWP_set_exit(curproc);
	release(&ptable.lock);

	if(curproc->is_thread) {
	 for(fd = 0; fd < NOFILE; fd++){
			if(curproc->ofile[fd]){
				fileclose(curproc->ofile[fd]);
				curproc->ofile[fd] = 0;
			}
		}
	 
		begin_op();
		iput(curproc->cwd);
		end_op();
		
		curproc->cwd = 0;

		acquire(&ptable.lock);
		wakeup1(curproc->parent);
		curproc->state = ZOMBIE;
		sched();
	}

	acquire(&ptable.lock);
	tcnt = curproc->thread_cnt;
  for(;;){
		if(!tcnt)
			break;
    // Scan through table looking for exited children.
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      // If in different LWP group
      if(p->pid != -curproc->pid)
        continue;
			if(p == curproc)
				continue;

    
			if(p->state == ZOMBIE){
        kfree(p->kstack);
        p->kstack = 0;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;

        p->tid = 0;
        p->retval = 0;


				struct run *r = (struct run*)((uint)p->stack_base - 4);
				r->next = curproc->sp_free_list;
				curproc->sp_free_list = r;
				curproc->thread_cnt--;
				LWP_delete(&(curproc->LWP_group), p);
				cnt++;
      }
    }
		if(cnt == tcnt)
			break;
    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
	release(&ptable.lock);

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

	if(curproc->is_stride){
    stride_tickets -= curproc->stride_ticket;
    mlfq_tickets += curproc->stride_ticket;
    stride_pop(curproc);
  }
  else
    mlfq_delete(&mlfq[curproc->mlfq_level], curproc);
 
  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
} 



// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler2(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

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
    }
    release(&ptable.lock);

  }
}

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
    // find right proc via policy
    acquire(&ptable.lock);
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

    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

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
		/* simplified switchuvm change kstack*/
		if(next == 0)
			panic("switchuvm: no process");
		if(next->kstack == 0)
			panic("switchuvm: no kstack");
	
		pushcli();
		mycpu()->gdt[SEG_TSS] = SEG16(STS_T32A, &mycpu()->ts,    
																	sizeof(mycpu()->ts)-1, 0); // SEG_TSS : 5 task state
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
void
yield2(void)
{
	acquire(&ptable.lock);
	myproc()->state = RUNNABLE;
	sched();
	release(&ptable.lock);
}
// Give up the CPU for one scheduling round.
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

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int
getppid(void) {
    return myproc() -> parent -> pid;
}

int
getlev(void)
{
  return myproc()->mlfq_level;
}
int
set_cpu_share(int share)
{
  struct proc *p, *it;
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
	it = p->is_thread ? p->parent : p;
	for(;it;it = it->next_LWP) {
  	it->is_stride = 1;
  	it->stride_ticket = share;
  	it->stride_pass = 0;
	}

  //push into stride queue
  if(p->is_thread)
		stride_push(p->parent);
	else
		stride_push(p);

  mlfq_tickets -= share;
  stride_tickets += share;

  release(&ptable.lock);

  return 0;
}

//function to enqueue proc into queue
//enqueue when 
//1) proc is created and became runnable
//2) proc goes to lower priority mlfq
void
mlfq_push(struct queue* q, struct proc* p)
{
  util_push(q, p);
}
//function to dequeue
//
void
mlfq_delete(struct queue* q, struct proc* p)
{
  util_delete(q, p);
}

struct proc*
mlfq_get_proc(void)
{
  struct proc *p, *tmp=0;
  int i;
  for(i = 0; i < 3; ++i)
    for(p = mlfq[i].next; p; p = p->next){
			tmp = util_get_runnable_LWP(p);
      if(tmp && tmp->state == RUNNABLE) 
        return tmp;
		}
  //there's no runnable proc in mlfq
  return 0;
}

void
mlfq_replace(struct proc* p)
{
  struct queue *q = &mlfq[p->mlfq_level];
  p->possesed_ticks = 0;
  mlfq_delete(q, p);
  mlfq_push(q, p);
}

void
mlfq_degrade_level(struct proc* p)
{
  if(p->mlfq_level == 2)
    panic("mlfq_level can't be 3\n");
  
  struct queue *q_prev = &mlfq[p->mlfq_level];
  struct queue *q_next = &mlfq[++(p->mlfq_level)];

  p->possesed_ticks = p->passed_ticks = 0;

  mlfq_delete(q_prev, p);
  mlfq_push(q_next, p);
}

void
mlfq_boost(void)
{
  struct queue *q0 = &mlfq[0];
  struct queue *q1 = &mlfq[1];
  struct queue *q2 = &mlfq[2];
  struct proc *p;
  if(!q1->size && !q2->size)
    return;
  else if(q1->size && !q2->size){
    if(!q0->size)
      q0 = q1;
    else
      mlfq_merge(q0, q1);
  }
  else if(!q1->size && q2->size){
    if(!q0->size)
      q0 = q2;
    else
      mlfq_merge(q0,q2);
  }
  else{
    if(!q0->size){
      mlfq_merge(q1, q2);
      q0 = q1;
    }
    else{
      mlfq_merge(q0, q1);
      mlfq_merge(q0, q2);
    }
  }
  q1->next = q1->prev = 0;
  q2->next = q2->prev = 0;
  q1->size = q2->size = 0;

  for(p = q0->next; p; p = p->next){
    p->mlfq_level = 0;
    p->passed_ticks = 0;
    p->possesed_ticks = 0; // should be considered again.
  }
}

void
mlfq_merge(struct queue *q1, struct queue *q2){
  q1->prev->next = q2->next;
  q2->next->prev = q1->prev;
  q1->prev = q2->prev;
  q1->prev->next = 0;

  q1->size += q2->size;
}
// stride ========
// prevent pass not to be overflowed.
void
stride_stand(void)
{
  struct queue *q = &stride;
  struct proc *p = q->next;
  int min_pass;

  //standardization mlfq, stride pass;
  //pass can be negative. doesn't matter
  //stride_pass could be negative after this function
  stride_pass -= mlfq_pass;
  mlfq_pass = 0;

  if(!p)
    return;
  
  min_pass = p->stride_pass;
  for(;p;p = p->next)
    p->stride_pass -= min_pass;

  //prevent not to call stride_stand agina
  //in case of failing to find proc to sched.
  total_ticks++;
  return;
}

void
stride_push(struct proc *p)
{
  struct queue *q = &stride;
  struct proc *it;
  struct proc *it_prev = 0;

  // empty queue
  if(!q->next){
    util_push(q,p);
    return;
  }

  for(it = q->next;it;it_prev = it, it = it->next)
    if(it->stride_pass > p->stride_pass)
      break;
  // p has most pass; it == 0
  if(!it)
    util_push(q,p);
  else if(!it_prev){
    p->next = q->next;
    p->next->prev = p;
    q->next = p;
    p->prev = 0;
    q->size++;
  }
  else{
    it_prev->next = p;
    p->prev = it_prev;
    p->next = it;
    it->prev = p;
    q->size++;
  }
}

void
stride_pop(struct proc *p)
{
    util_delete(&stride, p);
}

struct proc*
stride_get_proc(void)
{
  struct queue *q = &stride;
  struct proc *p, *tmp = 0;

  for(p = q->next;p;p = p->next) {
		tmp = util_get_runnable_LWP(p);
    if(tmp && tmp->state == RUNNABLE)
      break;
	}
	return tmp;
}
// util ==========
void
util_delete(struct queue* q, struct proc* p)
{
  if(!q->size)
    panic("can't pop from empty queue\n");
  
  if(p->next && p->prev){
    p->prev->next = p->next;
    p->next->prev = p->prev;
  }
  else if(p->next && !p->prev){
    q->next = p->next;
    p->next->prev = 0; }
  else if(!p->next && p->prev){
    q->prev = p->prev;
    p->prev->next = 0;
  }
  else if(!p->next && !p->prev){
    q->next = q->prev = 0;
  }

  q->size--;
}

void
util_push(struct queue* q, struct proc* p)
{
  //empty queue
  if(!q->size){
    q->next = q->prev = p;
    p->next = p->prev = 0;
  }
  //append to list;enqueue
  else{
    p->prev = q->prev;
    p->prev->next = p;
    q->prev = p;
    p->next = 0;
  }
  q->size++;
}

#define THREAD_CREATE_FAIL -1
// [Project 2]
int
thread_create(thread_t *thread, void *(*start_routine)(void*),
                                void *args) 
{
  int i;
  uint sz, sp;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process (fork)
  if((np = allocproc()) == 0)
    return THREAD_CREATE_FAIL;
  // Thread can't be directly schdeuled by mlfq_queue
  mlfq_delete(&mlfq[0], np);
  LWP_delete(&np->LWP_group, np);

  acquire(&ptable.lock);
  // Make stack frame for stack
  // If there's used stack base for thread, just give it.
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

  // Add information for master thread
  curproc->thread_cnt++;
  LWP_push(&curproc->LWP_group, np);
  
  // Add information worker thread
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;
  np->cwd = idup(curproc->cwd); 
  
  np->pgdir = curproc->pgdir;
  np->tf->eip = (uint)start_routine;

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));
  np->tid = ++thread_count;
  np->pid = -curproc->pid; // same LWP group
  np->is_thread = 1;
  np->stack_base = sp;
  np->is_stride = curproc->is_stride;

  np->tf->esp = np->stack_base - 8;
  *((uint*)(np->tf->esp + 4)) = (uint)args;
  *((uint*)(np->tf->esp)) = 0xFFFFFFFF;

  // returns the thread id.
  *thread = np->tid;
  for(i = 0; i < NOFILE; ++i)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);

  np->state = RUNNABLE;
  release(&ptable.lock);
  
  return 0;
}

void
thread_exit(void *retval)
{
  // code from exit()
  struct proc *curproc = myproc();
  int fd;

  if(curproc == initproc)
    panic("init exiting");
  
  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }
 
  begin_op();
  iput(curproc->cwd);
  end_op();
	
  curproc->cwd = 0;
  
  acquire(&ptable.lock);
  
  // Set return value before waking parent
  curproc->retval = retval;
  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}
 
int
thread_join(thread_t thread, void **retval)
{
  // code from wait()
  struct proc *p;
  struct proc *curproc = myproc();
  int havethreads;

  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havethreads = 0;
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      // Different thread, continue
      if(p->tid != thread)
        continue;
      // If in different LWP group
      if(p->pid != -curproc->pid)
        continue;
			havethreads = 1;

      if(p->state == ZOMBIE){
        *retval = p->retval;
        kfree(p->kstack);
        p->kstack = 0;
       // deallocuvm(p->pgdir, p->sz, p->sz - 2*PGSIZE);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;

        p->tid = 0;
        p->retval = 0;

			// clean up stack and put into free list
				struct run *r = (struct run*)((uint)p->stack_base - 4);
				r->next = curproc->sp_free_list;
				curproc->sp_free_list = r;
				curproc->thread_cnt--;
				LWP_delete(&(curproc->LWP_group), p);
				*p = empty;
				release(&ptable.lock);

        return 0;
      }
    }
    // No point waiting if we don't have any children.
    if(!havethreads || curproc->killed){
      release(&ptable.lock);
      return -1;
    }
    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}


struct proc*
util_get_runnable_LWP(struct proc *p){
	struct queue *q = &p->LWP_group;
	struct proc *iter;

	if(p->is_thread)
		panic("p can't be thread");

	for(iter = q->next; iter; iter = iter->next_LWP)
		if(iter->state == RUNNABLE)
			break;


	return iter;
}

void LWP_push(struct queue *q, struct proc *p){
  //empty queue
  if(!q->size){
    q->next = q->prev = p;
    p->next_LWP = p->prev_LWP = 0;
  }
  //append to list;enqueue
  else{
    p->prev_LWP = q->prev;
    p->prev_LWP->next_LWP = p;
    q->prev = p;
    p->next_LWP = 0;
  }
  q->size++;

}

void LWP_delete(struct queue *q, struct proc *p){
  if(!q->size)
    panic("LWP_delete can't pop from empty queue\n");
  
  if(p->next_LWP && p->prev_LWP){
    p->prev_LWP->next_LWP = p->next_LWP;
    p->next_LWP->prev_LWP = p->prev_LWP;
  }
  else if(p->next_LWP && !p->prev_LWP){
    q->next = p->next_LWP;
    p->next_LWP->prev_LWP = 0; }
  else if(!p->next_LWP && p->prev_LWP){
    q->prev = p->prev_LWP;
    p->prev_LWP->next_LWP = 0;
  }
  else if(!p->next_LWP && !p->prev_LWP){
    q->next = q->prev = 0;
  }

  q->size--;
}
