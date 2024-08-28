	#include<acs_logm_scxlogfile.h>

	acs_logm_scxlogfile::acs_logm_scxlogfile(acs_logm_configurationdata & aConfigData)
	: acs_logm_logfile(aConfigData)
	{
		DEBUG("%s","Entering acs_logm_scxlogfile()");
		DEBUG("%s","Leaving acs_logm_scxlogfile()");
	}
	acs_logm_scxlogfile::~acs_logm_scxlogfile()
	{
		DEBUG("%s","Entering ~acs_logm_scxlogfile()");
		DEBUG("%s","Leaving ~acs_logm_scxlogfile()");

	}
	bool acs_logm_scxlogfile::houseKeepingAction()
	{
		DEBUG("%s","Entering acs_logm_scxlogfile::houseKeepingAction()");
		std::list<std::string> mySubDirectoryList;
		if(extractSubDirectories(mySubDirectoryList) == true)
		{
			std::list<std::string>::iterator myIter = mySubDirectoryList.begin();
			std::list<std::string>::iterator myIterEnd = mySubDirectoryList.end();
			for(;myIter != myIterEnd;++myIter)
			{
				acs_logm_configurationdata* myConfigDataPtr = new acs_logm_configurationdata(theConfigurationData);
				myConfigDataPtr->theFolderPath = (*myIter);
				acs_logm_logfile* myLogFilePtr = new acs_logm_logfile(*myConfigDataPtr);
				myLogFilePtr->houseKeepingAction();
				delete myLogFilePtr;
				delete myConfigDataPtr;
			}
			calculateNextCleanUpTime();

			myIter = mySubDirectoryList.begin();
			for(;myIter != myIterEnd;)
			{
				mySubDirectoryList.erase(myIter++);
			}
		}
		else
		{
			DEBUG("%s","Problem in retreving SubDirectoryList");
		}
		DEBUG("%s","Leaving acs_logm_scxlogfile::houseKeepingAction()");
		return true;
	}



