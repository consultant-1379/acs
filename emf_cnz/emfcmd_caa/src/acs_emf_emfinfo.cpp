/*=================================================================== */
/**
   @file acs_emf_emfinfo.cpp

   This file contains the code for the emfinfo command. This command
   is used to fetch media info / dvd owner info / displaying history of copy operations.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       30/11/2010     XRAMMAT   Initial Release
 **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iomanip>
#include <getopt.h>
#include <ace/ACE.h>
#include <saAis.h>
#include <acs_apgcc_adminoperation.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Util.H>
#include "acs_emf_common.h"
#include <acs_emf_defs.h>
#include "acs_emf_execute.h"
#include "acs_emf_tra.h"
#include "acs_emf_aeh.h"

using namespace std;

static bool fetchAllHistoryAttibutes(const ACE_TCHAR* historyDnName);
static void PrintUsage();

extern char* optarg;
extern int   optind;
extern int   opterr;

/**
 * @brief	This function fetches all the histroy attributes from IMM.
 *
 * @param	historyDnName
 * 			DN of history Object.
 *
 * @return	ACE_INT32
 */
bool fetchAllHistoryAttibutes(const ACE_TCHAR* historyDnName)
{
	ACE_TCHAR dataBuffer[1024];
	ACE_TCHAR pszAttrValue[4096]; //to be checked data should be more...
	bool isTODVDOperation = false;
	std::string dnOfEMFHistoryObject(historyDnName);
	bool isOperationInProgress = false;

	// For attrEMFHistoryNodeName
	if(ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoNodeName,pszAttrValue) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("Nodename:"), pszAttrValue);
		cout << endl << dataBuffer;
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",(char *)theEMFHistoryInfoNodeName,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryNodeStatus
	if(ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoNodeState,pszAttrValue) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("State:"), pszAttrValue);
		cout << dataBuffer;

		if (strcmp(pszAttrValue,PROCESSING) == 0)
		{
			isOperationInProgress = true;
		}
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoNodeState,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryCurrOpreation
	int operType = 0;
	if( ACS_EMF_Common::fetchEMFHistoryIntegerAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoLastOperation,operType)== 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("Current/last operation:"), operType ? "Copying To DVD" : "Copying From DVD");
		cout << endl << dataBuffer;
		if(operType == TODVD)
			isTODVDOperation = true;
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoLastOperation,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryOperatioResult
	ACE_TCHAR status[100];
	if(ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoOperResult,pszAttrValue ) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("Result:"), pszAttrValue);
		cout << dataBuffer;
		if(ACE_OS::strcmp(pszAttrValue,SUCCESSFUL) == 0)
		{
			ACE_OS::sprintf(status,"%s",DONE);
		}
		else
		{
			if (isOperationInProgress)
			{
				ACE_OS::sprintf(status,"%s","");
			}
			else
			{
				ACE_OS::sprintf(status,"%s",FAILED);
			}
		}
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoOperResult,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryUser
	if( ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoUser,pszAttrValue) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("User:"), pszAttrValue);
		cout << dataBuffer;
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoUser,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryStartTime
	if( ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoStartTime,pszAttrValue) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("Operation start time:"), pszAttrValue);
		cout << dataBuffer;
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoStartTime,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryEndTime
	if( ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoEndTime,pszAttrValue) == 0 )
	{
		ACE_OS::sprintf(dataBuffer, ACE_TEXT("%-40s%s\n"), ACE_TEXT("Operation end time:"), pszAttrValue);
		cout << dataBuffer;
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoEndTime,dnOfEMFHistoryObject.c_str());
		return false;
	}

	// For attrEMFHistoryFileName
	if( ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistoryObject , (char *)theEMFHistoryInfoFileName,pszAttrValue) == 0 )
	{
		if(ACE_OS::strcmp(pszAttrValue,ACE_TEXT("NONE"))!=0)
		{
			if (!isTODVDOperation &&  ACE_OS::strcmp(status,FAILED)==0)
			{
				return true;
			}
			else
			{
				ACE_TCHAR* token;
				token = ACE_OS::strtok(pszAttrValue," ");
				while (token != NULL)
				{
					ACE_OS::sprintf(dataBuffer, ACE_TEXT("   %-37.35s%s\n"), token, status);
					cout << dataBuffer;
					token = ACE_OS::strtok(NULL," ");
				}
			}
		}
	}
	else
	{
		DEBUG(1,"EMFINFO:fetchAllHistoryAttibutes failed while fetching info of %s attribute from %s object!!",theEMFHistoryInfoFileName,dnOfEMFHistoryObject.c_str());
		return false;
	}

	return true;
}

/**
 * @brief	This function prints the usage of command.
 *
 *
 * @return	void
 */
void PrintUsage()
{
	cerr << "Incorrect usage\n"
			<< "Usage: emfinfo [-h [-n nodename]]\n"
			<< "       emfinfo -m\n" << endl ;
} // End of PrintUsage
/**
 * @brief	Main function
 *
 * @param 	argc
 * 			Number of arguments.
 *
 * @param 	argv
 * 			Array of arguments.
 *
 * @return	ACE_IN32
 */
int ACE_TMAIN( ACE_INT32 argc, ACE_TCHAR* argv[] )
{
	ACS_EMF_Tra::ACS_EMF_Logging.Open("EMF");
	ACS_PRC_API prcObj;
	int nExitCode = 0;
	if((prcObj.askForNodeState()) != 1)
	{
		ERROR(1,"%s","EMFINFO: PRC API is returning NODE STATE as PASSIVE");
		cerr << ACS_EMF_Execute::GetResultCodeText(EMF_RC_NOTACTIVENODE, &nExitCode) << endl << endl;
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return nExitCode;
	}
	int   ch;
	int   h_flag = 0;
	int   n_flag = 0;
	int   m_flag = 0;
	bool error = false;
	ACE_TCHAR* pNodename = NULL;
	long long int timeOutVal_180sec = 180*(1000000000LL);

	opterr = 0;
	while ((ch = getopt(argc, argv, "hmn:")) != EOF)
	{
		switch (ch)
		{
		case 'h':
			h_flag++;
			break;
		case 'm':
			m_flag++;
			break;
		case 'n':
			n_flag++;
			pNodename = optarg;
			break;
		case '?':
			error = true;
			break;
		default:
			error = true;
			break;
		} // End of switch
	} // End of while

	// Check given parameters
	if ((argv[optind] != NULL) || (h_flag > 1) || (n_flag > 1) || (m_flag > 1) || (argc != optind))
	{
		error = true;
	}

	if ((m_flag > 0) && ((h_flag > 0) || (n_flag > 0)))
	{
		error = true;
	}

	if(h_flag == 0 && n_flag == 1)
	{
		error = true;
	}

	if (error)
	{
		PrintUsage();
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return 2;
	}

	if ((n_flag == 1) && pNodename)
	{
		if (ACE_OS::strlen(pNodename) > 32)
		{
			PrintUsage();
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			return 2;
		}
	}

	ACE_TCHAR* pszAttrValue;//to be checked data should be more...
	std::string dnNameOfNode;
	std::string dnOfEMFHistObj;
	ACE_TCHAR dataBuffer[1024];
	ACS_CC_ReturnType enResult;

	if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
	{
		DEBUG(1,"%s","EMFINFO:EMF related XML are not configured!!");
		DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
		cout<<"Unable to connect to server\n\n";
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return 117;
	}

	if (m_flag == 1)
	{
		dnNameOfNode.append(ACS_EMF_Common::parentDNofEMF);

		// To EMF Feature is enabled or not
		OmHandler* omHandlerPtr = new OmHandler ();
		if( omHandlerPtr->Init() == ACS_CC_FAILURE )
		{
			DEBUG(1,"%s","EMFINFO:FAILED to init OmHandler for reading operationalState");
			DEBUG(1,"%s","EMFINFO:Exiting from emfcopy command with exit code as 117");
			cout<<"Unable to connect to server\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			return 117;
		}

		ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
		paramToFindPtr->attrName = new ACE_TCHAR [30];
		int operState = 0;

		ACE_OS::strcpy( paramToFindPtr->attrName , theEMFOperationalState);
		if( ( omHandlerPtr->getAttribute( dnNameOfNode.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
		{
			if (paramToFindPtr->attrValuesNum != 0)
			{
				operState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
			}
		}
		else
		{
			DEBUG(1,"%s","EMFINFO:EMFINFO failed to fetch operationalState from EMF MO!!");
			DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
			cout<<"Unable to connect to server\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			delete [] paramToFindPtr->attrName;
			omHandlerPtr->Finalize();
			return 117;
		}
		omHandlerPtr->Finalize();
		// END

		if (operState == 1)
		{
			acs_apgcc_adminoperation admOp;
			ACS_CC_ReturnType result;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
			int returnValue;

			result = admOp.init();

			if (result != ACS_CC_SUCCESS)
			{
				DEBUG(1,"%s","EMFINFO:EMF admin instance initialization is failed!!");
			}

			if (m_flag ==1 )
			{
				result = admOp.adminOperationInvoke(dnNameOfNode.c_str() , 0, EMF_CMD_TYPE_GETMEDIAINFO, vectorIN, &returnValue, timeOutVal_180sec, vectorOut);
			}

			if(result != ACS_CC_SUCCESS)
			{
				DEBUG(1,"EMFINFO:FAILED to invoke AdminOperation on %s",dnNameOfNode.c_str());
				DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
				cout<<"Unable to connect to server\n\n";
				admOp.freeadminOperationReturnValue (vectorOut);
				admOp.finalize();
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 117;
			}
			else
			{
				if( returnValue == SA_AIS_OK)
				{
					admOp.freeadminOperationReturnValue (vectorOut);
					admOp.finalize();

					// On successful Admin Operation invoke
					//					OmHandler* omHandlerPtr = new OmHandler ();
					if( omHandlerPtr->Init() == ACS_CC_FAILURE )
					{
						DEBUG(1,"%s","EMFINFO:FAILED to init OmHandler for reading Media and Owner Info");
						DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
						cout<<"Unable to connect to server\n\n";
						ACS_EMF_Tra::ACS_EMF_Logging.Close();
						return 117;
					}

					//					ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
					//					paramToFindPtr->attrName = new ACE_TCHAR [30];

					if (m_flag == 1)
					{
						// For Media Info Flag
						/*Fetch the Data For Media Type */
						cout << "DEVICE MEDIA INFORMATION\n\n";

						int val = -1;
						ACE_OS::strcpy( paramToFindPtr->attrName , theEMFInfoMediaType);
						if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNode.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
						{
							bool isRewritableMedia = false;
							if (paramToFindPtr->attrValuesNum != 0)
							{
								val = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));

								if(val == 2 || val == 3 || val == 4)
								{
									isRewritableMedia = true;
								}
								else
								{
									isRewritableMedia = false;
								}

								switch(val)
								{
								case 0:
									pszAttrValue = (char *) "DVD+R";
									break;
								case 1:
									pszAttrValue = (char *) "DVD-R";
									break;
								case 2:
									pszAttrValue = (char *) "DVD+RW";
									break;
								case 3:
									pszAttrValue = (char *) "DVD-RW";
									break;
								case 4:
									pszAttrValue = (char *) "THUMBDRIVE";
									break;
								}
								ACE_OS::sprintf(dataBuffer,
										ACE_TEXT("%-30s%s\n"),
										ACE_TEXT("Current media:"),
										pszAttrValue);
								cout << endl << dataBuffer;
								if(isRewritableMedia)
								{
									ACE_OS::sprintf(dataBuffer,
											ACE_TEXT("%-30s%s\n"),
											ACE_TEXT(""),
											ACE_TEXT("Writable and erasable"));
								}
								else
								{
									ACE_OS::sprintf(dataBuffer,
											ACE_TEXT("%-30s%s\n"),
											ACE_TEXT(""),
											ACE_TEXT("Writable"));
								}
								cout << dataBuffer;
							}
						}
						else
						{
							DEBUG(1,"%s","EMFINFO:EMFINFO failed to fetch media type info from MediaAndOwner!!");
							DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
							cout<<"Unable to connect to server\n\n";
							ACS_EMF_Tra::ACS_EMF_Logging.Close();
							delete [] paramToFindPtr->attrName;
							omHandlerPtr->Finalize();
							return 117;
						}

						// For Total DVD size
						ACE_OS::strcpy( paramToFindPtr->attrName , theEMFInfoTotalSize);

						if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNode.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
						{
							val = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
							ACS_EMF_Common::GetHWVersion();
							if( ACS_EMF_Common::GetHWVariant() >= 3)
							{
								pszAttrValue = (char *) "32.0 GB";
							}
							else
							{
								pszAttrValue = (char *) "4.37 GB";
							}

							ACE_OS::sprintf(dataBuffer,
									ACE_TEXT("%-30s%s \n"),
									ACE_TEXT("Total size:"),
									pszAttrValue);
							cout << dataBuffer;
						}
						else
						{
							DEBUG(1,"%s","EMFINFO:EMFINFO failed to fetch total size info from MediaAndOwner!!");
							DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
							cout<<"Unable to connect to server\n\n";
							ACS_EMF_Tra::ACS_EMF_Logging.Close();
							delete [] paramToFindPtr->attrName;
							omHandlerPtr->Finalize();
							return 117;
						}

						// For DVD Used Space
						ACE_OS::strcpy( paramToFindPtr->attrName , theEMFInfoUsedSpace);
						unsigned int val1 = 0;
						if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNode.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
						{
							val1 = *(reinterpret_cast<unsigned int*>(*(paramToFindPtr->attrValues)));
							ACE_OS::sprintf(dataBuffer,
									ACE_TEXT("%-30s%u %%\n"),
									ACE_TEXT("Used space:"),
									val1);
							cout << dataBuffer;
						}
						else
						{
							DEBUG(1,"%s","EMFINFO:EMFINFO failed to fetch used space info from MediaAndOwner!!");
							DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
							cout<<"Unable to connect to server\n\n";
							ACS_EMF_Tra::ACS_EMF_Logging.Close();
							delete [] paramToFindPtr->attrName;
							omHandlerPtr->Finalize();
							return 117;
						}

						// For DVD Free Space
						ACE_OS::strcpy( paramToFindPtr->attrName , theEMFInfoFreeSpace);

						if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNode.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
						{
							val1 = *(reinterpret_cast<unsigned int*>(*(paramToFindPtr->attrValues)));
							ACE_OS::sprintf(dataBuffer,
									ACE_TEXT("%-30s%u %%\n"),
									ACE_TEXT("Free space:"),
									val1);
							cout << dataBuffer;
						}
						else
						{
							DEBUG(1,"%s","EMFINFO:EMFINFO failed to fetch free space info from MediaAndOwner!!");
							DEBUG(1,"%s","EMFINFO:Exiting from emfinfo command with exit code as 117");
							cout<<"Unable to connect to server\n\n";
							ACS_EMF_Tra::ACS_EMF_Logging.Close();
							delete [] paramToFindPtr->attrName;
							omHandlerPtr->Finalize();
							return 117;
						}
					}

					delete [] paramToFindPtr->attrName;
					omHandlerPtr->Finalize();
				}  // END for myOIValidationReturnValue == SA_AIS_OK
				else
				{
					string errorText_temp = "";
					string errorText = "";
					if ( vectorOut.size() == 1 )
					{
						switch ( vectorOut[0].attrType )
						{
						case ATTR_STRINGT:
							errorText_temp = reinterpret_cast<char *>(vectorOut[0].attrValues);
							if ( errorText_temp.length() > 8 )
							{
								errorText = errorText_temp.substr(8);   // clipping the starting part @ComNbi@ in the actual error text
								cout << errorText.c_str() << endl << endl;
							}
							else
							{
								cout << "Other Error"  << endl << endl;
							}
							nExitCode = 1;
							break;
						default:
							ERROR(1,"%s","Unable to interpret the error");
							break;
						}
					}
					admOp.freeadminOperationReturnValue (vectorOut);
					admOp.finalize();
				}

			}
		}
		else
		{
			DEBUG(1,"%s","EMFINFO:operationalState is not enabled in EMF MO!!");
			DEBUG(1,"%s","EMFINFO:Exiting from emfcopy command with exit code as 36");
			cout<<"External media access is not enabled\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			delete [] paramToFindPtr->attrName;
			omHandlerPtr->Finalize();
			return 36;
		}
	}
	else if((h_flag == 1))
	{
		ACE_TCHAR pszAttrValue1[512];//to be checked
		int index = 1;
		cout << endl << "EXTERNAL MEDIA COPY INFORMATION" << endl << endl;
		while(1)
		{
			dnOfEMFHistObj.clear();
			dnOfEMFHistObj.append(ACE_TEXT(theEMFHistoryInfoRDN));
			dnOfEMFHistObj.append("=");
			char index_string[10];
			sprintf(index_string,"%d",index);
			dnOfEMFHistObj.append(index_string);
			dnOfEMFHistObj.append(",");
			dnOfEMFHistObj.append(ACS_EMF_Common::parentDNofEMF.c_str());

			if(n_flag == 1)
			{
				// For -n option
				if( ACS_EMF_Common::fetchEMFHistoryAttribute(dnOfEMFHistObj, (char*)theEMFHistoryInfoNodeName,pszAttrValue1) == 0 )
				{
					// Checking the Node Name
					if (ACE_OS::strcmp(pszAttrValue1,pNodename) == 0)
					{
						if( !fetchAllHistoryAttibutes(dnOfEMFHistObj.c_str()))
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				// For -h option
				if( !fetchAllHistoryAttibutes(dnOfEMFHistObj.c_str()) )
				{
					break;
				}
			}
			index++;
		}
	}
	else
	{
		// with out any option
		cout << endl << "EXTERNAL MEDIA COPY INFORMATION" << endl << endl;
		dnOfEMFHistObj.clear();
		dnOfEMFHistObj.append(ACE_TEXT(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN));
		dnOfEMFHistObj.append(",");
		dnOfEMFHistObj.append(ACS_EMF_Common::parentDNofEMF.c_str());

		if( !fetchAllHistoryAttibutes(dnOfEMFHistObj.c_str()))
		{
			//Error Code
			DEBUG(1,"%s","EMFINFO:EMFINFO failed while fetching info from emfCurrTrasactionInfo object!!");
			DEBUG(1,"%s","EMFINFO:This could be due to fetching failure or no history objects were created!!");
		}
	}

	cout << endl;
	ACS_EMF_Tra::ACS_EMF_Logging.Close();
	return nExitCode;
} // End of main

