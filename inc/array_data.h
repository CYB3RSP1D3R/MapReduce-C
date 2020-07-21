#ifndef ARRAY_DATA
#define ARRAY_DATA

#include <stdlib.h>

struct array_data {
        void * data;
        size_t size;
        size_t nmemb;
};

#define addressof(parray, pos) (pos < (parray)->nmemb ? ((parray)->data + (parray)->size * pos) : NULL)

struct array_data *create_array_data(void * data, size_t size, size_t nmemb);
struct array_data *resize(struct array_data *parray, size_t newnmemb);
void delete_array_data(struct array_data *parray);
void *findmemb(struct array_data *parray, bool (*func) (void *, void*));

#endif // ARRAY_DATA