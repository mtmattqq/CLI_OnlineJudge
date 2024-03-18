#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define MAX_LAYERS 64

enum T {
    SLOW_GET = 1, FAST_GET, INSERT, REMOVE
};

typedef struct node {
    int64_t val;
    struct node *next, *prev, *below;
} Node;

typedef struct skiplist {
    Node **head;
    int64_t fast_layers;
} SkipList;

typedef struct fast_get_ret {
    Node *get_ret;
    Node **prev;
} FastGetRet;

SkipList NewList();
void DeleteList(SkipList *L);

bool CoinFlip(int64_t k, int64_t i);

Node* SlowGet(SkipList *L, int64_t data);
FastGetRet FastGet(SkipList *L, int64_t data, bool print);
Node* InsertNode(Node *pre, Node *below, int64_t data);
void Insert(SkipList *L, int64_t data);
void Remove(SkipList *L, int64_t data);

int main(void) {
    int64_t m;
    scanf("%ld", &m);
    SkipList L = NewList();

    for(int64_t i = 0; i < m; ++i) {
        int64_t t, k;
        scanf("%ld%ld", &t, &k);
        switch (t) {
        case SLOW_GET:
            SlowGet(&L, k);
            break;
        case FAST_GET:
            free(FastGet(&L, k, true).prev);
            break;
        case INSERT:
            Insert(&L, k);
            break;
        case REMOVE:
            Remove(&L, k);
            break;
        }
    }

    DeleteList(&L);
}

SkipList NewList() {
    SkipList ret;
    ret.head = (Node**) calloc(MAX_LAYERS * 16, sizeof(Node*));
    for(int i = 0; i < MAX_LAYERS; ++i) {
        ret.head[i] = (Node*) calloc(1, sizeof(Node));
        ret.head[i]->val = INT64_MAX;
        ret.head[i]->below = (i == 0) ? NULL : ret.head[i - 1];
        ret.head[i]->prev = ret.head[i];
    }
    ret.fast_layers = 0;
    return ret;
}

void DeleteList(SkipList *L) {
    free(L->head);
}

bool CoinFlip(int64_t k, int64_t i) {
    if(i == 0) return 0;
    return (k >> (i - 1)) & 1L;
}

Node* SlowGet(SkipList *L, int64_t data) {
    bool has_before = false;
    Node *node = L->head[0]->next;
    while(node != NULL && data < node->val) {
        printf("%ld ", node->val);
        has_before = true;
        node = node->next;
    }
    if(node != NULL && node->val == data) {
        printf("%ld\n", node->val);
        return node;
    } else {
        if(!has_before) printf("-1");
        printf("\n");
        return NULL;
    }
}

FastGetRet FastGet(SkipList *L, int64_t data, bool print) {
    bool has_before = false;
    Node *node = L->head[L->fast_layers];
    Node **previous = (Node**) calloc(MAX_LAYERS, sizeof(Node*));
    for(int i = 0; i < MAX_LAYERS; ++i)
        previous[i] = L->head[i];

    int64_t cur_layer = L->fast_layers;

    while(node->below != NULL) {
        previous[cur_layer] = node->prev;
        if(print && node->val != INT64_MAX) printf("%ld ", node->val);
        while(node->next != NULL && data <= node->next->val) {
            node = node->next;
            has_before = true;
            if(print) printf("%ld ", node->val);
        }
        if(data < node->val) previous[cur_layer] = node;
        else previous[cur_layer] = node->prev;

        cur_layer--;
        node = node->below;
    }

    previous[cur_layer] = node->prev;
    while(node != NULL && data < node->val) {
        if(print && node->val != INT64_MAX) {
            printf("%ld ", node->val);
            has_before = true;
        }
        previous[cur_layer] = node;
        node = node->next;
    }

    if(node != NULL && node->val == data) {
        if(print) printf("%ld\n", node->val);
        FastGetRet ret = {node, previous};
        return ret;
    } else {
        if(print) {
            if(!has_before) printf("-1");
            printf("\n");
        }
        FastGetRet ret = {NULL, previous};
        return ret;
    }
}

Node* InsertNode(Node *pre, Node *below, int64_t data) {
    Node *new_node = (Node*) calloc(1, sizeof(Node));
    new_node->val = data;
    assert(pre != NULL);
    Node *next = pre->next;
    pre->next = new_node;
    new_node->prev = pre;
    new_node->next = next;
    if(next != NULL) next->prev = new_node;
    new_node->below = below;
    return new_node;
}

void Insert(SkipList *L, int64_t data) {
    FastGetRet fast_get_ret = FastGet(L, data, false);
    Node *below = InsertNode(fast_get_ret.prev[0], NULL, data);
    for(int i = 1; CoinFlip(data, i); ++i) {
        below = InsertNode(fast_get_ret.prev[i], below, data);
        if(i > L->fast_layers) (L->fast_layers) = i;
    }
    free(fast_get_ret.prev);
}

void RemoveNode(Node *pre) {
    assert(pre != NULL);
    if(pre->next == NULL) return;

    Node *next = pre->next;
    pre->next = pre->next->next;
    if(pre->next != NULL) {
        pre->next->prev = pre;
    }
    free(next);
}

void Remove(SkipList *L, int64_t data) {
    FastGetRet fast_get_ret = FastGet(L, data, false);
    if(fast_get_ret.get_ret == NULL) return;
    RemoveNode(fast_get_ret.prev[0]);
    for(int i = 1; CoinFlip(data, i); ++i) {
        RemoveNode(fast_get_ret.prev[i]);
        if(L->head[i]->next == NULL) (L->fast_layers)--;
    }
    free(fast_get_ret.prev);
}