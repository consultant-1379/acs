/*=================================================================== */
   /**
   @file acs_emf_emfcopy.cpp

   This file contains the code for the emfcopy command. This command
   is used to initiate a copy operation from DVD to APG folder and
   from APG to DVD.

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
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include <ace/ACE.h>
#include <saAis.h>
#include <string.h>
#include <acs_apgcc_adminoperation.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Util.H>
#include <acs_emf_defs.h>
#include "acs_emf_execute.h"
#include "acs_emf_common.h"
#include "acs_emf_tra.h"
#include "acs_emf_aeh.h"

using namespace std;

#define ACS_EMF_LABEL_ARG							ACE_TEXT("label")
#define ACS_EMF_VERIFY_ARG						    ACE_TEXT("verify")
#define ACS_EMF_OVERWRITE_ARG						ACE_TEXT("overwriteEnable")

static void PrintUsage();

extern ACE_TCHAR* optarg;
extern ACE_INT32  optind;
extern ACE_INT32  opterr;

/**
 * @brief	This function prints the usage of command.
 *
 *
 * @return	void
 */
void PrintUsage()
{
	cerr<< "Incorrect usage\n"
			<< "Usage:emfcopy -m TODVD [-l label] [-o [-f]] [-n] source...\n"
			<< "      emfcopy -m FROMDVD [-o [-f]] source... destination\n\n";


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
ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR* argv[])
{
	ACS_EMF_Tra::ACS_EMF_Logging.Open("EMF");

	// This command can only be executed on active node.
	ACS_PRC_API prcObj;
	int nExitCode = 0;
	if((prcObj.askForNodeState()) != 1)
	{
		ERROR(1,"%s","EMFCOPY: PRC API is returning NODE STATE as PASSIVE");
		cerr << ACS_EMF_Execute::GetResultCodeText(EMF_RC_NOTACTIVENODE, &nExitCode) << endl << endl;
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return nExitCode;
	}

	ACE_INT32   ch;
	ACE_INT32   f_flag = 0;
	ACE_INT32   l_flag = 0;
	ACE_INT32   m_flag = 0;
	ACE_INT32   n_flag = 0;
	ACE_INT32   o_flag = 0;
	ACE_INT32   u_flag = 0;

	// This flag indicates copy of specific files from DVD/CD to given location
	ACE_INT32   d_flag = 1;
	ACE_TCHAR* 	pType  = NULL;
	ACE_TCHAR* 	pLabel = NULL;
	opterr = 0;
	bool bError = false;
	long long int timeOutVal_180sec = 180*(1000000000LL);

	while ((ch = getopt(argc, argv, "fl:m:nou")) != EOF)
	{
		switch (ch)
		{
		case 'f':
			f_flag++;
			break;

		case 'l':
			l_flag++;
			pLabel = optarg;
			break;

		case 'm':
			m_flag++;
			pType = optarg;
			break;

		case 'n':
			n_flag++;
			break;

		case 'o':
			o_flag++;
			break;

		case 'u':
			u_flag++;
			break;

		case '?':
			bError = true;

		default:
			break;
		} // End of case
	} // End of while

	// Option parsing
	if (f_flag > 1 || l_flag > 1 || m_flag != 1 || n_flag > 1 || o_flag > 1 || u_flag > 1)
	{
		bError = true;
	}

	// Usage error
	if (bError)
	{
		PrintUsage();
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return 2;
	}

	// Parse media destination
	int dwCmdCode = 0;

	if (ACE_OS::strcmp(pType, "TODVD") == 0)
	{
		if (u_flag > 0 || (f_flag > 0 && o_flag == 0))
		{
			bError = true;
		}
		else
		{
			dwCmdCode = EMF_CMD_TYPE_COPYTOMEDIA;
		}
	}
	else if (ACE_OS::strcmp(pType, "FROMDVD") == 0)
	{
		if (l_flag > 0 || n_flag > 0 || u_flag > 0)
		{
			bError = true;
		}
		else if ( d_flag == 1 && f_flag == 1 && o_flag == 0)
		{
			bError = true;
		}
		else if (d_flag == 1 && (argc - optind)  == 1)
		{
			bError = true;
		}
		else
		{
			dwCmdCode = EMF_CMD_TYPE_COPYFROMMEDIA;
		}
	}
	else
	{
		bError = true;
	}

	// Check in no files or destination path were given
	if (optind >= argc || argv[optind] == NULL)
	{
		bError = true;
	}

	if (bError)
	{
		PrintUsage();
		ACS_EMF_Tra::ACS_EMF_Logging.Close();
		return 2;
	}

	std::vector<std::string> myArgumentList;
	ACE_INT32 nFileIndex = 0;
	ACE_TCHAR szFilePath[512];
	vector<string> vecFileArg;
	vector<string> vecFileNames;
	std::string name;
	string strFile;
	ACE_INT32 dwResult = EMF_RC_OK;
	bool bOverwrite = (o_flag == 1);

	if(dwCmdCode == EMF_CMD_TYPE_COPYTOMEDIA)
	{
		bool bVerify = (n_flag == 0);
		// get command arguments
		while (optind < argc && argv[optind] != NULL)
		{
			nFileIndex++;
			ACE_OS::strncpy(szFilePath, argv[optind], 511);
			szFilePath[511] = '\0';
			std::string filePath(szFilePath);
#if 0
			if(dwCmdCode == EMF_CMD_TYPE_COPYTOMEDIA)
			{
				ACS_EMF_Common::ExpandPath(filePath);
				dwResult = ACS_EMF_Common::ValidateFileArgumentToMedia( filePath.c_str() );
			}
			if (dwResult != EMF_RC_OK)
			{
				DEBUG(1,"%s","EMFCOPY:Validation of file arguments failed for TODVD Operation!!");
				cerr << ACS_EMF_Execute::GetResultCodeText(dwResult, &nExitCode) << endl << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return nExitCode;
			}
#endif
			strFile = filePath;
			vecFileArg.push_back(strFile);
			//cout<<"to media"<<strFile;
			optind++;
		} // End of while
		if (l_flag && (strlen(pLabel) != 0))
		{
			dwResult = ACS_EMF_Common::ValidateLabel(pLabel);
			if (dwResult != EMF_RC_OK)
			{
				DEBUG(1,"GetResultCode: %d",dwResult);
				cerr << ACS_EMF_Execute::GetResultCodeText(dwResult, &nExitCode) << endl << endl;
				DEBUG(1,"%s","EMFCOPY:Validation of Label failed for TODVD Operation!!");
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return nExitCode;
			}
		}

		if (bOverwrite && f_flag == 0)
		{
			// Issue check printout
			if ( dwCmdCode == EMF_CMD_TYPE_COPYTOMEDIA) /*(dwCmdCode == EMF_CMD_TYPE_COPYTOTAPE ||*/
			{
				cout << "Are you sure you want to overwrite/erase any existing data on the media?\n"
						<< "Enter y or n [default: n]\03: " << flush;
			}
			else
			{
				cout << "Are you sure you want to overwrite any existing file(s) with the same name(s)?\n"
						<< "Enter y or n [default: n]\03: " << flush;
			}

			ACE_TCHAR szResponce[64];
			(void)cin.getline(szResponce, 63);

			// Quit here?
			if (!(*szResponce) || ACE_OS::strcmp(szResponce, "no") == 0 || ACE_OS::strcmp(szResponce, "n") == 0)
			{
				cout << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 0;
			}

			if (ACE_OS::strcmp(szResponce, "yes") != 0 && ACE_OS::strcmp(szResponce, "y") != 0)
			{
				cerr << "Unreasonable value" << endl << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 3;
			}
		}

		// Add command arguments to pass to server
		std::string myStr;
		myStr = (pLabel ? pLabel : "");

		std::string dnNameOfNodeObj;

		if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
		{
			DEBUG(1,"%s","EMFCOPY:EMF related XML are not configured!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			cout<<"Unable to connect to server\n\n";
			return 117;
		}
		dnNameOfNodeObj.append(ACS_EMF_Common::parentDNofEMF);
		// To EMF Feature is enabled or not
		OmHandler* omHandlerPtr = new OmHandler ();
		if( omHandlerPtr->Init() == ACS_CC_FAILURE )
		{
			DEBUG(1,"%s","EMFCOPY:FAILED to init OmHandler for reading operationalState");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
			cout<<"Unable to connect to server\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			return 117;
		}

		ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
		paramToFindPtr->attrName = new ACE_TCHAR [30];
		int operState = 0;
		ACS_CC_ReturnType enResult;

		ACE_OS::strcpy( paramToFindPtr->attrName , theEMFOperationalState);
		if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNodeObj.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
		{
			if (paramToFindPtr->attrValuesNum != 0)
			{
				operState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
			}
		}
		else
		{
			DEBUG(1,"%s","EMFCOPY:EMFCOPY failed to fetch operationalState from EMF MO!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
			cout<<"Unable to connect to server\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			delete [] paramToFindPtr->attrName;
			omHandlerPtr->Finalize();
			return 117;
		}
		omHandlerPtr->Finalize();
        // END.

		if (operState == 1)
		{
			// EMF Feature is enabled
			// Admin action using APGCC
			acs_apgcc_adminoperation admOp;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
			std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
			int returnValue;
			ACS_CC_ReturnType result;

			ACS_APGCC_AdminOperationParamType firstElem;
			ACS_APGCC_AdminOperationParamType secondElem;
			ACS_APGCC_AdminOperationParamType thirdElem;

			firstElem.attrName = (char *) ACS_EMF_LABEL_ARG;
			firstElem.attrType = ATTR_STRINGT;
			firstElem.attrValues=reinterpret_cast<void*>((char *)myStr.c_str());

			secondElem.attrName = (char *) ACS_EMF_VERIFY_ARG;
			secondElem.attrType = ATTR_STRINGT ;
			secondElem.attrValues = reinterpret_cast<void*>((char *)(bVerify ? "true" : "false"));

			thirdElem.attrName = (char *) ACS_EMF_OVERWRITE_ARG;
			thirdElem.attrType = ATTR_STRINGT ;
			thirdElem.attrValues = reinterpret_cast<void*>((char *)(bOverwrite ? "true" : "false"));

			/*insert parameter into the vector*/
			vectorIN.push_back(firstElem);
			vectorIN.push_back(secondElem);
			vectorIN.push_back(thirdElem);

			int i = 0;
			std::string flist;
			name.clear();
			flist.clear();
			name.append("fileList");

			for ( i = 0; i < nFileIndex;i++)
			{
				flist.append(vecFileArg.at(i));
				flist.append(";");
			}

			ACS_APGCC_AdminOperationParamType elem;
			elem.attrName = (char *)name.c_str();
			elem.attrType = ATTR_STRINGT ;
			elem.attrValues = reinterpret_cast<void*>((char *)flist.c_str());
			vectorIN.push_back(elem);

			result = admOp.init();

			if (result != ACS_CC_SUCCESS)
			{
				DEBUG(1,"%s","EMFCOPY:EMF admin instance initialization is failed!!");
			}

			if (bOverwrite)
			{
				result = admOp.adminOperationInvoke(dnNameOfNodeObj.c_str() , 0, EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA, vectorIN, &returnValue, timeOutVal_180sec, vectorOut);
			}
			else
			{
				result = admOp.adminOperationInvoke(dnNameOfNodeObj.c_str() , 0, EMF_CMD_TYPE_COPYTOMEDIA, vectorIN, &returnValue, timeOutVal_180sec, vectorOut);
			}
			string errorText_temp = "";
			string errorText = "";
			if ( vectorOut.size() == 1 )
			{
				//			for(int j =0; j < 2; j++)
				//			{
				switch ( vectorOut[0].attrType )
				{
					case ATTR_STRINGT:
						errorText_temp = reinterpret_cast<char *>(vectorOut[0].attrValues);
						if ( errorText_temp.length() > 8 )
						{
							errorText = errorText_temp.substr(8);	// clipping the starting part @ComNbi@ in the actual error text
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
				//			}
			}

			admOp.freeadminOperationReturnValue (vectorOut);
			vecFileArg.clear();
			vecFileNames.clear();

			if (result != ACS_CC_SUCCESS)
			{
				cout<<"Unable to connect to server\n\n";
				DEBUG(1,"EMFCOPY:FAILED to invoke AdminOperation on %s",dnNameOfNodeObj.c_str());
				DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
				DEBUG(1,"EMFCOPY: ERROR CODE = %d",admOp.getInternalLastError());
				DEBUG(1,"EMFCOPY: ERROR MESSAGE = %s",admOp.getInternalLastErrorText());
				admOp.finalize();
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 117;
			}

			admOp.finalize();
		}
		else
		{
			DEBUG(1,"%s","EMFCOPY:operationalState is not enabled in EMF MO!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 36");
			cout<<"External media access is not enabled\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			delete [] paramToFindPtr->attrName;
			omHandlerPtr->Finalize();
			return 36;
		}
	}
	else // EMF_CMD_TYPE_COPYFROMDVD Operation
	{
		while (optind < argc && argv[optind] != NULL)
		{
			nFileIndex++;
			strncpy(szFilePath, argv[optind], 511);
			szFilePath[511] = '\0';
			if(dwCmdCode == EMF_CMD_TYPE_COPYFROMMEDIA)
			{
				if ( d_flag == 1)
				{

					if (optind == argc)
					{

						dwResult = ACS_EMF_Common::ValidateFileArgumentFromMedia(szFilePath);

					}

				}

			}

			if (dwResult != EMF_RC_OK)
			{
				cerr << ACS_EMF_Execute::GetResultCodeText(dwResult, &nExitCode) << endl << endl;
				DEBUG(1,"%s","EMFCOPY:Validation of file arguments failed for FROMDVD Operation!!");
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return nExitCode;
			}

			strFile = szFilePath;
			//cout<< ":from media"<<strFile;
			vecFileArg.push_back(strFile);
			optind++;
		}

		if (bOverwrite && f_flag == 0)
		{
			cout << "Are you sure you want to overwrite any existing file(s) with the same name(s)?\n"
					<< "Enter y or n [default: n]\03: " << flush;

			ACE_TCHAR szResponce[64];
			(void)cin.getline(szResponce, 63);

			// Quit here?
			if (!(*szResponce) || ACE_OS::strcmp(szResponce, "no") == 0 || ACE_OS::strcmp(szResponce, "n") == 0)
			{
				cout << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 0;
			}

			if (ACE_OS::strcmp(szResponce, "yes") != 0 && ACE_OS::strcmp(szResponce, "y") != 0)
			{
				cerr << "Unreasonable value" << endl << endl;
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 3;
			}
		}

		// Admin action using APGCC
		acs_apgcc_adminoperation admOp;
		ACS_CC_ReturnType result;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
		std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
		int returnValue;
		std::string dnNameOfNodeObj;
		ACS_APGCC_AdminOperationParamType firstElem;

		firstElem.attrName = (char *) ACS_EMF_OVERWRITE_ARG;
		firstElem.attrType = ATTR_INT32T ;
		int val = (bOverwrite ? 1 : 0);
		firstElem.attrValues = reinterpret_cast<void*>(&val);

		/*insert parameter into the vector*/
		vectorIN.push_back(firstElem);
		std::string flist;
		std::string dname;
		ACS_APGCC_AdminOperationParamType fileElem;
		ACS_APGCC_AdminOperationParamType dirElem;
		dname.clear();
		name.clear();
		flist.clear();

		if (nFileIndex > 1)
		{   // SPECIFIC FILE COPY FROM DVD MEDIA
			name.append("fileList");
			int i = 0;
			for ( i = 0; i < nFileIndex - 1;i++)
			{
				flist.append(vecFileArg.at(i));
				flist.append(";");
			}

			fileElem.attrName = (char *)name.c_str();
			fileElem.attrType = ATTR_STRINGT ;
			fileElem.attrValues = reinterpret_cast<void*>((char *)flist.c_str());
			vectorIN.push_back(fileElem);

			dname.append("destinationFolder");
			dirElem.attrName = (char *)dname.c_str();
			dirElem.attrType = ATTR_STRINGT ;
			dirElem.attrValues = reinterpret_cast<void*>((char *)vecFileArg.at(nFileIndex - 1).c_str());
			vectorIN.push_back(dirElem);


		}
		else
		{
			// FULL CONTETNTS COPY
			dname.append("dname");
			dirElem.attrName = (char *)dname.c_str();
			dirElem.attrType = ATTR_STRINGT ;
			dirElem.attrValues = reinterpret_cast<void*>((char *)vecFileArg.at(nFileIndex-1).c_str());
			vectorIN.push_back(dirElem);
		}
		if (ACS_EMF_Common::fetchDnOfRootObjFromIMM() == -1)
		{
			cout<<"Unable to connect to server\n\n";
			DEBUG(1,"%s","EMFCOPY:EMF related XML are not configured!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			return 117;
		}

		dnNameOfNodeObj.append(ACS_EMF_Common::parentDNofEMF);
		// To EMF Feature is enabled or not
		OmHandler* omHandlerPtr = new OmHandler ();
		if( omHandlerPtr->Init() == ACS_CC_FAILURE )
		{
			DEBUG(1,"%s","EMFCOPY:FAILED to init OmHandler for reading operationalState");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
			cout<<"Unable to connect to server\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			return 117;
		}

		ACS_CC_ImmParameter* paramToFindPtr = new ACS_CC_ImmParameter();
		paramToFindPtr->attrName = new ACE_TCHAR [30];
		int operState = 0;
		ACS_CC_ReturnType enResult;

		ACE_OS::strcpy( paramToFindPtr->attrName , theEMFOperationalState);
		if( ( enResult = omHandlerPtr->getAttribute( dnNameOfNodeObj.c_str(), paramToFindPtr ) ) == ACS_CC_SUCCESS )
		{
			if (paramToFindPtr->attrValuesNum != 0)
			{
				operState = *(reinterpret_cast<int*>(*(paramToFindPtr->attrValues)));
			}
		}
		else
		{
			DEBUG(1,"%s","EMFCOPY:EMFCOPY failed to fetch operationalState from EMF MO!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
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
			result = admOp.init();

			if (result != ACS_CC_SUCCESS)
			{
				DEBUG(1,"%s","EMFCOPY:EMF admin instance initialization is failed!!");
			}

			result = admOp.adminOperationInvoke(dnNameOfNodeObj.c_str() , 0, EMF_CMD_TYPE_COPYFROMMEDIA, vectorIN, &returnValue, timeOutVal_180sec, vectorOut);

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

			vecFileArg.clear();
			vecFileNames.clear();

			if (result != ACS_CC_SUCCESS)
			{
				cout<<"Unable to connect to server\n\n";
				DEBUG(1,"EMFCOPY:FAILED to invoke AdminOperation on %s",dnNameOfNodeObj.c_str());
				DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 117");
				DEBUG(1,"EMFCOPY: ERROR CODE = %d",admOp.getInternalLastError());
				DEBUG(1,"EMFCOPY: ERROR MESSAGE = %s",admOp.getInternalLastErrorText());
				admOp.finalize();
				ACS_EMF_Tra::ACS_EMF_Logging.Close();
				return 117;
			}

			admOp.finalize();
		}
		else
		{
			DEBUG(1,"%s","EMFCOPY:operationalState is not enabled in EMF MO!!");
			DEBUG(1,"%s","EMFCOPY:Exiting from emfcopy command with exit code as 36");
			cout<<"External media access is not enabled\n\n";
			ACS_EMF_Tra::ACS_EMF_Logging.Close();
			delete [] paramToFindPtr->attrName;
			omHandlerPtr->Finalize();
			return 36;
		}
	}

	ACS_EMF_Tra::ACS_EMF_Logging.Close();
	return nExitCode;
} // End of main

