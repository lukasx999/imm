#ifndef _SORT_H
#define _SORT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>



typedef struct {
    const char **strings;
    size_t       strings_len;
    const char **sorted;
    size_t       sorted_len;
} Matches;

extern Matches matches_init(const char **strings, size_t strings_len);
/* Matches `query` against the list of strings in `m->strings`, and puts the results in `m->sorted` */

extern void matches_sort(Matches *m, const char *query, bool case_sensitive);

/* returns NULL when indexed out of bounds */
extern const char *matches_get(const Matches *m, size_t index);

extern void matches_destroy(Matches *m);



#endif // _SORT_H
