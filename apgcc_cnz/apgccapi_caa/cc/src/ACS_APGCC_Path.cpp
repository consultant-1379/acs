// COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
// All rights reserved.
//
// The Copyright to the computer program(s) herein
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with
// the written permission from Ericsson Utvecklings AB or in
// accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been
// supplied.

// CHANGES
//
//    RELEASE REVISION HISTORY
//
//    DATE        NAME        DESCRIPTION
//    2006-08-09  QCHAROH     APG43 adaptations-Made use of LCTAPI's GetDataDiskPath()method
//                            to get the required paths instead of fetching them from
//                            registry.Implemented the creation of directories here.
//                            Install shield doesn't create the directories anymore.
//                            Check of existence of directories is done when the path is
//                            queried and created newly if they are not existing.
//                            Code using expandShare() method of MCS_Util has
//                            been commented.No use of shares in apg43.
//*****************************************************************************


#include <ace/INET_Addr.h>
#include "ACS_APGCC_Path.H"
#include "ACS_APGCC_Util.H"
// PR_01 #include "commondll.h"   //Using GetDataDiskPath()of LCTAPI to get the required path-apg43
#include "ACS_APGCC_Event.H"
// PR_01 #include "process.h"
// PR_01 #include <shlobj.h>
// PR_01 #include <accctrl.h>
#include <stdlib.h>    	  // PR_01 for char * getenv(const char *)
#include <ace/OS_Errno.h> // PR_01 for errno
using namespace std;


#define DEFAULT_HOME "%AP_HOME%/APGCC"
#define DEFAULT_DATA "%TEMP%/APGCC/data"
#define DEFAULT_LOGS "%TEMP%/APGCC/logs"


//#define DEBUG_PATH

#ifdef NDEBUG        // Release version
#define IFDEBUG(x)
#else                // Debug version
#ifdef DEBUG_PATH
#define IFDEBUG(x)   x;
#else
#define IFDEBUG(x)
#endif
#endif

//#include "ACS_TRA_Trace.h"
//#include "mcs_trace.h"


//ACS_TRA_trace traPath= ACS_TRA_DEF ("MCS_Path",   "C200");

// Removes duplicated characters
//-----------------------------------------------------------------
void remove_reduntant_char(string& str, char ch)
{
	string::size_type i = 0;

   while ( i < str.length() ) {

      if ( str[i] == ch ) {

         i++;
         while ( str[i] == ch ) {
            str.erase(i,i);
         }
      }
      i++;
   }
}



//----------------------------------------------
//-------------- ACS_APGCC_Path Class ----------------
//----------------------------------------------



ACS_APGCC_Path::ACS_APGCC_Path()
:found_(false)
{
}

ACS_APGCC_Path::ACS_APGCC_Path(const char* path)
:found_(false)
{
	if ( path ) {
		this->expand(path);
	}
}


ACS_APGCC_Path::ACS_APGCC_Path(const string& path)
:found_(false)
{
	if ( path.length() > 0 ) {
		this->expand(path);
	}
}


ACS_APGCC_Path::~ACS_APGCC_Path()
{
}


bool ACS_APGCC_Path::found() const
{
   return found_;
}


void ACS_APGCC_Path::setpath(const string& path)
{
	this->path(path);
}

void ACS_APGCC_Path::setpath(const char* path)
{
	this->path(path);
}

void ACS_APGCC_Path::path(const string& path)
{
	this->expand(path);
}

void ACS_APGCC_Path::path(const char* path)
{
   if (path)
	   this->expand(path);
}

const char* ACS_APGCC_Path::path()
{
	return path_.c_str();
}

const char* ACS_APGCC_Path::path() const
{
	return path_.c_str();
}

ACS_APGCC_Path::operator const char*() const
{
  return path_.c_str();
}

ACS_APGCC_Path::operator char*() const
{
  return (char*)path_.c_str();
}

const char* ACS_APGCC_Path::operator*() const
{
  return path_.c_str();
}

/*
ACS_APGCC_Path::operator const char*()
{
      return path_.c_str();
}
*/

void ACS_APGCC_Path::expand(const string& s)
{
   this->path_ = s;
   this->expand_path( this->path_ );
}



void ACS_APGCC_Path::expand_path(string& s)
{

   set<char> cs;
   string::size_type i, start, end,len;
	int j;
	//int len;
   string t, n, r, orig;
   string server, share, prefix;
   wstring wserver, wshare;
   char* cp;
   bool unc_checked = false;
   bool force_rerun;
   string::size_type pos;
//apg43-start
   char szLogicName[20];
   char *szPath= NULL;
   DWORD dwLen = 50;
   int value_return=-1;
   char objRef[50];
//apg43-end

//apg43-directory creation-start
   char *szPathBase = NULL;
   char szLogicNameBase [20];
   bool exist = false;
   int McsReturn = ERROR_SUCCESS, FileResult = 0;
   // PR_01 LPTSTR err_text[150];
   //apg43-directory creation-end

   //mcc_trace(traPath, "expland_path 1");

   found_ = false;
   orig   = s;

   // Check if the path includes any quations marks (") and
   // remove those
   while ( (pos=s.find("\"")) != string::npos )
      s.erase(pos,1);

   cs.insert( '/' );
   cs.insert( '\\' );


	i = 0;
	len = s.length();

   // Check for two beginning back- or forward-slashes
   // indicated an UNC-coded path
   if ( cs.count(s[0]) && cs.count(s[1]) ) {

      unc_checked = true;
	  //mcc_trace(traPath, "expland_path 2");

      // Skip starting slashes
		while (i < len && cs.count(s[i]) > 0)
			i++;

      prefix = "\\\\";

      // Copy server name
      while (i < len && cs.count(s[i]) == 0) {
         prefix += s[i];
			i++;
      }

      prefix += "\\";

      // Skip ending slashes
		while (i < len && cs.count(s[i]) > 0)
			i++;

      // Delete prefix from path
      s.erase(0,i);

      i = 0;
      len = s.length();
   }


Rerun:

   force_rerun = false;
	r = "";
	i = 0;
	len = s.length();
   share = "";

   // commented for apg43-No use of shares-Start

   // Check if this is an absolute or a relative path
   //if ( s[1] != ':' ) {

   //   // A relative path; Expand share name

   //   MCS::expandShare(s);

   //   i = 0;

   //} // if relative

   // commented for apg43-No use of shares-end.


	while ( i < len ) {

		// Skip separators
		while (i < len && cs.count(s[i]) > 0)
			i++;

      // Save start pos
      start = i;

		// Store item
      r = ""; j = 0;
      while (i < len && cs.count(s[i]) == 0) {
		   r += s[i];
		   i++;
		   j++;
      }

      // Save end pos
      end = i;

      n = "";

      int rz = 0;

      // Check item
      switch (r[0])
      {
      case '$':
         r.erase(0,1);
         rz++;
         cp = ::getenv(r.c_str());
         if ( cp ) {
            n = cp;
         }
         else {

            rz = 0;
            // PR_01 if ( ::stricmp(r.c_str(),"ACS_APGCC_DATA") == 0 )
            if ( ::strcmp(r.c_str(),"ACS_APGCC_DATA") == 0 )
               r = "ACS_APGCCAPDATA";
            // PR_01 else if ( ::stricmp(r.c_str(),"ACS_APGCC_LOGS") == 0 )
            else if ( ::strcmp(r.c_str(),"ACS_APGCC_LOGS") == 0 )
               r = "ACS_APGCCAPLOGS";
            // PR_01 else if ( ::stricmp(r.c_str(),"ACS_APGCC_HOME") == 0 )
            else if ( ::strcmp(r.c_str(),"ACS_APGCC_HOME") == 0 )
               r = "ACS_APGCCAPHOME";
            else
               rz++;

            cp = ::getenv(r.c_str());
            if ( cp ) {
               n = cp;
            }
            else {

				//mcc_trace(traPath, "expland_path 3");
               /*PR_01 BEGIN
                * if ( reg.openKeyRead(ACS_APGCC_REG_HKEY,ACS_APGCC_REG_COMMON) ) {
                  reg.getValue(r.c_str(),n);
                  reg.close();
               } PR_01 END */

				if(r=="ACS_APGCCAPHOME"){
					/* PR_01 BEGIN
					if ( reg.openKeyRead(ACS_APGCC_REG_HKEY,ACS_APGCC_REG_COMMON) ) {
						reg.getValue(r.c_str(),n);
						reg.close();
					}
					   PR_01 END  */
				}//r==MCSAPHOME
				else if(r=="ACS_APGCCAPDATA" || r=="ACS_APGCCAPLOGS"){
					if(r=="ACS_APGCCAPDATA"){
						memset((void *)szLogicName,'\0',20);
						strcpy(szLogicName,"ACS_APGCC_DATA$");
						memset((void *)szLogicNameBase,'\0',20);
						strcpy(szLogicNameBase,"ACS_APGCC_DATA_BASE$");
					}
					else if(r=="ACS_APGCCAPLOGS"){ /* PR_01 Ex MCSAPLOG */
						memset((void *)szLogicName,'\0',20);
						strcpy(szLogicName,"ACS_APGCC_LOGS$");
						memset((void *)szLogicNameBase,'\0',20);
						strcpy(szLogicNameBase,"ACS_APGCC_LOGS_BASE$");
					}
					while(-1==value_return){
						if (szPath != NULL){
							free(szPath);
							szPath=NULL;
						}
						//mcc_trace(traPath, "expland_path 4");
						szPath = (char*) malloc(dwLen*sizeof(char));
						if (NULL!=szPath){
							//value_return= GetDataDiskPath(szLogicName,szPath,&dwLen);
						}
						else{
							::sprintf(objRef,"process id is %d",getpid());
						/*	ACS_APGCC_Event::send(MERR_MCS,
											MTXT_ADDINFO,
											objRef,
											"malloc failed to allocate memory");*/
							n="";
							//mcc_trace(traPath, "expland_path 5");
							break;
						}
					}//value_return=-1


					if(value_return<=-2){
						::sprintf(objRef,"process id is %d",getpid());
					/*	ACS_APGCC_Event::send(MERR_UNREASONVALUE,
										MTXT_UNRESVALUE,
										objRef,
										"Unable to fetch path from LCTAPI");*/

					}//value_return<=-2

					//reinitialize for ACS_APGCC_DATA_BASE path
					dwLen = 50;
					value_return = -1;

					while(-1==value_return){
						if (szPathBase != NULL){
							free(szPathBase);
							szPathBase=NULL;
						}
						//mcc_trace(traPath, "expland_path 6");
						szPathBase = (char*) malloc(dwLen*sizeof(char));
						if (NULL!=szPathBase){
							// PR_01 value_return= GetDataDiskPath(szLogicNameBase,szPathBase,&dwLen);
						}
						else{
							::sprintf(objRef,"process id is %d",getpid());
						/*	ACS_APGCC_Event::send(MERR_MCS,
											MTXT_ADDINFO,
											objRef,
											"malloc failed to allocate memory");*/
							n="";
							//mcc_trace(traPath, "expland_path 7");
							break;
						}
					}//value_return=-1

					if(value_return<=-2){
						::sprintf(objRef,"process id is %d",getpid());
				/*		ACS_APGCC_Event::send(MERR_UNREASONVALUE,
										MTXT_UNRESVALUE,
										objRef,
										"Unable to fetch path from LCTAPI");*/
					}//value_return<=-2


					// Check if data dir is created or not. If not created, create it and set
					// the right permissions before returning the log/data dir path
					while (!exist) {
						//mcc_trace(traPath, "expland_path 8");
						//PR_01 McsReturn = SHCreateDirectoryEx(NULL, (LPCSTR)szPathBase, NULL);
						McsReturn = mkdir(szPathBase,0x666);

						if (McsReturn == ERROR_SUCCESS) {
							FileResult = 0; /* PR_01
												SetFilePerm ((LPTSTR)szPathBase,
													  (LPTSTR)"MCSADMG",
													  GENERIC_ALL,
													  (LPTSTR)"set",
													  SUB_CONTAINERS_AND_OBJECTS_INHERIT,
													  err_text);
											  PR_01 END */
							if (0 != FileResult) {
								//if (NULL != szPath){
									free(szPath);
									szPath=NULL;
								//}
								//if (NULL != szPathBase)
									free(szPathBase);
									szPathBase=NULL;
								::sprintf(objRef,"process id is %d",getpid());
							/*	ACS_APGCC_Event::send(MERR_MCS,
												MTXT_ADDINFO,
												objRef,
												"Could not set right permissions to MCS path");*/
								n="";
								break;
							}
							FileResult = 0;
											/* PR_01 BEGIN
											 *
												 SetFilePerm ((LPTSTR)szPathBase,
													  (LPTSTR)"MCSUSRG",
													   GENERIC_READ | GENERIC_EXECUTE,
													  (LPTSTR)"set",
													   SUB_CONTAINERS_AND_OBJECTS_INHERIT,
													   err_text);
							 				   PR_01 END */

							if (0 != FileResult) {
								//if (NULL != szPath){
									free(szPath);
									szPath=NULL;
								//}
								//if (NULL != szPathBase)
									free(szPathBase);
									szPathBase=NULL;
									//mcc_trace(traPath, "expland_path 9");
								::sprintf(objRef,"process id is %d",getpid());
							/*	ACS_APGCC_Event::send(MERR_MCS,
												MTXT_ADDINFO,
												objRef,
												"Could not set right permissions to MCS path");*/
								n="";
								break;
							}
						}
						else if ((errno == ENOENT) ||
								/* PR_01 (McsReturn == ERROR_BAD_PATHNAME) ||*/
								(errno == ENAMETOOLONG ))
								/* PR_01 (McsReturn == ERROR_FILENAME_EXCED_RANGE) || */
								/* PR_01 (McsReturn == ERROR_CANCELLED)) The user canceled the operation. in Windows */
								{
									//if (NULL != szPath){
										free(szPath);
										szPath=NULL;
									//}
									//if (NULL != szPathBase)
										free(szPathBase);
										szPathBase=NULL;
									::sprintf(objRef,"process id is %d",getpid());
								/*	ACS_APGCC_Event::send(MERR_MCS,
													MTXT_ADDINFO,
													objRef,
													"Error in creating ACS_APGCC_DATA_BASE path");*/
									n="";
									//mcc_trace(traPath, "expland_path 10");
									break;
								}
							if ( /* PR_01 (McsReturn == ERROR_FILE_EXISTS) || */
									(errno == EEXIST) || McsReturn == -1 )
								/* PR_01 (McsReturn == ERROR_ALREADY_EXISTS) ||
								(McsReturn == ERROR_SUCCESS)) */ {
									/* PR_01 McsReturn = SHCreateDirectoryEx(NULL, (LPCSTR)szPath, NULL); */
									McsReturn = mkdir(szPath,0x666);
									if ((errno == EEXIST) ||
										/* PR_01 (McsReturn == ERROR_ALREADY_EXISTS) || */
										(McsReturn == 0 )){
											exist = true;
										}
									else if ( /* PR_01 (errno == ERROR_PATH_NOT_FOUND) || */
											(errno == ENOENT) ||
											/* (errno == ERROR_BAD_PATHNAME) || */
										/* (errno == ERROR_FILENAME_EXCED_RANGE) || */
										 (errno == ENAMETOOLONG ) ||
										(McsReturn == -1 )) {
											//if (NULL != szPath){
												free(szPath);
												szPath=NULL;
											//}
											//if (NULL != szPathBase)
												free(szPathBase);
												szPathBase=NULL;

											 //mcc_trace(traPath, "expland_path 11");
											::sprintf(objRef,"process id is %d",getpid());
								/*			ACS_APGCC_Event::send(MERR_MCS,
															MTXT_ADDINFO,
															objRef,
															"Error in creating ACS_APGCC_DATA path");*/
											n="";
											break;
										}
								}
					}

				//mcc_trace(traPath, "expland_path 12");
				if (NULL!=szPath)
				{
					n=szPath;
					free(szPath);
					szPath=NULL;
				}

				if (NULL!=szPathBase)
				{
					free(szPathBase);
					szPathBase=NULL;
				}

				// enungai  start


				// enungai end
				/*n=szPath;*/
				}//MCSAPDATA or MCSAPLOGS
            }
		 //Check for a possible share name
		 if ( n[1] != ':' )
		 force_rerun = true;
         }//first else in this case
         break;

      case '%':
         /* PR_ 01 if ( ::ExpandEnvironmentStrings(r.c_str(),buf,1024) > 0 ) {*/
    	  char * p_buf;
    	  p_buf = ::getenv(r.c_str());
    	  if (  p_buf  != NULL )  {
			  n = p_buf;
         }
         break;

      default:
         break;
      } // switch

      if ( n.length() > 0 ) {

         s.erase(start,r.length()+rz);
         s.insert(start,n);

         // Update index
         i = start + n.length() + 1;
         len = s.length();

         if ( n[0] == '$' || n[0] == '%' || force_rerun ) {
            // Need to be expanded once more
            goto Rerun;
         }
      }

   } // while


   // Check if the path still contains not expanded strings
   if ( s.find("%") != string::npos ) {
      /* PR_01 if ( ::ExpandEnvironmentStrings(s.c_str(),buf,1024) > 0 ) {*/
	   char * p_buf;
	   p_buf = ::getenv(s.c_str());
	   if (  p_buf  != NULL )  {
		  s = p_buf;
	   }
   }


   // Add preifx if any
   if ( prefix.length() > 0 ) {
      s.insert(0,prefix);
   }


   /*
   // @@ TODO: Should we enclose space characters in the path
   // within a pair of quotation marks??
   if ( s.find(" ") != string::npos ) {
      pos = s.find_last_of("/\\");
      if ( pos != string::npos ) {
         string::size_type start;
         if ( unc_checked )
            start = s.find_first_of("/\\",2);
         else
            start = s.find_first_of("/\\");

         if ( pos != start ) {
            // Insert quotation marks
            s.insert(pos+1,"\"");
            s.insert(0,"\"");
         }
      }
   }
   */

   if ( s != orig )
      found_ = true;
   else if ( s[1] == ':' )
      found_ = true;

   /*PR_01
    * ACS_APGCC::replace(s,"/","\\");
    * */
   remove_reduntant_char(s,'\\');
}



