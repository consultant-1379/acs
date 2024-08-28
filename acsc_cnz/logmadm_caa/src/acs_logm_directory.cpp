	#include<acs_logm_directory.h>

	acs_logm_directory::acs_logm_directory(acs_logm_configurationdata & aConfigData)
	: acs_logm_housekeeping(aConfigData)
	{
		DEBUG("%s","Entering acs_logm_directory()");
		DEBUG("%s","Leaving acs_logm_directory()");
	}
	acs_logm_directory::~acs_logm_directory()
	{
		DEBUG("%s","Entering ~acs_logm_directory()");
		DEBUG("%s","Leaving ~acs_logm_directory()");

	}
	bool acs_logm_directory::isSearchPatternAvailable(std::string& aPath)
	{
		DEBUG("Entering isSearchPatternPresent() %s",aPath.c_str());
		bool myReturnVal = false;
		if(!isDirectoryEmpty(aPath))
		{
			if(!theConfigurationData.theSearchPattern.compare(".*"))
			{
				DEBUG("Search Pattern is ANY file - Leaving isSearchPatternPresent() %s",aPath.c_str());
				myReturnVal = true;
			}
			else
			{
				std::string myDummyStr = aPath + "/" + theConfigurationData.theSearchPattern;
				if(isFileObjectDirectory(myDummyStr) == false)
				{
					myReturnVal = isFileExists(myDummyStr);
				}
			}
		}
		else
		{
			DEBUG("Directory is empty - Leaving isSearchPatternPresent() %s",aPath.c_str());
			myReturnVal = true;
		}
		DEBUG("Leaving isSearchPatternPresent() %s",aPath.c_str());
		return myReturnVal;
	}

	bool acs_logm_directory::extractToBeDeletedFileObjects()
	{
		DEBUG("%s","Entering acs_logm_directory extractToBeDeletedFileObjects()");
		std::list<std::string>::iterator myIter = theFileObjectsList.begin();
		std::list<std::string>::iterator myIterEnd = theFileObjectsList.end();
		double myElapsedTime;
		for(;myIter != myIterEnd; ++myIter)
		{
			std::string myStr = (*myIter);
			if(isFileObjectDirectory(myStr))
			{
				if(isSearchPatternAvailable(myStr))
				{
					if(getElapsedTimeSincelastModified(myStr,myElapsedTime))
					{
						theTimeSortedFileObjectMap.insert(pair<double,std::string>(myElapsedTime,myStr));
					}
					else
					{
						ERROR("Error in getting  getElapsedTimeSincelastModified %s",myStr.c_str());
					}
				}
				else
				{
					DEBUG("Search Pattern not present %s",myStr.c_str());
				}
			}

		}
		DEBUG("%s","Leaving acs_logm_directory extractToBeDeletedFileObjects()");
		return true;
	}











