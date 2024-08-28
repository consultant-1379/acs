/*
 * MktrFile.cpp
 *
 *  Created on: Sep 3, 2010
 *      Author: xludesi
 */

#include "MktrFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <assert.h>
#include <errno.h>
#include "ace/Log_Msg.h"
#include <iostream>
#include <string.h>


MktrFile::MktrFile(const std::string& name)
{
    this->_fileName = name;
    this->_checkFileType();
}


void MktrFile::_checkFileType()
{
	this->_setErroMsg("",0);
	struct stat fileInfo;
	if(stat(this->_fileName.c_str(),&fileInfo)==0)
	{
		this->_lastAccessFt = fileInfo.st_atime;
		this->_lastWriteFt = fileInfo.st_mtime;
		if(S_ISDIR(fileInfo.st_mode))
			this->_fileType = FILE_IS_A_DIRECTORY;
		else if(S_ISREG(fileInfo.st_mode))
			this->_fileType = FILE_IS_A_FILE;
		else
			this->_fileType = FILE_IS_OF_ANOTHER_TYPE;
	}
	else
    {
		this->_fileType = FILE_NOT_EXISTS;
		this->_setErroMsg("stat() error",errno);
    }
}

std::string MktrFile::_parentOf(const std::string& path)
{
	// To be checked
    std::string parent;
    size_t ind = path.find_last_of("/");
    if(ind != std::string::npos)
    {
        parent = path.substr(0, ind);
    }
    else
    {
        parent = "";
    }
    return parent;
}


std::list<MktrFile> MktrFile::list()
{
    std::list<MktrFile> fileList;
    std::list<std::string> list = this->_fileNames();
    std::list<std::string>::iterator it;
    for(it = list.begin(); it != list.end(); it++)
    {
        MktrFile mktrFile(*it);
        fileList.push_front(mktrFile);
    }
    return fileList;
}


std::list<MktrFile> MktrFile::list(const std::string& fileAlike)
{
	std::list<MktrFile> fileList;

	std::string dir_path;				// we'll set it to the part of the string "fileAlike" containing the directory path
	std::string file_pattern;			// we'll set it to the part of the string "fileAl1ke" containing the file pattern

	// let's get the two parts of "fileAlike"
	size_t pos = fileAlike.find_last_of("/");
	if(pos != std::string::npos)
	{
		dir_path = fileAlike.substr(0,pos);
		if(pos == fileAlike.size() - 1)
			file_pattern = "";
		else
			file_pattern = fileAlike.substr(pos+1);

		// now open directory and scan it
		DIR *dp;
		struct dirent *ep;
		dp = opendir(dir_path.c_str());
		if (dp != NULL)
		{
		   while ((ep=readdir(dp)) != NULL)
		   {
			   if(strcmp(ep->d_name,".")!=0  && strcmp(ep->d_name,".." )!= 0  && fnmatch(file_pattern.c_str(), ep->d_name, FNM_NOESCAPE)==0)
			   {
				   // the current directory entry has a name that matches the supplied file pattern. Add it to the fileList
				   MktrFile nFile(dir_path + "/" + ep->d_name);
				   fileList.push_front(nFile);
			   }
		   }

		   // close directory
		   closedir(dp);
		}

	}

	return fileList;
}


bool MktrFile::createDir()
{
	bool status = false;

	this->_setErroMsg("",0);

	if(mkdir(this->_fileName.c_str(), MktrFile::MKDIR_MASK) != 0)
	{
		if(errno == EEXIST)
			status = true;
		else
		{
			this->_setErroMsg("mkdir() error creating dir " + this->_fileName, errno );
			status = false;
		}
	}
	else
		status = true;

	this->_checkFileType();

	return status;
}


bool MktrFile::createDirEx()
{
	bool retVal;
	MktrCmd cmd;

	this->_setErroMsg("",0);
	std::string cmdLine = "mkdir -p " + this->_fileName;
	if(cmd.execute(cmdLine)==false)
	{
		//this->_errorCode = errno;
		this->_setErroMsg(cmd.getLastErrorMsg(), errno);
		retVal = false;
	}
	else
	{
		this->_checkFileType();
		retVal = true;
	}

	return retVal;
}

//overloaded method takes folder permission as input to create folder
// Start of TR IA72764
bool MktrFile::createDirEx(const unsigned int permissions)
{
        bool retVal = false;
        MktrCmd cmd;
	char buffer[256];

        this->_setErroMsg("",0);
	sprintf(buffer, "%d", permissions);
        std::string cmdLine = "mkdir -p -m " + std::string(buffer) + " " + this->_fileName;
        if(cmd.execute(cmdLine)==false)
        {
                //this->_errorCode = errno;
                this->_setErroMsg(cmd.getLastErrorMsg(), errno);
                retVal = false;
        }
        else
        {
                this->_checkFileType();
                retVal = true;
        }

        return retVal;
}
// End of TR IA72764


MktrFile* MktrFile::copyTo(const std::string& dest)
{
	this->_setErroMsg("",0);
    if(this->isFile())
    {
        if(this->_copyFile(this->_fileName, dest))
        {
            return new MktrFile(dest);
        }
        else
        {
            return NULL;
        }
    }
    else if(this->isDir())
    {
    	if(mkdir(_fileName.c_str(), MktrFile::MKDIR_MASK) != 0)
        {
            if(errno != EEXIST)
            {
                this->_setErroMsg("mkdir() error creating directory " + _fileName, errno );
                return NULL;
            }
        }
        std::list<std::string> list = this->_fileNames();
        std::list<std::string>::iterator it;

        for(it = list.begin(); it != list.end(); it++)
        {
            std::string src = *it;
            std::string newSrc = this->_fileName+"/";
            newSrc += src;
            std::string newDest = dest+src;
            this->_copyFile(newSrc, newDest);
            //TBD
        }
        return new MktrFile(dest);
    }
    else
    {
        return NULL;
    }
}



MktrFile* MktrFile::copyPathTo(const std::string& dest)
{
	this->_setErroMsg("",0);
    if(this->isFile())
    {
        if(this->_copyFile(this->_fileName, dest))
        {
            return new MktrFile(dest);
        }
        else
        {
            return NULL;
        }
    }
    else if(this->isDir())
    {
    	if(mkdir(_fileName.c_str(), MktrFile::MKDIR_MASK) != 0)
        {
            if(errno != EEXIST)
            {
                this->_setErroMsg("mkdir() error creating directory " + _fileName, errno );
                return NULL;
            }
        }
        std::list<std::string> list = this->_fileNames();
        std::list<std::string>::iterator it;

        for(it = list.begin(); it != list.end(); it++)
        {
            std::string src = *it;
            std::string newSrc = src;//this->_fileName+"/";
            //newSrc += src;
            std::string newDest = dest +"/";
            this->_copyFile(newSrc, newDest);
            //TBD
        }
        return new MktrFile(dest);
    }
    else
    {
        return NULL;
    }
}


bool MktrFile::_copyFile(const std::string& src, const std::string& dst)
{
	/*
	this->_setErroMsg("",0);
	std::string cmdLine = "cp -fp " + src + "  " + dst;
	int retCode = system(cmdLine.c_str());
	if(retCode == 0)
		return true;
	else
	{
		this->_setErroMsg("error copying file " + src + " to " + dst, errno );
		return false;
	}
	*/

	bool retVal = true;
	this->_setErroMsg("",0);

	MktrCmd cmd;
	std::string cmdLine = "cp -fp " + src + "  " + dst;
	if(cmd.execute(cmdLine)==false)
	{
		this->_setErroMsg(cmd.getLastErrorMsg(), errno);
		retVal = false;
	}

	return retVal;
}


std::list<std::string> MktrFile::_fileNames()
{
	std::list<std::string> fileNamesList;

	if(isDir())
    {
		std::string dir_path = this->_fileName;
		std::string file_pattern = "*";

        // open directory and scan it
		DIR *dp;
		struct dirent *ep;
		dp = opendir(dir_path.c_str());
		if (dp != NULL)
		{
		   while ((ep=readdir(dp)) != NULL)
		   {
			   if(strcmp(ep->d_name,".")!=0  && strcmp(ep->d_name,".." )!= 0  && fnmatch(file_pattern.c_str(), ep->d_name, FNM_NOESCAPE)==0)
			   {
				   // the current directory entry has a name that matches the supplied file pattern. Add it to the fileList
				   std::string str = dir_path + "/" + ep->d_name;
				   fileNamesList.push_front(str);
			   }
		   }

		   // close directory
		   closedir(dp);
		}
    }

    return fileNamesList;
}


bool MktrFile::remove()
{
	this->_setErroMsg("",0);

	if(this->isDir())
    {
        // OK, this object represents a DIRECTORY. Delete the files contained in it
    	std::list<MktrFile> fList = list();

    	// invoke recursively to remove the files in the DIRECTORY
    	std::list<MktrFile>::iterator it;
        for(it = fList.begin(); it != fList.end(); it++)
        {
			MktrFile file = *it;

			// recursive invocation
			file.remove();
        }
        fList.clear();


        // now remove the directory
        if(rmdir(this->_fileName.c_str())==0)
        {
            this->_fileName = "";
            this->_checkFileType();
            return true;
        }
        else
        {
        	this->_setErroMsg("remdir() error removing directory " + _fileName, errno);
            return false;
        }
    }
    else if(this->isFile())
    {
    	 // OK, this object represents a FILE; delete it
    	if(::remove(this->_fileName.c_str())==0)
        {
            _fileName = std::string("");
            _fileType = FILE_NOT_EXISTS;
            return true;
        }
        else
        {
        	this->_setErroMsg("remove() error removing file " + _fileName, errno);
        	return false;
        }
    }
	else
	{
		return false;
	}
}

bool MktrFile::removeFileFromDir()
{
	this->_setErroMsg("",0);

	if(this->isDir())
    {
        // OK, this object represents a DIRECTORY. Delete the files contained in it
    	std::list<MktrFile> fList = list();

    	// invoke recursively to remove the files in the DIRECTORY
    	std::list<MktrFile>::iterator it;
        for(it = fList.begin(); it != fList.end(); it++)
        {
			MktrFile file = *it;

			// recursive invocation
			file.remove();
        }
        fList.clear();
        return true;
    }
	else
	{
		return false;
	}
}


bool MktrFile::renameTo(const std::string& dest, bool force)
{
    this->_setErroMsg("",0);

	if(::rename(this->_fileName.c_str(),dest.c_str()) == 0)
	{
		this->_fileName = dest;
		return true;
	}

	// rename failed ! let's see why
	this->_setErroMsg("rename() error renaming " + _fileName + " to " + dest, errno);
	if(force && (errno == EEXIST))
	{
		// destination 'dest' exists.  Since force == true, we try to delete it
		MktrFile oldFile(dest);
		if(oldFile.remove())
		{
			// we have successfully deleted destination; now retry rename...
			if(::rename(this->_fileName.c_str(),dest.c_str()) == 0)
			{
				this->_fileName = dest;
				this->_setErroMsg("",0);
				return true;
			}
			else
			{
				this->_setErroMsg("rename() error renaming " + _fileName + " to " + dest, errno);
			}
		}
	}

	return false;
}


std::string MktrFile::getLastErrorMsg()
{
	char buff[256];
	std::string last_error_descr = strerror_r (this->_errorCode, buff, sizeof(buff));
	return _errorDetail + " ERRDESC = " + last_error_descr;
}


void MktrFile::_setErroMsg(std::string mess, int errorCode)
{
	this->_errorCode = errorCode;
	this->_errorDetail = mess;
}


MktrDate MktrFile::lastAccessTime()
{
	MktrDate retDate;
	struct tm localTime;

	if(localtime_r(&this->_lastAccessFt, &localTime) != NULL)
	{
		MktrDate* date = MktrDate::create(localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, localTime.tm_hour, localTime.tm_min);
		if(date != NULL)
		{
			retDate = *date;
			delete date;
		}
	}

	return retDate;
}

MktrDate MktrFile::lastWriteTime()
{

	MktrDate retDate;
	struct tm localTime;

	if(localtime_r(&this->_lastWriteFt, &localTime) != NULL)
	{
		MktrDate* date = MktrDate::create(localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, localTime.tm_hour, localTime.tm_min);
		if(date != NULL)
		{
			retDate = *date;
			delete date;
		}
	}

	return retDate;
}


MktrFile::~MktrFile()
{
	// TODO Auto-generated destructor stub
}
