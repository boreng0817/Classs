# DBMS PROJECT5

## [Concurrency Control] 

***MILESTONE 2***

Goal for project5

> Conflict serializable!
>
> strict-2PL
>
> Deadlock detection
>
> record level locking w/ Shared & Exclusive mode



프로젝트 5는 다음과 같은 목표를 갖고 begin_trx(), end_trx(), db_update(), db_find() 4개의 api를 구현하는 것이다.

목표는 다음과 같은 방식으로 달성하려고 하였다.

> __conflict serializable__ 
>
> trx이 record lock을 잡으려고 시도하면, conflict하다면 lock table에 append한 뒤, running_trx가 끝나기를 기다린다.

> __S2PL__ 
>
> end_trx에서 모든 lock을 풀고, end하는 trx을 기다리는 모든 trx들을 깨운다.

> __Deadlock detection__
>
> lock table에 lock_obj를 넣으려고 하면, dependency graph에 edge가 하나가 추가된다고 볼 수 있다.
>
> 이 때, trx을 node라고 생각하면 trx_node는 여러개의 in-degree edge를 (여러 trx이 기다리는 상황) 갖고 있을 수 있지만, out-degree (내가 기다리는 trx)는 단 한개 (혹은 없는 상황 => running trx) 갖고 있을 수 있기 때문에, 이 점을 고려하여 conflict가 생겨 edge가 추가될 때, cycle이 생기는지 확인 하였다. 개략적으로, 다음의 흐름과 같다. 
>
> ~~~c
> void DeadlockDetection(int trx_id) {
> 	add node into dependecy graph;
>     check cycle {
>         trx_t *ptr = trx_table[trx_id];
>         trx_t *iter = trx -> trx_wait_for_end;
>         
>         while (iter != NULL AND iter != ptr)
>             iter = iter -> trx_wait_for_end;
>         
>         if (iter == trx)
>             => there is cycle;
>         else // iter == NULL
>             => there is no cycle
>     }
>     
>     if cycle is detected
>         return DEADLOCK;
>     else
>         return CONFLICT;
> }
> ~~~



> __record level locking__
>
> 아마 이번 과제의 꽃이 아닐까 싶다. 아직도 맞게 작동하는지 잘 모르겠지만, 다음과 같이 구현하였다.
>
> ~~~c
> record_lock_state acquire_record_lock(information_for_lock_t info) {
>     
>     if list is empty -> append and return success;
>     if list is not empty {
>         if lock is acquired {//is running 
>         	check lock is from same trx;
>             if is, return SUCCESS, and
>                 if only locked obj.mode = S and info.mode = X
>                     promote;
>             else if lock is from different trx {
>                 if only locked obj.mode == info.mode == S
>                     append and return success;
>                 else
>                     append and check for cycle;
>             }
>         }
>         else if lock is not acquired {
>             just append and check for cycle;
>         }
>     }
>     if trx is woken by other trx, return SUCCESS;
> ~~~
>
> 대략 이런 흐름인데, 굉장히 복잡하게 구현하여 디버그를 하기도 힘들었고, 예외 케이스를 모두 처리했는지도 잘 모르겠으나 일단 배운 내용을 토대로 구현해보았다.
>
> 그리고 lock_table은 <table id, page id>로 매핑되어 lock_list를 얻을 수 있게 해쉬를 사용하였는데, 리스트는 레코드별로 정렬하기 위하여 각 레코드의 마지막 lock_obj를 lock_t -> tail boolean을 하나 사용하여 체크하였다.



**[느낀점]**

이번 디비 수업을 통틀어 체감 난이도가 가장 높았던것 같다. 일단 멀티 쓰레딩이라는 새로운 프로그래밍 방법을 도입하는 부분부터, 많이 생소한 개념인 lock_table_mgr, deadlock같은 것들을 직접 구현하는데 많은 노력이 들었다.

작동을 올바르게 하는지는 모르겠지만, 그러기를 간절하게 빈다 ㅜㅜ. 굉장히 뿌듯하고, 다음 recovery파트를 구현하면 그래도 기본적인 디비의 흐름을 온전히 내것으로 만들 수 있다는 생각에 스스로가 대견하다. 

또한, 구현을 직접 해보면서 직면하는 문제들을 고민하고 해결하면서 개념이 내것이 되는것이 느껴졌다. 



_Milestone 2 ends_ 

---



### ***Milestone 1***



---

## Design of lock mgr

이번 과제는 lock manager를 구현하여 db에 concurrency control을 지원하는 과제이다.

Milestone 1은 나의 lock mgr의 design과 begin_trx(), end_trx()를 구현하는 것이다.



Lock mgr는 다음과 같은 기능을 제공해야한다.

- conflict-serializable schedule for trxs
- S2PL
- Deadlock detection
- Row-level locking with mode S and X

__[Conflict-serializable]__

앞의 join project까지, 현재의 디비는 single thread만 사용한다. 따라서 성능을 높이기 위해, 여러 thread를 사용하고 DB의 성질인 isolation & consistency를 위해 transaction들을 잘 schedule할 필요가 있다. 따라서 pcc의 대표적인 S2PL을 사용하여 locking protocol을 잘 구현하는게 이번 과제의 목표이다.

__[S2PL]__

Two phase locking은 cascading aborts의 가능성이 있기 때문에, conflict가 생기는 trxs끼리 serial하게 실행하고, Read/Write에 대해 lock을 걸어 trx이 commit (=end) 하는 시점에서 trx가 수행한 lock들을 모두 풀어주는게 Strict two phase locking이다. 따라서 milestone 1에 포함된 end_trx()함수를 다음과 같은 흐름으로 구현하였다. 

~~~c
int end_trx(int tid) {
    if (tid is not in trx_table)
        return 0; // fail signal
    int trx_index = find trx index in trx_table; // trx_table would be list
    trx_t *trx = get trx from trx_table(trx_index);
    
    while (trx -> trx_locks is not empty)
        delete from lock obj in lock_table;
    
    free(trx);
    erase trx from trx_table(trx_index);
    
    return tid;
}
~~~

간단하게 end_trx를 pseudo-code로 기술하였다. trx가 commit (=end)하는 시점에 lock을 모두 풀고, milestone 1에서는 구현하지 않았지만, 다음 lock을 모드에 따라 handle하고, commit되길 기다리는 trx들을 다시 깨우는 작업 등을 추가해야할 것으로 생각한다.

__[Deadlock detection]__

Deadlock은 trx_a가 trx_b를 끝나기를 기다리고, trx_b는 trx_a가 끝나기를 기다려 누군가 하나 abort 해야하는 상황을 deadlock이라고 하는데, 이는 trx_t 구조체에 있는 wait_lock를 통해 cycle을 detection하여 abort를 하면 될것 같다. 그리고 trx_t에 read_lock_count, write_lock_count같은 priority를 두어 abort될 victim을 고르면 될것 같다. 구체적으로, count를 두는 경우에는 write_lock_count가 작은 trx을, 동률일 경우 read_lock_count가 작은 trx을, 만약 둘 다 같다면 tid가 더 큰 trx (늦게 들어왔으니)를 abort하면 될것 같다.

__[Row level locking with Shared & Exclusive mode]__

Lock mgr은 read의 경우 S(Shared)를, write의 경우 X(Exclusive)모드를 사용하여 lock obj를 lock table에 넣으면 될것 같다. 이 부분은 db_find와 db_update를 설계하는 부분과 많은 것이 겹친다.

__int db_find(table_id, key, ret_val, trx_id)__  

project4에 있던 db_find api를 수정하여 구현하면 될것 같다. 물론 index layer에 있는 find/find_leaf같은 operation을 수정하여 record에 접근하는 경우 S mode로 lock obj를 생성해 넣어주면 될것 같다.

__int db_update(table_id, key, values, trx_id)__

db_find와 마찬가지로 이전에 구현한 find operation을 적당히 고쳐 값을 copy하는 대신 변경하는 방식으로 구현하면 될것 같다. 찾아 가는 과정중에 있는 모든 record들에 대해 어느 경우에는 S를 바꾸는 경우에 X를 사용하면 될것 같다.

internal_page에 lock을 거는 경우를 곰곰히 생각해보면 좋을것 같다. -> 항상 S로 들어가기 때문이다.

# Implementation

end_trx는 위에 디자인 부분에 코드가 있고, 나머지 begin 부분의 흐름은 다음과 같다.

~~~c
int begin_trx() {
    allocate trx;
    give trx_id with __sync_fetch_and_add(&trx_count, 1);
    
    push into trx_list;
    return trx_id;
}
~~~

비교적 end_trx와 간단하게 할당을 해주고, 번호를 준뒤 trx_list에 trx를 넣고 리턴해주는 방식으로 구현을 하였다.

list<T>에 대해 이해가 부족하여, lock_list를 초기화 하는건 다음 구현으로 미루려고 한다.



그리고 lock_t와 trx_t 구조체는 다음과 같이 설계하였다.

~~~c
typedef struct lock_t {
    int table_id;
    int record_id;
    int trx_id; // end 어느 trx의 lock obj인지 체크하기 위해서
    enum lock_mode mode; // enum{SHARED, EXCLUSIVE}
    struct trx_t *trx; // 어떻게 사용할지 약간 고민이 필요함
}
~~~

~~~c
typedef struct trx_t {
    int trx_id;
    enum trx_state state; // enum{IDLE, RUNNING, WAITING}
    int lock_count; // lock_count_r, lock_count_w로 나누어 priority를 줄수 있을 것 같다.
    list<lock_t*> trx_locks;
    lock_t *wait_lock;
}
~~~

그리고 추가적으로 lock_hash_table_t와 trx_table_t를 만들어야한다.

lock_hash_table은 <hash_digestion, list<lock_t*> > array를 사용하면 될것 같고, 각 원소의 lock obj 갯수와 같은 추가 정보를 담아도 좋을것 같다.

trx_table_t는 일단 doubly linked list로 구현하였는데, 이후 필요한 정보가 생기면 넣어주면 좋을것 같다. ex) # of trxs



milestone 1을 위해 다음과 같은 함수와, 함수 prototype을 설계 하였다.

int trx_get_index_of_trx_table(int tid)

- trx_table_t (현재는 trx_list)에서 tid의 index를 찾는 함수. 만약 찾는 trx가 존재하지 않는다면 -1을 리턴

[prototype]

int lock_table_hash(int table_id, int record_id)

- lock_table_t를 위한 hash 함수. hash function에 들어갈 인자들은 (table_id, primary key)나 (table_id, <page_num, record_num>)을 쓸 수 있다.

int lock_list_get_index_of_lock(int tid)

- lock_table_t에 알맞은 index를 찾은 후, tid에 해당하는 lock obj의 index를 찾는 함수. 같은 record를 같거나 다른 모드로 여러번 접근하는 경우를 잘 고려해야할것 같다.

 _Project5 WIKI ends_

----