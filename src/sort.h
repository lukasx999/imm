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
extern void    matches_sort(Matches *m, const char *query);
extern void    matches_destroy(Matches *m);
extern const char *matches_get(const Matches *m, size_t index); // returns NULL on out of bounds indexing



#endif // _SORT_H
