//======================================================================
//
// NAME
//      Directory.cpp
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
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-07 by EEDSTL
// CHANGES
//     
//======================================================================

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <ace/ACE.h>
#include <syslog.h>
//#include <ACE_OS_NS_sys_stat.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_tra.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class Directory
	//================================================================================

	Directory::Directory(const string& dirName) : name(dirName)
	{
	}

	Directory::~Directory()
	{
	}


	void Directory::clear(bool recursive) const
	{
		DEBUG("%s","Entering Directory::clear()");
		ACE_Dirent_Selector ds;

		this->open(ds);

		vector<string> fileNamesError;

		try
		{
			for (int i = 0; i < ds.length(); i++)
			{
				ACE_stat state;
				string fileName = this->name + '/' + ds[i]->d_name;

				if (ACE_OS::stat(fileName.c_str(), &state) == 0)
				{
					int fileType = state.st_mode;

					if ((fileType & S_IFREG) == S_IFREG)
					{
						if (ACE_OS::unlink(fileName.c_str()) != 0)
						{
							stringstream s;
							s << "Could not remove file '" << fileName << "'. Cause: " << ACE_OS::strerror(ACE_OS::last_error()) << "." << endl;
							fileNamesError.push_back(s.str());
						}
					}
					else if ((fileType & S_IFDIR) == S_IFDIR && fileName[fileName.size() - 1] != '.')
					{
						if (recursive)
						{
							Directory subdir(fileName);
							subdir.remove();
						}
					}
				}
			}
		}
		catch (...)
		{
			this->close(ds);
			throw;
		}

		this->close(ds);

		if (!fileNamesError.empty())
		{
			stringstream s;

			for (unsigned int i = 0; i < fileNamesError.size(); i++)
				s << fileNamesError[i];

			ACS_HCS_THROW(Directory::ExceptionIo, "clear()", "Error clearing directory '" << this->name << "'.", s.str());
		}
		DEBUG("%s","Leaving Directory::clear()");
	}

	void Directory::close(ACE_Dirent_Selector& ds) const
	{
		DEBUG("%s","Entering Directory::close()");
		if (ds.close() == -1)
			ACS_HCS_THROW(Directory::ExceptionIo, "close()", "Error closing directory '" << this->name << "'.", "");
		DEBUG("%s","Leaving Directory::close()");
	}

	void Directory::create()
	{
		DEBUG("%s","Entering Directory::create()");
		if (this->exists())
			return;

		size_t posEnd = this->name.find_last_of("/\\");

		if (posEnd != string::npos)
		{
			Directory dir(this->name.substr(0, posEnd));
			cout << "After last of: " << this->name << endl;
			dir.create();
		}
		
		cout << "Creating: " << this->name << endl;

		ACE_OS::umask(0002);

		if (ACE_OS::mkdir((this->name.c_str()), (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0)  // Fix for TR HX19945
		{
			unsigned int errNo = ACE_OS::last_error();

			if (errNo != EEXIST)
			{
				syslog(LOG_INFO, "Creation error: %d", errNo);
				ACS_HCS_THROW(Directory::ExceptionIo, "create()", "Error creating directory '" << this->name << "'.", ACE_OS::strerror(errNo));
			}
		}
		DEBUG("%s","Leaving Directory::create()" );
	}

	void Directory::createDir(const string& dirName)
	{
		DEBUG("%s","Entering Directory::createDir()");
		Directory dir(dirName);
		try
		{
			dir.create();
		}
		catch(...)
		{
			ACS_HCS_THROW(Directory::ExceptionIo, "createDir()", "Error creating directory" , ACE_OS::strerror(errno));
		}
		DEBUG("%s","Leaving Directory::createDir()");
	}

	void Directory::createFile(const string& fileName) const
	{
		DEBUG("%s","Entering Directory::createFile()");
		string name = this->name + '/' + fileName;

		try
		{
			ofstream ofs(name.c_str());

			if (ofs.is_open())
				ofs.close();
			else
				ACS_HCS_THROW(Directory::ExceptionIo, "createFile()", "Error creating file '" << fileName << "'.", ACE_OS::strerror(ACE_OS::last_error()));
		}
		catch (...)
		{
			ACS_HCS_THROW(Directory::ExceptionIo, "createFile()", "Error creating file '" << fileName << "'.", ACE_OS::strerror(ACE_OS::last_error()));
		}
		DEBUG("%s","Leaving Directory::createFile()");
	}

	bool Directory::exists() const
	{
		DEBUG("%s","Leaving Directory::exists()");
		return this->exists(this->name);
	}

	bool Directory::exists(string fileName) const
	{
		DEBUG("%s","Entering Directory::exists()");
		if (fileName.find_last_of("./\\", 0) == string::npos && fileName.find_last_of(":", 1) == string::npos)
		{
			fileName = this->name + '/' + fileName;
		}

		ACE_stat state;
	
		if (ACE_OS::stat(fileName.c_str(), &state) != 0)
		{
			unsigned int errNo = ACE_OS::last_error();

			if (errNo == ENOENT)
				return false;
			ACS_HCS_THROW(Directory::ExceptionIo, "exists()", "Error retrieving state of file/directory '" << fileName << "'.", ACE_OS::strerror(errNo));
		}
		DEBUG("%s","Leaving Directory::exists()");
		return true;
	}

	time_t Directory::getModificationTime(string fileName) const
	{
		DEBUG("%s","Entering Directory::getModificationTime()");
		if (fileName.find_last_of("./\\", 0) == string::npos && fileName.find_last_of(":", 1) == string::npos)
			fileName = this->name + '/' + fileName;

		ACE_stat state;

		if (ACE_OS::stat(fileName.c_str(), &state) != 0)
			ACS_HCS_THROW(Directory::ExceptionIo, "getModificationTime()", "Error retrieving modification time of file/directory '" << fileName << "'.", ACE_OS::strerror(ACE_OS::last_error()));

		DEBUG("%s","Leaving Directory::getModificationTime()");
		return state.st_mtime;
	}

	const string& Directory::getName() const
	{
		DEBUG("%s","Leaving Directory::getName()");
		return this->name;
	}

	unsigned long Directory::getSize() const
	{
		DEBUG("%s","Entering Directory::getSize()");
		off_t size = 0;
		ACE_Dirent_Selector ds;

		this->open(ds);

		try
		{
			for (int n = 0; n < ds.length(); ++n)
			{
				ACE_stat state;
				string fileName = this->name + '/' + ds[n]->d_name;

				if (ACE_OS::stat(fileName.c_str(), &state) == 0)
				{
					int fileType = state.st_mode;

					if ((fileType & S_IFREG) == S_IFREG)
					{
						//cout << "ls(): file[" << n << "]=" << ds[n]->d_name << endl;

						size += state.st_size;
					}
					else if ((fileType & S_IFDIR) == S_IFDIR && fileName[fileName.size() - 1] != '.')
					{
						Directory subdir(fileName);
						size += subdir.getSize();
					}
				}
			}
		}
		catch (...)
		{
			this->close(ds);
			throw;
		}

		this->close(ds);

		DEBUG("%s","Leaving Directory::getSize() ");
		return (unsigned long)size;
	}

	Directory::FileNames& Directory::ls(FileNames& fileNames, bool recursive, bool sorted, unsigned int what) const
	{
		DEBUG("%s","Leaving Directory::ls()");
		return this->ls(fileNames, "*", recursive, sorted, what);
	}

	Directory::FileNames& Directory::ls(FileNames& fileNames, const char* wildcard, bool recursive, bool sorted, unsigned int what) const
	{
		DEBUG("%s","Entering Directory::ls()");
		if (sorted)
		{
			std::map<time_t, std::list<string> > sortedFileNames;
			this->lsOldestFirst(sortedFileNames, wildcard, recursive, what);

			std::map<time_t, std::list<string> >::const_iterator sortedFileName;

			for (sortedFileName = sortedFileNames.begin(); sortedFileName != sortedFileNames.end(); ++sortedFileName)
			{
				std::list<string>::const_iterator fileName;

				for (fileName = (*sortedFileName).second.begin(); fileName != (*sortedFileName).second.end(); ++fileName)
					fileNames.push_back(*fileName);
			}

			DEBUG("%s","Leaving Directory::ls()");
			return fileNames;
		}

		ACE_Dirent_Selector ds;

		this->open(ds);

		try
		{
			for (int n = 0; n < ds.length(); ++n)
			{
				if (*(ds[n]->d_name) == '.')
					continue; // Skip hidden files and directories

				ACE_stat state;
				string fileName = this->name + '/' + ds[n]->d_name;

				if (ACE_OS::stat(fileName.c_str(), &state) == 0)
				{
					int fileType = state.st_mode;

					if ((fileType & S_IFREG) == S_IFREG && Directory::matches(ds[n]->d_name, wildcard))
					{
						if ((what & LSW_FILES) == LSW_FILES)
						{
							//cout << "ls(): file[" << n << "]=" << ds[n]->d_name << endl;
							fileNames.push_back(fileName);
						}
					}
					else if ((fileType & S_IFDIR) == S_IFDIR && fileName[fileName.size() - 1] != '.')
					{
						if ((what & LSW_DIRS) == LSW_DIRS)
						{
							//cout << "ls(): dir[" << n << "]=" << ds[n]->d_name << endl;
							fileNames.push_back(fileName);
						}

						if (recursive)
						{
							//cout << "ls(): dir[" << n << "]=" << ds[n]->d_name << endl;
							Directory subdir(fileName);
							subdir.ls(fileNames, wildcard, recursive, sorted, what);
						}
					}
				}
			}
		}
		catch (...)
		{
			this->close(ds);
			throw;
		}

		this->close(ds);

		DEBUG("%s","Leaving Directory::ls()");
		return fileNames;
	}

	std::map<time_t, std::list<string> >& Directory::lsOldestFirst(std::map<time_t, std::list<string> >& fileNames, const char* wildcard, bool recursive, unsigned int what) const
	{
		DEBUG("%s","Entering Directory::lsOldestFirst()");
		ACE_Dirent_Selector ds;

		this->open(ds);

		try
		{
			for (int n = 0; n < ds.length(); ++n)
			{
				if (*(ds[n]->d_name) == '.')
					continue;  // Skip hidden files and directories

				ACE_stat state;
				string fileName = this->name + '/' + ds[n]->d_name;

				if (ACE_OS::stat(fileName.c_str(), &state) == 0)
				{
					int fileType = state.st_mode;

					if ((fileType & S_IFREG) == S_IFREG && Directory::matches(ds[n]->d_name, wildcard))
					{
						if ((what & LSW_FILES) == LSW_FILES)
						{
							//cout << "lsOldestFirst(): file[" << n << "]=" << ds[n]->d_name << endl;
							fileNames[state.st_mtime].push_back(fileName);
						}
					}
					else if ((fileType & S_IFDIR) == S_IFDIR && fileName[fileName.size() - 1] != '.')
					{
						if ((what & LSW_DIRS) == LSW_DIRS)
						{
							//cout << "lsOldestFirst(): dir[" << n << "]=" << ds[n]->d_name << endl;
							fileNames[state.st_mtime].push_back(fileName);
						}

						if (recursive)
						{
							//cout << "lsOldestFirst(): dir[" << n << "]=" << ds[n]->d_name << endl;
							Directory subdir(fileName);
							subdir.lsOldestFirst(fileNames, wildcard, recursive, what);
						}
					}
				}
			}
		}
		catch (...)
		{
			this->close(ds);
			throw;
		}

		this->close(ds);

		DEBUG("%s","Leaving Directory::lsOldestFirst() ");
		return fileNames;
	}

	bool Directory::matches(const string& fileName, const char* wildcard)
	{
		//DEBUG("%s","Entering Directory::matches()");
		if (wildcard == 0)
			return true;

		string pattern = wildcard;

		bool matchFromStart = (*(pattern.rbegin())  == '*');
		bool matchTillEnd   = (*(pattern.begin()) == '*');

		pattern.erase(pattern.find_last_not_of("*") + 1).erase(0, pattern.find_first_not_of("*"));

		if (fileName.size() < pattern.size())
		{
			DEBUG("%s","Leaving Directory::matches()");
			return false;
		}
		if (matchFromStart && matchTillEnd)
		{
			 DEBUG("%s","Leaving Directory::matches()");
			return (pattern.empty() || fileName.compare(pattern) == 0);
		}
		if (matchFromStart)
		{
			 DEBUG("%s","Leaving Directory::matches()");
			return (fileName.find(pattern.c_str(), 0, pattern.size()) != string::npos);
		}
		
		if (matchTillEnd)
		{
			 DEBUG("%s","Leaving Directory::matches()");
			return (fileName.find(pattern.c_str(), fileName.size() - pattern.size(), pattern.size()) != string::npos);
		}
		//DEBUG("%s","Leaving Directory::matches()");
		return (fileName.find(pattern) != string::npos);
	}

	void Directory::moveTo(const string& dirName)
	{
		DEBUG("%s","Entering Directory::moveTo()");
		ACE_Dirent_Selector ds;

		this->open(ds);

		try
		{
			for (int n = 0; n < ds.length(); ++n)
			{
				ACE_stat state;
				string nameFrom = this->name + '/' + ds[n]->d_name;
				string nameTo   = dirName    + '/' + ds[n]->d_name;

				if (ACE_OS::stat(nameFrom.c_str(), &state) == 0)
				{
					int fileType = state.st_mode;

					if ((fileType & S_IFDIR) == S_IFDIR && nameFrom[nameFrom.size() - 1] == '.')
						continue;

					if (ACE_OS::rename(nameFrom.c_str(), nameTo.c_str()) == -1)
						ACS_HCS_THROW(Directory::ExceptionIo, "moveTo()", "Could not move file or directory '" << nameFrom << "' to '" << nameTo << "'.", ACE_OS::strerror(ACE_OS::last_error()));
				}
			}
		}
		catch (...)
		{
			this->close(ds);
			throw;
		}

		this->close(ds);
		DEBUG("%s","Leaving Directory::moveTo()");
	}

	void Directory::open(ACE_Dirent_Selector& ds) const
	{
		DEBUG("%s","Entering Directory::open()");
		int status;

		//status = ds.open (ACE_TEXT (TEST_DIR), selector, comparator);
		status = ds.open(this->name.c_str());

		if (status == -1) 
			ACS_HCS_THROW(Directory::ExceptionIo, "open()", "Error opening directory '" << this->name << "'.", "");
		DEBUG("%s","Leaving Directory::open()");
	}

	void Directory::remove()
	{
		//DEBUG("%s","Entering Directory::remove()");
		if (!this->exists())
		{
			 DEBUG("%s","Leaving Directory::remove()");
			return;
		}

		this->clear(true);
		ACE_OS::rmdir(this->name.c_str());
		//DEBUG("%s","Leaving Directory::remove()");
	}

	void Directory::remove(string fileName) const
	{
		//DEBUG("%s","Entering Directory::remove()");
		if (fileName.find_last_of("./\\", 0) == string::npos && fileName.find_last_of(":", 1) == string::npos)
			fileName = this->name + '/' + fileName;

		ACE_stat state;

		if (ACE_OS::stat(fileName.c_str(), &state) == 0)
		{
			int fileType = state.st_mode;

			if ((fileType & S_IFREG) == S_IFREG)
			{
				if (ACE_OS::unlink(fileName.c_str()) != 0)
					ACS_HCS_THROW(Directory::ExceptionIo, "remove()", "Could not remove file '" << fileName << "'.", ACE_OS::strerror(ACE_OS::last_error()));
			}
			else if ((fileType & S_IFDIR) == S_IFDIR && fileName[fileName.size() - 1] != '.')
			{
				Directory subdir(fileName);
				subdir.remove();
			}
		}
		//DEBUG("%s","Leaving Directory::remove()");
	}

	void Directory::rename(string fileNameFrom, string fileNameTo) const
	{
		DEBUG("%s","Entering Directory::rename()");
		if (fileNameFrom.find_last_of("./\\", 0) == string::npos && fileNameFrom.find_last_of(":", 1) == string::npos)
			fileNameFrom = this->name + '/' + fileNameFrom;

		if (fileNameTo.find_last_of("./\\", 0) == string::npos && fileNameTo.find_last_of(":", 1) == string::npos)
			fileNameTo = this->name + '/' + fileNameTo;

		ACE_stat state;

		if (ACE_OS::stat(fileNameFrom.c_str(), &state) == 0)
		{
			int fileType = state.st_mode;

			if ((fileType & S_IFDIR) == S_IFDIR && fileNameFrom[fileNameFrom.size() - 1] == '.')
				return;

			if (ACE_OS::rename(fileNameFrom.c_str(), fileNameTo.c_str()) == -1)
				ACS_HCS_THROW(Directory::ExceptionIo, "rename()", "Could not move file or directory '" << fileNameFrom << "' to '" << fileNameTo << "'.", ACE_OS::strerror(ACE_OS::last_error()));
		}
		DEBUG("%s","Leaving Directory::rename()");
	}

	void Directory::copyFile(string fileNameFrom, string fileNameTo) const
	{
		DEBUG("%s","Entering Directory::copyFile()");
		if (fileNameFrom.find_last_of("./\\", 0) == string::npos && fileNameFrom.find_last_of(":", 1) == string::npos)
			fileNameFrom = this->name + '/' + fileNameFrom;

		if (fileNameTo.find_last_of("./\\", 0) == string::npos && fileNameTo.find_last_of(":", 1) == string::npos)
			fileNameTo = this->name + '/' + fileNameTo;

		ifstream ifs(fileNameFrom.c_str());

		if (ifs.is_open())
		{
			if (ifs.good())
			{
				ofstream ofs(fileNameTo.c_str());

				if (ofs.is_open())
				{
					const int BUF_SIZE = 8192;
					char buf[BUF_SIZE];

					while (ifs.good())
					{
						ifs.read(buf, BUF_SIZE);
						ofs.write(buf, ifs.gcount());
					}

					ofs.close();
				}
				else
				{
					ACS_HCS_THROW(Directory::ExceptionIo, "copy()", "Could not copy file '" << fileNameFrom << "' to '" << fileNameTo << "'.", ACE_OS::strerror(ACE_OS::last_error()));
				}
			}

			ifs.close();
		}
		else
		{
			ACS_HCS_THROW(Directory::ExceptionIo, "copy()", "Could not copy file '" << fileNameFrom << "' to '" << fileNameTo << "'.", ACE_OS::strerror(ACE_OS::last_error()));
		}
		DEBUG("%s","Leaving Directory::copyFile()");
	}
}
