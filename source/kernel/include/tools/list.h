#ifndef LIST_H
#define LIST_H

typedef struct _list_node_t {
    struct _list_node_t * pre;
    struct _list_node_t * next;
}list_node_t;

static inline void list_node_init (list_node_t * node) {
    node->pre = node->next = (list_node_t *)0;
}

static inline list_node_t * list_node_pre(list_node_t * node) {
    return node->pre;
}

static inline list_node_t * list_node_next(list_node_t * node) {
    return node->next;
}

typedef struct _list_t {
    list_node_t * first;
    list_node_t * last;
    int count;
}list_t;

void list_init (list_t * list);

static inline int list_is_empty (list_t * list) {
    return list->count == 0;
}

static inline int list_count (list_t * list) {
    return list->count;
}

static inline list_node_t * list_first (list_t * list) {
    return list->first;
}

static inline list_node_t * list_last (list_t * list) {
    return list->last;
}

#endif