	#include<acs_logm_logfile.h>

	acs_logm_logfile::acs_logm_logfile(acs_logm_configurationdata & aConfigData)
	: acs_logm_housekeeping(aConfigData)
	{
		DEBUG("%s","Entering acs_logm_logfile()");
		DEBUG("%s","Leaving acs_logm_logfile()");
	}
	acs_logm_logfile::~acs_logm_logfile()
	{
		DEBUG("%s","Entering ~acs_logm_logfile()");
		DEBUG("%s","Leaving ~acs_logm_logfile()");

	}
	bool acs_logm_logfile::isSearchPatternAvailable(std::string& aPath)
	{
		bool myReturnVal = false;
		DEBUG("Entering acs_logm_logfile isSearchPatternAvailable() %s",aPath.c_str());
		DEBUG("The Search pattern %s",theConfigurationData.theSearchPattern.c_str());
		if(theConfigurationData.theSearchPattern == std::string(".*"))
		{
			DEBUG("Search Pattern is ANY file - Leaving isSearchPatternPresent() %s",aPath.c_str());
			myReturnVal = true;
		}
		else
		{
			size_t myFound = aPath.find(theConfigurationData.theSearchPattern);
			if(myFound != string::npos)
			{
				DEBUG("%s","Search pattern found Leaving acs_logm_logfile isSearchPatternAvailable() ");
				myReturnVal = true;
			}
		}
		DEBUG("%s","Leaving acs_logm_logfile isSearchPatternAvailable()");
		return myReturnVal;
	}
	bool acs_logm_logfile::extractToBeDeletedFileObjects()
	{
		DEBUG("%s","Entering acs_logm_logfile extractToBeDeletedFileObjects()");
		std::list<std::string>::iterator myIter = theFileObjectsList.begin();
		std::list<std::string>::iterator myIterEnd = theFileObjectsList.end();
		double myElapsedTime;
		for(;myIter != myIterEnd; ++myIter)
		{
			std::string myStr = (*myIter);
			if(!isFileObjectDirectory(myStr))
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
			}

		}
		DEBUG("%s","Leaving acs_logm_logfile extractToBeDeletedFileObjects()");
		return true;
	}

