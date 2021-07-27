# DBMS PROJECT3

## [Buffer management] </br>

> Project3는 기존 Project2의 IO 수를 줄이기 위해, buffer layer 추가하여 성능 향상을 목표로 한다.</br>

> 기존에 open, delete, find, insert에 다수의 table들을 동시에 관리할수 있게 변경하고, </br>

> shutdown, close, db_init api를 추가하였다. </br>

> Buffer mananger의 design과 고칠 점이나 개선할수 있는 부분들에 대해서 적어보도록 하겠다. </br>

----

## Design

먼저, buffer manager의 형태를 다음과 같이 설계하였다. </br>

structure [Buffer] </br>

 - int capacity ; frame의 갯수
 - int num_used_frame ; 사용 중인 frame의 수
 - frame (ptr)frames  ; frame array
 - int (ptr)fds ; table_id와 대응하는 fds array
 - int (ptr)table_ids ; table이름을 간단하게 hash한 값 array.
 - stack empty_frame ; 빈 frame을 관리하기 위한 stack DST
 - int LRU_head ; doubly linked list의 HEAD
 - int LRU_tail ; doubly linked list의 TAIL
 - hash frame_in_usage ; 사용중인 frame들을 hash하여 담아 둔 hash table
 - stack (ptr)table_buffer ; close를 쉽게하려고 설계하였으나 생각과 달라 쓰지 못함 개선할 부분. </br>
 
 fds array에는, i번째 table의 fd가 들어있다. </br>
 그리고 table_ids array에는 table의 이름을 hash한 값이 들어있다. </br>
 이는 open한 table을 또 open하는 경우를 체크하기 위해 사용했다. </br>

 빈 프레임들을 관리하기 위해 stack을 사용하였다. </br>
 빈 프레임을 얻는데 O(1) </br>
 빈 프레임을 넣는데 O(1) </br> 
 frame 전체를 scan하는 것보다 굉장히 효율적이다. </br>

 그리고, LRU list는 doubly linked list로 frame에 prev, next를 추가하여 </br>
 따로 자료구조를 마련하지 않고, int 2개씩 더 사용하여 구현하였다. </br> 

 그 다음으로, 사용 중인 frame들을 검색하기 위해 hash를 사용하였고, </br> 
 hash table의 크기는 frame 갯수 x 10만큼의 수보다 큰 소수를 사용하였다. </br>
 linear probing으로 collision을 해결하였으나, workload가 커지면 커질수록 </br>
 대부분의 hash table이 DEL된 상태여서 거의 linear search를 하는 정도의 </br>
 상황까지 나오기도 했다. 아마 다음 과제를 위해 같은 hash값에 한하여 linking을 하는 </br>
 방향이나, 다른 해결방안을 제시해야 할것 같다. </br> 

 마지막으로, 스택 배열을 만들어서 테이블별로 사용중인 frame들을 구분하려고 시도했으나, </br>
 추가, 삭제가 일어날때 마다 stack을 scan하고, 또한 shifting까지 해야하는 번거로움이 </br>
 생겨 close table을 실행할 때, linear scan과 비슷하다고 생각되어 구현을 하지 않았다. </br>
 hash array같은 다른 방안을 고려해서, close table을 효율적으로 하는 방안을 추가해도 </br>
 좋을것 같다.

 그리고 frame sturcture는 </br> 
 page / page_num / table_id / is_dirty / is_pinned / next / prev 로 구성되어있고,</br> 
 과제 명세에 나와있는 것과 거의 유사하다.</br>

 ## Data structure와 operations

 ### Stack </br>
 스택은 사용 가능한 frame을 관리하기 위해 사용한다.</br>
 stack의 operation은 다음과 같다</br>
 > init 
 > free
 > push O(1)
 > pop O(1)

 ### Hash</br>
 해시는 사용중인 frame을 linear scan 대신, O(1)을 기대하여 search하려고 사용한다.</br>
 해시의 table size는 #frame x 10 보다 큰 소수를 사용하였다.</br>
 hash의 operation은 다음과 같다</br>
 > init
 > free
 > get&set table_size O(1)
 > hash function for table, page O(1)
 > insert O(1)
 > delete O(1)
 > get frame_index O(1)</br>
 이와 같이 생각하고 구현하였으나, linear probing 사용했다는 점과</br>
 operation수가 굉장히 많아지면 hash table 대부분이 DEL로 채워져 거의 linear scan과</br>
 비슷한 수준의 insert/delete/find를 했다.</br>
 굉장히 큰 성능 저하가 생기기 때문에, 이는 꼭 다음 과제를 위해 고쳐져야 할것이다.</br>
 probing 대신, hash 칸을 linked list로 바꾸는 것도 고려해볼만 하고,</br>
 probing의 방식을 double hashing이나 qudratic probing으로 바꾸면 덜 clustered하여</br>
 성능의 향상을 기대해볼수 있을것 같다.</br>

 ### LRU list</br>
 Doubly linked list로 구현하였고, 따로 자료구조를 쓰지 않고</br>
 frame에 next/prev와 buffer header에 head, tail을 만들어 사용했다.</br>
 LRU list는 다음과 같은 operation이 있다.</br>
 > delete O(1)</br>
 > append O(1)</br>
 > pop O(1)</br>

 pinning이 거의 없다고 가정하면, pop과 delete가 거의 constant operation이라고 기대해볼수 있다.</br>
 frame을 상위 layer에게 넘겨주거나, replacement가 필요한 상황에 delete가 발생한다.</br>
 상위 레이어에게 넘겨줄 땐 delete이후 append가 동반된다.</br>
 pop은 buffer header가 가르키는 LRU list의 head가 victim이 됐을 때 delete 내부에서 실행된다.</br>
 append는 가장 최근에 사용된 frame을 LRU list의 가장 끝에 붙이는 operation이다.</br>

 ## Pinning, replacement policy</br>
 Project2를 구현한 방식이 insert와 delete의 part function을 수행하고, 모든 정보를 적은 뒤에</br>
 필요한 page_number를 다음 function의 argument로 전달하는 방식을 사용했는데,</br>
 이 때문에 pinning은 한 함수에서 read한 뒤에 write가 꽤 이후에 될 frame을 대상으로 pinning을 하였다.</br>

 그리고 buf_read, buf_write의 return value가 사용되는 frame의 index이기 때문에, 간단하게</br>
 pinning, unpinning function의 argument로 넘기는 방식을 이용했다.</br>

 Replacement policy는 LRU방식을 사용하였고, 시간이 된다면 ARC라는 policy를 사용해보면 어떨까라는 생각을</br>
 해보았으나, Project3 제출기한 안에 맞추지는 못했다. 이후 과제를 진행할 때, 적용해보면 좋을것 같다.</br>


----
## 느낀점, 개선할점</br>

일단, 개선할 점으로는 역시 frame search를 하는 hash table에 큰 개선이</br>
필요하다. O(1)을 기대하고 구현하였으나, 실상 보니 거의 O(n)정도의 시간이</br>
필요로해 보였다. clustering을 꼭 해결해야 성능의 큰 향상이 있을것 같다.</br>

그리고 close를 빠르게 하기위해 table마다 frame index들을 관리하는게</br>
어떨지 고려해보면 좋을 것 같다.</br>

헤더파일을 적절히 선언해두면, layered architecture를 꼼꼼하게 지킬수 있다는</br>
얘기를 동기와 하였는데, 마치 객체지향의 상속과 비슷한 개념으로 헤더파일을 수정하면</br>
좋을 것 같다.</br>
----
## API </br>

int init_db(int buf_num)</br>
> buf_num만큼 버퍼를 만들어 db를 작동시키는 함수.</br>

int open_table(char *pathname)</br>
> table을 열어보고, 없다면 만든 뒤 열고, 있다면 그대로 열어 </br>
> fds, table_ids에 추가하였다. 최대 10개까지 열리고, 이 이상 열리면 더이상 열리지 않도록 구현했다.</br>
> 또한, unique id는 buffer가 들고있는 index와 같아 처음으로 열린 파일은 1, close 없이 열기만 한다면</br>
> 1부터 10까지 채워나가는 방식이고, 만약 지운다면 그 칸을 빈 칸이라고 메모 해두고 그 다음 open할 때 채우는</br>
> 방식을 사용했다.</br>

int db_insert(int table_num, my_key_t key, char* value)</br>
> 실제 bpt에서 실행되는 insert의 wrapper functino으로 table num이 valid한지 체크하고,</br>
> 맞다면 실행을, 아니라면 failure signal을 리턴하는 방식으로 구현하였다.</br>

int db_find(int table_num, my_key_t key, char* ret_val)</br>
> 위와 같은 방식으로 find를 구현하였다.</br>

int db_delete(int table_num, my_key_t key)</br>
> 위와 같은 방식으로 delete를 구현하였다.</br>

int close_table(int table_id)</br>
> frame을 scan하여 table_id와 matching하면 evict하는 방식으로 close를 구현하였다.</br>
> 만약 없는 table_id를 넣거나, 열리지 않았던 table을 닫으려고 하면 failure signal을 리턴하였다.</br>

int shutdown_db()</br>
> 모든 frame을 evict하고, buffer를 free해준뒤 프로그램을 종료시키는 함수이다.</br>

*project 3 WIKI ends*</br>

----