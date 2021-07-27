# DBMS PROJECT2

## <span style="color:purple">_MILESTONE 2_<span>

> milestone 2는 주어진 bpt.c를 기반으로 disk-based bpt를 구현하는 것이다. </br>

> 과제 명세에 따라 open, isnert, delete, find operation을 구현하였다. </br>

> 본 보고서는 구현에 있어 in-memory와 다른 점, 그리고 다른점을 어떻게 구현했는지, </br>

> 구현함에 있어 고칠 점이나 개선할수 있는 부분들에 대해서 적어보도록 하겠다. </br>

----
## In-memory bpt와 Disk-based bpt의 차이
가장 큰 차이가 있다면, 아무래도 규모의 차이가 있을것 같다.
DB파일이 너무 커져 프로그램이 모두 메모리에 올려 다루지 못하는 상황에서는 
디스크에 정보들을 기록해가면서 읽기, 쓰기를 반복하는 bpt를 만들어야한다. </br>

따라서 in-memory에서 node나 record, pointer등 차이가 나는 부분들을
Disk-based bpt에 알맞게 수정해야하는데, 먼저 node는 4가지 page 구조체를
만들어 사용하였다. </br>  

header page, internal page, leaf page, free page. 
그리고 file_manager api를 사용하여 통일된 함수로 4가지 페이지를
다루기 위해서 같은 크기의 page_t를 만들어 5가지 구조체로 node를 만들었다. </br>

그리고 주어진 코드에서 in-memory bpt는 internal node와 leaf node를
통일된 order를 사용하여 구현하였지만 disk-based는 internal page와
leaf page의 order를 다르게하여 구현하였다. (본 과제를 하면서 in-memory bpt도 
상황에 따라 다른 order로 구현이 가능할것 같다) 따라서 코드를 수정하는데 있어서,
node와 page사이를 적당히 mapping하는게 가장 어려운 숙제였던것 같다. </br>

----
## 구현

**internal page의 page_0**
insert와 delete를 구현하면서, 시간을 가장많이 잡아먹은 녀석이다.
in-memeory의 node는 key가 order - 1개, pointer가 order개 있다.
dist-based는 records array에 order - 1개를 넣고, 그리고 0번째 pointer로
page_0을 사용하는데, 이를 mapping하는게 코드를 수정하는 점에서 가장 큰 관건이었다.
특히 두드러진 부분이 delete구현에 있어 neighbor_index인데, leftmost child의
neighbor_index는 -1로 이부분만 처리해주면 됐지만, page는 in-memeory의 pointer
index보다 하나씩 적었다. </br>

> page.page_0 == node 0th pointer </br>

> page 0th pointer ( = page_num ) == node 1st pointer ... </br>


**delayed merge**
merge operation을 정직하게 하면 굉장히 효율이 떨어지는 경우가 생긴다.
merge된 page에 값을 하나 insert하면 다시 split해야하고, 
이런 경우가 반복되면 insert, delete operation이 느려진다. 
따라서 이를 보완하기 위해서 merge의 기준을 낮추는 방법이 있다.
명세에서 제시한 delayed merge대로 page에 남은게 없을 때 merge를 진행했다.
*DELAYED_MERGE* 글로벌 변수를 하나 사용하여, 매크로와 비슷하게 사용하였는데,
delete를 진행하면서 toggle_delayed_merge를 통해 기능을 키고 끄고를 가능하게
구현하였다.
DELAYED_MERGE가 0이라면, 기존에 min_key를 사용하고,
1이라면 min_key를 1로 사용하여 0인 경우만 merge를 하였다. </br>

**free page**
인서트를 계속하다보면, free page를 할당해줘야하는데, 기존에 파일 크기를
정해두고 가기 때문에, 남은 페이지가 없는 경우가 생긴다. 이 때문에, 헤더가 
가르키는 free_page_num이 0인 상황에, 즉 더 이상 남은 페이지가 없는 상황에
파일 크기를 2배로 늘리는 작업을 했다 (expand_file()).
기본 파일 크기는 page 1024개였고, 2배씩 늘리는 방식으로 구현하였다. </br>

**global variable**
구현을 해나가면서 굉장히 반복되는 작업들을 해야하는 상황이 몇가지가 있었는데,
root page의 번호를 알기 위해 헤더 페이지를 읽는다거나,
Read Write를 할 때 file description이 필요한 상황이 많았다.
그래서 g_fd, g_head를 두어 불필요한 작업을 좀 줄였다.</br>

그 외에도 int DELAYED_MERGE, queue q를 전역변수로 두었는데,
DELAYED_MERGE는 변경을 하는 상황이 생겨 전역변수로, queue는
트리를 출력할 때 필요해 선언하게 됐다. </br>

**unique table id**
추후 과제를 위해 테이블에 unique한 값을 두라고 과제 명세에 있었다.
간단하게 이름을 적당한 simple_hash를 통해 table의 id를 배정하였고,
파일을 열때 받는 char pointer로 얻을수 있는 값이기 때문에, 따로
metadata같은걸 사용하지 않아도 됐다.</br>

----
## 느낀점, 개선할점

주어진 in-memeory 코드를 적당히 바꿔가면서 과제를 진행했는데,
page_0의 존재를 알아차리지 못하여 굉장히 고생을 많이 했다.
그래도 손으로 직접 짜보니 bpt의 이해에 지대한 도움이 됐다.</br>

구현을 계속하면서 고칠점이 가끔 관찰됐는데,
계속 사용되는 page를 page_num이 아닌 포인터로 함수에 넘겨가며 사용하는
방식이라던가, 헤더파일을 api 구분에 맞게 쪼개는 작업이라던가 고치기엔 
너무 늦어버린 상황이 왔다. 시간이 좀 생긴다면 수정을 하고싶다.
특히 구조체와 page_num으로 insert, delete를 진행하다보면
불필요한 i/o가 생기는 상황이 왔지만, 이미 구현한게 많아 바로잡지 못했다.
특히 이 부분을 추후에 시간이 난다면 성능 향상을 위해 꼭 고치도록 해야겠다. </br>

----
## Main 

메인 함수에는 다음과 같은 operation이 가능하다.</br>

> i <k> <value>  -- Insert <k> <value> pair </br>

> f <k>  -- Find the value under key <k> </br>

> d <k>  -- Delete key <k> and its associated value </br>

> t -- Print the B+ tree </br>

> m -- toggle delayed merge. (defult : do delay merge) </br>

> p -- toggle printing tree when insert or delete is succeeded. </br>

> q -- Quit. (Or use Ctl-D.) </br>

원래 주어진 in-memeory main에서 있던 기능 몇가지는 빼고,
m, p를 추가로 넣었다. </br>
기본적으로 insert와 delete이후 트리를 출력하는데, disk-based tree는
너무나 크기 때문에, 매번 operation을 하고 트리를 출력하면 너무나 프로그램이 느려진다.
트리 출력이 꽤 무거운 작업이기도 하다. 그래서 insert와 delete이후 출력 결과를 키고 끄고하는게
p 이다.</br>
그리고 m은 delayed merge를 키고 끄는 operation이다.</br>

*milestone2 ends*

----
