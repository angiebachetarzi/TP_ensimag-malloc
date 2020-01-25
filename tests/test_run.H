/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#ifndef TEST_2_H
#define TEST_2_H

#include <vector>
using namespace std;

extern "C" {
#include "../src/mem.h"
#include "../src/mem_internals.h"
}

#include "test_run_cpp.h"



class allocat 
{
public:
    int idx;
    void *adr;
    int size;
    allocat(int s) : idx {0}, adr {0}, size {s}  {}
    void doAlloc();
    void doLiberer();
};

/*  remplissage de la liste */

template < int n >
void fillList_fibo (vector<allocat> &li)
{
    fillList_fibo<n / 4> (li);
    if (n >= 128)
	li.push_back ( allocat(n/2) );
    fillList_fibo<n / 6> (li);
};

template < >
void fillList_fibo<0> (vector<allocat> &li) {}

template < >
void fillList_fibo<1> (vector<allocat> &li) {}

template < >
void fillList_fibo<2> (vector<allocat> &li) {}

template < >
void fillList_fibo<3> (vector<allocat> &li) {}


#endif
