/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <gtest/gtest.h>
#include <vector>

#include "../src/mem.h"
#include "../src/mem_internals.h"

constexpr int NBCHUNK = 8;
using namespace std;


TEST( Mark, mmix ) {
    unsigned long tab[NBCHUNK] = {};
  
    void *pmem0 = mark_memarea_and_get_user_ptr(tab, 32, SMALL_KIND);
    ASSERT_NE(pmem0, (void*)0);
    ASSERT_EQ(pmem0, & tab[2] );
    ASSERT_EQ(tab[1], knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(tab[2], knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(tab[0], 32UL);
    ASSERT_EQ(tab[3], 32UL);
    
    Alloc a0 = mark_check_and_get_alloc(pmem0);
    ASSERT_EQ(a0.size, 32UL);
    ASSERT_EQ(a0.ptr, tab);

    void *pmem1 = mark_memarea_and_get_user_ptr(tab, 33, SMALL_KIND);
    memset(pmem1, 0, 1);

    ASSERT_NE(pmem1, (void*)0);
    ASSERT_EQ(pmem1, & tab[2] );
    ASSERT_EQ(tab[1], knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(tab[0], 33UL);
    ASSERT_EQ(* (unsigned long *) (((char*)tab)+17),
	      knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(* (unsigned long *) (((char*)tab)+25), 33UL);

    Alloc a1 = mark_check_and_get_alloc(pmem1);
    ASSERT_EQ(a1.size, 33UL);
    ASSERT_EQ(a1.ptr, tab);

    void *pmem32 = mark_memarea_and_get_user_ptr(tab, 64, SMALL_KIND);
    memset(pmem32, 0, 32);
    
    ASSERT_NE(pmem32, (void*)0);
    ASSERT_EQ(pmem32, & tab[2] );
    ASSERT_EQ(tab[1], knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(tab[6], knuth_mmix_one_round((unsigned long)tab) & ~(0b11UL));
    ASSERT_EQ(tab[0], 64UL);
    ASSERT_EQ(tab[7], 64UL);

    Alloc a32 = mark_check_and_get_alloc(pmem32);
    ASSERT_EQ(a32.size, 64UL);
    ASSERT_EQ(a32.ptr, tab);

}
