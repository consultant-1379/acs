//======================================================================
//
// NAME
//      Directory.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Implementation of file and directory related operations in HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-07 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Directory_h
#define AcsHcs_Directory_h

#include <list>
#include <map>
#include <string>

#include <ace/Dirent_Selector.h>

#include "acs_hcs_exception.h"

namespace AcsHcs
{
	/**
	* Directory - OS-independent representation of a Directory.
	*/
	class Directory
	{
	public:
		/**
		* FileNames - List of file names.
		*/
		typedef std::list<std::string> FileNames;

		/**
		* Enumerates possible types of elements to be listed when calling method ls().
		*/
		enum LsWhat
		{
			LSW_FILES = 0x1,
			LSW_DIRS  = 0x2,
			LSW_FILES_DIRS  = LSW_FILES | LSW_DIRS
		};

		/**
		* Directory::ExceptionIo - IO Exception.
		* Directory::ExceptionIo is thrown in the case of IO problems.
		*/
		class ExceptionIo : public Exception
		{
		public:
			ExceptionIo(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionIo() throw() {}
		};

		/**
		* Constructor.
		* @param[in] dirName The name of the directory.
		*/
		Directory(const std::string& dirName);

		/**
		* Destructor.
		*/
		virtual ~Directory();

		/**
		* Remove all files from this Directory.
		* @param[in] recursive If true, also subdirectories are considered.
		* @throws Directory::ExceptionIo
		*/
		virtual void clear(bool recursive = false) const;

		/**
		* Create the Directory in the file system if it does not exist.
		* @throws Directory::ExceptionIo
		*/
		virtual void create();

		/**
		* Create the directory in the file system if it does not exist.
		* @param[in] dirName The name of the directory to be created.
		* @throws Directory::ExceptionIo
		*/
		static void createDir(const std::string& dirName);

		/**
		* Create the file in the file system if it does not exist.
		* @param[in] fileName The name of the file to be created, relative to this Directory.
		* @throws Directory::ExceptionIo
		*/
		virtual void createFile(const std::string& fileName) const;

		/**
		* Check existence of the Directory in the file system.
		* @return true if the directory exists, false otherwise.
		* @throws Directory::ExceptionIo
		*/
		virtual bool exists() const;

		/**
		* Check existence of a file or directory.
		* @param[in] fileName The name of the file or directory to be removed.
		* @throws Directory::ExceptionIo
		*/
		virtual bool exists(std::string fileName) const;

		/**
		* Get the modification time of a file or directory.
		* @param[in] fileName The name of the file or directory.
		* @throws Directory::ExceptionIo
		*/
		time_t getModificationTime(std::string fileName) const;

		/**
		* Get the name of this Directory.
		* @return The name of this Directory.
		*/
		const std::string& getName() const;

		/**
		* Get the size in bytes of this Directory.
		* @return The size in bytes of this Directory.
		*/
		unsigned long getSize() const;

		/**
		* List all files of this Directory and return them in 'fileNames'.
		* @param[inout] fileNames The file names of all files of this Directory.
		* @param[in]    wildcard  Wildcard for files to be listed.
		* @param[in]    recursive If true, also subdirectories are considered.
		* @param[in]    sorted    If true, the list of file names is sorted according to modification time, oldest first.
		* @param[in]    what      Bitvector indicating what types of elements (files and/or directories) shall be listed.
		* @return The list of file names found.
		* @throws Directory::ExceptionIo
		*/
		virtual FileNames& ls(FileNames& fileNames, bool recursive = false, bool sorted = false, unsigned int what = LSW_FILES) const;
		virtual FileNames& ls(FileNames& fileNames, const char* wildcard, bool recursive = false, bool sorted = false, unsigned int what = LSW_FILES) const;

		/**
		* Move all files and directories from this Directory to the directory pointed to by 'dirName'.
		* @param[in] The name of the directory where everything should be moved to.
		* @throws Directory::ExceptionIo
		*/
		virtual void moveTo(const std::string& dirName);

		/**
		* Remove the directory from the file system, including all subdirectories.
		*/
		virtual void remove() ;

		/**
		* Remove a file or directory from the file system.
		* In the case of a directory also all subdirectories are removed.
		* @param[in] fileName The name of the file or directory to be removed.
		* @throws Directory::ExceptionIo
		*/
		virtual void remove(std::string fileName) const;

		/**
		* Rename file 'fileNameFrom' to 'fileNameTo'.
		* @param[in] fileNameFrom The file name of the file to be renamed.
		* @param[in] fileNameTo   The new name of file 'fileNameFrom'.
		* @throws Directory::ExceptionIo
		*/
		virtual void rename(std::string fileNameFrom, std::string fileNameTo) const;

		/**
		* Copy file 'fileNameFrom' to 'fileNameTo'.
		* @param[in] fileNameFrom The file name of the source file.
		* @param[in] fileNameTo   The file name of the destination file.
		* @throws Directory::ExceptionIo
		*/
		virtual void copyFile(std::string fileNameFrom, std::string fileNameTo) const;

	private:
		static bool matches(const std::string& fileName, const char* wildcard);

		Directory();
		Directory(const Directory&);
		Directory& operator=(const Directory&);

		std::map<time_t, std::list<std::string> >& lsOldestFirst(std::map<time_t, std::list<std::string> >& fileNames, const char* wildcard, bool recursive, unsigned int lsTypes) const;

		void close(ACE_Dirent_Selector& ds) const;
		void open(ACE_Dirent_Selector& ds) const;

		const std::string name;
	};
}

#endif // AcsHcs_Directory_h
