//======================================================================
//
// NAME
//      acs_hcs_cache.h
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
//      Cache for storing the temporary files.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-30 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Cache_h
#define AcsHcs_Cache_h

#include <map>
#include <string>

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Atomic_Op.h>
//#include <Lock.h> 
//#include <Mutex.h> 

#include "acs_hcs_tra.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_synchron.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_directory.h"

namespace AcsHcs
{
	/**
	* Cache - Caches files.
	* This class is a Singleton.
	*/

	typedef ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> count;

	class Cache : public Directory
	{
		friend class ACE_Singleton<Cache, ACE_Thread_Mutex>;

	public:
		/**
		* Use type Singleton to obtain the singleton instance.
		* Example: Cache::Singleton::get();
		*/
		typedef Synchron::Singleton<Cache> Singleton;

		/**
		* Cache::ExceptionIo - IO Exception.
		* Cache::ExceptionIo is thrown in the case of IO problems.
		*/
		class ExceptionIo : public Exception
		{
		public:
			ExceptionIo(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionIo() throw() {}
		};

		/**
		* Destructor.
		*/
		~Cache();

		/**
		* Clear the Cache, i.e. remove all files stored.
		* @throws Cache::ExceptionIo
		*/
		void clear();

		/**
		* Move all files and directories from this Cache to the directory pointed to by 'dirName'.
		* @param[in] The name of the directory where everything should be moved to.
		*/
		void moveTo(const std::string& dirName);

		/**
		* Remove the Cache.
		* @throws Cache::ExceptionIo
		*/
		void remove();

		/**
		* Get the name of the file indexed by 'key'.
		* If there is no related file, an empty string is returned.
		* @param[in] key The key for which the file name shall be returned.
		* @return The name of the file indexed by 'key' or empty string if not found.
		* @throws Cache::ExceptionIo
		*/
		std::string get(const std::string& key) const;
		std::string get(const DocumentXml::Node& key) const;

		/**
		* Create the Cache.
		* @throws Cache::ExceptionIo
		*/
		void create();

		/**
		* Creates an empty file for key 'key'.
		* Users of this method are supposed to use the file name returned for further processing.
		* @param[in] key The key for which the file shall be created.
		* @return The name of the file created.
		* @throws Cache::ExceptionIo
		*/
		std::string touch(const std::string& key);
		std::string touch(const DocumentXml::Node& key);

	private:
                static count cnt; 	

		Cache();
		Cache(const Cache&);
		Cache& operator=(const Cache&);

		std::string getFileName() const;

		std::map<std::string, std::string> fileNames;
		mutable ACE_RW_Thread_Mutex mutexFileNames;
	};
}



#endif // AcsHcs_Cache_h
