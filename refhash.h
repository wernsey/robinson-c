struct HashTbl;
typedef struct HashTbl HashTable;


#ifdef NDEBUG
HashTable *ht_create();
#else
HashTable *ht_create_(const char *file, int line);
#define ht_create() ht_create_(__FILE__, __LINE__)
#endif

void *ht_get(HashTable *tbl, const char *name);

#ifdef NDEBUG
void *ht_retain(HashTable *tbl, const char *name, void *value);
#else
void *ht_retain_(HashTable *tbl, const char *name, void *value, const char *file, int line);
#define ht_retain(tbl, k, v) ht_retain_(tbl, k, v, __FILE__, __LINE__)
#endif

void *ht_put(HashTable *tbl, const char *name, void *value);

const char *ht_next(HashTable *tbl, const char *key);
