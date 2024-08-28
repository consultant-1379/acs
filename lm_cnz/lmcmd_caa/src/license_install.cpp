#include<iostream>
#include<string>
using namespace std;
#include "acs_lm_defs.h"
#include "acs_lm_common.h"
#include "acs_lm_cmdclient.h"
#include "acs_lm_cmd.h"
#include <acs_apgcc_adminoperation.h>
#include <saAis.h>
#include <ACS_APGCC_Util.H>
#include <acs_apgcc_omhandler.h>

static std::string ftpVolPath = "";
static void removefiles();
int invokeLMAdminOperation(std::string myLicenseModeRDN,std::string lservPath)
{
	DEBUG("%s","invokeLMAdminOperation");
	DEBUG("myLicenseModeRDN = %s",myLicenseModeRDN.c_str());
	//bool myRes = true;
	int nExitCode = 0;
	string errorString("");
	// Admin action using APGCC
	acs_apgcc_adminoperation admOp;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	int returnValue = 0;
	ACS_CC_ReturnType result;
	ACS_APGCC_AdminOperationParamType elem;
	elem.attrName = (char *)LKF_FILEPATH_ATTRIBUTE;
	DEBUG("lservPath.c_str() = %s",lservPath.c_str());
	elem.attrType = ATTR_STRINGT;
	elem.attrValues=reinterpret_cast<void*>((char *)lservPath.c_str());
	/*insert parameter into the vector*/
	vectorIN.push_back(elem);
	result = admOp.init();
	if(result != ACS_CC_SUCCESS)
	{
		WARNING("%s","license_install:lm admin instance initialization is failed!!");
	}
	DEBUG("admOp.adminOperationInvoke is invoked: %s","Called");
	ACE_INT64 timeOutValue = 120 * 1000000000LL;
	result = admOp.adminOperationInvoke(myLicenseModeRDN.c_str() , 0, 1, vectorIN, &returnValue,
			timeOutValue, vectorOut);
	INFO("The size of vectorOut.size= %d",vectorOut.size());
	INFO("The size of returnValue = %d",returnValue);

	if (result != ACS_CC_SUCCESS) 
	{
		cout<< "ERROR: "<< lservPath <<" Unable to connect to server\n\n";
		admOp.finalize();
		return 117;
	}

	if ( vectorOut.size() == 0 )
	{
		/*if(returnValue != 1)
		{
			cout<< "ERROR: "<< lservPath <<" Unable to connect to server\n\n";
			admOp.finalize();
			return 117;
		}*/
		//else
		{
			std::cout<<"OK: " << lservPath << std::endl;
		}
	}
		/*switch ( vectorOut[0].attrType )
		{
			case ATTR_INT32T:
				{
					nExitCode = *reinterpret_cast<int *>(vectorOut[0].attrValues);
					if(nExitCode != 0)
					{
						std::cout<<"ERROR: "<< lservPath << " " << (ACS_LM_Common::getApplicationErrorText(nExitCode)).c_str()<<std::endl;
						ERROR("%s","Error in adminOperationInvoke");
					}
					else 
					{
						std::cout<<"OK: " << lservPath << std::endl;
					}

					break;
				}
			default:
				{
					break;
				}
		}*/
	else
	{
		/*if(returnValue != 1)
				{
					cout<< "ERROR: "<< lservPath <<" Unable to connect to server\n\n";
					admOp.finalize();
					return 117;
				}*/
		//else
		{
			switch ( vectorOut[0].attrType )
					{
						case ATTR_STRINGT:
							{
								errorString = reinterpret_cast<char *>(vectorOut[0].attrValues);
								DEBUG("errorString  = %s", errorString.c_str());
								if(!errorString.substr(8).compare("Incorrect usage")){
									std::cout<<"ERROR: "<< lservPath << " " <<"Incorrect usage"<<std::endl;
									nExitCode = 2;
								}else if(!errorString.substr(8).compare("Unable to connect to server")){
									std::cout<<"ERROR: "<< lservPath << " " <<"Unable to connect to server"<<std::endl;
									nExitCode = 117;
								}else if(!errorString.substr(8).compare("Error when executing (general fault)")){
									std::cout<<"ERROR: "<< lservPath << " " <<"Error when executing (general fault)"<<std::endl;
									nExitCode = 1;
								}else if(!errorString.substr(8).compare("Physical file error")){
									std::cout<<"ERROR: "<< lservPath << " " <<"Physical file error"<<std::endl;
									nExitCode = 8;
								}else if(!errorString.substr(8).compare("Command allowed only on AP1 active node")){
									std::cout<<"ERROR: "<< lservPath << " " <<"Command allowed only on AP1 active node"<<std::endl;
									nExitCode = 19;
								}else if(!errorString.substr(8).compare( "License key already exists")){
									std::cout<<"ERROR: "<< lservPath << " " << "License key already exists"<<std::endl;
									nExitCode = 20;
								}else if(!errorString.substr(8).compare( "License key not found")){
									std::cout<<"ERROR: "<< lservPath << " " << "License key not found"<<std::endl;
									nExitCode = 21;
								}else if(!errorString.substr(8).compare( "Unreasonable value")){
									std::cout<<"ERROR: "<< lservPath << " " << "Unreasonable value"<<std::endl;
									nExitCode = 3;
								}else if(!errorString.substr(8).compare( "Invalid license key file path")){
									std::cout<<"ERROR: "<< lservPath << " " << "Invalid license key file path"<<std::endl;
									nExitCode = 10;
								}else if(!errorString.substr(8).compare( "Invalid license key file")){
									std::cout<<"ERROR: "<< lservPath << " " << "Invalid license key file"<<std::endl;
									nExitCode = 23;
								}else if(!errorString.substr(8).compare( "Test Mode not allowed")){
									std::cout<<"ERROR: "<< lservPath << " " << "Test Mode not allowed"<<std::endl;
									nExitCode = 26;
								}else if(!errorString.substr(8).compare( "Emergency State Mode not allowed")){
									std::cout<<"ERROR: "<< lservPath << " " << "Emergency State Mode not allowed"<<std::endl;
									nExitCode = 27;
								}else if(!errorString.substr(8).compare( "Emergency State Mode already activated")){
									std::cout<<"ERROR: "<< lservPath << " " << "Emergency State Mode already activated"<<std::endl;
									nExitCode = 28;
								}else if(!errorString.substr(8).compare( "Missing License Key File")){
									std::cout<<"ERROR: "<< lservPath << " " << "Missing License Key File"<<std::endl;
									nExitCode = 24;
								}else if(!errorString.substr(8).compare( "Command not executed, AP backup in progress")){
									std::cout<<"ERROR: "<< lservPath << " " << "Command not executed, AP backup in progress"<<std::endl;
									nExitCode = 114;
								}

								/*if(nExitCode != 0)
								{
									std::cout<<"ERROR: "<< lservPath << " " << (ACS_LM_Common::getApplicationErrorText(nExitCode)).c_str()<<std::endl;
									ERROR("%s","Error in adminOperationInvoke");
								}
								else
								{
									std::cout<<"OK: " << lservPath << std::endl;
								}*/

								break;
							}
						default:
							{
								break;
							}
					}
		}

	}
	admOp.freeadminOperationReturnValue (vectorOut);
	admOp.finalize();
	return nExitCode;
}

static ACS_LM_AppExitCode checkOpt(const int argc, char** argv)
{
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_INCUSAGE;
	if(!ACS_LM_Common::isCommandAllowed())
	{
		return ACS_LM_RC_CMD_NOTALLOWED;
	}

	if(argc == 3)
	{
		if(strcmp(argv[1], "-where") == 0)
		{
			if( (strcmp(argv[2], "FTP") == 0) ||  (strcmp(argv[2], "SFTP") == 0) )
			{
				exitCode = ACS_LM_RC_OK;
				ftpVolPath = "/license_file";
			}
			else
			{
				exitCode = ACS_LM_RC_UNREAS;
			}
		}
	}
	else if(argc == 1)
	{
		ftpVolPath = "/license_file";
		exitCode = ACS_LM_RC_OK;
	}

	return exitCode;
}

int main(int argc, char** argv)
{
	ACS_LM_Logging.Open("LM");
	ACS_LM_AppExitCode exitCode = ACS_LM_RC_OK;
	std::string lservPath;
	exitCode = checkOpt(argc,argv);

	if(exitCode != ACS_LM_RC_OK)
	{
		ftpVolPath = "/license_file";
		ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
		ACS_LM_Common::getlatestFile(ftpVolPath, lservPath);
		if (lservPath.length() != 0)
		{
			std::cout<< "ERROR: "<< lservPath << " " <<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		}
		else
		{
			std::cout<< "ERROR: "<<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
		}
		removefiles();
	}
	else
	{
		if(argc == 3)
		{
			std::cout<< "OK: " << ftpVolPath << std::endl;
		}
		else
		{
			std::string lservPath;
			bool status = ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
			if(status == false)
			{
				INFO("License_install %s","ACS_LM_Common::getLicenseKeyFilePath() FAILED");
				exitCode = ACS_LM_RC_MISSINGLKF;
				std::cout<<"ERROR: " << (ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
				return exitCode;

			}
			else
				DEBUG("ftpVolPath= %s", ftpVolPath.c_str());

			bool myResult = ACS_LM_Common::getlatestFile(ftpVolPath, lservPath);
			if(myResult == false)
			{
				INFO("License_install %s","ACS_LM_Common::getlatestFile() FAILED");
				exitCode = ACS_LM_RC_MISSINGLKF;
				std::cout<<"ERROR: "<< (ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
				return exitCode;

			}
			if(lservPath.length()==0)
			{
				INFO("License_install %s","lservPath.length() is Zero");
				exitCode = ACS_LM_RC_MISSINGLKF;
				std::cout<<"ERROR: " <<(ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
			}
			else
			{
				std::string myLMPath = ftpVolPath + "/"+ lservPath;
				INFO("myLMPath = %s",myLMPath.c_str());
				std::string myLMPath1 = "";
				myLMPath1 = lservPath;
				OmHandler* theOmHandlerPtr = 0;
				theOmHandlerPtr = new OmHandler();
				if(theOmHandlerPtr != 0)
				{
					if( theOmHandlerPtr->Init() == ACS_CC_FAILURE )
					{
						ERROR("License_install %s","theOmHandlerPtr->Init() FAILED");
						exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
						std::cout<<"ERROR: " << lservPath << " " << (ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
						delete theOmHandlerPtr;
						theOmHandlerPtr =0;
						return exitCode;
					}
					// Fetch DN of Root of LM

					if (ACS_LM_Common::fetchDnOfRootObjFromIMM (theOmHandlerPtr) == -1)
					{
						ERROR("License_install %s","fetchDnOfRootObjFromIMM FAILED");
						exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
						std::cout<<"ERROR: " << lservPath << " " << (ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
						delete theOmHandlerPtr;
                                                theOmHandlerPtr =0;
						return exitCode;
					}
					//        				std::string myLicenseModeRDN = LICENSEMODE_RDNVALUE;
					//        				myLicenseModeRDN += ",";
					//        				myLicenseModeRDN += ACS_LM_Common::dnOfLMRoot;
					std::string myLicenseModeRDN = ACS_LM_Common::dnOfLMRoot;
					//bool myAdminOpRest = invokeLMAdminOperation(myLicenseModeRDN,myLMPath1);
					int myAdminOpRest = invokeLMAdminOperation(myLicenseModeRDN,myLMPath1);

					//if(myAdminOpRest == false)
					if(myAdminOpRest != 0)
					{
						//exitCode = ACS_LM_RC_SERVERNOTRESPONDING;
						removefiles();
						delete theOmHandlerPtr;
                                                theOmHandlerPtr =0;
						return myAdminOpRest;
						//std::cout<<"ERROR: " << (ACS_LM_Common::getApplicationErrorText(exitCode)).c_str()<<std::endl;
					}
					else
					{
						exitCode = ACS_LM_RC_OK;
					}
					removefiles();
					if( theOmHandlerPtr != 0)
					{
						delete theOmHandlerPtr;
						theOmHandlerPtr =0;
					}	
				}
			}
		}

	}
	return exitCode;	
}

static void removefiles()
{
	//std::string ftpVolPath = ACS_LM_Common::getDataDiskPath("FTP_VOL$");
	//bool status = ACS_LM_Common::getLicenseKeyFilePath(ftpVolPath);
	bool status = ACS_LM_Common::doFileDelete(ftpVolPath);
	DEBUG("ftpVolPath= %s", ftpVolPath.c_str());
	if(status == true)
	{
		DEBUG("%s","removefiles() :All file get deleted successfully")
	}
	else
	{
		DEBUG("%s","removefiles() :Deletion of all file get failed")
	}
}



