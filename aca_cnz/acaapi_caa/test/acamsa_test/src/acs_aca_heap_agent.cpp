#include "acs_aca_heap_agent.h"
#include <iostream>
#include <cstdlib>

namespace {
//	unsigned long heapCounter = 0;

	void * pfxAlloc(size_t size) {
		void * p = malloc(size);
		
		//printf("  ## CLIENT NEW : %lu, size = %u, p = %p ##\n", ++heapCounter, size, p);

		return p;
	}

	void pfxFree(void * p) {
		//printf("  ## CLIENT DELETE : %lu, p = %p ##\n", heapCounter--, p);

		free(p);
	}
}

//Global new and delete instalces
//================================================================================================
void * operator new(size_t size) throw(std::bad_alloc){
	return pfxAlloc(size);
}

void operator delete(void * p) throw(std::bad_alloc) {
	pfxFree(p);
}

//================================================================================================
void * operator new[](size_t size) throw(std::bad_alloc){
	return pfxAlloc(size);
}

void operator delete[](void * p) throw(std::bad_alloc) {
	pfxFree(p);
}
//================================================================================================
