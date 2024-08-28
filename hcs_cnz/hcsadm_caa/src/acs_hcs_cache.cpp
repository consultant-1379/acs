//======================================================================
//
// NAME
//      Cache.cpp
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
//      2011-05-30 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <ace/ACE.h>
#include <ace/Synch.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_cache.h"
#include "acs_hcs_tracer.h"
#include "acs_hcs_global_ObjectImpl.h"
using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Class Cache
	//================================================================================

       //ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> Cache::count =0 ;
        count Cache::cnt = 0;

     
	Cache::Cache() : Directory(Configuration::Singleton::get()->getCacheDir())
	{
	}

	Cache::~Cache()
	{
	}

	void Cache::clear()
	{
		//DEBUG("%s","Entering Cache::clear()");
	//	Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard(this->mutexFileNames);
		time_t now = /*ACE_OS::*/time(0);
		std::list<string> expiredFiles;
		set<string>  validFiles;

		// Collect all expired and valid files in the cache.
		// Remove the expired ones.

		for (map<string, string>::const_iterator it = this->fileNames.begin(); it != this->fileNames.end(); ++it)
		{
			const string& fileName = (*it).second;
			//time_t expirationTime = ACE_OS::strtoul(fileName.substr(fileName.size() - 18, 10).c_str(), 0, 10);
			
			//start of TR HT95978
			std::size_t found = fileName.find_last_of("/\\");
			time_t expirationTime = ACE_OS::strtoul(fileName.substr(found+1,10).c_str(), 0, 10);
			// end of TR HT95978
			
			if (expirationTime <= now)
			{
				expiredFiles.push_back((*it).first);

				try
				{
					Directory::remove(fileName);
				}
				catch (const Directory::ExceptionIo& ex)
				{
					//ACS_HCS_TRACE("clear(): Could not remove file '" << fileName << "' from cache. Cause: " << ex.info());
				}
				catch (...)
				{
					//ACS_HCS_TRACE("clear(): Could not remove file '" << fileName << "' from cache. Cause: Unknown error.");
				}
			}
			else
			{
				validFiles.insert(fileName);
			}
		}

		// Synchronise the internal registry.

		std::list<string>::const_iterator expiredFile;

		for (expiredFile = expiredFiles.begin(); expiredFile != expiredFiles.end(); ++expiredFile)
			this->fileNames.erase(*expiredFile);

		// Remove all other files which are unknown to the cache.

		Directory::FileNames allFiles;
		this->ls(allFiles);

		for (Directory::FileNames::const_iterator fileName = allFiles.begin(); fileName != allFiles.end(); ++fileName)
		{
			if (validFiles.find(*fileName) == validFiles.end())
			{
				try
				{
					Directory::remove(*fileName);
				}
				catch (const Directory::ExceptionIo& ex)
				{
					//ACS_HCS_TRACE("clear(): Could not remove file '" << *fileName << "' from cache. Cause: " << ex.info());
				}
				catch (...)
				{
					//ACS_HCS_TRACE("clear(): Could not remove file '" << *fileName << "' from cache. Cause: Unknown error.");
				}
			}
		}
		//DEBUG("%s","Leaving Cache::clear()");
	}

	void Cache::create()
	{
	        //DEBUG("%s"," Entering Cache::create()"); 
		try
		{
			Directory::create();
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ACS_HCS_THROW(Cache::ExceptionIo, "create()", ex.what(), ex.cause());
		}
		//DEBUG("%s","Leaving Cache::create()");
	}

	string Cache::get(const string& key) const
	{
		//DEBUG("%s","Entering Cache::get()");
		Synchron::GuardRead<ACE_RW_Thread_Mutex> guard(this->mutexFileNames);

		map<string, string>::const_iterator it = this->fileNames.find(key);

		if (it == this->fileNames.end())
			return "";

		//DEBUG("%s","Leaving Cache::get()");
		return it->second;
	}

	string Cache::get(const DocumentXml::Node& key) const
	{
		//DEBUG("%s","Leaving Cache::get()");
		return this->get(key.toString());
	}

	string Cache::getFileName() const
	{
		//DEBUG("%s","Entering  Cache::getFileName()");
		stringstream s;
		s << (/*ACE_OS::*/time(0) + Configuration::Singleton::get()->getCacheTtl()) << 0 << setw(3) << setfill('0') << (Cache::cnt++) << ".txt";

		//DEBUG("%s","Leaving  Cache::getFileName()");
		return s.str();
	}

	void Cache::moveTo(const string& dirName)
	{
		DEBUG("%s","Entering Cache::moveTo()");
		try
		{
			Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard(this->mutexFileNames);

			this->fileNames.clear();
			Directory::moveTo(dirName);
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ACS_HCS_THROW(Cache::ExceptionIo, "moveTo()", ex.what(), ex.cause());
		}
		DEBUG("%s","Leaving Cache::moveTo()");
	}

	void Cache::remove()
	{
		DEBUG("%s","Entering Cache::remove()");
		try
		{
			Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard(this->mutexFileNames);

			this->fileNames.clear();
			Directory::remove();
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ACS_HCS_THROW(Cache::ExceptionIo, "remove()", ex.what(), ex.cause());
		}
		DEBUG("%s","Leaving Cache::remove()");
	}

	string Cache::touch(const string& key)
	{
		//DEBUG("%s","Entering Cache::touch()");
		Synchron::GuardWrite<ACE_RW_Thread_Mutex> guard(this->mutexFileNames);

		map<string, string>::iterator it = this->fileNames.find(key);

		if (it != this->fileNames.end())
			return it->second;

		try
		{
			string fileName = this->getFileName();
		
			this->createFile(fileName);

			fileName = this->getName() + '/' + fileName;
			this->fileNames[key] = fileName;

			return fileName;
		}
		catch (const Directory::ExceptionIo& ex)
		{
			ACS_HCS_THROW(Cache::ExceptionIo, "touch()", "Could not touch file.", ex.info());
		}
		//DEBUG("%s","Leaving Cache::touch()");
		return EMPTY_STR;
	}

	string Cache::touch(const DocumentXml::Node& key)
	{
		//DEBUG("%s","Leaving Cache::touch()");
		return this->touch(key.toString());
	}
}
