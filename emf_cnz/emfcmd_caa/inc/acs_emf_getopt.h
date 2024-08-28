#ifndef GETOPT_H
#define GETOPT_H

//#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int getopt(int argc, char* const argv[], const char* opts);

extern char* optarg;
extern int   opterr;
extern int   optind;
extern int   optopt;

#ifdef __cplusplus
}
#endif

#endif
