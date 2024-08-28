
#include<acs_logm_apgccwrapper.h>



	acs_logm_apgccwrapper::acs_logm_apgccwrapper()
	{
		DEBUG("%s","Entering acs_logm_apgccwrapper  ");
		DEBUG("%s","Leaving acs_logm_apgccwrapper  ");

	}
	acs_logm_apgccwrapper::~acs_logm_apgccwrapper()
	{
		DEBUG("%s","Entering ~acs_logm_apgccwrapper  ");
		DEBUG("%s","Leaving ~acs_logm_apgccwrapper  ");


	}

	bool acs_logm_apgccwrapper::getAttributesFromInstance(OmHandler & aOmHandler, std::string& aRDNName,acs_logm_configurationdata& aConfigurationData)
	{
		DEBUG("Entering acs_logm_apgccwrapper::getAttributesFromInstance for RDN  %s",aRDNName.c_str());

		ACS_APGCC_ImmObject myLogmConfigObj;
		myLogmConfigObj.objName =  aRDNName ;


		//char tmpstr[250];
		int  myValue = 0;


		if( aOmHandler.getObject(&myLogmConfigObj) == ACS_CC_FAILURE )
		{
			ERROR("acs_logm_apgccwrapper::getAttributesFromInstance %s","myOmHandler.getObject failed");
			return false;

		}
		std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr2;

		for ( attributesListItr2 = myLogmConfigObj.attributes.begin();
						attributesListItr2 != myLogmConfigObj.attributes.end() ;
						++attributesListItr2 )
		{
			if((*attributesListItr2).attrName == TIMEBASED_CLASS_RDN)
			{
				if((*attributesListItr2).attrValuesNum != 0)
				{

					std::string myLogName1 (reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",TIMEBASED_CLASS_RDN,myLogName1.c_str());
					size_t myPos1 = myLogName1.find_first_of('=');

					if( myPos1 != std::string::npos )
					{
							std::string myLogName = myLogName1.substr(myPos1+1);
							DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",TIMEBASED_CLASS_RDN,myLogName.c_str());
							aConfigurationData.theLogName = myLogName;
					}
					aConfigurationData.theCleanupInitiation = acs_logm_configurationdata::TIMEBASED;
				}
			}
			if((*attributesListItr2).attrName == PERIODICBASED_CLASS_RDN)
			{
				if((*attributesListItr2).attrValuesNum != 0)
				{


					std::string myLogName1 (reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",PERIODICBASED_CLASS_RDN,myLogName1.c_str());
					size_t myPos1 = myLogName1.find_first_of('=');


					if( myPos1 != std::string::npos )
					{

						std::string myLogName = myLogName1.substr(myPos1+1);
						DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",PERIODICBASED_CLASS_RDN,myLogName.c_str());
						aConfigurationData.theLogName = myLogName;
					}
					aConfigurationData.theCleanupInitiation = acs_logm_configurationdata::PERIODICBASED;
				}
			}
			if((*attributesListItr2).attrName == ATTR_SEARCH_PATH)
			{
				if((*attributesListItr2).attrValuesNum != 0)
				{


					std::string myStr1 (reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",ATTR_SEARCH_PATH,myStr1.c_str());
					std::string myAbsoluteLogPath = getAbsoluteLogPath(myStr1);
					aConfigurationData.theFolderPath = myAbsoluteLogPath + "/";
				}
			}
			else if((*attributesListItr2).attrName== ATTR_SEARCH_PATTERN)
			{
				if((*attributesListItr2).attrValuesNum != 0)
				{

					std::string myStrPattern (reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %s",ATTR_SEARCH_PATTERN,myStrPattern.c_str());
					size_t myPos = myStrPattern.find_first_of('.');
					if(myPos != std::string::npos)
					{
						aConfigurationData.theSearchPattern = myStrPattern.substr(myPos);
					}
					else
					{
						aConfigurationData.theSearchPattern = myStrPattern;

					}

				}

			}
			else if((*attributesListItr2).attrName== ATTR_NUMBER_OF_FILES )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_NUMBER_OF_FILES,myValue);
					aConfigurationData.theNumberofFilesToBeMaintained = myValue;

				}
			}
			else if((*attributesListItr2).attrName== ATTR_FILE_AGE )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance -%s  - %d",ATTR_FILE_AGE,myValue);
					aConfigurationData.theNumberofOldDays = myValue;

				}
			}
			else if((*attributesListItr2).attrName== ATTR_INITATION_HOUR )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_INITATION_HOUR,myValue);
					aConfigurationData.theInitiationHour = myValue;

				}
			}
			else if((*attributesListItr2).attrName== ATTR_INITATION_MINUTE )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_INITATION_MINUTE,myValue);
					aConfigurationData.theInitiationMinute = myValue;

				}
			}
			else if((*attributesListItr2).attrName== ATTR_INITATION_FREQUENCY )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_INITATION_FREQUENCY,myValue);
					if(myValue == 0)
					{
						aConfigurationData.theCleanUpFrequency = acs_logm_configurationdata::DAILY;
					}
					else
					{
						aConfigurationData.theCleanUpFrequency = acs_logm_configurationdata::WEEKLY;
					}

				}
			}
			else if((*attributesListItr2).attrName== ATTR_PERIODIC_INTERVAL )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_PERIODIC_INTERVAL,myValue);
					aConfigurationData.thePeriodicInterval = myValue;

				}
			}
			else if((*attributesListItr2).attrName== ATTR_CLEAN_ENABLE )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_CLEAN_ENABLE,myValue);
					if(myValue == 0)
					{
						aConfigurationData.theCleaningEnabled = false;
					}
					else
					{
						aConfigurationData.theCleaningEnabled = true;
					}

				}
			}
			else if((*attributesListItr2).attrName== ATTR_FILE_TYPE )
			{
				if((*attributesListItr2).attrValuesNum !=0)
				{
					myValue = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
					DEBUG("acs_logm_apgccwrapper::getAttributesFromInstance - %s - %d",ATTR_FILE_TYPE,myValue);
					if(myValue == 0)
					{
						aConfigurationData.theCleanFileObject = acs_logm_configurationdata::FILE;
					}
					else if(myValue == 1)
					{
						aConfigurationData.theCleanFileObject = acs_logm_configurationdata::FOLDER;
					}
					else if(myValue == 2)
					{
						aConfigurationData.theCleanFileObject = acs_logm_configurationdata::BOTH;
					}

				}
			}

		}
		DEBUG("Leaving acs_logm_apgccwrapper::getAttributesFromInstance for RDN  %s",aRDNName.c_str());
		return true;
	}

	bool acs_logm_apgccwrapper::getInstances(std::string aClass , std::list<acs_logm_configurationdata>& aInstanceList)
	{
		DEBUG("Entering acs_logm_apgccwrapper::getInstances for Class  %s",aClass.c_str());

		std::vector<std::string> myRDNVector;

		OmHandler myOmHandler;

		if( myOmHandler.Init() == ACS_CC_FAILURE )
		{
			ERROR("acs_logm_apgccwrapper::getInstances %s","myOmHandler.Init() Failed");
			return false;
		}

		if( myOmHandler.getClassInstances(aClass.c_str(), myRDNVector ) ==  ACS_CC_FAILURE)
		{
			ERROR("acs_logm_apgccwrapper::getInstances %s","myOmHandler.getClassInstances() Failed");
			return false;
		}

		std::vector<std::string>::iterator myIter = myRDNVector.begin();
		std::vector<std::string>::iterator myIterEnd = myRDNVector.end();

		for(;myIter != myIterEnd; ++ myIter)
		{
			acs_logm_configurationdata myLogmConfigData;
			if(getAttributesFromInstance(myOmHandler,(*myIter),myLogmConfigData) == true)
			{
				aInstanceList.push_back(myLogmConfigData);
			}
		}

		myOmHandler.Finalize();

		DEBUG("Leaving acs_logm_apgccwrapper::getInstances for Class  %s",aClass.c_str());
		return true;

	}

	std::string acs_logm_apgccwrapper::getAbsoluteLogPath(std::string& aSymbolicPathName)
	{
		std::string myAbsolutePath;
		ACS_APGCC_CommonLib objCommonLib;
		ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;
		DEBUG( "%s"," getAbsoluteLogPath called ") ;
		if(aSymbolicPathName == std::string(STS_MP))
		{
				DEBUG( "%s ", "STS_MP case" ) ;
				string fileMFuncName = STSOUTPUT_NBI_FOLDER;
				char folderPath[1024] = { 0 };
				int folderPathLen = 1024;
				returnType = objCommonLib.GetFileMPath( fileMFuncName.c_str(),folderPath,folderPathLen );
				if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
				{
					std::string myStr("Retreiving fileM path for STS MP folder failed");
					DEBUG( "FAILURE : %s ", myStr.c_str() ) ;
					myAbsolutePath = "/data/opt/ap/internal_root/data_transfer/source/sts";
				}

				myAbsolutePath = folderPath;
				DEBUG( "STS_MP path %s ", myAbsolutePath.c_str()) ;
		}
		if(aSymbolicPathName == std::string(TRA_LOG))
		{
			myAbsolutePath = TRA_LOG_PATH;
			DEBUG( "TRA_LOG path %s ", myAbsolutePath.c_str()) ;
		}
		if(aSymbolicPathName == std::string(CORE))
		{
			myAbsolutePath = CORE_PATH;
			DEBUG( "CORE path %s ", myAbsolutePath.c_str()) ;
		}
		if(aSymbolicPathName == std::string(SCX_LOG))
		{
			DEBUG( " %s ", "SCX LOG case") ;
			int myDwLen = 50;
			char * mySzPath = new char[50];
			memset(mySzPath,0,sizeof(char[50]));
			returnType = objCommonLib.GetDataDiskPath(APZ_DATA_SYMBOL, mySzPath, myDwLen);
			switch(returnType)
				{
					case ACS_APGCC_DNFPATH_SUCCESS:
					{
						if( strcmp("",mySzPath) == 0 )
						{
							std::string myStr("Retreiving datadisk path for SCX folder failed");
							ERROR( "FAILURE : %s ", myStr.c_str() ) ;
							myAbsolutePath = "/data/apz/data/boot/scx/scx_logs";
						}
						else
						{
							//append the remaining part of the parameter
							std::string aStr(mySzPath);
							myAbsolutePath = aStr + SCX_LOG_PATH;
							DEBUG( "SCX_LOG  path %s ", myAbsolutePath.c_str()) ;
						}
						break;
					}
					default:
					{
						std::string myStr("Retreiving datadisk path for SCX folder failed");
						ERROR( "FAILURE : %s ", myStr.c_str() ) ;
						myAbsolutePath = "/data/apz/data/boot/scx/scx_logs";
						break;
					}

				}
				delete []mySzPath;
		}
		if(aSymbolicPathName == std::string(CMX_LOG))
		{
			DEBUG( " %s ", "CMX LOG case") ;
			int myDwLen = 50;
			char * mySzPath = new char[50];
			memset(mySzPath,0,sizeof(char[50]));
			returnType = objCommonLib.GetDataDiskPath(APZ_DATA_SYMBOL, mySzPath, myDwLen);
			switch(returnType)
			{
				case ACS_APGCC_DNFPATH_SUCCESS:
					{
						if( strcmp("",mySzPath) == 0 )
						{
							std::string myStr("Retreiving datadisk path for CMX folder failed");
							ERROR( "FAILURE : %s ", myStr.c_str() ) ;
							myAbsolutePath = "/data/apz/data/boot/cmx/cmx_logs";
						}
						else
						{
							//append the remaining part of the parameter
							std::string aStr(mySzPath);
							myAbsolutePath = aStr + CMX_LOG_PATH;
							DEBUG( "CMX_LOG  path %s ", myAbsolutePath.c_str()) ;
						}
						break;
					}
				default:
					{
						std::string myStr("Retreiving datadisk path for CMX folder failed");
						ERROR( "FAILURE : %s ", myStr.c_str() ) ;
						myAbsolutePath = "/data/apz/data/boot/cmx/cmx_logs";
						break;
					}

			}
			delete []mySzPath;
		}
		if(aSymbolicPathName == std::string(SMX_LOG))
		{
			DEBUG( " %s ", "SMX LOG case") ;
			int myDwLen = 50;
			char * mySzPath = new char[50];
			memset(mySzPath,0,sizeof(char[50]));
			returnType = objCommonLib.GetDataDiskPath(APZ_DATA_SYMBOL, mySzPath, myDwLen);
			switch(returnType)
			{
				case ACS_APGCC_DNFPATH_SUCCESS:
					{
						if( strcmp("",mySzPath) == 0 )
						{
							std::string myStr("Retreiving datadisk path for SMX folder failed");
							ERROR( "FAILURE : %s ", myStr.c_str() ) ;
							myAbsolutePath = "/data/apz/data/boot/smx/smx_logs";
						}
						else
						{
							//append the remaining part of the parameter
							std::string aStr(mySzPath);
							myAbsolutePath = aStr + SMX_LOG_PATH;
							DEBUG( "SMX_LOG  path %s ", myAbsolutePath.c_str()) ;
						}
						break;
					}
				default:
					{
						std::string myStr("Retreiving datadisk path for SMX folder failed");
						ERROR( "FAILURE : %s ", myStr.c_str() ) ;
						myAbsolutePath = "/data/apz/data/boot/smx/smx_logs";
						break;
					}

			}
			delete []mySzPath;
		}
		return myAbsolutePath;
	}

