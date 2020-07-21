
#ifndef MAP_REDUCE_ARRAY
#define MAP_REDUCE_ARRAY

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "array_data.h"

typedef bool (*filter_func) (void *, void *);
typedef void (*map_func) (void *, void *);
typedef void *(*reduce_func)(void *, void *);

struct thread_map_args {
        struct array_data *indata;
        void *outmem;
        size_t outsize;
        map_func func;
};



// TODO: struct for the pthread input
void *thread_map(void *args);

struct array_data *map(struct array_data *indata, void *outmem, size_t outsize, map_func func);
void *reduce(struct array_data *indata, reduce_func func);

#endif // MAP_REDUCE_ARRAY
