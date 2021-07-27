# DBMS PROJECT4

## [Join Table] 

> 이번 과제에서는 self join을 포함한 두 테이블을 Join하는 operation을 추가하는 Project이다.
>
> 특히, Join 중 natural join만 구현했고, join의 대상은 table의 primary key를 대상으로 하였다.

----

## Design

이번 과제는 project2, 3과 대비하여 복잡하지 않았다. 

그 이유는 

> 1) 현재 우리가 만든 table은 <key> <vale>인 간단한 형태의 record들로 이루어져있다.
>
> 2) 또한, alt 1 index를 사용하여 구현하였기 때문에 leaf단에 있는 record들이 모두 정렬되어 있는 형태이다.
>
> 3) 마지막으로, key들의 중복을 허용하지 않는다.



이러한 이유로, 여러 join methods 중 sort-merge join을 사용했다. 

- 정렬이 되어있고, 중복이 없으니 worst case도 피할 수 있다.



[A] -> A테이블에 있는 Leaf page의 숫자라고 두면,

A, B 테이블을 join하는데 필요한 IO는 [A] + [B] + Height of A + Height of B로 

O(Sum of two table's #Page)이다.





Join Operation을 위해, 다음과 같은 두 개의 helper function을 구현하였다.

leaf_page_t *join_get_first_leaf(int table_id);

leaf_page_t *join_get_right_sibling(int table_id, leaf_page_t *leaf);

> *join_get_frist_leaf*
>
>  해당 [table_id]의 leftmost leaf 페이지를 담고 있는 frame의 page부분 주소를 리턴한다.

> *join_get_right_sibling*
>
> 해당 [table_id]에 있는 [leaf]페이지의 오른쪽 leaf page를 담고 있는 frame의 page부분 주소를 리턴한다.
>
> 만약 해당 [leaf]페이지가 rightmost leaf면 NULL을 리턴한다. 



# Update

이전 과제를 하면서 찾은 버그들이나, 추가한 함수들이다.

- frame *buf_get_frame(tablenum_t table_num, pagenum_t page_num); 를 buffer_mgr에 추가로 구현하였다.

  > 기존의 buf_read_page의 wrapper function으로, buf_read_page의 3번째 인자 page_t *dest에 NULL을 넘기면 memcpy를 하지않고 frame의 index를 받아와 frame array의 해당 index에 위치한 frame의 주소를 리턴하는 함수이다.

- int buf_read_page(tablenum_t table_num, pagenum_t page_num, page_t *dest); 에서 dest에 NULL이 들어오는 경우 memcpy를 하지않고 index만 리턴하는 방식으로 구현을 약간 변경하였다.



 _Project4 WIKI ends_

----