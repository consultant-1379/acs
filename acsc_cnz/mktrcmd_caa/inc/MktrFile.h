/*
 * MktrFile.h
 *
 *  Created on: Sep 3, 2010
 *      Author: xludesi
 */

#ifndef MKTRFILE_H_
#define MKTRFILE_H_

#include <string>
#include <list>
#include "MktrDate.h"
#include "MktrCmd.h"

class MktrFile
{
public:

	MktrFile(const std::string & path);
	virtual ~MktrFile();

	bool isDir() { return _fileType == FILE_IS_A_DIRECTORY;  }
	bool isFile(){ return _fileType == FILE_IS_A_FILE;  }
	bool exists(){ return _fileType != FILE_NOT_EXISTS; }
	bool isAccessible(){ return _fileType != FILE_PERMISTION_DENIED; }
	std::string name(){ return _fileName;}

	bool createFile();
	bool createDir();			// equivalent to "mkdir <dirpathname>"
	bool createDirEx();		// equivalent to "mkdir -p <localdirpathname>"
	bool createDirEx(const unsigned int permissions);             // equivalent to "mkdir -p -m <permissions> <finaldirpathname>"

	bool renameTo(const std::string& dest, bool force=true);


	// Remove the file (if the object represents a FILE) or the directory (if the object represents a DIRECTORY).
	// Returns true on success, false on failure
	bool remove();

	// Remove the file (if the object represents a FILE) or the directory (if the object represents a DIRECTORY).
	// Returns true on success, false on failure
	bool removeFileFromDir();

	// Returns a list of MktrFile objects, each of which represent a file or a directory item contained in the directory whose path is "_filename"
	std::list<MktrFile> list();

	// Returns a list of MktrFile objects (representing files and/or directories), matching the pattern "fileAlike"
	static std::list<MktrFile> list(const std::string& fileAlike);


	MktrFile* copyTo(const std::string& dest);
	MktrFile* copyPathTo(const std::string& dest);
	std::string getLastErrorMsg();


	MktrDate lastAccessTime();
	MktrDate lastWriteTime();



private:

	enum MktrFileConstants
	{
		FILE_NOT_EXISTS = 0,
		FILE_PERMISTION_DENIED = 1,
		FILE_IS_A_FILE = 2,
		FILE_IS_A_DIRECTORY = 3,
		FILE_IS_OF_ANOTHER_TYPE = 4
	};


	static const int MKDIR_MASK  = 0777;


	unsigned long _errorCode;
	std::string _errorDetail;
	int _fileType;
	std::string _fileName;
	std::string _zipCmdLine;
	time_t _lastAccessFt;
	time_t _lastWriteFt;


	void _setErroMsg(std::string mess, int errorCode);

	bool _copyFile(const std::string& src, const std::string& dest);

	// Returns the list of the (fully qualified) names of the items contained in the directory whose path is "_filename"
	std::list<std::string> _fileNames();

	// Check and set internally the type of the item (FILE or DIRECTOY) corresponding to the name "_filename"
	void _checkFileType();


	static std::string _parentOf(const std::string& path);
};

#endif /* MKTRFILE_H_ */
