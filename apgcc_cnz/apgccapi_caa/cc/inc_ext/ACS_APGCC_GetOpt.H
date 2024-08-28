#ifndef ACS_APGCC_GETOPT_H
#define ACS_APGCC_GETOPT_H


//
// NOTE: For use in test program only!
//


/*
Usage:

  int port = opt_getnum(argc,argv,"p",1234);

  char* s  = opt_getstr(argc,argv,"s","Default string");

  Link with mcs_int?.lib

*/

int opt_getnum(int argc, char** argv, const char* optstr, int defnum)
{
   int i=1;
   while ( i < argc ) {
      if ( argv[i][0] == '-' ) {
         if ( argv[i][1] == optstr[0] ) {
            i++;
            return atoi(argv[i]);
         }
      }
      i++;
   }
   return defnum;
}

char* opt_getstr(int argc, char** argv, const char* optstr, char* defstr)
{
   int i=1;
   while ( i < argc ) {
      if ( argv[i][0] == '-' ) {
         if ( argv[i][1] == optstr[0] ) {
            i++;
            return argv[i];
         }
      }
      i++;
   }
   return defstr;
}

const char* opt_getstr(int argc, char** argv, const char* optstr, const char* defstr)
{
   int i=1;
   while ( i < argc ) {
      if ( argv[i][0] == '-' ) {
         if ( argv[i][1] == optstr[0] ) {
            i++;
            return argv[i];
         }
      }
      i++;
   }
   return defstr;
}


#endif //ACS_APGCC_GETOPT_H
