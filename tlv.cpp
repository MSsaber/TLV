#include <stdio.h>
#include <iostream>
#include <string.h>
#include "tlv.h"

#ifdef MEM_WRITE
#undef MEM_WRITE
#endif

#ifdef MEM_READ
#undef MEM_READ
#endif

#define MEM_WRITE(dest, src, l, m)    \
    do {                              \
            memcpy(dest + m, src, l); \
            m += l;                   \
    } while(0)

#define MEM_READ(dest, src, l, m)     \
    do {                              \
            memcpy(dest, src + m, l); \
            m += l;                   \
    } while(0)

#ifdef __DEBUG__
extern void tlv_debug_log(const char *fmt, ...);
#define TLV_DEBUG(fmt, ...) tlv_debug_log(fmt, ##__VA_ARGS__)
#else
#define TLV_DEBUG(fmt, ...)
#endif

static tlv_t * tlv_get_tail(tlv_t *node);

tlv_t * tlv_create_node(value_manager vm)
{
    tlv_t *node = new tlv_t;
    if (node) {
        memset(node, 0, sizeof(tlv_t));
        node->vm = vm;
        TLV_DEBUG("create node(%p), vm : %p\n", node, vm);
    }
    return node;
}

tlv_t * tlv_create_node(tag_t tag, len_t len,
                        void *value, value_manager vm)
{
    tlv_t *node = new tlv_t;
    if (node) {
        node->vm = vm;
        node->tag = tag;
        node->len = len;
        node->value = value;
        node->next = nullptr;
        TLV_DEBUG("create tag 0x%x, node(%p), vm : %p\n",
                  tag, node, vm);
    }
    return node;
}

void tlv_destory_node(tlv_t *node, bool free_all)
{
    if (!node) return;

    tlv_t *next = node;
    while (next) {
        tlv_t *tmp = next->next;
        if (next->vm) next->vm(next->value);
        TLV_DEBUG("node(%p) delete\n", next);
        delete next;
        if (!free_all) next = nullptr;
        else next = tmp;
    }
}

int tlv_buffer_length(tlv_t *head)
{
    int length = 0;
    tlv_t *next = head;

    while (next) {
        length += sizeof(tag_t) + sizeof(len_t) + next->len;
        next = next->next;
    }

    return length;
}

tlv_t * tlv_add_node2tail(tlv_t *node, tlv_t *new_node)
{
    tlv_t *tail = tlv_get_tail(node);

    if (tail) {
        tail->next = new_node;
        TLV_DEBUG("add success : prev_node(%p), next_node(%p)\n", tail, new_node);
    } else {
        TLV_DEBUG("add node(%p) failed\n", new_node);
    }

    return tail;
}

tlv_t * tlv_add_node2tail(tlv_t *node, tag_t tag, len_t len,
                          void *value, value_manager vm)
{
    tlv_t *new_node = tlv_create_node(tag, len, value, vm);
    tlv_t *tail = nullptr;
    if (new_node) {
        tail = tlv_add_node2tail(node, new_node);
    } else {
        TLV_DEBUG("node alloc failed, add node failed\n");
    }
    return tail;
}

tlv_t * tlv_find_node_by_tag(tlv_t *node, tag_t tag)
{
    tlv_t *next = node;
    while (next) {
        if (next->tag == tag) break;
        next = next->next;
    }
    return next;
}

tlv_t * tlv_parse_data(void *data, int data_len)
{
    int move = 0;
    tlv_t *head = nullptr, *next = nullptr;
    while (move != data_len) {
        tag_t tag;
        len_t len;
        MEM_READ(&tag, (char *)data, sizeof(tag_t), move);
        MEM_READ(&len, (char *)data, sizeof(len_t), move);
        if (len > data_len - move) {
            TLV_DEBUG("bad tlv format, parse failed\n");
            tlv_destory_node(head, true);
            break;
        }

        if (!head) {
            if ((head = tlv_create_node(tag, len, (char *)data + move)))
                next = head;
            else
                goto error;
        } else {
            if (tlv_add_node2tail(next, tag, len, (char *)data + move))
                next = next->next;
            else
                goto error;
        }
        move += len;
    }
    return head;
error:
    TLV_DEBUG("out of memory\n");
    tlv_destory_node(head, true);
    return nullptr;
}

bool tlv_packge_data(tlv_t *head, void *buf, int *buf_len)
{
    int move = 0;
    int len = tlv_buffer_length(head);
    tlv_t *next = head;

    if (*buf_len < len) return false;

    while (next && buf) {
        MEM_WRITE((char *)buf, &next->tag, sizeof(tag_t), move);
        MEM_WRITE((char *)buf, &next->len, sizeof(len_t), move);
        MEM_WRITE((char *)buf, next->value, next->len, move);
        next = next->next;
    }

    *buf_len = move;

    return true;
}

void tlv_traverse(tlv_t *head, tlv_cb func)
{
    tlv_t *next = head, *cur = nullptr;

    if (!func) return;

    while (next) {
        cur = next;
        next = next->next;
        func(cur);
    }
}

static tlv_t * tlv_get_tail(tlv_t *node)
{
    tlv_t *next = node, *cur = nullptr;

    while (next) {
        cur = next;
        next = next->next;
    }

    return cur;
}