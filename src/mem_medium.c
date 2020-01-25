/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

unsigned int puiss2(unsigned long size) {
    unsigned int p=0;
    size = size -1; // allocation start in 0
    while (size) {  // get the largest bit
        p++;
        size >>= 1; // size = size >> 1;
    }
    if (size > (1 << p))
        p++;
    return p;
}

void* get_index_available_block(int k){
    //case : no available big blocks
    if (k >= arena.medium_next_exponant + FIRST_ALLOC_MEDIUM_EXPOSANT) {
        unsigned long new_size = mem_realloc_medium();
        unsigned int new_k = puiss2(new_size);
        //make sure it is an available block
        assert(arena.TZL[new_k] != NULL);
        //make sure sizes are the same
        assert(k == new_k);
    }

    // either find an available block
    void * curr = arena.TZL[k];
    if(curr != NULL){
        arena.TZL[k] = *(void**)curr;
        return curr;
    }

    // or divide a block number k+1 (recursively)
    void *ptr = get_index_available_block(k + 1);
    //get buddy
    void *buddy = (void *)((uintptr_t)ptr ^ (1 << k));
    //update TZL
    arena.TZL[k] = buddy;

    return ptr;
}


void *split_blocks(int k) {
    //no more big blocks
    if (arena.TZL[k] == NULL) {
        int next_k = k;
        while (arena.TZL[next_k] == NULL) {
            next_k++;
            //reallocate
            if (next_k >= arena.medium_next_exponant + FIRST_ALLOC_MEDIUM_EXPOSANT) {
                mem_realloc_medium();
            }
        }
        //update TZL
        arena.TZL[next_k-1] = arena.TZL[next_k];
        arena.TZL[next_k] = NULL;
        
        //update buddy
        *(void **)arena.TZL[next_k-1] = (void *)((uintptr_t)arena.TZL[next_k-1] ^ (1 << k));
        // buddy needs to point to null
        *(void **)((uintptr_t)arena.TZL[next_k-1] ^ (1 << k)) = NULL;
        return split_blocks(k);
    }
    //allocate
    void *ptr = arena.TZL[k];
    arena.TZL[k] = *(void **)ptr;
    return ptr; 
}

void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);

    unsigned int k = puiss2(size+32);

    void *p = get_index_available_block(k);
    
    p = mark_memarea_and_get_user_ptr(p, size+32, MEDIUM_KIND);
    
    return p;
}
void efree_medium_buddy(void* ptr_a, unsigned int k) {
    //get buddy
    void *buddy = (void *) ((uintptr_t)ptr_a ^ (1 << k));
    //look for buddy
    void *current = arena.TZL[k];
    void *previous = current;
    while(*(void **) current != NULL && current != buddy) {
        previous = current;
        current = *(void **) current;
    }
    //buddy is available
     if (*(void **) current != NULL) {
         *(void **) previous = *(void **) current;
         void *next;
         if (ptr_a > buddy)
             next = buddy;
         else
             next = ptr_a;
         efree_medium_buddy(next, k + 1);
        
     }
     //bussy not available => update head of list
     else {
         *(void **)ptr_a = arena.TZL[k];
         arena.TZL[k] = ptr_a;
     }
}
void efree_medium(Alloc a) {
    unsigned int k = puiss2(a.size);
    efree_medium_buddy(a.ptr, k);
    *(void **)a.ptr = NULL;
}


