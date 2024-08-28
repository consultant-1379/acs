#ifndef GETOPT_H
#define GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

int getopt (int argc, char* const argv[], const char* opts);

extern char* optarg;
extern int opterr, optind, optopt;
#ifdef __cplusplus
}
#endif

#endif
