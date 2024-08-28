

	#include <acs_logm_base.h>

	acs_logm_base::acs_logm_base()
	{
		 DEBUG(" %s","Entering acs_logm_base");
		 DEBUG(" %s","Leaving acs_logm_base");
	}
	acs_logm_base::~acs_logm_base()
	{
		DEBUG(" %s","Entering ~acs_logm_base");
		DEBUG(" %s","Leaving ~acs_logm_base");
	}
	bool acs_logm_base::isFileObjectDirectory(string& aPath)
	{
		DEBUG("Entering isFileObjectDirectory %s",aPath.c_str());
		bool myDirFlag = false;
		DIR *pDir = NULL;

		pDir 	= opendir(aPath.c_str());
		if(pDir != NULL)
		{
			DEBUG("%s -- is a directory",aPath.c_str());
			myDirFlag = true;
			closedir (pDir);
		}
		DEBUG("Leaving isFileObjectDirectory %s",aPath.c_str());
		return myDirFlag;
	}
	bool acs_logm_base::isFileExists(string& aPath)
	{
		DEBUG("Entering isFileExists %s",aPath.c_str());
		bool myReturnVal = false;
		FILE *pFile = 0;
		pFile = fopen(aPath.c_str(),"r");
		if (pFile != 0)
		{
			fclose(pFile);
			myReturnVal = true;
		}
		else
		{
			DEBUG("File does not exist  %s",aPath.c_str());
			myReturnVal = false;
		}

		DEBUG("Leaving isFileExists %s",aPath.c_str());
		return myReturnVal;
	}
	bool acs_logm_base::isDirectoryEmpty(std::string& aPath)
	{
		DEBUG("Entering isDirectoryEmpty %s",aPath.c_str());
		int count=0;
		DIR *pDir;
		pDir = opendir(aPath.c_str());
		if(pDir == NULL)
		{
			ERROR("Error in opening directory %s",aPath.c_str());
			DEBUG("Leaving isDirectoryEmpty %s",aPath.c_str());
			return false;
		}
		while(readdir(pDir)!= NULL)
		{
			++count;
		}
		closedir (pDir);
		if (count == 2)
		{
			return true;
		}
		DEBUG("Leaving isDirectoryEmpty %s",aPath.c_str());
		return false;
	}

	bool acs_logm_base::getElapsedTimeSincelastModified(string& aPath,double & aElapsedTime )
	{
		DEBUG("Entering getElapsedTimeSincelastModified %s",aPath.c_str());

		bool myRes =  acs_logm_filetime::isFileObjectExists(aPath);
		if(!myRes)
		{
			ERROR("%s","Invalid File Path");
			aElapsedTime = 0;
			return false;
		}
		acs_logm_filetime myFileTime(aPath);
		aElapsedTime = myFileTime.elapsedTimeWithCurrentTime();
		if( aElapsedTime < 0)
		{
			DEBUG("getElapsedTimeSincelastModified is negative %f",aElapsedTime);
			DEBUG("Leaving getElapsedTimeSincelastModified %s",aPath.c_str());
			return false;
		}
		DEBUG("getElapsedTimeSincelastModified %f",aElapsedTime);
		DEBUG("Leaving getElapsedTimeSincelastModified %s",aPath.c_str());
		return true;

	}

	bool acs_logm_base::deletedirectory(string& aPath)
	{
		DEBUG("Entering deleteFulldirectory %s",aPath.c_str());
		DIR *pDir = 0;
		struct dirent *entry = 0;
		std::list<std::string> myFileList;
		pDir = opendir(aPath.c_str());
		if(pDir == NULL)
		{
			ERROR("Error - Unable to Open Directory %s",aPath.c_str());
			DEBUG("Leaving deleteFulldirectory %s",aPath.c_str());
			return false;
		}
		while((entry = readdir(pDir))!= NULL)
		{
			std::string myStr(entry->d_name);
			if( myStr != "." && myStr != "..")
			{
				std::string myFullPath = aPath + std::string("/") + myStr;
				myFileList.push_back(myFullPath);
			}
		}
		closedir (pDir);
		std::list<std::string>::iterator myIter = myFileList.begin();
		std::list<std::string>::iterator myIterEnd = myFileList.end();
		int myReturnVal = 0;
		bool myFileRemoveFlag = true;
		for(;myIter != myIterEnd;++myIter)
		{
			if(isFileObjectDirectory((*myIter)))
			{
				DEBUG("Calling Function Recursively %s","acs_logm_base::deleteFulldirectory");
				myFileRemoveFlag = deletedirectory((*myIter));
			}
			else
			{
				myReturnVal  = remove((*myIter).c_str());
				if(myReturnVal != 0)
				{
					DEBUG("Could not delete File %s",(*myIter).c_str());
					myFileRemoveFlag = false;
				}
			}
		}
		if ( myFileRemoveFlag  == true)
		{
			int myRetVal = rmdir(aPath.c_str());
			if(myRetVal != 0)
			{
				ERROR("Could not delete directory %s",aPath.c_str());
				DEBUG("Leaving deleteFulldirectory %s",aPath.c_str());
				return false;
			}
			DEBUG("Leaving deleteFulldirectory %s",aPath.c_str());
			return true;
		}
		else
		{
			DEBUG("Error in removing one of the file in directory %s",aPath.c_str());
			DEBUG("Leaving deleteFulldirectory %s",aPath.c_str());
			return false;

		}
	}

	bool acs_logm_base::deleteEmptydirectory(string& aPath)
	{
		DEBUG("Entering deleteEmptydirectory %s",aPath.c_str());
		int myRetVal = rmdir(aPath.c_str());
		if(myRetVal != 0)
		{
			ERROR("Could not delete directory %s",aPath.c_str());
			DEBUG("Leaving deleteEmptydirectory %s",aPath.c_str());
			return false;
		}
		DEBUG("Leaving deleteEmptydirectory %s",aPath.c_str());
		return true;
	}
	bool acs_logm_base::deleteFile(string& aPath)
	{
		DEBUG("Entering deleteFile %s",aPath.c_str());
		int myReturnVal  = remove(aPath.c_str());
		if(myReturnVal != 0)
		{
			ERROR("Could not  deleteFile %s",aPath.c_str());
			DEBUG("Leaving deleteFile %s",aPath.c_str());
			return false;
		}
		DEBUG("Leaving deleteFile %s",aPath.c_str());
		return true;
	}




