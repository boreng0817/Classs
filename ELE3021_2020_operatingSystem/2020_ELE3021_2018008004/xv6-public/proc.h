// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state

struct run {
  struct run *next;
};

struct queue {
  struct proc *next;
  struct proc *prev;
  int size;
};

struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)

  //mlfq
  int mlfq_level;              // Level of MLFQ scheduling
  int passed_ticks;            // Total number of ticks proccess had 
  int possesed_ticks;          // The number of ticks since scheduled

  //stride
  int is_stride;               // = 1 when stride; = 0 when mlfq
  int stride_ticket;           // Tickets possessed by this proc
  int stride_pass;             // Pass of this proc
  //mlfq && stride
  struct proc *next;           // Link for doubly linked list
  struct proc *prev;           // Link for doubly linked lsit (queue)
  
  //LWP
  struct run *sp_free_list;
  int thread_cnt;
  int is_thread;
  thread_t tid;
  void *retval;
  uint stack_base;
  struct queue LWP_group;
	struct proc* next_LWP;
	struct proc* prev_LWP;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

extern int total_ticks;
//mlfq
void mlfq_push(struct queue*, struct proc*);
void mlfq_delete(struct queue*, struct proc*);
struct proc *mlfq_get_proc(void);
void mlfq_replace(struct proc*);
void mlfq_degrade_level(struct proc*);
void mlfq_boost(void);
void mlfq_merge(struct queue*, struct queue*);
//stride
void stride_stand(void);
void stride_push(struct proc*);
void stride_pop(struct proc*);
struct proc *stride_get_proc(void);

//util
void util_delete(struct queue*, struct proc*);
void util_push(struct queue*, struct proc*);
struct proc *util_get_runnable_LWP(struct proc*);

// [Project 2]
int thread_create(thread_t*, void *(*)(void*), void*);
void thread_exit(void*) __attribute__((noreturn));
int thread_join(thread_t, void**);
void LWP_push(struct queue*, struct proc*);
void LWP_delete(struct queue*, struct proc*);
