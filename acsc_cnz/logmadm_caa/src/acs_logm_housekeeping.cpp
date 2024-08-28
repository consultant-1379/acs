#include<acs_logm_housekeeping.h>
#include<acs_logm_apgccwrapper.h>

using namespace std;

	acs_logm_housekeeping::acs_logm_housekeeping()
:acs_logm_base()
{
	DEBUG("%s","Entering acs_logm_housekeeping");
	theConfigurationData.theNumberofOldDays = 0;
	theConfigurationData.theNumberofFilesToBeMaintained = 0;
	DEBUG("%s","Leaving acs_logm_housekeeping");

}

	acs_logm_housekeeping::acs_logm_housekeeping(acs_logm_configurationdata& aConfigurationData)
:acs_logm_base()
{
	DEBUG("%s","Entering parameter constructor acs_logm_housekeeping");
	theConfigurationData = aConfigurationData;

	DEBUG("%s","Leaving parameter constructoracs_logm_housekeeping");
}

acs_logm_housekeeping::~acs_logm_housekeeping()
{
	DEBUG("%s","Entering ~acs_logm_housekeeping");

	DEBUG("%s","Leaving ~acs_logm_housekeeping");

}

void acs_logm_housekeeping::resetAll()
{
	DEBUG("%s","Entering acs_logm_housekeeping::resetAll()");


	std::list<std::string>::iterator myListIter = theFileObjectsList.begin();
	std::list<std::string>::iterator  myListIterEnd = theFileObjectsList.end();
	for(;myListIter != myListIterEnd;)
	{
		theFileObjectsList.erase(myListIter++);
	}
	std::multimap<double,std::string>::iterator myIter = theTimeSortedFileObjectMap.begin();
	std::multimap<double,std::string>::iterator myIterEnd = theTimeSortedFileObjectMap.end();
	for(;myIter != myIterEnd ;)
	{
		theTimeSortedFileObjectMap.erase(myIter++);
	}

	DEBUG("%s","Leaving acs_logm_housekeeping::resetAll()");
}

void acs_logm_housekeeping::calculateNextCleanUpTime()
{
	DEBUG("Entering acs_logm_housekeeping::calculateNextCleanUpTime for %s",theConfigurationData.theLogName.c_str());
	acs_logm_time myCurrentTime;
	theNextCleanUpTime = myCurrentTime;
	if(theConfigurationData.theCleanupInitiation == acs_logm_configurationdata::TIMEBASED)
	{
		int myNumberDays = 1;
		if(theConfigurationData.theCleanUpFrequency == acs_logm_configurationdata::WEEKLY)
		{
			myNumberDays = 7;
		}
		theNextCleanUpTime.addDays(myNumberDays);
		theNextCleanUpTime.setTime(theConfigurationData.theInitiationHour,theConfigurationData.theInitiationMinute,0);
	}
	else if(theConfigurationData.theCleanupInitiation == acs_logm_configurationdata::PERIODICBASED)
	{
		theNextCleanUpTime.addMinutes(theConfigurationData.thePeriodicInterval);
	}
	DEBUG("The next cleanup time for the %s object is == ",theConfigurationData.theLogName.c_str());
	theNextCleanUpTime.logTimeData();

	DEBUG("Leaving acs_logm_housekeeping::calculateNextCleanUpTime for %s",theConfigurationData.theLogName.c_str());
}
bool acs_logm_housekeeping::extractFolderContent()
{
	DIR *dir ;
	struct dirent *dp  ;
	DEBUG("Entering extractFolderContent %s",theConfigurationData.theFolderPath.c_str());
	dir=opendir(theConfigurationData.theFolderPath.c_str());
	if(dir == NULL)
	{
		ERROR("Error in opening the Directory %s",theConfigurationData.theFolderPath.c_str());
		DEBUG("Leaving extractFolderContent %s",theConfigurationData.theFolderPath.c_str());
		return false;
	}
	else
	{
		while((dp=readdir(dir))!= NULL)
		{
			std::string myStr;
			myStr.reserve(256);
			myStr.assign(dp->d_name);
			std::string tmpString(theConfigurationData.theFolderPath);
			tmpString.append(myStr);
			string myDotString(".");
			string myDoubleDotString("..");
			if((myStr != myDotString) && (myStr != myDoubleDotString))
			{
				DEBUG("Path in list is : %s",tmpString.c_str());
				theFileObjectsList.push_back(tmpString);
			}
		}
		closedir(dir);
	}

	DEBUG("Leaving extractFolderContent %s",theConfigurationData.theFolderPath.c_str());
	return true;

}
bool acs_logm_housekeeping::extractSubDirectories(std::list<std::string>& aSubDirectoryList)
{
	DIR *dir ;
	struct dirent *dp  ;
	DEBUG("Entering acs_logm_housekeeping::extractSubDirectories %s",theConfigurationData.theFolderPath.c_str());
	dir=opendir(theConfigurationData.theFolderPath.c_str());
	if(dir == NULL)
	{
		ERROR("Error in opening the Directory %s",theConfigurationData.theFolderPath.c_str());
		DEBUG("Leaving acs_logm_housekeeping::extractSubDirectories %s",theConfigurationData.theFolderPath.c_str());
		return false;
	}
	else
	{
		while((dp=readdir(dir))!= NULL)
		{
			std::string myStr;
			myStr.reserve(256);
			myStr.assign(dp->d_name);
			std::string tmpString(theConfigurationData.theFolderPath);
			tmpString.append(myStr);
			tmpString.append("/");
			string myDotString(".");
			string myDoubleDotString("..");
			if((myStr != myDotString) && (myStr != myDoubleDotString))
			{
				DEBUG("Path in list is : %s",tmpString.c_str());
				if(isFileObjectDirectory(tmpString) == true)
				{
					aSubDirectoryList.push_back(tmpString);
				}
			}
		}
		closedir(dir);
	}

	DEBUG("Leaving acs_logm_housekeeping::extractSubDirectories %s",theConfigurationData.theFolderPath.c_str());
	return true;
}
bool acs_logm_housekeeping::extractToBeDeletedFileObjects()
{

	return true;
}
bool acs_logm_housekeeping::isSearchPatternAvailable(std::string& aPath)
{
	DEBUG("acs_logm_housekeeping::isSearchPatternAvailable %s",aPath.c_str());
	return true;
}
bool acs_logm_housekeeping::clearOldFileObjects()
{
	DEBUG(" %s","Entering acs_logm_housekeeping::clearOldFileObjects");
	std::multimap<double,std::string>::iterator myIter = theTimeSortedFileObjectMap.begin();
	std::multimap<double,std::string>::iterator myIterStart = theTimeSortedFileObjectMap.begin();
	std::multimap<double,std::string>::iterator myIterEnd = theTimeSortedFileObjectMap.end();

	bool myDeletion = true;
	string coreFilesPath(CORE_PATH);
	coreFilesPath = coreFilesPath + "/";
	if (! strcmp(coreFilesPath.c_str(),theConfigurationData.theFolderPath.c_str()))
	{
		DEBUG(" %s","starting core dumps cleanup activity");
		DIR  *handle;
		struct dirent  *entry;
		struct stat  fileStatus;

		std::multimap<long,string> coreFiles;
		string  fName, dirName(CORE_PATH), pathAbs;
		long totalSize = 0;

		if ( ! coreFiles.empty() )
		{
			coreFiles.clear();
		}

		if ((handle = opendir (dirName.c_str())) == NULL)
		{
			DEBUG("Error in opening %s directory",dirName.c_str());
			return false;
		}
		//DEBUG(" %s","calculating the size of all core dumps together");
		while ((entry = readdir(handle)) != NULL)
		{
			fName = entry->d_name;
			if (fName.substr(0,5).compare("core-") == 0)
			{
				pathAbs.assign(dirName);
				pathAbs = pathAbs + "/" + fName;

				stat (pathAbs.c_str(), &fileStatus);

				totalSize = totalSize + fileStatus.st_size;
				coreFiles.insert(std::pair<long,string>(atoi((fName.substr(fName.find_last_of("-")+1)).c_str()),fName));
			}
		}
		closedir (handle);
		std::multimap<long,string>::iterator coreFiles_iterator;
		if ( ! coreFiles.empty() )
		{
			coreFiles_iterator = coreFiles.begin();
		}
		else
		{
			INFO(" %s","There are no core files at the moment");
			DEBUG(" %s","ending core dumps cleanup activity");
			DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
			return true;
		}

		if ( theConfigurationData.theCleanupInitiation == acs_logm_configurationdata::PERIODICBASED )
		{
			DEBUG(" %s","Periodic based CORE files clean starts");
			DEBUG("size of all core dumps together = %ld",totalSize);

			while (totalSize > FIVE_GIGA) // 5GB
			{
				fName = coreFiles_iterator->second;
				pathAbs.assign(dirName);
				pathAbs = pathAbs + "/" + fName;

				stat (pathAbs.c_str(), &fileStatus);

				totalSize = totalSize - fileStatus.st_size;

				if (!deleteFile(pathAbs))
				{
					myDeletion = false;
					DEBUG("Error in deletion file %s",pathAbs.c_str());
				}
				else
				{
					DEBUG("removed the core dump - %s ",pathAbs.c_str());
					DEBUG("size of all core dumps together after removing one coredump is = %ld",totalSize);
				}

				if (coreFiles_iterator != coreFiles.end())
				{
					coreFiles_iterator++;
				}
				else
				{
					DEBUG(" %s","No core files to delete further");
					break;
				}
			}
			DEBUG(" %s","Periodic based CORE files clean is finished");
		}
		else if ( theConfigurationData.theCleanupInitiation == acs_logm_configurationdata::TIMEBASED )
		{
			DEBUG(" %s","Time based CORE files clean starts");
			for(;coreFiles_iterator != coreFiles.end();++coreFiles_iterator)
			{
				std::string myFileObject = coreFilesPath + coreFiles_iterator->second;
				acs_logm_filetime myFileTime(myFileObject);
				if(myFileTime.isFileOlderThanGivenDays(theConfigurationData.theNumberofOldDays))
				{
					DEBUG("File is old enough to be deleted %s",myFileObject.c_str());
					if(isFileObjectDirectory(myFileObject))
					{
						if(isDirectoryEmpty(myFileObject))
						{
							if(!deletedirectory(myFileObject))
							{
								myDeletion = false;
								DEBUG("Error in deleting Directory %s",myFileObject.c_str());
							}

						}
						else
						{
							if(!deletedirectory(myFileObject))
							{
								myDeletion = false;
								DEBUG("Error in deleting Directory %s",myFileObject.c_str());
							}

						}

					}
					else
					{
						if (!deleteFile(myFileObject))
						{
							myDeletion = false;
							DEBUG("Error in deletion file %s",myFileObject.c_str());
						}
					}
				}
				else
				{
					DEBUG("File is not old enough to be deleted %s",myFileObject.c_str());
				}
			}
			DEBUG(" %s","Time based CORE files clean is finished");
		}

		if ( ! coreFiles.empty() )
		{
			coreFiles.clear();
		}

		if(myDeletion == false)
		{
			ERROR("%s","Error in clearOldFileObjects ");
			DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
			return false;
		}
		DEBUG(" %s","ending core dumps cleanup activity");
		DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
		return true;
	}
	if(theTimeSortedFileObjectMap.size() <= theConfigurationData.theNumberofFilesToBeMaintained )
	{
		DEBUG("The Number of File Objects is less than specified size. No deleting %s",theConfigurationData.theLogName.c_str());
		DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
		return true;
	}
	unsigned int myNumberofFilesMaintained = theConfigurationData.theNumberofFilesToBeMaintained;
	for(;myNumberofFilesMaintained != 0; --myNumberofFilesMaintained)
	{
		myIterStart = ++myIter ;
	}

	for(;myIterStart != myIterEnd;++myIterStart)
	{
		std::string myFileObject = myIterStart->second;
		acs_logm_filetime myFileTime(myFileObject);
		if(myFileTime.isFileOlderThanGivenDays(theConfigurationData.theNumberofOldDays))
		{
			DEBUG("File is old enough to be deleted %s",myFileObject.c_str());
			if(isFileObjectDirectory(myFileObject))
			{
				if(isDirectoryEmpty(myFileObject))
				{
					if(!deletedirectory(myFileObject))
					{
						myDeletion = false;
						DEBUG("Error in deleting Directory %s",myFileObject.c_str());
					}

				}
				else
				{
					if(!deletedirectory(myFileObject))
					{
						myDeletion = false;
						DEBUG("Error in deleting Directory %s",myFileObject.c_str());
					}

				}

			}
			else
			{
				if (!deleteFile(myFileObject))
				{
					myDeletion = false;
					DEBUG("Error in deletion file %s",myFileObject.c_str());
				}
			}
		}
		else
		{
			DEBUG("File is not old enough to be deleted %s",myFileObject.c_str());
		}
	}
	if(myDeletion == false)
	{
		ERROR("%s","Error in clearOldFileObjects ");
		DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
		return false;
	}
	DEBUG(" %s","Leaving acs_logm_housekeeping::clearOldFileObjects");
	return true;

}

bool acs_logm_housekeeping::houseKeepingAction()
{
	DEBUG("Entering houseKeepingAction for %s",theConfigurationData.theLogName.c_str());
	bool myResult = false;
	resetAll();

	if(theConfigurationData.theCleaningEnabled == true)
	{
		if(extractFolderContent())
		{
			if(extractToBeDeletedFileObjects())
			{
				if(clearOldFileObjects())
				{
					myResult = true;
				}
				else
				{
					ERROR("Error clearOldFileObjects Leaving houseKeepingAction for %s",theConfigurationData.theLogName.c_str());
					myResult = false;
				}

			}
			else
			{
				ERROR("Error extractToBeDeletedFileObjects Leaving houseKeepingAction for %s",theConfigurationData.theLogName.c_str());
				myResult = false;
			}
		}
		else
		{
			ERROR("Error extractFolderContent - Leaving houseKeepingAction for %s",theConfigurationData.theLogName.c_str());
			myResult = false;
		}

		resetAll();
	}
	else
	{
		DEBUG("houseKeepingAction is not enabled for %s",theConfigurationData.theLogName.c_str());
	}
	calculateNextCleanUpTime();
	DEBUG("Leaving houseKeepingAction for %s",theConfigurationData.theLogName.c_str());
	return myResult;

}

void acs_logm_housekeeping::getConfigurationData(acs_logm_configurationdata & aConfigurationData)
{
	aConfigurationData = theConfigurationData;
}
bool acs_logm_housekeeping::isTimeForCleanUpElapsed()
{
	theNextCleanUpTime.logTimeData();
	return theNextCleanUpTime.isTimeElapsed();
}
std::string acs_logm_housekeeping::getLogName()
{
	return theConfigurationData.theLogName;
}



