/******************************************************
 * Copyright Grégory Mounié 2008-2018                 *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <gtest/gtest.h>
#include <string.h>
#include <assert.h>
#include "../src/mem.h"
#include "../src/mem_internals.h"

TEST(Medium,buddy) {
  constexpr unsigned long ALLOC_MEM_SIZE = FIRST_ALLOC_MEDIUM/2;

  void *mref = emalloc(ALLOC_MEM_SIZE); // first allocation
  ASSERT_NE( mref, (void*) 0);
  memset(mref, 1, ALLOC_MEM_SIZE);
  efree(mref);
  ASSERT_NE( nb_TZL_entries(), 1U);
  
  void *mref2 = emalloc(ALLOC_MEM_SIZE); // again, should give same address
  ASSERT_NE( mref2, (void*) 0);
  ASSERT_EQ( mref2, mref ); // both allocation should give same address
  efree(mref2);
  ASSERT_NE( nb_TZL_entries(), 1U);  

  // Next two allocations should be buddys of 128 Bytes 
  void *m1 = emalloc(65); 
  ASSERT_NE( m1, (void *)0 );
  memset( m1, 1, 65);

  void *m2 = emalloc(65);
  ASSERT_NE( m2, (void *)0 );
  memset( m2, 1, 1);

  unsigned long vref = (unsigned long) mref;
  unsigned long v1 = (unsigned long)m1;
  unsigned long v2 = (unsigned long)m2;
  ASSERT_EQ( (v1-vref)^(v2-vref), 128UL );

  efree(m1);
  efree(m2);
  ASSERT_NE( nb_TZL_entries(), 1U);  

  // after fusion, the merge allow to get back the same full block
  void *mref3 = emalloc(ALLOC_MEM_SIZE);
  ASSERT_NE( mref3, (void *)0 );
  memset(mref3, 1, ALLOC_MEM_SIZE);
  ASSERT_EQ( mref3, mref );

  efree(mref3);
  ASSERT_NE( nb_TZL_entries(), 1U);
}
