#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int32_t val, num;
} Number;

void insert(Number a[], int32_t *idx, int32_t v);
int cmp(const void *a, const void *b);
void find_beautiful(int32_t choosed[], Number a[], int32_t idx, int32_t cur, int32_t n, int32_t **ans, int32_t *ans_idx);
bool beautiful(int32_t a, int32_t b);
void print(int32_t **ans, int32_t ans_idx, int32_t n);

int main(void) {
    int32_t n, idx = 0;
    scanf("%d", &n);
    Number a[20] = {0};
    for(int32_t i = 0; i < n; ++i) {
        int32_t v;
        scanf("%d", &v);
        insert(a, &idx, v);
    }
    qsort(a, idx, sizeof(Number), cmp);

    int32_t choosed[20] = {0};
    int32_t *pool = (int32_t*) calloc(200010 * 20, sizeof(int32_t));
    int32_t **ans = (int32_t**) calloc(200010, sizeof(int32_t*));
    for(int i = 0; i < 200010; ++i) {
        ans[i] = pool + i * 20;
    }

    int32_t ans_idx = 0;
    find_beautiful(choosed, a, idx, 0, n, ans, &ans_idx);
    print(ans, ans_idx, n);
}

void insert(Number a[], int32_t *idx, int32_t v) {
    for(int32_t i = 0; i < *idx; ++i) {
        if(a[i].val == v) {
            a[i].num++;
            return;
        }
    }
    a[*idx].val = v;
    a[*idx].num = 1;
    (*idx)++;
}

int cmp(const void *a, const void *b) {
    Number *na = (Number *) a;
    Number *nb = (Number *) b;
    return na->val - nb->val;
}

void find_beautiful(int32_t choosed[], Number a[], int32_t idx, int32_t cur, int32_t n, int32_t **ans, int32_t *ans_idx) {
    if(cur == n) {
        for(int i = 0; i < n; ++i)
            ans[*ans_idx][i] = choosed[i];
        (*ans_idx)++;
        return;
    }

    for(int i = 0; i < idx; ++i) {
        if(a[i].num > 0 && (cur < 2 || beautiful(a[i].val - choosed[cur - 1], choosed[cur - 1] - choosed[cur - 2]))) {
            a[i].num--;
            choosed[cur] = a[i].val;
            find_beautiful(choosed, a, idx, cur + 1, n, ans, ans_idx);
            a[i].num++;
        }
    }
}

bool beautiful(int32_t a, int32_t b) {
    if(a == 0 || b == 0) return false;
    return (a > 0 && b < 0) || (a < 0 && b > 0);
}

void print(int32_t **ans, int32_t ans_idx, int32_t n) {
    printf("%d\n", ans_idx);
    for(int32_t i = 0; i < ans_idx; ++i) {
        for(int32_t j = 0; j < n; ++j) {
            printf("%d ", ans[i][j]);
        }
        printf("\n");
    }
}