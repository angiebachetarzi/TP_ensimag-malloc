/*****************************************************
 * Copyright Grégory Mounié 2008-2018                *
 * This code is distributed under the GLPv3+ licence.*
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include "mem.h"
#include "mem_internals.h"

/** squelette du TP allocateur memoire */

MemArena arena = {};


/* ecrire votre code ici */


void *
emalloc(unsigned long size)
{
    /*  ecrire votre code ici */
    if (size == 0)
	return NULL;
	    
    if (size >= LARGEALLOC)
	return emalloc_large(size);
    else if (size <= SMALLALLOC)
	return emalloc_small(size); // allocation de taille CHUNKSIZE
    else
	return emalloc_medium(size);
}

void 
efree(void *ptr)
{
    /* ecrire votre code ici */

    Alloc a = mark_check_and_get_alloc(ptr);
    switch( a.kind ) {
    case SMALL_KIND:
	efree_small(a);
	break;
    case MEDIUM_KIND:
	efree_medium(a);
	break;
    case LARGE_KIND:
	efree_large(a);
	break;
    default:
	assert(0);
    }
}


