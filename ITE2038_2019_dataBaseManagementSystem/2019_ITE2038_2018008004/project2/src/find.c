#include "bpt.h"

header_page_t g_head;

/* function for finding leaf page that could contain key.
 * utilized in various functions (delete, insert, find)
 */
pagenum_t find_leaf_page(my_key_t key) {
    int i;
    pagenum_t root_page, next_page;
    internal_page_t internal_page;

    root_page = next_page = g_head.root_page_num;

    if (root_page == 0) {
        return 0; //need to start_new_root
    } else {
        file_read_page(root_page, (page_t*)&internal_page);
    }

    while (internal_page.is_leaf == 0) {

        i = 0;
        while (i < internal_page.num_of_keys) {
            if( key >= internal_page.records[i].key )  ++i;
            else break;
        }

        if (i == 0) {
            next_page = internal_page.page_0;
        } else {
            next_page = internal_page.records[i - 1].page_num;
        } 

        file_read_page(next_page, (page_t*)&internal_page);
    }


    return next_page;
}

/* master find opertion.
 * if key is found, fill value into ret_val
 * return 0 when success, -1 when failed.
 */

int db_find(my_key_t key, char* ret_val) { 
    int i;
    pagenum_t ret_page, leaf_page_num;
    leaf_page_t leaf_page;

    leaf_page_num = find_leaf_page(key);

    if (leaf_page_num == 0) {
        /* not found */
        return -1;
    } else {
        file_read_page(leaf_page_num, (page_t*)&leaf_page);

        for (i = 0; i < leaf_page.num_of_keys; ++i) {
            if (leaf_page.records[i].key == key) break;
        }
        
        if (i == leaf_page.num_of_keys) {
            return -1;
        } else {
            strcpy(ret_val, leaf_page.records[i].value);
            return 0;
        }
    }
    return ret_page;
}
