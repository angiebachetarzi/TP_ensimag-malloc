/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>

#include "test_run.H"

using namespace std;

/* options */
static bool be_verbose = false; 


/* manipulation de la liste des allocations */

void allocat::doAlloc()
{
    static int nb=0;

    assert (size > 0);
  
    adr = emalloc (size);


    if ( adr == (char *)0 )
    {
	/* l'allocation a echoue */
	printf ("Une allocation de %d a echoue !\n", size );
      
	adr = nullptr;
    }
    else
    {
	idx = nb;
	nb ++;
	if (be_verbose)
	    printf ("alloc %d (taille: %d): %p\n", idx, size, adr);
      
	/* ecrire dans le bloc */
	memset (adr, 0, size );
    }
  
  
}

void allocat::doLiberer()
{
    assert (size > 0);
  
    if (adr)
    {
	/* ecrire dans le bloc */
	memset (adr, 0, size );

	if (be_verbose)
	    printf ("libere %d (taille: %d): %p\n", idx,size, adr);
      
	efree (adr);
	adr = nullptr;
    }
}

void random_run_cpp(bool verbose=false)
{
    be_verbose = verbose;
  
  
    vector<allocat> liste_allocation;
    
    /* choisir les allocations */
    fillList_fibo<FIRST_ALLOC_MEDIUM*16> ( liste_allocation );
    
    /* afficher l'etat de la memoire */
    
    /* faire les allocations */
    for(auto &l: liste_allocation)
	l.doAlloc();
  
    /* melanger la liste */
    std::shuffle ( liste_allocation.begin(), liste_allocation.end(),
		   std::mt19937_64() );
  
    /* faire les deallocations */
    for(auto &l: liste_allocation)
	l.doLiberer ();
  
}

