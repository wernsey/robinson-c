/*1 refcnt.h
 *# Reference counter for C.\n
 *{
 ** Objects are allocated with {{rc_alloc()}} which returns a reference counted
 *# block of memory. The initial reference count is set to one.
 ** Use {{rc_set_dtor()}} to register a destructor function on an object that
 *# gets called when the object is reclaimed. This destructor can be used to
 *# release references to other objects which the object being released may hold.
 ** {{rc_release()}} decrements an object's reference count. If it becomes 0
 *# the object's destructor is called and the object is free()'d. 
 ** Use {{rc_retain()}} to increment an object's reference count.
 *}
 *2 Debug mode
 *# The reference counter contains some additional features to track where memory 
 *# is allocated, retained and released if compiled with NDEBUG not defined.\n 
 *# This can help to troubleshoot potential memory leaks where not all objects are released.\n
 *# It adds some overhead, so it is disabled if {{NDEBUG}} is defined for release
 *# mode builds.\n
 *# Call {{rc_init()}} at the start of your program to register a function with
 *# {{atexit()}} that will print all unreleased objects and their usages to 
 *# {{stdout}} when the program terminates.\n
 *2 References
 *{
 ** http://www.xs-labs.com/en/archives/articles/c-reference-counting/
 *}
 *2 License
 *[
 *# Author: Werner Stoop
 *# This is free and unencumbered software released into the public domain.
 *# http://unlicense.org/
 *]
 *2 API
*/

/*@ void *rc_alloc(size_t size)
 *# Allocates a reference counted object of {{size}} bytes.
 *# The object's reference count is initialised to 1, so every call to
 *# {{rc_alloc()}} should have a corresponding call to {{rc_release()}}
 */
#ifdef NDEBUG
void *rc_alloc(size_t size);
#else
void *rc_alloc_(size_t size, const char *file, int line);
#define rc_alloc(size) rc_alloc_(size, __FILE__, __LINE__)
#endif

/*@ char *rc_strdup(const char *s)
 *# 
 */
#ifdef NDEBUG
char *rc_strdup(const char *s);
#else
void *rc_strdup_(const char *s, const char *file, int line);
#define rc_strdup(s) rc_strdup_(s, __FILE__, __LINE__)
#endif

/*@ char *rc_memdup(const char *s)
 *# 
 */
#ifdef NDEBUG
void *rc_memdup(const void *p, size_t size);
#else
void *rc_memdup_(const void *p, size_t size, const char *file, int line);
#define rc_memdup(p, s) rc_memdup_(p, s, __FILE__, __LINE__)
#endif

/*@ void *rc_realloc(void *p, size_t size)
 *# `realloc()`s the object `p`.
 *#
 *# **Be careful:**
 *# It will only work if `p` has one and only one reference to it (for the same reasons
 *# that normal `realloc()` also only works with only one reference), but it is still 
 *# useful in some of the use cases where normal `realloc()` would be.  \
 *# Debug mode has an `assert()` for this.
 */
#ifdef NDEBUG
void *rc_realloc(void *p, size_t size);
#else
void *rc_realloc_(void *p, size_t size, const char *file, int line);
#define rc_realloc(p, s) rc_realloc_(p, s, __FILE__, __LINE__)
#endif

/*@ void *rc_retain(void *p)
 *# Increments an object's reference count.\n
 *# Every call to {{rc_retain()}} should have a corresponding call to 
 *# {{rc_release()}}
 */
#ifdef NDEBUG
void *rc_retain(void *p);
#else
void *rc_retain_(void *p, const char *file, int line);
#define rc_retain(p) rc_retain_(p, __FILE__, __LINE__)
#endif

/*@ void rc_release(void *p)
 *# Decrements an object's referenc count.\n
 *# If the reference count reaches 0, the object's destructor 
 *# (see {{rc_set_dtor()}})) gets called, and the object is reclaimed.
 */
#ifdef NDEBUG
void rc_release(void *p);
#else
void rc_release_(void *p, const char *file, int line);
#define rc_release(p) rc_release_(p, __FILE__, __LINE__)
#endif

#ifdef NDEBUG
void *rc_assign(void **p, void *val);
#else
void *rc_assign_(void **p, void *val, const char *file, int line);
#define rc_assign(p, v) rc_assign_(p, v, __FILE__, __LINE__)
#endif


/*@ typedef void (*ref_dtor)(void *)
 *# Type for the destructor function that gets called on an object if its 
 *# memory gets reclaimed (see {{rc_set_dtor()}}).\n
 *# It takes a single parameter: The object being reclaimed.
 */
typedef void (*ref_dtor)(void *);

/*@ void rc_set_dtor(void *p, ref_dtor dtor)
 *# Registers a destructor function {{dtor}} on an object {{p}}.\n
 *# The destructor function is called on an object before its memory is reclaimed
 *# when its reference count reaches 0 in {{rc_release()}}. This gives the object
 *# an opportunity to release references it has on other objects and to close
 *# other resources.\n
 *# Destructors are optional. 
 */
void rc_set_dtor(void *p, ref_dtor dtor);

/*@ void rc_init()
 *# Initializes the troubleshooting mode of the reference counter.\n
 *# It does nothing if {{NDEBUG}} is defined for release builds.
 */
void rc_init();
