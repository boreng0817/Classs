#include "join.h"

//buffer g_buf;

int _join_table(int table_id_1, int table_id_2, char *pathname) {


    /*    check tb1, tb2 is oppend 
     * && check pathname is available for usage.
     */

    leaf_page_t *leaf_a, *leaf_b;
    int fd, a = 0, b = 0;
    FILE* fptr;

    if (g_buf -> table_ids[table_id_1] == BUF_EMPTY_TABLE ||
        g_buf -> table_ids[table_id_2] == BUF_EMPTY_TABLE)
        return -1; // JOIN FAILURE
    
    fd = open(pathname, O_RDWR);

    if (fd > 0)
        return -1; // File is opened already.
    else {
        fptr = fopen(pathname, "w");
    }

    /* define CAST_FRAME_TO_PAGE */
    leaf_a = join_get_first_leaf(table_id_1);
    leaf_b = join_get_first_leaf(table_id_2);

    while (leaf_a != NULL && leaf_b != NULL) {
        while (a < leaf_a -> num_of_keys &&
                leaf_a -> records[a].key < leaf_b -> records[b].key)
            a++;

        while (b < leaf_b -> num_of_keys &&
                leaf_a -> records[a].key > leaf_b -> records[b].key)
            b++;

        if (a == leaf_a -> num_of_keys && b == leaf_b -> num_of_keys) {
            leaf_a = join_get_right_sibling(table_id_1, leaf_a);
            leaf_b = join_get_right_sibling(table_id_2, leaf_b);
            a = b = 0;
            continue; 
        } else if (a == leaf_a -> num_of_keys) {
            leaf_a = join_get_right_sibling(table_id_1, leaf_a);
            a = 0;
            continue;
        } else if (b == leaf_b -> num_of_keys) {
            leaf_b = join_get_right_sibling(table_id_2, leaf_b);
            b = 0;
            continue;
        } else {
            // A) append to output buffer -> best implementation
        /*
            char output[260] = '\0';
            join_key_append(output, leaf_a -> records[a].key);
            strcat(output, leaf_a -> records[a].value);
            strcat(output, ",");
            join_key_append(output, leaf_b -> records[b].key);
            strcat(output, leaf_b -> records[b].value);
        */
            fprintf(fptr, "%ld,%s,%ld,%s\n", leaf_a -> records[a].key,
                                            leaf_a -> records[a].value,
                                            leaf_b -> records[b].key,
                                            leaf_b -> records[b].value);
            a++;
            b++;
        }
    }

    fclose(fptr);
    return 0;
}

leaf_page_t *join_get_first_leaf(int table_id) {
    /* TO-DO */
    /* use util_print_leaves */
    leaf_page_t *leaf;
    internal_page_t *internal;
    header_page_t *head;

    head = CAST_FRAME_TO_HEAD(buf_get_frame(table_id, 0));
    if (head -> root_page_num == 0)
        return NULL;
    internal = CAST_FRAME_TO_INTERNAL(buf_get_frame(
                                        table_id, head -> root_page_num));

    while (internal -> is_leaf == 0)
        internal = CAST_FRAME_TO_INTERNAL(buf_get_frame(
                                            table_id, internal -> page_0));
    return (leaf_page_t*)internal;
}

leaf_page_t *join_get_right_sibling(int table_id, leaf_page_t *leaf) {

    if (leaf -> right_sibling == 0)
        return NULL;
    else
        return CAST_FRAME_TO_LEAF(buf_get_frame(
                    table_id, leaf -> right_sibling));
}
