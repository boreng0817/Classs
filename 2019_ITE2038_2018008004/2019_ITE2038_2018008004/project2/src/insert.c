#include "bpt.h"

header_page_t g_head;

/* Master insert
 * if key is not in tree, do insert
 * if exists, return -1 for failure sign
 */
int db_insert(my_key_t key, char* value) { 

    pagenum_t insert_page_num;

    insert_page_num = find_leaf_page(key);
    leaf_page_t leaf;
    char temp_val[120];

    if (db_find(key, temp_val) == 0)
        return -1;

    /* if tree is empty, start new tree
     */
    if (insert_page_num == 0) {
        return create_root(key, value);
    }

    file_read_page(insert_page_num, (page_t*)&leaf);

    /* No overflow, just insert
     */
    if (leaf.num_of_keys < LEAF_ORDER) {
        return insert_into_leaf(insert_page_num, key, value);
    }

    /* Overflow occurs. 
     * split node.
     */
    return insert_into_leaf_after_splitting(insert_page_num, key, value);
}

/* Initial insertion. start a new tree.
 */
int create_root(my_key_t key, char* value) { 

    pagenum_t root_num;
    leaf_page_t root;

    root_num = file_alloc_page();

    root.parent = 0;
    root.is_leaf = 1;
    root.num_of_keys = 1;
    insert_record_leaf(&root, key, value, 0);
    root.right_sibling = 0;

    file_write_page(root_num, (page_t*)&root);

    g_head.root_page_num = root_num;
    file_write_page(0, (page_t*)&g_head);

    return 0;
}

/* simply insert into page *[pagenum] a [key]-[value] pair.
 */
int insert_into_leaf(pagenum_t pagenum, my_key_t key, char *value) { 

    int insertion_index, i;
    leaf_page_t leaf;

    file_read_page(pagenum, (page_t*)&leaf);
    
    insertion_index = 0;
    while ( insertion_index < leaf.num_of_keys 
         && leaf.records[insertion_index].key < key )
        insertion_index++;

    for (i = leaf.num_of_keys; i > insertion_index; --i) {
        insert_record_leaf(&leaf, leaf.records[i - 1].key, 
                           leaf.records[i - 1].value, i);
    }

    insert_record_leaf(&leaf, key, value, insertion_index);
    leaf.num_of_keys++;

    file_write_page(pagenum, (page_t*)&leaf);
    return 0;
}

/* Insert a new record, and split the node.
 * Additionally insert middle key-page_num into parent.
 */
int insert_into_leaf_after_splitting(pagenum_t pagenum, my_key_t key, 
                                     char *value) { 

    leaf_page_t new_leaf, leaf;
    int insertion_index, split, i, j;
    my_key_t new_key;
    pagenum_t new_page;
    record *temp_records;

    file_read_page(pagenum, (page_t*)&leaf);
    memset(&new_leaf, 0, PAGE_SIZE);

    temp_records = (record*)malloc(sizeof(record) * (LEAF_ORDER + 1));

    insertion_index = 0;
    while (insertion_index < LEAF_ORDER 
        && leaf.records[insertion_index].key < key) {
        insertion_index++;
    }

    for (i = 0, j = 0; i < leaf.num_of_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_records[j].key = leaf.records[i].key;
        strcpy(temp_records[j].value, leaf.records[i].value);
    }

    temp_records[insertion_index].key = key;
    strcpy(temp_records[insertion_index].value, value);

    leaf.num_of_keys = 0;
    split = cut(LEAF_ORDER);

    
    for (i = 0; i < split; ++i) {
        insert_record_leaf(&leaf, temp_records[i].key,
                           temp_records[i].value, i);
        leaf.num_of_keys++;
    }

    for (i = split, j = 0; i < LEAF_ORDER + 1; ++i, ++j) {
        insert_record_leaf(&new_leaf, temp_records[i].key,
                           temp_records[i].value, j);
        new_leaf.num_of_keys++;
    }

    free(temp_records);

    new_page = file_alloc_page();
    new_key = new_leaf.records[0].key;

    new_leaf.right_sibling = leaf.right_sibling;
    new_leaf.is_leaf = 1;
    new_leaf.parent = leaf.parent;
    
    leaf.right_sibling = new_page;

    file_write_page(pagenum, (page_t*)&leaf);
    file_write_page(new_page, (page_t*)&new_leaf);
    //recursively insert into parent until overflow stops. 
    return insert_into_parent(pagenum, new_key, new_page, new_page);
}

/* Insert a [key]-[value] into parent node.
 */

int insert_into_parent(pagenum_t left, my_key_t key, 
                       pagenum_t value, pagenum_t right) {

    int left_index;
    internal_page_t parent, child;
    pagenum_t parent_num;

    file_read_page(left, (page_t*)&child);

    parent_num = child.parent;

    // Case: insert into new root.
    if (parent_num == 0)
        return insert_into_new_root(left, key, right);

    
    left_index = get_left_index(parent_num, left);
    file_read_page(parent_num, (page_t*)&parent);

    // Case: simply insert into node
    if(parent.num_of_keys < INTERNAL_ORDER) {
        return insert_into_node(parent_num, left_index, key, right);
    }
    
    // Case: harder case. insert and split node.
    return insert_into_node_after_splitting(parent_num, left_index, 
                                            key, right);
}

/* Creates a new root for two child page.
 * Insert the appropriate key into new root
 */
int insert_into_new_root(pagenum_t left, my_key_t key, pagenum_t right) { 
    
    internal_page_t root, lchild, rchild;
    pagenum_t root_pagenum = file_alloc_page();

    root.records[0].key = key;
    root.records[0].page_num = right;
    root.page_0 = left;
    root.num_of_keys = 1;
    root.parent = 0;
    root.is_leaf = 0;

    g_head.root_page_num = root_pagenum;

    file_write_page(root_pagenum, (page_t*)&root);
    file_read_page(left, (page_t*)&lchild);
    file_read_page(right, (page_t*)&rchild);

    lchild.parent = root_pagenum;
    rchild.parent = root_pagenum;

    file_write_page(left, (page_t*)&lchild);
    file_write_page(right, (page_t*)&rchild);
    file_write_page(0, (page_t*)&g_head);
    
    return 0;
}

/* Insert [key]-[right] into node at ideal place
 */
int insert_into_node(pagenum_t internal_num, int left_index, 
                     my_key_t key, pagenum_t right) { 

    int i;
    internal_page_t internal;
    file_read_page(internal_num, (page_t*)&internal);
    
    for (i = internal.num_of_keys ; i > left_index + 1 ; --i) {
    insert_record_internal(&internal, internal.records[i - 1].key,
                           internal.records[i - 1].page_num, i);
    }

    internal.records[left_index + 1].key = key;
    internal.records[left_index + 1].page_num = right;
    internal.num_of_keys++;

    file_write_page(internal_num, (page_t*)&internal); 

    return 0;
}

/* Inserts [key]-[right] into node, and split.
 * do addtional insert into parent.
 */
int insert_into_node_after_splitting(pagenum_t old_num, int left_index,
                                     my_key_t key, pagenum_t right) {

    int i, j, split;
    pagenum_t k_prime, new_page_num, child_num;
    internal_record *temp_records;
    internal_page_t old_page, new_page, child;

    /* Gather all records including new records
     * into temp_records.
     */
    temp_records = (internal_record*)malloc(sizeof(internal_record) \
                                           * (INTERNAL_ORDER + 2));
    memset(&new_page, 0, PAGE_SIZE);

    file_read_page(old_num, (page_t*)&old_page);

    for (i = 0, j = 0; i < old_page.num_of_keys; ++i, ++j) {
        if (j == left_index + 1) j++;
        temp_records[j].key = old_page.records[i].key;
        temp_records[j].page_num = old_page.records[i].page_num;
    }

    temp_records[left_index + 1].key = key;
    temp_records[left_index + 1].page_num = right;

    split = cut(INTERNAL_ORDER + 1);
    
    old_page.num_of_keys = 0;

    for (i = 0; i < split - 1; ++i) {
        insert_record_internal(&old_page, temp_records[i].key, 
                               temp_records[i].page_num, i);
        old_page.num_of_keys++;
    }

    /* Split temp_records into old and new page.
     */

    k_prime = temp_records[i].key; 
    new_page.page_0 = temp_records[i].page_num;

    for (++i, j = 0 ; i < INTERNAL_ORDER + 1; ++i, ++j) {
        insert_record_internal(&new_page, temp_records[i].key,
                               temp_records[i].page_num, j);
        new_page.num_of_keys++;
    }

    new_page.parent = old_page.parent;
    new_page.is_leaf = 0;
    
    new_page_num = file_alloc_page();
    
    file_read_page(new_page.page_0, (page_t*)&child);
    child.parent = new_page_num;
    file_write_page(new_page.page_0, (page_t*)&child);

    for (i = 0; i < new_page.num_of_keys ; ++i) {
        child_num = new_page.records[i].page_num;
        file_read_page(child_num, (page_t*)&child);
        child.parent = new_page_num;
        file_write_page(child_num, (page_t*)&child);
    }

    for (i = old_page.num_of_keys; i < INTERNAL_ORDER ; ++i) {
        old_page.records[i].key = 0;
        old_page.records[i].page_num = 0;
    }

    file_write_page(old_num, (page_t*)&old_page);
    file_write_page(new_page_num, (page_t*)&new_page);

    free(temp_records);

    /* Do additional insert operate in parent.
     */
    return insert_into_parent(old_num, k_prime, new_page_num, new_page_num);
}

/* util function for helping split operation 
 */
int cut(int length) {
    return length % 2 == 0 ? length/2 : length/2 + 1;
}

/* util function for helping insertion into parent
 */
int get_left_index(pagenum_t parent, pagenum_t left) {
    
    internal_page_t parent_page;
    int left_index = 0;

    file_read_page(parent, (page_t*)&parent_page);
   
    if (parent_page.page_0 == left)
        return left_index - 1;

    while (left_index < parent_page.num_of_keys 
        && parent_page.records[left_index].page_num != left)
        left_index++;
    return left_index;
}

/* util function for inserting records into leaf
 */
void insert_record_leaf(leaf_page_t *leaf, my_key_t key, 
                        char *value, int i) {
    
    leaf -> records[i].key = key;
    strcpy(leaf -> records[i].value, value);
}

/* util function for inserting records into internal page
 */
void insert_record_internal(internal_page_t *internal, my_key_t key, 
                            pagenum_t value, int i) {

    internal -> records[i].key = key;
    internal -> records[i].page_num = value;
}
