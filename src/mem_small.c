/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

void *
emalloc_small(unsigned long size)
{
    //if the chunkpool is empty, allocate a new one
    //create a new list for the chunkpool
    if (arena.chunkpool == NULL) {
        unsigned long new_chunkpool_size =  mem_realloc_small();
        int chunk_number = (new_chunkpool_size / CHUNKSIZE);

        void* current = arena.chunkpool;

        for (int i = 0; i < chunk_number; i++){
            //actual ptr points to the next one
            *(void **)current = current + CHUNKSIZE;
            //move to the next chunk
            current = current + CHUNKSIZE;
        }

        //*(void **)(p - CHUNKSIZE) = 0;
    }
    
    //get the first chunk of the pool
    void *first = arena.chunkpool;
    //update the chunkpool to get the first ptr
    arena.chunkpool = *(void**)first;

    //call function to mark the ptr
    return mark_memarea_and_get_user_ptr(first, CHUNKSIZE, SMALL_KIND);
}

void efree_small(Alloc a) {
    //get the a to the head of the list
    *(void **)a.ptr = arena.chunkpool;
    //update the chunkpool
    arena.chunkpool = a.ptr;
}
