# project4 wiki

---

### Content

- Prologue
- Implementation detail
- Experiment, analysis
- Epilogue

---

### Prologue

GOAL : Implement the Border-Collie protocol on PostgreSQL

### Border-Collie, lock-free logging system

In DBMS, logging is critical part for maintaining `Durability` and `Atomicity` of DBMS. So, handling logs is very important issue for constructing DBMS but we should check if logging system makes bottleneck points. Border-Collie gives huge performance increase to such DBMS that uses 1) central write ahead log buffer 2) giant lock for protecting the WAL buffer 3) flushing logs into storage needs to be done by grabbing a lock.

It solves each problem by these steps.

1) Removing mutex that protects WAL buffer,

2) Reserving log’s space with atomic instruction (Fetch-and-Add)

3) Write log into buffer,

4) Border-Collie calculates “Recoverable Logging Boundary”, and WAL-writer flush some buffers to disk.

Let’s continue to implementation part.

---

## Implementation details.

In Postgres, it uses one central WAL buffer that protected by `WALInsertLocks`, which is array of `WALInsertLockPadded`. Size of this array is defined with `NUM_XLOGINSERT_LOCKS` which is 8 in default. So, it actually uses fine-grained lock instead of one giant lock. (But in some case, it becomes a giant lock when we call `WALInsertLockAcquireExclusive`) 

I tried to remove this lock, but I didn’t remove it properly so I will explain how I detach WAL-writer’s flush and worker’s logging.

For managing source code, I used 2 macros.

```c
// In src/backend/access/transam/xlog.c
#define BORDER_COLLIE 1
// #define WORK_IN_PROCESS 1
```

If we define `BORDER_COLLIE` macro, logging system will partially switched into border-collie algorithm. Remaining parts are in `WORK_IN_PROCESS`, which isn’t working properly. You can comment and uncomment these macro and test the performance. Uncommenting `WORK_IN_PROCESS` will makes `segfault`.

As I mentioned before, Postgres uses fine grained lock called `WALInsertLocks`. Each process can hold this lock using it’s pid, so the size of the lock array represents the maximum number of concurrent log writing. I extended this number 8 → 64 for more concurrency.

I added some feature for implementing border-collie algorithm.

```c
#ifdef BORDER_COLLIE
enum Flag {
  WHITE,
  BLACK,
  UNDEFINED
};
#endif

...

typedef struct
{
  LWLock		lock;
 	XLogRecPtr	insertingAt;
 	XLogRecPtr	lastImportantAt;
#ifdef BORDER_COLLIE
  XLogRecPtr  currentLSN;  // ADDED
  enum Flag   flagColor;   // ADDED
#endif
 } WALInsertLock;
```

Since I add some fields in `WALInsertLock`, I modified initiation function as well.

```c
// In XLOGShememInit
#ifdef BORDER_COLLIE
		WALInsertLocks[i].l.currentLSN = InvalidXLogRecPtr;
  	WALInsertLocks[i].l.flagColor = UNDEFINED;
#endif
```

### Setting flag

In `XLogInsertRecord`, it first grabs a lock and **1) reserve logging point** and **2) write log into WAL buffer.**

  1) reserving logging space

```c
// In XLogInsertRecord()
// We reserve a space for writing log
ReserveXLogInsertLocation(rechdr->xl_tot_len, &StartPos, &EndPos,
 								  &rechdr->xl_prev);
inserted = true;
// We set currentLSN, and set flag as black
#ifdef BORDER_COLLIE
    WALInsertLocks[MyLockNo].l.flagColor = BLACK;
    WALInsertLocks[MyLockNo].l.currentLSN = StartPos;
#endif
```

Since we reserved a log space, we set start point as BLACK flag.

  1-1) reserving space with atomic instruction

```c
// In ReserveXLogInsertLocation()

#ifdef BORDER_COLLIE 
  /* reserve log space for XLog with atomic fetch and add */
  startbytepos = __sync_fetch_and_add(&Insert->CurrBytePos, size);
  endbytepos = startbytepos + size;
  prevbytepos = Insert->PrevBytePos;
  Insert->PrevBytePos = startbytepos;
#else
  SpinLockAcquire(&Insert->insertpos_lck);

  startbytepos = Insert->CurrBytePos;
  endbytepos = startbytepos + size;
  prevbytepos = Insert->PrevBytePos;
  Insert->CurrBytePos = endbytepos;
  Insert->PrevBytePos = startbytepos;

  SpinLockRelease(&Insert->insertpos_lck);
#endif
```

I used atomic instruction to reserve log buffer space, but before we reserve log space, it internally grabs a lock, which has no effect. But if I properly remove lock in `XLogInsertRecord`, this codes will become meaningful.

  2) write log into WAL buffer

```c
// In XLogInsertRecord()
// We reserve a space for writing log

CopyXLogRecordToWAL(rechdr->xl_tot_len, isLogSwitch, rdata,
 							StartPos, EndPos, insertTLI);
 
#ifdef BORDER_COLLIE
    WALInsertLocks[MyLockNo].l.flagColor = WHITE;
    WALInsertLocks[MyLockNo].l.currentLSN = EndPos;
#endif
```

When `CopyXLogRecordToWAL` is done, we can set it’s flag as WHITE and `currentLSN` as `EndPos` of log.

### Calculating Recoverable Logging Boundary

`WAL-writer`, which is background process that periodically flush WAL logs into storage. In vanilla Postgres, it grabs whole lock with `LWLockAcquire(WALWriteLock, LW_EXCLUSIVE)` so during flushing, no worker thread can write log into buffer. I modified this as a non-blocking, so more logs can be written into buffer.

```c
// In XLogBackgroundFlush()

#ifdef BORDER_COLLIE
  XLogRecPtr cutoff;
  XLogRecPtr RLB;

  /* cutoff flag */
  cutoff = (XLogRecPtr)XLogCtl->Insert.CurrBytePos;
  /* set RLB as max value */
  RLB = PG_UINT64_MAX;

  /* 
   * get RLB (Recoverable Logging Boundary) 
   * Main BORDER_COLLIE algorithm
   */
  for (int i = 0; i < NUM_XLOGINSERT_LOCKS; ++i) {
    XLogRecPtr currLSN = WALInsertLocks[i].l.currentLSN;
    enum Flag flag = WALInsertLocks[i].l.flagColor;

    if (flag == BLACK) { 
      RLB = MIN(currLSN, RLB);
    } else if (flag == WHITE) {
      RLB = MIN(MAX(cutoff, currLSN), RLB);
    } else {
      continue;
    }
  }

  /* set request */
  WriteRqst.Write = RLB == PG_UINT64_MAX ? 0 : RLB;

  /* 
   * Since XLogWrite needs critical section, but it doesn't 
   * have to acuire Lock via LWLockAcquire().
   */
	if (WriteRqst.Write > LogwrtResult.Write ||
		WriteRqst.Flush > LogwrtResult.Flush)
	{
    START_CRIT_SECTION();
    XLogWrite(WriteRqst, insertTLI, flexible);
    END_CRIT_SECTION();
  }

	AdvanceXLInsertBuffer(InvalidXLogRecPtr, insertTLI, true);

  return true;

#else // NO BORDER_COLLIE

... 

#endif // NO BORDER_COLLIE
```

I almost rewrite the pseudo code of border-collie algorithm. It calculates RLB based on workers currentLSN and flag. 

---

## Experiment, analysis

Test environment

```bash
# Used shell script
#!/bin/sh
./terminate.sh
./run.sh --start --initdb
./run.sh --createdb
./run_sysbench.sh --cleanup --prepare
./run_sysbench.sh --run
```

Hardware spec

```
[In VMWare]

Architecture:                    x86_64
Model name:                      Intel(R) Core(TM) i5-8600K CPU @ 3.60GHz
CPU(s):                          4
RAM:                             4Gig
OS:                              Ubuntu 20.04
```

- modify thread number 12 → 4 in `./run_sysbench.sh`
- modify running time 3600 → 120

```
[BORDER_COLLIE]
SQL statistics:
    queries performed:
        read:                            0
        write:                           3014408
        other:                           0
        total:                           3014408
    transactions:                        3014408 (25119.90 per sec.)
    queries:                             3014408 (25119.90 per sec.)
    ignored errors:                      282    (2.35 per sec.)
    reconnects:                          0      (0.00 per sec.)

Throughput:
    events/s (eps):                      25119.9012
    time elapsed:                        120.0008s
    total number of events:              3014408

Latency (ms):
         min:                                    0.08
         avg:                                    0.16
         max:                                   93.75
         95th percentile:                        0.27
         sum:                               478490.58

Threads fairness:
    events (avg/stddev):           753602.0000/15709.30
    execution time (avg/stddev):   119.6226/0.01
```

```
[VANILLA POSTGRES]
SQL statistics:
    queries performed:
        read:                            0
        write:                           2727530
        other:                           0
        total:                           2727530
    transactions:                        2727530 (22728.94 per sec.)
    queries:                             2727530 (22728.94 per sec.)
    ignored errors:                      258    (2.15 per sec.)
    reconnects:                          0      (0.00 per sec.)

Throughput:
    events/s (eps):                      22728.9367
    time elapsed:                        120.0025s
    total number of events:              2727530

Latency (ms):
         min:                                    0.08
         avg:                                    0.18
         max:                                  117.70
         95th percentile:                        0.31
         sum:                               478399.49

Threads fairness:
    events (avg/stddev):           681882.5000/29187.86
    execution time (avg/stddev):   119.5999/0.02
```

⇒ `BORDER_COLLIE : 25119 trx/sec` vs `VANILLA : 22728 trx/sec`

It has little improvement, but it would be better if I have more cores in my laptop. I couldn’t test the performance based on the number of thread. I wonder how the performance change when the more cores execute sysbench. I think overhead in WAL-writer is alleviated which makes small performance improvement. I guess removing giant mutex will make large improvement to Postgres.

---

### Epilogue

Project4 was very challenging, due to lack of experience modifying large project. It was a good lesson for understanding big program and applying theory into real life (partially).

Since duration of project4 was overlapped with final tests and other projects, I couldn’t spend much time on project4. I really want to remove that macro `WORK_IN_PROCESS`, but due to difficulty and lack of time, I couldn’t finish the entire project. 

Anyway, I sincerely thanks to TA and professor for providing valuable projects and theory. I learnt so many things through this course.

Thanks for reading my WIKI.

---

*WIKI of project4 ends.  __fin*