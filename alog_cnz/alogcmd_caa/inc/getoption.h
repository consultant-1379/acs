#ifndef GETOPT_H
#define GETOPT_H

//#include "ACS_APGCC_Common_Define.h"

#ifdef __cplusplus
extern "C" {
#endif

int getoption(int argc, char* const argv[], const char* opts);

extern char* optarg;
extern int   opterr;
extern int   optind;
extern int   optopt;

#ifdef __cplusplus
}
#endif

#endif
