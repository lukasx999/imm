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



#if 1

void fuzzy_sort(const char *query, const char **strings, size_t strings_count) {
    qsort_r(strings, strings_count, sizeof(char*), comp, (void *) query);
}

#else

typedef struct {
    const char *query;
    const char *string;
    int result_dist;
} ThreadArgs;


static void *threaded_lev(void *args_) {
    ThreadArgs *args = args_;
    args->result_dist = lev(args->query, args->string);
    return NULL;
}


void fuzzy_sort(const char *query, const char **strings, size_t strings_count) {

    pthread_t thread_ids[strings_count];
    ThreadArgs args[strings_count];

    for (size_t i=0; i < strings_count; ++i) {
        args[i].query       = query;
        args[i].string      = strings[i];
        args[i].result_dist = 0;
        pthread_create(&thread_ids[i], NULL, threaded_lev, &args[i]);
    }

    for (size_t i=0; i < strings_count; ++i) {
        pthread_join(thread_ids[i], NULL);
        // int dist = ret->result_dist;
    }

}
#endif
