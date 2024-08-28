#ifndef __HEAP_AGENT_H__
#define __HEAP_AGENT_H__

#include <stdexcept>
#include <cstddef>

//Global new and delete operators
void * operator new (size_t size) throw(std::bad_alloc) ;
void operator delete (void * p) throw(std::bad_alloc);

void * operator new[](size_t size) throw(std::bad_alloc);
void operator delete[](void * p) throw(std::bad_alloc);

#endif
