#include "array_data.h"

struct array_data *create_array_data(void * data, size_t size, size_t nmemb) 
{
        struct array_data *newdata = malloc(sizeof(*newdata));
        if (NULL == newdata) {
                return NULL;
        }

        if (NULL == data) {
                newdata->data = malloc(size * nmemb);
                if (NULL == newdata->data) {
                        free(newdata);
                return NULL;
        }
        } else {
                newdata->data = data;
        }

        newdata->size = size;
        newdata->nmemb = nmemb;
        return newdata;
}
struct array_data *resize(struct array_data *parray, size_t newnmemb)
{
        parray->data = realloc(parray->data, newnmemb * parray->size);
        if (NULL == parray->data) {
                free(parray);
                return NULL;
        }
        parray->nmemb = newnmemb;
        return parray;
}
void delete_array_data(struct array_data *parray)
{
        free(parray->data);
        free(parray);
}

void *findmemb(struct array_data *parray, bool (*func) (void *, void*)) 
{
        if (0 == parray->nmemb || 0 == parray->size) {
                return NULL;
        }

        void *panswer = parray;

        for (size_t i = 1; i < parray->nmemb; i++)
        {
                if (func(addressof(parray, i), panswer)) {
                        panswer = addressof(parray, i);
                }
        }
        
        return panswer;
}