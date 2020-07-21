#include "mapreduce_array.h"

static inline void mth_map_delete_data(struct array_data *outdata,
                                       struct thread_map_args *args,
                                       int nthreads)
{
        if (NULL != outdata) { // outdata clear is used only in case of error
                delete_array_data(outdata);
        }

        if (NULL != args) {
                for (int i = 0; i < nthreads; i++) {
                        delete_array_data(args[i].indata);
                }
                free(args);
        }
}

struct array_data *multithread_map(struct array_data *indata, size_t outsize, int nthreads, map_func func) // TODO: rework structures and datatypes
{
        if (NULL == indata 
            || NULL == indata->data
            || NULL == func
            || 0 == indata->nmemb 
            || 0 == indata->size 
            || 0 == outsize 
            || 0 == nthreads) {
                return NULL;
        }

        struct array_data *outdata = create_array_data(NULL, outsize, indata->nmemb);
        if (NULL == outdata) {
                mth_map_delete_data(outdata, NULL, 0);
                return NULL;
        }

        struct thread_map_args *args = malloc(sizeof(*args) * nthreads);
        if (NULL == args) {
                mth_map_delete_data(outdata, args, 0);
                return NULL;
        }

        pthread_t threads[nthreads-1];
        int thread_nmemb = indata->nmemb / nthreads;

        args[0].indata = create_array_data(addressof(indata, 0), indata->size, thread_nmemb + indata->nmemb % nthreads);
        args[0].func = func;
        args[0].outsize = outsize;
        args[0].outmem = addressof(outdata, 0);

        for (int i = 1; i < nthreads; i++)
        {
                args[i].indata = create_array_data(addressof(indata, i * thread_nmemb), indata->size, indata->nmemb / nthreads);
                if (NULL == args[i].indata) {
                        mth_map_delete_data(outdata, args, i + 1);
                        return NULL;
                }
                args[i].func = func;
                args[i].outsize = outsize;
                args[i].outmem = addressof(outdata, i * thread_nmemb);
        }

        for (int i = 0; i < nthreads; i++)
        {
                pthread_create(&threads[i], NULL, &thread_map, &args[i]);
        }
        
         
        for (int i = 0; i < nthreads; i++) {
                pthread_join(threads[i], NULL);
        }

        mth_map_delete_data(NULL, args, nthreads);
        return outdata;
}

void *thread_map(void *args)
{
        struct array_data *indata = ((struct thread_map_args *)args)->indata;
        size_t outsize = ((struct thread_map_args *)args)->outsize;
        map_func func = ((struct thread_map_args *)args)->func;
        void *outmem = ((struct thread_map_args *)args)->outmem;

        free(map(indata, outmem, outsize, func)); // TODO: make a special function for thread executing without initialize unused array_data member

        pthread_exit(EXIT_SUCCESS);
}

struct array_data *map(struct array_data *indata, void *outmem, size_t outsize, map_func func)
{
        if (0 == indata->size || 0 == indata->nmemb) {
                return NULL;
        }

        struct array_data *outdata = create_array_data(outmem, outsize, indata->nmemb);
        if (NULL == outdata) {
                return NULL;
        }

        for (size_t i = 0; i < indata->data; i++) {
                func(addressof(indata, i), addressof(outdata, i));
        }
        
        return outdata;
}

void *reduce(struct array_data *indata, reduce_func func)
{
        if (0 == indata->size || 0 == indata->nmemb) {
                return NULL;
        }

        void *outdata = malloc(indata->size);
        if (NULL == outdata) {
                return NULL;
        }

        memcpy(outdata, addressof(indata, 0), indata->size);

        if (1 != indata->nmemb) {
                return outdata;
        }
        
        for (size_t i = 0; i < indata->nmemb; i++) {
                outdata = func(outdata, addressof(indata, i));
        }

        return outdata;
}
