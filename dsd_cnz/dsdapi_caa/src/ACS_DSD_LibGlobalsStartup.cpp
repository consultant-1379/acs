#include <stdio.h>

__attribute__ ((constructor))
void library_startup () {}

__attribute__ ((destructor))
void library_cleanup () {}
