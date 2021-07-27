/* 
 * mm-implicit.c -  Simple allocator based on implicit free lists, 
 *                  first fit placement, and boundary tag coalescing. 
 *
 * Each block has header and footer of the form:
 * 
 *      31                     3  2  1  0 
 *      -----------------------------------
 *     | s  s  s  s  ... s  s  s  0  0  a/f
 *      ----------------------------------- 
 * 
 * where s are the meaningful size bits and a/f is set 
 * iff the block is allocated. The list has the following form:
 *
 * begin                                                          end
 * heap                                                           heap  
 *  -----------------------------------------------------------------   
 * |  pad   | hdr(8:a) | ftr(8:a) | zero or more usr blks | hdr(8:a) |
 *  -----------------------------------------------------------------
 *          |       prologue      |                       | epilogue |
 *          |         block       |                       | block    |
 *
 * The allocated prologue and epilogue blocks are overhead that
 * eliminate edge conditions during coalescing.
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"
#include "memlib.h"

/*
 * If NEXT_FIT defined use next fit search, else use first fit search 
 */
#define NEXT_FITx

/* my code */
#define BEST_FIT 1



/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* word size (bytes) */  
#define DSIZE       8       /* doubleword size (bytes) */
#define CHUNKSIZE  (1<<6)  /* initial heap size (bytes) */
#define OVERHEAD    8       /* overhead of header and footer (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(size_t *)(p))
#define PUT(p, val)  (*(size_t *)(p) = (val))  

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)  
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
/* $end mallocmacros */

/* Global variables */
static char *_heap_listp;  /* pointer to first block */  
#ifdef NEXT_FIT
static char *rover;       /* next fit rover */
#endif

/* my code!
 * works when BEST_FIT defines.
 * use stack base DST for implementing seg-lists.
 * it divides 10 different lists (0th not used)
 * each list can hold index * DSIZE block, and BIG_BLOCK holds larger.
 *
 * lists are based on doubly linked list,
 * so it has |HEAD| |NEXT| |PREV| .... |FOOT|
 *           4bytes
 *
 * each lists can do pop, push, delete_block
 */
/* my code */
#ifdef BEST_FIT
#define BIG_BLOCK 10
#define ALIGN(size) DSIZE * ((size+(DSIZE-1))/DSIZE)
static void *_seg_list_top[BIG_BLOCK + 1];
static int _stack_count[BIG_BLOCK + 1];
#define GET_NEXT(p) (*(size_t*)p)
#define GET_PREV(p) (*(size_t*)(p + 4))
#define SET_NEXT(p, val) (*(size_t*)(p) = (val))
#define SET_PREV(p, val) (*(size_t*)(p + 4) = (val))
#define IS_EMPTY(size) (_stack_count[size] == 0) 
#define HEAD 0xFFFFFFFF
#define TAIL 0xDEADBEEF
static void *pop(int index);
static void push(void *p, int index);
static void delete_block(void *p);
#endif


static int _heap_ext_counter=0;

/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);

char* get_heap_listp() {
    return _heap_listp;
}
char* set_and_get_heap_listp(char* ptr) {
    _heap_listp = ptr;
    return _heap_listp;
}

/* 
 * mm_init - Initialize the memory manager 
 */
/* $begin mminit */
int mm_init(void) 
{
    /* create the initial empty heap */
    if (set_and_get_heap_listp(mem_sbrk(4*WSIZE)) == (void *)-1)
	    return -1;
    PUT(get_heap_listp(), 0);                        /* alignment padding */
    PUT(get_heap_listp()+WSIZE, PACK(OVERHEAD, 1));  /* prologue header */ 
    PUT(get_heap_listp()+DSIZE, PACK(OVERHEAD, 1));  /* prologue footer */ 
    PUT(get_heap_listp()+WSIZE+DSIZE, PACK(0, 1));   /* epilogue header */
    set_and_get_heap_listp(get_heap_listp()+DSIZE);

#ifdef NEXT_FIT
    rover = get_heap_listp();
#endif

#ifdef BEST_FIT
    /* initialize set_list and list(stack) count
     */
    int i;

    for (i = 1; i <= BIG_BLOCK; ++i) {
        _seg_list_top[i] = (void*)TAIL;
        _stack_count[i] = 0;
    }
#endif


    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
	return -1;
    return 0;
}
/* $end mminit */

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size) 
{

    size_t asize;
    char *p;

    /* if size == 0, do nothing.
     */
    if (size == 0)
        return NULL;

    /* get aligned (multiple of 8) number
     * corresponds to size, and add OVERHEAD for 
     * header and footer.
     * then get free block and place it.
     */
    asize = ALIGN(size) + OVERHEAD;
    p = find_fit(asize);
    place(p, asize);

    /* give allocated pointer to caller.
     */
    return p;
} 
/* $end mmmalloc */

/* 
 * mm_free - Free a block 
 */
/* $begin mmfree */
void mm_free(void *bp)
{

    /* if ptr is null, do nothing.
     */
    if (bp == 0)
        return;
    /* set list's number, then make it as
     * free block. remove block from the list
     * and coalesce the block.
     */
    int index = GET_SIZE(HDRP(bp)) / DSIZE;
    PUT(HDRP(bp), GET_SIZE(HDRP(bp)) & -1);
    PUT(FTRP(bp), GET_SIZE(HDRP(bp)) & -1);

    index = index < BIG_BLOCK ? index : BIG_BLOCK;
    push(bp, index);
    coalesce(bp);
}

/* $end mmfree */

/*
 * mm_realloc - naive implementation of mm_realloc
 */
void *mm_realloc(void *ptr, size_t size)
{
    int index, payload, free;
    char *temp;

    /* if ptr is null, malloc the ptr
     * with size.
     */
	if(ptr == NULL) {
        ptr = mm_malloc(size);
        return ptr;
    }

    /* if size is 0, free the ptr
     */
    if(size == 0) {
        mm_free(ptr);
        return ptr;
    }

    /* check block's size for
     * a) can it be extended
     * b) should move to other block
     * c) size shrinks
     * d) trivial add. when aligned size is
     *    the same with allocated block's size, do nothing.
     * *) if free block remains size with 8bytes,
     *    it can't hold NEXT and PREV with OVERHEAD
     *    at least, free block should be 16bytes.
     */

    temp = NEXT_BLKP(ptr);
    payload = GET_SIZE(HDRP(ptr)) - DSIZE;
    free = GET_SIZE(HDRP(temp));

    /* d) case. just return ptr.
     */
    if (GET_SIZE(HDRP(ptr)) == ALIGN(size) + OVERHEAD)
        return ptr;
  
    /* c) case. if free block remains 8bytes, do nothing.
     *          else, split free block and coalesce.
     */
    if (GET_SIZE(HDRP(ptr)) > ALIGN(size) + OVERHEAD) {
        
        /* *) case.
         */
        if( GET_SIZE(HDRP(ptr)) - OVERHEAD == ALIGN(size) + OVERHEAD )
            return ptr;
        
        free = GET_SIZE(HDRP(ptr));
        
        PUT(HDRP(ptr), PACK(ALIGN(size) + OVERHEAD, 1));
        PUT(FTRP(ptr), PACK(GET_SIZE(HDRP(ptr)), 1));
        
        temp = NEXT_BLKP(ptr);
        free = free - GET_SIZE(HDRP(ptr));
        PUT(HDRP(temp), PACK(free, 0));
        PUT(FTRP(temp), PACK(free, 0));

        coalesce(temp);
        return ptr;
    }

    /* a) block can extend to next free block.
     *    if free block has plenty space, split another free block.
     *    or give all space for realloc.
     */
    if (!GET_ALLOC(HDRP(temp)) && payload + free >= ALIGN(size)) {
        
        /* coalesce allocated and free block.
         */
        if (payload + free == ALIGN(size)) {
            delete_block(temp);
            PUT(FTRP(ptr), 0); //clear footer
            PUT(HDRP(ptr), PACK(ALIGN(size) + OVERHEAD, 1));
            PUT(FTRP(ptr), PACK(ALIGN(size) + OVERHEAD, 1));
        }
        /* *) if remaining free block is 8bytes,
         *    extend another 8bytes.
         */
        else if (payload + free - OVERHEAD == ALIGN(size)) {
            delete_block(temp);                                                 
            PUT(FTRP(ptr), 0); //clear footer                                   
            PUT(HDRP(ptr), PACK(ALIGN(size) + 2*OVERHEAD, 1));      
            PUT(FTRP(ptr), PACK(ALIGN(size) + 2*OVERHEAD, 1));
        }

        /* reallocate and split another free block.
         */
        else {
            int remain = free + payload - ALIGN(size);
            delete_block(temp);
            PUT(FTRP(ptr), 0);
            PUT(HDRP(ptr), PACK(ALIGN(size) + OVERHEAD, 1));
            PUT(FTRP(ptr), PACK(ALIGN(size) + OVERHEAD, 1));
            
            PUT(HDRP(NEXT_BLKP(ptr)), PACK(remain, 0));
            PUT(FTRP(NEXT_BLKP(ptr)), PACK(remain, 0));
            push(NEXT_BLKP(ptr), remain/DSIZE > BIG_BLOCK ? 
                    BIG_BLOCK : remain/DSIZE);
        }

        return ptr;
    }

    /* b) if block can't extend move to another space.
     */

    index = ALIGN(size) > payload ? payload : ALIGN(size); 
    temp = mm_malloc(size);

    memcpy(temp, ptr, index);

    mm_free(ptr);
    ptr = temp;

    return ptr;
}

/* 
 * mm_checkheap - Check the heap for consistency 
 */
void mm_checkheap(int verbose) 
{
    char *bp = get_heap_listp();

    if (verbose)
	printf("Heap (%p):\n", get_heap_listp());

    if ((GET_SIZE(HDRP(get_heap_listp())) != DSIZE) || !GET_ALLOC(HDRP(get_heap_listp())))
	printf("Bad prologue header\n");
    checkblock(get_heap_listp());

    for (bp = get_heap_listp(); GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (verbose) 
	    printblock(bp);
	checkblock(bp);
    }
     
    if (verbose)
	printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
	printf("Bad epilogue header\n");
}

/* The remaining routines are internal helper routines */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;
    _heap_ext_counter++;
	
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((bp = mem_sbrk(size)) == (void *)-1) 
	return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

    /* Coalesce if the previous block was free */
#ifdef BEST_FIT
    int index = size/DSIZE < BIG_BLOCK ? size/DSIZE : BIG_BLOCK;
    push(bp, index);
#endif
    return coalesce(bp);
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
/* $begin mmplace */
/* $begin mmplace-proto */
static void place(void *bp, size_t asize)
/* $end mmplace-proto */
{
    size_t csize = GET_SIZE(HDRP(bp));   

    if ((csize - asize) >= (DSIZE + OVERHEAD)) { 
	PUT(HDRP(bp), PACK(asize, 1));
	PUT(FTRP(bp), PACK(asize, 1));
	bp = NEXT_BLKP(bp);
	PUT(HDRP(bp), PACK(csize-asize, 0));
	PUT(FTRP(bp), PACK(csize-asize, 0));
#ifdef BEST_FIT
    /* push remaining free block to lists.
     */
    int size = (csize-asize)/DSIZE < BIG_BLOCK ? 
            (csize-asize)/DSIZE : BIG_BLOCK;
    push(bp, size);
#endif
    }
    else { 
	PUT(HDRP(bp), PACK(csize, 1));
	PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit(size_t asize)
{
#ifdef BEST_FIT
    /* check seg_list interative, and if adequate list is found,
     * delete and return it.
     * else, extend heap and return.
     */
    void *temp;
    int i = asize/DSIZE < BIG_BLOCK ? asize/DSIZE : BIG_BLOCK;

    for (; i < BIG_BLOCK && IS_EMPTY(i) ; ++i);
    
    if( IS_EMPTY(i) ) {
        extend_heap(MAX(asize/WSIZE * 2, CHUNKSIZE/WSIZE));
        return pop(i);
    } else {
        temp = _seg_list_top[i];
        while (temp != (void*)TAIL && GET_SIZE(HDRP(temp)) < asize)
            temp = (void*)GET_NEXT(temp);

        if (temp == (void*)TAIL) {
            extend_heap(MAX(asize/WSIZE * 2, CHUNKSIZE/WSIZE + DSIZE * 2));
         
            temp = _seg_list_top[i];                                    
            while (temp != (void*)TAIL && GET_SIZE(HDRP(temp)) < asize)            
                temp = (void*)GET_NEXT(temp);
        }
        delete_block(temp);
        return temp;
    }

#endif
#ifdef NEXT_FIT 
    /* next fit search */
    char *oldrover = rover;

    /* search from the rover to the end of list */
    for ( ; GET_SIZE(HDRP(rover)) > 0; rover = NEXT_BLKP(rover))
	if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
	    return rover;

    /* search from start of list to old rover */
    for (rover = get_heap_listp(); rover < oldrover; rover = NEXT_BLKP(rover))
	if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
	    return rover;

    return NULL;  /* no fit found */
#else 
    /* first fit search */
    void *bp;

    for (bp = get_heap_listp(); GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
	    return bp;
	}
    }
    return NULL; /* no fit */
#endif
}

/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) 
{
    /* coalese free blocks in cases.
     * initially delete block since it's in the list.
     * coalesce free blocks, and push into seg_list.
     */
    
    delete_block(bp);

    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        /* do nothing */
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
	delete_block(NEXT_BLKP(bp));
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size,0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
	delete_block(PREV_BLKP(bp));
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
	delete_block(NEXT_BLKP(bp));
    delete_block(PREV_BLKP(bp));
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
	    GET_SIZE(FTRP(NEXT_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

#ifdef NEXT_FIT
    /* Make sure the rover isn't pointing into the free block */
    /* that we just coalesced */
    if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp))) 
	rover = bp;
#endif

#ifdef BEST_FIT
    /* push into seg lists.
     */
    int index = size < BIG_BLOCK*DSIZE ? size/DSIZE : BIG_BLOCK;
    push(bp, index);
#endif
    return bp;
}


static void printblock(void *bp) 
{
    size_t hsize, halloc, fsize, falloc;

    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));  
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));  
    
    if (hsize == 0) {
	printf("%p: EOL\n", bp);
	return;
    }

    printf("%p: header: [%d:%c] footer: [%d:%c]\n", bp, 
	   hsize, (halloc ? 'a' : 'f'), 
	   fsize, (falloc ? 'a' : 'f')); 
}

static void checkblock(void *bp) 
{
    if ((size_t)bp % 8)
	printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
	printf("Error: header does not match footer\n");
}

/* my code */
/* pop block from the top of list coressponds to index.
 * return ptr.
 */
#ifdef BEST_FIT
static void *pop(int index) {

    void *temp;

    if (index > 11 || IS_EMPTY(index))
        return NULL;

    temp = _seg_list_top[index];

    if (_stack_count[index] == 1) {
        _seg_list_top[index] = (void*)TAIL;
    
    } else {
        _seg_list_top[index] = (void*)GET_NEXT(temp);
        SET_PREV((void*)GET_NEXT(temp), HEAD);
    }
    
    _stack_count[index]--;
    return temp;

}

/* another version of push for size-sorted list.
 * failed to debug since lack of time :(
 * it's respected to give better utilization for malloc lab.
 */
/*
static void push(void *p, int index) {
    
    if (p == NULL || index > 11) {
        return;
    }
    
    void *prev = HEAD, *temp = _seg_list_top[index];
    while (temp != TAIL && GET_SIZE(HDRP(p)) > GET_SIZE(HDRP(temp))) {        
        temp = GET_NEXT(temp);
        if (temp == TAIL)
            break;
    }

    if (temp == TAIL && prev == HEAD) {
        _seg_list_top[index] = p;
        SET_PREV(p, HEAD);
        SET_NEXT(p, TAIL);
    } 

    // When block is the largest 
    else if (temp == TAIL && prev != HEAD) {
        SET_NEXT(prev, p);
        SET_PREV(p, prev);
        SET_NEXT(p, TAIL);
    }

    // When block is the smallest 
    else if (temp != TAIL && prev == HEAD) {
        _seg_list_top[index] = p;
        SET_PREV(p, HEAD);
        SET_NEXT(p, temp);
        SET_PREV(temp, p);
    }

    // When block is in the middel of list 
    else {
        SET_NEXT(prev, p);
        SET_PREV(p, prev);
        SET_NEXT(p, temp);
        SET_PREV(temp, p);
    }
    _stack_count[index]++;

}
*/
/* push block into top of stack.
 */
static void push(void *p, int index) {
    
    if (p == NULL || index > 11) {
        return;
    }
       
    if (_seg_list_top[index] == (void*)TAIL) {
        _seg_list_top[index] = p;
        SET_PREV(p, HEAD);
        SET_NEXT(p, TAIL);
    
    } else {
        void* temp = _seg_list_top[index];
        SET_PREV(temp, (size_t)p);
        SET_NEXT(p, (size_t)temp);
        SET_PREV(p, (size_t)HEAD);
        _seg_list_top[index] = p;
    }
   // printf("push %p\n", p);
    _stack_count[index]++;

}

/* delete block from list, and link blocks that are
 * around pointer p.
 */
static void delete_block(void *p) {

    int index = GET_SIZE(HDRP(p)) / DSIZE;
    index = index < BIG_BLOCK ? index : BIG_BLOCK; 

    /* only one block in teh list */
    if (GET_NEXT(p) == TAIL && GET_PREV(p) == HEAD) {
        _seg_list_top[index] = (void*)TAIL;

    } 
    /* last element in the list */
    else if (GET_NEXT(p) == TAIL && GET_PREV(p) != HEAD) {
        SET_NEXT(GET_PREV(p), TAIL);

    } 
    /* first element in the list */
    else if (GET_NEXT(p) != TAIL && GET_PREV(p) == HEAD) {
        SET_PREV(GET_NEXT(p), HEAD);
        _seg_list_top[index] = (void*)GET_NEXT(p);

    } 
    /* it's in the middle of the list */
    else {
        SET_NEXT(GET_PREV(p), GET_NEXT(p));
        SET_PREV(GET_NEXT(p), GET_PREV(p));
    }

    SET_PREV(p, 0);
    SET_NEXT(p, 0);
    _stack_count[index]--;
}
#endif

