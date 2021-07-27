#include "buf.h"

#ifndef __JOIN_H__
#define __JOIN_H__
#define CAST_FRAME_TO_LEAF(x) (leaf_page_t*)&((x) -> page)
#define CAST_FRAME_TO_INTERNAL(x) (internal_page_t*)&((x) -> page)
#define CAST_FRAME_TO_HEAD(x) (header_page_t*)&((x) -> page)

int _join_table(int table_id_1, int table_id_2, char *pathname);
leaf_page_t *join_get_first_leaf(int table_id);
leaf_page_t *join_get_right_sibling(int table_id, leaf_page_t *leaf);

#endif /* __JOIN_H__ */
