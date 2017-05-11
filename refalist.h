/* ArrayList-like data structure.
 * Suitable for adding data where you don't know the maximum size beforehand,
 * so the array needs to grow dynamically.
 * Fast element lookup and iteration.
 * Not suitable for situations where items need to be removed frequently.
 */
#ifndef ALIST_H
#define ALIST_H

typedef struct ArrayList {
    unsigned int n; /* Number of elements in the list */
    unsigned int a; /* Allocated number of elements */
    void **els;       /* Actual elements */
} ArrayList;

#ifdef NDEBUG
ArrayList *al_create();
#else
ArrayList *al_create_(const char *file, int line);
#define al_create() al_create_(__FILE__, __LINE__)
#endif

unsigned int al_size(ArrayList *al);

/* Like al_add, but the list takes ownership
(so it doesn't retain the object internally) */
unsigned int al_add(ArrayList *al, void *p);

#ifdef NDEBUG
unsigned int al_retain(ArrayList *al, void *p);
#else
unsigned int al_retain_(ArrayList *al, void *p, const char *file, int line);
#define al_retain(al, p) al_retain_(al, p, __FILE__, __LINE__)
#endif

void *al_get(ArrayList *al, unsigned int i);

void *al_set(ArrayList *al, unsigned int i, void *p);

void *al_first(ArrayList *al);

void *al_last(ArrayList *al);

void al_sort(ArrayList *al, int (*cmp)(const void*, const void*));

#endif
