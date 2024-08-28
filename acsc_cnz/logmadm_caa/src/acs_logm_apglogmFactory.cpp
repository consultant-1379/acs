
    #include<acs_logm_apglogmFactory.h>


	acs_logm_apglogmFactory::acs_logm_apglogmFactory()
	{
		DEBUG("%s","Entering acs_logm_apglogmFactory::acs_logm_apglogmFactory");
		DEBUG("%s","Leaving acs_logm_apglogmFactory::acs_logm_apglogmFactory");

	}

	acs_logm_apglogmFactory::~acs_logm_apglogmFactory()
	{
		DEBUG("%s","Entering acs_logm_apglogmFactory::~acs_logm_apglogmFactory");
		DEBUG("%s","Leaving acs_logm_apglogmFactory::~acs_logm_apglogmFactory");

	}

	void acs_logm_apglogmFactory::createComponents(std::list<acs_logm_housekeeping*>& aHouseKeepingComponentsList)
	{
		DEBUG("%s","Entering acs_logm_apglogmFactory::createComponents");
		bool myResult = false;
		myResult= getConfigurationParametersClassName(PERIODICBASED_CLASS_NAME,aHouseKeepingComponentsList);
		if(!myResult)
		{
			DEBUG("%s","acs_logm_apglogmFactory::createComponents(PERIODICBASED_CLASS_NAME) Failed");
		}
		myResult= getConfigurationParametersClassName(TIMEBASED_CLASS_NAME,aHouseKeepingComponentsList);
		if(!myResult)
		{
			DEBUG("%s","acs_logm_apglogmFactory::createComponents(TIMEBASED_CLASS_NAME) Failed");
		}
		DEBUG("%s","Leaving acs_logm_apglogmFactory::createComponents");

	}

	bool acs_logm_apglogmFactory::getConfigurationParametersClassName(std::string aClassname,std::list<acs_logm_housekeeping*>& aHouseKeepingComponentsList)
	{
		acs_logm_apgccwrapper myLogmWrapper;
		std::list<acs_logm_configurationdata> myConfigurationDataList;
		bool myResult = false;
		myResult = myLogmWrapper.getInstances(aClassname,myConfigurationDataList);
		if(myResult)
		{
			std::list<acs_logm_configurationdata>::iterator myConfigurationDataListIter = myConfigurationDataList.begin() ;
			std::list<acs_logm_configurationdata>::iterator myConfigurationDataListEnd = myConfigurationDataList.end() ;
			for(;myConfigurationDataListIter != myConfigurationDataListEnd;++myConfigurationDataListIter)
			{
				(*myConfigurationDataListIter).logData();
				acs_logm_housekeeping* myPtr = 0;
                                // TR HQ52756 Fix - It is decided to drop LOGM cleanup on TRA folder
                                if(((*myConfigurationDataListIter).theLogName != "TRALOG"))
				{
					if(((*myConfigurationDataListIter).theLogName == "SCXLOG"))
					{
						myPtr = new acs_logm_scxlogfile((*myConfigurationDataListIter));

					}
					else if(((*myConfigurationDataListIter).theLogName == "CMXLOG"))
					{
						myPtr = new acs_logm_cmxlogfile((*myConfigurationDataListIter));

					}
					else if(((*myConfigurationDataListIter).theLogName == "SMXLOG"))
					{
						myPtr = new acs_logm_smxlogfile((*myConfigurationDataListIter));

					}
					else if(((*myConfigurationDataListIter).theLogName == "CORE"))
					{
						INFO("%s","Creating ConfigurationDataListIter for core dumps cleanup activiry");
						myPtr = new acs_logm_logfile((*myConfigurationDataListIter));
						if (myPtr != NULL)
						{
							DEBUG("%s","acs_logm_logfile::myConfigurationDataListIter pointer is created successfully");
						}
						else
						{
							DEBUG("%s","failed to create acs_logm_logfile::myConfigurationDataListIter pointer");
						}
					}
					else if((*myConfigurationDataListIter).theCleanFileObject == acs_logm_configurationdata::FILE)
					{
						myPtr = new acs_logm_logfile((*myConfigurationDataListIter));

					}
					else
					{
						myPtr = new acs_logm_directory((*myConfigurationDataListIter));
					}
				} // TR HQ52756 - END

				if (myPtr  != 0)
				{
					aHouseKeepingComponentsList.push_back(myPtr);
				}

			}

			myConfigurationDataListIter = myConfigurationDataList.begin() ;
			myConfigurationDataListEnd = myConfigurationDataList.end() ;
			for(;myConfigurationDataListIter != myConfigurationDataListEnd;)
			{
				myConfigurationDataList.erase(myConfigurationDataListIter++);
			}
		}
		return myResult;
	}

