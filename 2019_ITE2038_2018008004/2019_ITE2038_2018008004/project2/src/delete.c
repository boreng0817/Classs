#include "bpt.h"

header_page_t g_head;
int DELAYED_MERGE = 1;

/* Help delete operation.
 * returns node's left_sibling.
 * if node is leftmost, return -2
 */
int get_neigbor_index(pagenum_t page_num) {
    
    internal_page_t parent, node;
    int i;

    file_read_page(page_num, (page_t*)&node);
    file_read_page(node.parent, (page_t*)&parent);

    /* when page is the leftmost child, (page_0)
     * return -2
     */
    if (parent.page_0 == page_num)
        return -2;
    for (i = 0 ; i < parent.num_of_keys ; ++i) {
        if (parent.records[i].page_num == page_num)
            return i - 1;
    }
}

/* delete key, value in page *[page_num] */
void remove_entry_from_node(pagenum_t page_num, my_key_t key) {
    
    int key_index = 0, i;
    leaf_page_t leaf;
    internal_page_t internal;

    file_read_page(page_num, CAST_TO_PAGE(leaf));

    if (leaf.is_leaf == 1) {
        
        for (i = 0; i < leaf.num_of_keys; ++i) {
            if (leaf.records[i].key == key) {
                key_index = i;
                break;
            }
        }

        for (i = key_index; i < leaf.num_of_keys; ++i) {
            insert_record_leaf(&leaf, leaf.records[i+1].key, 
                               leaf.records[i+1].value, i);
        }
        
        leaf.records[i - 1].key = 0;
        memset(leaf.records[i -1].value, 0, VALUE_SIZE);

        leaf.num_of_keys--;

        file_write_page(page_num, CAST_TO_PAGE(leaf));
    } else {
        file_read_page(page_num, CAST_TO_PAGE(internal));

        for (i = 0; i < internal.num_of_keys; ++i) {
            if (internal.records[i].key == key) {
                key_index = i;
                break;
            }
        }

        for (i = key_index; i < internal.num_of_keys; ++i) {
            insert_record_internal(&internal, internal.records[i+1].key, 
                                   internal.records[i+1].page_num, i);
        }

        internal.records[i - 1].key = 0;
        internal.records[i - 1].page_num = 0;

        internal.num_of_keys--;

        file_write_page(page_num, CAST_TO_PAGE(internal));
    }
}

/* handle tree's root case by case */
void adjust_root() {

    internal_page_t root_page, child_page;
    pagenum_t free_num;
    file_read_page(g_head.root_page_num, (page_t*)&root_page);

    // Nonempty root. end delete operation
    if (root_page.num_of_keys > 0)
        return;

    free_num = g_head.root_page_num;

    /* empty root. */

    // If it has a child, promote first child into new root
    // empty root with child has single child

    if (root_page.is_leaf == 0) {
        g_head.root_page_num = root_page.page_0;

        file_read_page(g_head.root_page_num, (page_t*)&child_page);
        child_page.parent = 0; 

        file_write_page(0, (page_t*)&g_head);
        file_write_page(g_head.root_page_num, CAST_TO_PAGE(child_page));
    } 

    // If it is a leaf, tree is empty

    else {
        g_head.root_page_num = 0;
        file_write_page(0, CAST_TO_PAGE(g_head));
    }

    file_free_page(free_num);
}

/* Merge a node *[node_page_num] into neighbor page. *[neighbor_page_num]
 * When sum of #keys from two pages are below capacity
 */

void coalesce_nodes(pagenum_t node_page_num, pagenum_t neighbor_page_num,
                    int neighbor_index, int k_prime) {

    int i, j;
    int insertion_index, n_end;
    pagenum_t temp;
    internal_page_t temp_page, parent;

  
    //Swap node and neighbor when node is leftmost page
    if(neighbor_index == -2) {

        temp = node_page_num;
        node_page_num = neighbor_page_num;
        neighbor_page_num = temp;
    }

    file_read_page(neighbor_page_num, CAST_TO_PAGE(temp_page));

    insertion_index = temp_page.num_of_keys;

    /* Case internal page.
     * append k_prime, page_0 to insertion_index
     * then, move all pairs into neighbor
     */

    if (temp_page.is_leaf == 0) {
        internal_page_t node, neighbor_page, child;
        file_read_page(node_page_num, CAST_TO_PAGE(node));
        file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor_page));

        insert_record_internal(&neighbor_page, k_prime, 
                               node.page_0, insertion_index);
        neighbor_page.num_of_keys++;
        
        n_end = node.num_of_keys;

        for (i = insertion_index + 1, j = 0; j < n_end; ++i, ++j) {
            insert_record_internal(&neighbor_page, node.records[j].key,
                                   node.records[j].page_num, i);
            neighbor_page.num_of_keys++;
            node.num_of_keys--;
        }

        file_read_page(node.page_0, CAST_TO_PAGE(child));
        child.parent = neighbor_page_num;
        file_write_page(node.page_0, CAST_TO_PAGE(child));

        for (i = 0 ; i < neighbor_page.num_of_keys; ++i) {
            file_read_page(neighbor_page.records[i].page_num, 
                           CAST_TO_PAGE(child));
            child.parent = neighbor_page_num;
            file_write_page(neighbor_page.records[i].page_num,                   
                            CAST_TO_PAGE(child));
        }

        file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor_page));

        file_free_page(node_page_num);
    }

    /* Case leaf. appends all pairs from node into neighbor.
     * set neighbor's right_sibling to node's.
     */

    else {
        leaf_page_t node, neighbor_page;
        file_read_page(node_page_num, CAST_TO_PAGE(node));
        file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor_page));

        for (i = insertion_index, j = 0; j < node.num_of_keys; ++i, ++j) {
            insert_record_leaf(&neighbor_page, node.records[j].key,
                               node.records[j].value, i);
            neighbor_page.num_of_keys++;
        }
        neighbor_page.right_sibling = node.right_sibling;

        file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor_page));

        file_free_page(node_page_num);
    }

    delete_entry(temp_page.parent, k_prime);
}

/* handling underflow without merge
 * since page_0 exists in this disign,
 * modify in-memory function little bit 
 * (detail info in WIKI)
 * give a key to *[node_page_num] which had underflow
 */
void redistribute_node(pagenum_t node_page_num, pagenum_t neighbor_page_num,
                       int neighbor_index, int k_prime_index, int k_prime) {
    
    int i, insert_index;
    leaf_page_t temp;

    file_read_page(node_page_num, CAST_TO_PAGE(temp));

    /* CASE: underflow didn't occur in leftmost page;page_0.
     * give neighbor's last key-value pair to n
     */
    if (neighbor_index != -2) {
        if (temp.is_leaf == 0) {

            internal_page_t node, neighbor, parent, child;
            file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor));
            file_read_page(node_page_num, CAST_TO_PAGE(node));

            for (i = node.num_of_keys; i > 0; --i) {
                insert_record_internal(&node, node.records[i - 1].key,
                                       node.records[i - 1].page_num, i);
            }

            insert_record_internal(&node, k_prime, node.page_0, 0);
            node.page_0 = neighbor.records[neighbor.num_of_keys - 1].page_num;

            file_read_page(node.page_0, CAST_TO_PAGE(child));
            child.parent = node_page_num;
            file_write_page(node.page_0, CAST_TO_PAGE(child));

            file_read_page(node.parent, CAST_TO_PAGE(parent));                
            parent.records[k_prime_index].key = \
                            neighbor.records[neighbor.num_of_keys - 1].key;
            file_write_page(node.parent, CAST_TO_PAGE(parent));
            
            neighbor.records[neighbor.num_of_keys - 1].page_num = 0;

            node.num_of_keys++;
            neighbor.num_of_keys--;

            file_write_page(node_page_num, CAST_TO_PAGE(node));
            file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor));
            }

        /* CASE : underflow occured in page_0
         * give neighbor's leftmost key value pair
         */ 

        else {
            leaf_page_t node, neighbor;
            internal_page_t parent;

            file_read_page(node_page_num, CAST_TO_PAGE(node));
            file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor));

            for (i = node.num_of_keys; i > 0 ; --i) {
                insert_record_leaf(&node, node.records[i - 1].key,
                                   node.records[i - 1].value, i);
            }

            insert_index = neighbor.num_of_keys - 1;

            insert_record_leaf(&node, neighbor.records[insert_index].key,
                               neighbor.records[insert_index].value, 0);
            neighbor.records[insert_index].key = 0;
            memset(neighbor.records[insert_index].value, 0, VALUE_SIZE);

            file_read_page(node.parent, CAST_TO_PAGE(parent));
            parent.records[k_prime_index].key = node.records[0].key;
            file_write_page(node.parent, CAST_TO_PAGE(parent));

            node.num_of_keys++;
            neighbor.num_of_keys--;

            file_write_page(node_page_num, CAST_TO_PAGE(node));
            file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor));

        }
    } 

    else {
        if (temp.is_leaf == 1) {
            leaf_page_t node, neighbor;
            internal_page_t parent;
            
            file_read_page(node_page_num, CAST_TO_PAGE(node));                  
            file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor));

            insert_record_leaf(&node, neighbor.records[0].key,
                               neighbor.records[0].value, node.num_of_keys);

            file_read_page(node.parent, CAST_TO_PAGE(parent));
            parent.records[k_prime_index].key = neighbor.records[1].key;
            file_write_page(node.parent, CAST_TO_PAGE(parent));

            for (i = 0; i < neighbor.num_of_keys - 1; ++i) {
                insert_record_leaf(&neighbor, neighbor.records[i + 1].key, 
                                   neighbor.records[i + 1].value, i);
            }

            neighbor.records[i].key = 0;
            memset(neighbor.records[i].value, 0, VALUE_SIZE);

            node.num_of_keys++;
            neighbor.num_of_keys--;

            file_write_page(node_page_num, CAST_TO_PAGE(node));
            file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor));

        }

        else {

            internal_page_t node, neighbor, parent, child;
            file_read_page(node_page_num, CAST_TO_PAGE(node));
            file_read_page(neighbor_page_num, CAST_TO_PAGE(neighbor));

            insert_record_internal(&node, k_prime, 
                                   neighbor.page_0, node.num_of_keys);

            file_read_page(neighbor.page_0, CAST_TO_PAGE(child));
            child.parent = node_page_num;
            file_write_page(neighbor.page_0, CAST_TO_PAGE(child));

            file_read_page(node.parent, CAST_TO_PAGE(parent));
            parent.records[k_prime_index].key = neighbor.records[0].key;
            file_write_page(node.parent, CAST_TO_PAGE(parent));

            neighbor.page_0 = neighbor.records[0].page_num;

            for (i = 0; i < neighbor.num_of_keys - 1; ++i) {
                insert_record_internal(&neighbor, neighbor.records[i + 1].key,
                                       neighbor.records[i + 1].page_num, i);
            }

            neighbor.records[i].key = 0;
            neighbor.records[i].page_num = 0;

            node.num_of_keys++;
            neighbor.num_of_keys--;

            file_write_page(neighbor_page_num, CAST_TO_PAGE(neighbor));
            file_write_page(node_page_num, CAST_TO_PAGE(node));
        }
    }
}

/* delete entry [key] from page [node_page_num]
 * delete in node when callee is master delete
 * or, in internal page
 */
void delete_entry(pagenum_t node_page_num, my_key_t key) {

    internal_page_t node, parent, neighbor;
    int min_keys;
    pagenum_t neighbor_page_num;
    int neighbor_index;
    int k_prime_index, k_prime;
    int capacity;

    remove_entry_from_node(node_page_num, key);

    if (g_head.root_page_num == node_page_num) {
        adjust_root();
        return;
    }

    file_read_page(node_page_num, (page_t*)&node);

    min_keys = node.is_leaf ? cut(LEAF_ORDER) : cut(INTERNAL_ORDER);
    /* delayed merge */
    min_keys = DELAYED_MERGE ? 1 : min_keys;

    if (node.num_of_keys >= min_keys)
        return;

    neighbor_index = get_neigbor_index(node_page_num);

    // since this disk_based BPT has page_0,
    // handle neighbor index & k_prime_index considering page_0
    if (neighbor_index < 0 ) { // -2) node : page_0, neigh : page[0]
        k_prime_index = 0;     // -1) node : page[0], neigh : page_0
    } else {
        k_prime_index = neighbor_index + 1;
    }

    file_read_page(node.parent, (page_t*)&parent);

    k_prime = parent.records[k_prime_index].key;

    if (neighbor_index == -2) {
        neighbor_page_num = parent.records[0].page_num;
    } else if (neighbor_index == -1) {
        neighbor_page_num = parent.page_0;
    } else {
        neighbor_page_num = parent.records[neighbor_index].page_num;
    }

    capacity = node.is_leaf ? LEAF_ORDER + 1 : INTERNAL_ORDER;
    
    file_read_page(neighbor_page_num, (page_t*)&neighbor);
    file_read_page(node_page_num, CAST_TO_PAGE(node));

    // do merge operation
    if (neighbor.num_of_keys + node.num_of_keys < capacity) {
        coalesce_nodes(node_page_num, neighbor_page_num, 
                       neighbor_index, k_prime);
    } 
    
    // get an entry from neighbor page
    else {
        redistribute_node(node_page_num, neighbor_page_num, neighbor_index,
                          k_prime_index, k_prime);
    }

}


/* master delete. delete [key] in disk_based tree
 * do delete operation when key exists
 * if not, return -1 for failure sign
 */

int db_delete(my_key_t key) {

    char val[120];
    pagenum_t delete_page_num;

    if( db_find(key, val) != 0 )
        return -1;

    delete_page_num = find_leaf_page(key);

    delete_entry(delete_page_num, key);

    return 0;
}

/* toggle delayed_merge condtion
 * if 0, do normal merge operation
 * if 1, do delayed_merge operation
 */
int toggle_delayed_merge() {
    DELAYED_MERGE = !DELAYED_MERGE;
    return DELAYED_MERGE;
}

