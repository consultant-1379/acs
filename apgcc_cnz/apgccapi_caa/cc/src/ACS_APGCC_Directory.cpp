/*
 * ACS_APGCC_Directory.cpp
 *
 *  Created on: May 24, 2010
 *      Author: administrator
 */

#include <ace/OS.h>
#include "ACS_APGCC_Directory.H"
#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_Path.H"




ACS_APGCC_Directory::FileData::FileData()
:type(FILE)
{
}

ACS_APGCC_Directory::FileData::FileData(const ACS_APGCC_Directory::FileData& fd)
{
   cName = fd.cName;
   name  = fd.name;
   ext   = fd.ext;
   type  = fd.type;
}

ACS_APGCC_Directory::FileData::~FileData()
{
}

ACS_APGCC_Directory::FileData& ACS_APGCC_Directory::FileData::operator=(const ACS_APGCC_Directory::FileData& fd)
{
   if ( this != &fd ) {
      cName = fd.cName;
      name  = fd.name;
      ext   = fd.ext;
      type  = fd.type;
   }
   return *this;
}


ACS_APGCC_Directory::ACS_APGCC_Directory()
{
}

ACS_APGCC_Directory::~ACS_APGCC_Directory()
{
}

bool ACS_APGCC_Directory::read(const std::string& fileMask, const std::string& startDir)
{
	//WIN32_FIND_DATA findData;
	//HANDLE hFile;
	DIR * tstdp = 0;
	FileData fd;
	std::string savedDir;
	char buf[1024];
	ACS_APGCC_Path path;

	fileList_.erase(fileList_.begin(), fileList_.end());
	if ( startDir.length() > 0 ) {
		//	if ( GetCurrentDirectory(1023,buf) > 0 )
		if ( getcwd(buf,1023) > 0 )
			savedDir = buf;
		path.path(startDir.c_str());
      chdir(path.path());
      // SetCurrentDirectory( path.path() );
	}
	std::string _findData;
//	if ( (hFile=FindFirstFile("*.*",&findData)) == INVALID_HANDLE_VALUE ) {
    std::list<std::string> listData=FindFirstFile(_findData);
    if (listData.empty()) {
    	return false;
	}

	do {
		//fd.cName = findData.cFileName;
		fd.cName = listData.back();
		listData.pop_back();
		if ( ACS_APGCC::match(fd.cName,fileMask) ) {
    	  fd.name = ACS_APGCC::beforeLast(fd.cName,".");
          fd.ext  = ACS_APGCC::afterLast(fd.cName,".");
          tstdp=opendir(fd.cName.c_str());
      /*PR_01 BEGIN*/
         if(tstdp){
		//   if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			   /*END*/
			   fd.type = DIRECTORY;
         }
         else if ( ACS_APGCC::match(fd.ext,"dll") ){
        	 fd.type = DLL;
         }
		   else{
			   fd.type = FILE;
		   }
		   fileList_.push_back(fd);

      } // if match
     /*PR_01 BEGIN*/
	}while ( !listData.empty() );
	/* while ( FindNextFile(hFile, &findData) );
	FindClose(hFile);*/
  /*END*/
	if ( savedDir.length() > 0 )
		 chdir(path.path());
	//	SetCurrentDirectory( savedDir.c_str() );
	if(tstdp != 0){
		closedir(tstdp);
	}
	return true;
}


ACS_APGCC_Directory::List::iterator ACS_APGCC_Directory::begin()
{
   return fileList_.begin();
}


ACS_APGCC_Directory::List::iterator ACS_APGCC_Directory::end()
{
   return fileList_.end();
}


ACS_APGCC_Directory::List& ACS_APGCC_Directory::list()
{
   return fileList_;
}

const ACS_APGCC_Directory::List& ACS_APGCC_Directory::list() const
{
   return fileList_;
}
std::list<std::string> ACS_APGCC_Directory::FindFirstFile(std::string &str)
{
	ACE_UNUSED_ARG(str);
	DIR *dpdf;
	struct dirent *epdf;
	std::list<std::string> L;
	dpdf = opendir("./");
	if (dpdf != NULL) {
		while ((epdf = readdir(dpdf)) ){
			L.push_back(epdf->d_name);
		}
	}

	closedir(dpdf);
	//delete epdf;

	return L;

}
