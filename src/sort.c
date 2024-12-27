#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>

#include "./sort.h"



static size_t min(size_t a, size_t b, size_t c) {
    return (a < b) ? a : ((b < c) ? b : c);
}

static size_t lev(const char *a, const char *b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);

    return !len_b ? len_a
    : !len_a ? len_b
    : a[0] == b[0] ? lev(a+1, b+1)
    : 1 + min(lev(a+1, b), lev(a, b+1), lev(a+1, b+1));

}

static int comp(const void *a, const void *b, void *q) {
    const char *x     = *(const char **) a;
    const char *y     = *(const char **) b;
    const char *query = (const char *) q;
    return lev(query, x) - lev(query, y);
}

void fuzzy_sort(const char *query, const char **strings, size_t strings_count) {
    qsort_r(strings, strings_count, sizeof(char*), comp, (void *) query);
}
