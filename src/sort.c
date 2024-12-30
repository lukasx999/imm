#include <ctype.h>
#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#include "./sort.h"



Matches matches_init(const char **strings, size_t strings_len) {
    const char **sorted = calloc(strings_len, sizeof(const char*));
    return (Matches) {
        .strings     = strings,
        .strings_len = strings_len,
        .sorted      = sorted,
        .sorted_len  = 0,
    };
}


void matches_sort(Matches *m, const char *query, bool case_sensitive) {
    memset(m->sorted, 0, m->strings_len);
    size_t sorted_len = 0;

    for (size_t i=0; i < m->strings_len; ++i) {
        const char *s = m->strings[i];

        bool does_match = case_sensitive
            ? !strncmp(s, query, strlen(query))
            : !strncasecmp(s, query, strlen(query));

        if (does_match)
            m->sorted[sorted_len++] = s;

    }
    m->sorted_len = sorted_len;
}

void matches_destroy(Matches *m) {
    free(m->sorted);
    m->sorted = NULL;
}

const char *matches_get(const Matches *m, size_t index) {
    return index >= m->sorted_len
    ? NULL
    : m->sorted[index];
}
