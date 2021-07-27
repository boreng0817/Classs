#include "buf.h"

queue *q;

/* checker for leaves of table, which are 
 * actual records. For join project.
 */
void util_print_leaves(int table_id) {

    header_page_t head;
    leaf_page_t leaf;
    internal_page_t internal;
    int i;

    buf_read_page(table_id, 0, CAST_TO_PAGE(head));
    if (head.root_page_num == 0)
        return;

    buf_read_page(table_id, head.root_page_num, CAST_TO_PAGE(internal));
    i = head.root_page_num;

    while (!internal.is_leaf) {
        i = internal.page_0;
        buf_read_page(table_id, internal.page_0, CAST_TO_PAGE(internal));
    }

    buf_read_page(table_id, i, CAST_TO_PAGE(leaf));

    while (1) {
        for (i = 0; i < leaf.num_of_keys; ++i) 
            printf("[%-8lu][%-20s]\n", leaf.records[i].key, 
                                      leaf.records[i].value);
        if (leaf.right_sibling == 0)
            break;
        else
            buf_read_page(table_id, leaf.right_sibling, CAST_TO_PAGE(leaf));
    }

}

/* checker for frames in buffer.
 * helper function for debugging.
 */
void util_print_frames(buffer buf) {
    int i;                                                                      

    printf("util_print_frames===#element : %d\n", buf -> num_used_frame);

    for (i = 0; i < buf -> capacity; ++i) {                
        frame temp = BUF_GET_FRAME(buf, i);
        if (i%5 == 0)                                                           
            printf("\n");                                                       
        printf("[%-5d  %5u /%-5lu]", i, temp.table_id, temp.page_num);
    }                                                                           
}

/* checker for hash_table.
 * helper function for debugging.
 */
void util_print_hash_table(hash h) {
    int i;

    printf("util_print_hash_table===#element : %d\n", h -> num_element);

    for (i = 0; i < h -> capacity; ++i) {
        if (i%5 == 0)
            printf("\n");
        printf("[%-5d  %-10d]", i, h -> arr[i]);
    }
}

/* gives length of path from root to page
*/
int path_to_root(int table_num, pagenum_t page_num) {
    int length = 0;
    internal_page_t page;
    header_page_t head;
    buf_read_page(table_num, page_num, (page_t*)&page);
    buf_read_page(table_num, 0, CAST_TO_PAGE(head));
    while (page_num != head.root_page_num) {
        page_num = page.parent;
        length++;
        buf_read_page(table_num, page_num, (page_t*)&page);
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
void print_tree(int table_num, int do_print) {

    if( do_print == 0 )
        return;

    header_page_t head;
    pagenum_t n;
    internal_page_t internal, temp, parent;
    leaf_page_t leaf;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    buf_read_page(table_num, 0, CAST_TO_PAGE(head));

    if (head.root_page_num == 0) {
        printf("empty tree\n");
        return;
    }

    q = NULL;
    n = head.root_page_num;
    enqueue(n);
    while ( q != NULL ) {
        n = (dequeue());
        buf_read_page(table_num, n, (page_t*)&internal);

        if (internal.parent != 0 ) {
            buf_read_page(table_num, internal.parent, (page_t*)&parent);
            if(parent.page_0 == n) 
                new_rank = path_to_root(table_num, n);
            if (new_rank != rank) {
                rank = new_rank;
                printf("\n-------------------------\n");
            }
        }

        if (internal.is_leaf) {
            buf_read_page(table_num, n, (page_t*)&leaf);
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
    printf("Database Management Systems project3\n\n");
}

/* Second message to the user.
*/
void usage_2( void ) {
    printf("Enter any of the following commands after the prompt > :\n"
            "\ti <t> <k> <value>  -- Insert <k> <value> pair to <t> table (an integer, string).\n"
            "\tf <t> <k>  -- Find the value under key <k> in <t> table.\n"
            "\td <t> <k>  -- Delete key <k> and its associated value in <t> table.\n"
            "\tt <t> -- Print the table having table id : <t>.\n"
            "\tm -- toggle delayed merge. (defult : do delay merge)\n"
            "\tp -- toggle printing tree when insert or delete is succeeded.\n"
            "\to <string> -- Open table <string>.\n"
            "\tc <t> -- Close table with unique table id : <t>.\n"
            "\tj -- Join operation for two (or self) file.\n"
            "\tq -- Quit. (Or use Ctl-D.)\n");
}
