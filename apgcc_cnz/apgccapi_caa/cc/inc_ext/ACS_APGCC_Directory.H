/*
 * ACS_APGCC_Directory.H
 *
 *  Created on: May 24, 2010
 *      Author: administrator
 */

#ifndef ACS_APGCC_DIRECTORY_H
#define ACS_APGCC_DIRECTORY_H

#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

#ifndef INCLUDED_LIST
#  include <list>
#  define INCLUDED_LIST
#endif
#  include "ACS_APGCC_OS.H"
#include <dirent.h>


class ACS_APGCC_Directory
{
public:

	typedef enum { FILE=0, DIRECTORY=1, DLL=2 } FileType;

	class FileData {
   public:
      FileData();
      FileData(const FileData&);
      ~FileData();
      FileData& operator=(const FileData&);

      // Data members
		std::string cName;   // Complete file name
      std::string name;    // File name without extension
      std::string ext;     // Extension
		FileType type;       // Type of file
	};

	typedef std::list<FileData> List;
	typedef List::iterator iterator;

	ACS_APGCC_Directory();
	virtual ~ACS_APGCC_Directory();

   bool read(const std::string& fileMask="*", const std::string& startDir="");
   std::list<std::string> FindFirstFile(std::string &str) ;
   iterator begin();
   iterator end();

   List& list();
   const List& list() const;

protected:

   List fileList_;

private:

   ACS_APGCC_Directory(const ACS_APGCC_Directory&);
   ACS_APGCC_Directory& operator=(const ACS_APGCC_Directory&);

};



#endif /* ACS_APGCC_DIRECTORY_H */
