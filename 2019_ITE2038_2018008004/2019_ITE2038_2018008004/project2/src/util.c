#include "bpt.h"

header_page_t g_head;
queue *q;

/* gives length of path from root to page
 */
int path_to_root(pagenum_t page_num) {
    int length = 0;
    internal_page_t page;
    file_read_page(page_num, (page_t*)&page);
    while (page_num != g_head.root_page_num) {
        page_num = page.parent;
        length++;
        file_read_page(page_num, (page_t*)&page);
    }
    return length;
}

/* util function for tree traversal
 * [queue]
 */
void enqueue( pagenum_t n ) {
    queue *c, *new_node = (queue*)malloc(sizeof(queue));
    new_node -> page = n;
    if( q == NULL ) {
        q = new_node;
        q -> next = NULL;
    } else {
        c = q;
        while( c -> next != NULL ) {
            c = c -> next;
        }
        c -> next = new_node;
        new_node -> next = NULL;
    }
}

/* util function for tree traversal                                             
 * [queue]                                                                      
 */
pagenum_t dequeue() {
    queue *n = q;
    q = q->next;
    n->next = NULL;
    pagenum_t temp = n -> page;
    free(n);
    return temp;
}

/* Printing tree for easy understanding of tree structure
 * if do_print = 0, dont't print tree
 * if do_print = 1, print tree
 */
void print_tree(int do_print) {

    if( do_print == 0 )
        return;

    pagenum_t n;
    internal_page_t internal, temp, parent;
    leaf_page_t leaf;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (g_head.root_page_num == 0) {
        printf("empty tree\n");
        return;
    }

    q = NULL;
    n = g_head.root_page_num;
    enqueue(n);
    while ( q != NULL ) {
        n = (dequeue());
        file_read_page(n, (page_t*)&internal);

        if (internal.parent != 0 ) {
            file_read_page(internal.parent, (page_t*)&parent);
            if(parent.page_0 == n) 
                new_rank = path_to_root(n);
            if (new_rank != rank) {
                rank = new_rank;
                printf("\n-------------------------\n");
            }
        }

        if (internal.is_leaf) {
            file_read_page(n, (page_t*)&leaf);
            printf("[@%lu/#key=%d]", n, leaf.num_of_keys);
            for( i = 0 ; i < leaf.num_of_keys ; ++i) {
                printf("%lu ", leaf.records[i].key);
            }
        } else {
            printf("[@%lu]=", n);
            n = internal.page_0;
            enqueue(n);
            printf("[page_0:%lu]", n);
            for (i = 0; i < internal.num_of_keys; ++i) {
                printf("[%lu:%lu] ", internal.records[i].key, internal.records[i].page_num);
                n = internal.records[i].page_num;
                enqueue(n);
            }
        }
        printf("| ");
    }
    printf("\n===========================================\n");
}
            
/* First message to the user.
 */
void usage_1( void ) {
	printf("Disk base B+ Tree of Order "
           "[Internal:%d][Leaf:%d].\n", INTERNAL_ORDER + 1, LEAF_ORDER + 1);
    printf("Database Management Systems project2\n\n");
}

/* Second message to the user.
 */
void usage_2( void ) {
	printf("Enter any of the following commands after the prompt > :\n"
	"\ti <k> <value>  -- Insert <k> <value> pair (an integer, string).\n"
	"\tf <k>  -- Find the value under key <k>.\n"
	"\td <k>  -- Delete key <k> and its associated value.\n"
	"\tt -- Print the B+ tree.\n"
    "\tm -- toggle delayed merge. (defult : do delay merge)\n"
    "\tp -- toggle printing tree when insert or delete is succeeded.\n"
	"\tq -- Quit. (Or use Ctl-D.)\n");
}
