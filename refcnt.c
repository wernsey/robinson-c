/*
Reference counter for C. 
Based on this article:
http://www.xs-labs.com/en/archives/articles/c-reference-counting/
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "refcnt.h"

#ifndef NDEBUG

/* to guard against buffer overruns */
#define SENTINEL    0xDEADBEEF

typedef struct history_list {
	const char *desc;
	const char *file;
	int line;
	struct history_list *next;
} HistoryList;
#endif

typedef struct refobj {
	unsigned int refcnt;
	ref_dtor dtor;
#ifndef NDEBUG
	int is_str;
	HistoryList *list;
	struct refobj *next, *prev;
    size_t size;
#endif
} RefObj;

#ifndef NDEBUG
static void add_list_item(RefObj *r, const char *file, int line, const char *desc) {
	HistoryList *hl = malloc(sizeof *hl), *it;
	hl->next = NULL;
	if(r->list) {
		for(it = r->list; it->next; it = it->next);
		it->next = hl;
	} else {
		r->list = hl;
	}
	hl->file = file;
	hl->line = line;
	hl->desc = desc;
}

static void free_list(HistoryList *hl) {
	if(hl->next) 
		free_list(hl->next);
	free(hl);
}

static int rc_alloc_count = 0;
static int rc_free_count = 0;
static RefObj *rc_root = NULL;

static size_t alloced = 0, max_alloced = 0;

static void exit_fun() {
	RefObj *r;
	/* a = Allocated objects, d = deallocated objects, g = uncollected garbage */
	printf("** Ref Counts..: a:%d d:%d g:%d\n", rc_alloc_count, rc_free_count, rc_alloc_count - rc_free_count);
	printf("** Mem Usage...: max:%u cur:%u\n", max_alloced, alloced);
	for (r = rc_root; r; r = r->next) {
		HistoryList *it;
		if(r->is_str) {
			printf("** - String '%s' with %u references\n", (char*)r + sizeof *r, r->refcnt);
		} else {
			printf("** - Object %p with %u references\n", (char*)r + sizeof *r, r->refcnt);
		}
		for(it = r->list; it; it = it->next) {
			printf("      - %s @ %s:%d\n", it->desc, it->file, it->line);
		}
	}
    
}
#endif

void rc_init() {
#ifndef NDEBUG
	atexit(exit_fun);
#endif
}

#ifdef NDEBUG
void *rc_alloc(size_t size) {
#else
void *rc_alloc_(size_t size, const char *file, int line) {
#endif
	void *data;    

#ifndef NDEBUG    
	RefObj *r = malloc((sizeof *r) + size + sizeof(int));
#else
    RefObj *r = malloc((sizeof *r) + size);
#endif
	if(!r) 
		return NULL;
	data = (char*)r + sizeof *r;
	r->refcnt = 1;
	r->dtor = NULL;
#ifndef NDEBUG
    
    *(int *)((char *)data + size) = SENTINEL;
    
	r->is_str = 0;
	r->list = NULL;
    r->size = size;
    alloced += size;
    if(alloced > max_alloced)
        max_alloced = alloced;
	add_list_item(r, file, line, "rc_alloc");
	r->next = rc_root;
	rc_root = r;
	if(r->next) r->next->prev = r;
	r->prev = NULL;	
	rc_alloc_count++;
#endif	
	return data;
}

#ifdef NDEBUG
void *rc_realloc(void *p, size_t size) {
#else
void *rc_realloc_(void *p, size_t size, const char *file, int line) {
#endif
	RefObj *r;
	if(!p) 
		return NULL;
	r = (RefObj *)((char *)p - sizeof *r);
	assert(r->refcnt == 1);
#ifdef NDEBUG    	
    r = realloc(r, (sizeof *r) + size);
#else
    r = realloc(r, (sizeof *r) + size + sizeof(int));
    char *data = (char*)r + sizeof *r;
    *(int *)((char *)data + size) = SENTINEL;

    alloced = alloced - r->size + size;    
    if(alloced > max_alloced)
        max_alloced = alloced;
    r->size = size;
	if(r->prev)
		r->prev->next = r;
	else
		rc_root = r;
	if(r->next)
		r->next->prev = r;
	add_list_item(r, file, line, "rc_realloc");
#endif
	return (char*)r + sizeof *r;
}

#ifdef NDEBUG
char *rc_strdup(const char *s) {
#else
void *rc_strdup_(const char *s, const char *file, int line) {
	RefObj *r;
#endif
	size_t len = strlen(s);
	char *n = rc_alloc(len + 1);
	if(!n) 
		return NULL;
	memcpy(n, s, len + 1);
#ifndef NDEBUG
	r = (RefObj *)((char *)n - sizeof *r);
	assert(r->list);
	r->list->file = file;
	r->list->line = line;
	r->list->desc = "rc_strdup";
	r->is_str = 1;
#endif	
	return n;
}


#ifdef NDEBUG
void *rc_memdup(const void *p, size_t size) {
#else
void *rc_memdup_(const void *p, size_t size, const char *file, int line) {
	RefObj *r;
#endif
	char *n = rc_alloc(size);
	if(!n) 
		return NULL;
	memcpy(n, p, size);
#ifndef NDEBUG
	r = (RefObj *)((char *)n - sizeof *r);
	assert(r->list);
	r->list->file = file;
	r->list->line = line;
	r->list->desc = "rc_memdup";
	r->is_str = 0;
#endif	
	return n;
}


#ifdef NDEBUG
void *rc_retain(void *p) {
#else
void *rc_retain_(void *p, const char *file, int line) {
#endif
	RefObj *r;
	if(!p) 
		return NULL;
	r = (RefObj *)((char *)p - sizeof *r);
	r->refcnt++;
#ifndef NDEBUG
	add_list_item(r, file, line, "rc_retain");
#endif
	return p;
}

#ifdef NDEBUG
void rc_release(void *p) {
#else
void rc_release_(void *p, const char *file, int line) {
#endif
	RefObj *r;
	if(!p) 
		return;
	r = (RefObj *)((char *)p - sizeof *r);
	r->refcnt--;
	if(r->refcnt == 0) {
#ifndef NDEBUG
        
        char *data = (char*)r + sizeof *r;
        int sentinel = *(int *)(data + r->size);        
        if(sentinel != SENTINEL) {
            assert(r->list);        
            fprintf(stderr, "** Buffer overrun on object allocated at %s:%d\n", r->list->file, r->list->line);
            fflush(stderr);
        }
        
        assert(alloced >= r->size);
        alloced -= r->size;
		rc_free_count++;
		if(rc_root == r) rc_root = r->next; 
		if(r->next) r->next->prev = r->prev;
		if(r->prev) r->prev->next = r->next;
		free_list(r->list);
#endif
		if(r->dtor != NULL) {
			r->dtor(p);
		}
		free(r);
	}
#ifndef NDEBUG
	else {
		add_list_item(r, file, line, "rc_release");
	}
#endif
}

void rc_set_dtor(void *p, ref_dtor dtor) {
	RefObj *r;
	if(!p) return;
	r = (RefObj *)((char *)p - sizeof *r);
	r->dtor = dtor;
}

#ifdef NDEBUG
void *rc_assign(void **p, void *val) {
#else
void *rc_assign_(void **p, void *val, const char *file, int line) {
#endif
    if(*p) {
#ifdef NDEBUG
        rc_release(*p);
#else
        rc_release_(*p, file, line);
#endif
    }
    *p = val;
    return val;
}
