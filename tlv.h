/*
 * tlv format operation api
 */

#ifndef X_TLV_H
#define X_TLV_H

#define __DEBUG__
#define SHORT_TYPE

#if defined(CHAR_TYPE)
#define _T char
#elif defined(SHORT_TYPE)
#define _T short
#elif defined(LONG_TYPE)
#define _T long
#endif

typedef _T tag_t;
typedef _T len_t;

struct __tlv;

typedef void(*value_manager) (void *);
typedef void(*tlv_cb)(struct __tlv *);

typedef struct __tlv {
    struct __tlv *next;
    value_manager vm;
    tag_t tag;
    len_t len;
    void *value;
} tlv_t;

tlv_t * tlv_create_node(value_manager vm = NULL);

tlv_t * tlv_create_node(tag_t tag, len_t len, void *value, value_manager vm = NULL);

void tlv_destory_node(tlv_t *node, bool free_all = false);

int tlv_buffer_length(tlv_t *head);

tlv_t * tlv_add_node2tail(tlv_t *node, tlv_t *new_node);

tlv_t * tlv_add_node2tail(tlv_t *node, tag_t tag, len_t len, void *value, value_manager vm = NULL);

tlv_t * tlv_find_node_by_tag(tlv_t *node, tag_t tag);

tlv_t * tlv_parse_data(void *data, int data_len);

void tlv_traverse(tlv_t *head, tlv_cb func);

bool tlv_packge_data(tlv_t *head, void *buf, int *buf_len);

#endif /*X_TLV_H*/