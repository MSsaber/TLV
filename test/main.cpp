#include <stdio.h>
#include <stdarg.h>

#include "tlv.h"

#define LIST_SIZE 10

void tlv_debug_log(const char *fmt, ...)
{
    int n;
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    n = vsprintf(buf, fmt, ap);
    printf("%s", buf);
    va_end(ap);
}

static void hexdump(const char *buf, int len)
{
    if (!buf || !len) {
        printf("invalid buf\n");
        return;
    }

    for (int i = 0; i < len; i++) {
        printf("0x%02x", buf[i]);
        if (i != 0 && (i + 1) % 8 == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    printf("\n");
}

static void free_int(void *ptr)
{
    int *p = (int *)ptr;
    printf("free %p\n", p);
    delete p;
}

static void print_node(tlv_t *node)
{
    printf("tag %d len %d\n", node->tag, node->len);
    hexdump((const char *)node->value, node->len);
}

static tlv_t * generate_test_list(void)
{
    tlv_t *head = NULL, *next = NULL;
    for (int i = 0; i < LIST_SIZE; i++) {
        if (i == 0) {
            head = tlv_create_node(free_int);
            head->tag = i;
            head->len = sizeof(int);
            head->value = (void *)new int(i);
            next = head;
        } else {
            next = tlv_add_node2tail(next, i, sizeof(i), new int(i), free_int);
            next = next->next;
        }
    }

    return head;
}

int main(void)
{
    char *buf = NULL;
    tlv_t *head = generate_test_list();
    tlv_t *list2 = NULL;

    int len = tlv_buffer_length(head);
    buf = new char[len];
    if (buf && tlv_packge_data(head, buf, &len)) {
        hexdump(buf, len);
    }
    tlv_destory_node(head, true);

    list2 = tlv_parse_data(buf, len);
    tlv_traverse(list2, print_node);
    tlv_destory_node(list2, true);
    delete buf;
    return 0;
}