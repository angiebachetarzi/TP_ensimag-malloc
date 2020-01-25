/******************************************************
 * Copyright Grégory Mounié 2008-2013                 *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/time.h>
#include <sys/resource.h>

#include <gtest/gtest.h>
#include <vector>

#include "../src/mem.h"
#include "../src/mem_internals.h"

#include "test_run_cpp.h"
#include "test_generic.h"

using namespace std;

TEST(Basic, zero) {
  ASSERT_EQ( emalloc(0), nullptr );
}

TEST(Basic, onetinysmall) {
    void *ptr = emalloc(1);
    ASSERT_NE( ptr, nullptr );
    memset(ptr, 1, 1);
    efree(ptr);
}

TEST(Basic, onelargesmall) {
    void *ptr = emalloc(64);
    ASSERT_NE( ptr, nullptr );
    memset(ptr, 1, 64);
    efree(ptr);
}

TEST(Basic, onemedium) {
    void *ptr = emalloc(65);
    ASSERT_NE( ptr, nullptr );
    memset(ptr, 1, 65);
    efree(ptr);
}

TEST(Basic, onelarge) {
    void *ptr = emalloc(LARGEALLOC);
    ASSERT_NE( ptr, nullptr );
    memset(ptr, 1, LARGEALLOC);
    efree(ptr);
}


TEST(Basic, oneallocloopsmall) {
    vector<void *> tab(100);
    
    for (auto &t : tab) {
	t = emalloc(64);
	ASSERT_NE( t, nullptr );
	memset(t, 1, 64);
    }
    for (auto t : tab) {
	efree(t);
    }
}

TEST(Basic, oneallocloopmedium) {
    vector<void *> tab(100);
    
    for (auto &t : tab) {
	t = emalloc(65);
	ASSERT_NE( t, nullptr );
	memset(t, 1, 65);
    }
    for (auto t: tab) {
	efree(t);
    }
}

TEST(Generic, loopevenoddsmall) {
  constexpr int nb=2*50;
  vector<void *> tab(nb);

  for(auto &t: tab)
    {
      t = emalloc(64);
      ASSERT_NE( t, (void *)0 );
      memset(t, 1, 64);
    }
  for(int i=0; i < nb; i+=2)
    {
      efree( tab[i] );
    }
  for(int i=1; i < nb; i+=2)
    {
      efree( tab[i] );
    }
}

TEST(Generic, loopevenoddmedium) {
  constexpr int nb=2*50;
  vector<void *> tab(nb);

  for(auto &t: tab)
    {
      t = emalloc(65);
      ASSERT_NE( t, (void *)0 );
      memset(t, 1, 65);
    }
  for(int i=0; i < nb; i+=2)
    {
      efree( tab[i] );
    }
  for(int i=1; i < nb; i+=2)
    {
      efree( tab[i] );
    }
}

TEST(Generic, aleatoire) {
  
  for(int i=0; i< 10; i++)
    {
      random_run_cpp(false);
    } 
}
