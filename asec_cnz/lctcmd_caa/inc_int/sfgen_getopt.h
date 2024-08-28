/****************************************************************
*			SFGEN_GETOPT.H FILE WAS CREATED ON JULY 1, 2008		*
*			AS A PART OF THE IMPLEMENTATION OF THE OS			*
*			HARDENING FEATURE IP IMPLEMENTATION.				*
*			IMPLEMENTATION EXECUTED BY SUJANA AND KALYAN.		*
*****************************************************************/

#ifndef SFGEN_GETOPT_H
#define SFGEN_GETOPT_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int getopt(int argc, LPSTR const argv[], const LPSTR opts);

/**********************************************************
*	THIS METHOD IS USED TO IMBIBE PROPER OPTIONS.		  *
***********************************************************/

extern LPSTR optarg;
extern int   opterr;
extern int   optind;
extern int   optopt;

#ifdef __cplusplus
}
#endif

#endif