//******************************************************************************
//
//  NAME
//   rtrch.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 2002. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//  This file implements the 'change' command for RTR which
//  allows a user to re-configure the parameter settings.
// 	 
//  DOCUMENT NO
//	   190 89-CAA 109 0573
//
//  AUTHOR 
//  2005-10-14 by EAB/UZ/DG UABCAJN
//
//  CHANGE HISTORY
//  VERSION	DATE	MODIFIED BY	DESCRIPTION
//  R4E		070226	XCSVAAN		For TR HH71761
//
// CHANGES
// RELEASE REVISION HISTORY
// Version   DATE		NAME			DESCRIPTION
//           2007-05-2	GP/AP/A QALEDIR	Blade Cluster adaptations
//  R1K      2010-11-17  XCSSWKO        TR HN24042  
//******************************************************************************
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <acs_rtr_cmd_util.h>
#include <iostream>
#include <acs_rtr_lib.h>
#include "ACS_CS_API.h"
#include <ace/ACE.h>
#include<string>
#include "acs_rtr_global.h"

#include "acs_rtr_defaultvalues.h"
using namespace std;
#define RESPBUFSIZE             128
#define N_of_AP 16

// string constants
const char* const UNRVAL="Unreasonable value";
const char* const VALRANGE="Valid Value Range: ";
const char* const R_AP="apidentity";
const char* const R_A="(Bytes 512-32768)"; // value range for -a parameter
const char* const R_B="(ENABLE, DISABLE)";
const char* const R_C="(32 HEX DIGITS)";
const char* const R_D="(FIXED,VARIABLE,EVEN)";
const char* const R_E="(Seconds 1-7200)";
const char* const R_G="(Numeral 0-255)";
const char* const R_H="(KiloBytes 64-32768 and >MinFileSize)";   //TR HN24042 
const char* const R_J="(Seconds 1-86400)";
const char* const R_M="(YES, NO)";
const char* const R_N="(Bytes 512-32768)";
const char* const R_O="(Numeral 0-255)";
// Range got updated to handle in terms of bytes for TR HH71761
const char* const R_P="(Bytes 0-16777216 and <FileSize)";


//*************************************************************************
// isHexString
//
// Checks whether a string contains hex characters (0..9,a..f) or not.

// return values:
//   true  : ok
//   false : no hex string
//*************************************************************************
bool isHexString(int exp_length,unsigned char* s)
{
	bool ret=true;
	if (exp_length!=32) ret=false;
	for (int i=0;i<exp_length;i++)
	{
		if (!isdigit(s[i]) && (s[i]<'A' || s[i]>'F') && 
				(s[i]<'a' || s[i]>'f'))
		{
			ret=false;
			break;
		}
	}
	return ret;
}


//*************************************************************************
// checkTime
//
// Checks that the value for GenerationTime is within its allowed range.

// return values:
//   true  : ok
//   false : unreasonable value
//*************************************************************************
bool checkTime(const char* optVal,char* errText)
{
	// 00:00 .. 23:55, 5 minute intervals.
	bool ret=true;
	if (strlen(optVal)!=5)
	{
		strcpy(errText,"format is hh:mm");
		ret=false;
	}
	else if (optVal[2] != ':')
	{
		strcpy(errText,"use colon to separate hh and mm");
		ret=false;
	}
	else
	{
		for (int i=0; i<5; i++)
		{
			if (i==2) continue;

			if (!isdigit(optVal[i])) 
			{
				strcpy(errText,"only digits allowed");
				ret=false;
				break;
			}
		}
		if (ret)
		{
			if (((optVal[0]-48)*10 + (optVal[1]-48)) > 23) 
			{
				strcpy(errText,"value range:0-23");
				ret= false;
			}
			else if (((optVal[3]-48)*10 + (optVal[4]-48)) > 55)
			{
				strcpy(errText,"value range:0-55");
				ret= false;
			}
			else if ((optVal[4]-48)!=0 && (optVal[4]-48) != 5) 
			{
				strcpy(errText,"only 5 minute intervals allowed");
				ret=false;
			}
		}
	}
	return ret;
}

bool checkAlphabet(const char* arg)
{
bool flag = true;
for(unsigned int i=0;  i< strlen(arg) && flag == true; i++ )
{
   if( isalpha(arg[i]) )
     flag = true;
   else
     flag = false;
}
return flag;
}

bool checkNumeric(const char* arg)
{
bool flag = true;
for(unsigned int i=0;  i< strlen(arg) && flag == true; i++ )
{
   if( isdigit(arg[i]) )
     flag = true;
   else
     flag = false;
}
return flag;
}

//*************************************************************************
// checkValidity
//
// Checks the value range of a parameter.

// return values:
//   true  : ok
//   false : non valid string
//*************************************************************************
bool checkValidity(parID p,const char* arg,char* errText)
{

	bool ret=true;
	if (strlen(arg)==0)
	{
		strcpy(errText,"no parameter value");
		return false;
	}
	//int parValue;
	ACE_INT32 parValue = 0;
	strcpy(errText,VALRANGE);
	if (p==a_blockLength || p==e_blockHoldTime || p==g_blockPaddingChar ||
		p==h_fileSize || p==j_fileHoldTime || p==n_fileRecordLength ||
		p==o_filePaddingChar || p==p_minFileSize)
	{
		parValue=atoi(arg);
		if (p==a_blockLength)
		{
                        if(checkAlphabet(arg))
	                {
        			strcat(errText,R_A);
                                ret=false;
			}                	
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_BLOCKLENGTH || parValue>MAX_BLOCKLENGTH)
			{
				strcat(errText,R_A);
				ret=false;
			}
                        }
                        else
                        {
                        	strcat(errText,R_A);
                         	ret=false;
                        }
		}
		else if (p==e_blockHoldTime)
		{
                        if(checkAlphabet(arg))
                        {
                                strcat(errText,R_E);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_BLOCKHOLDTIME || parValue>MAX_BLOCKHOLDTIME)
			{
				strcat(errText,R_E);
				ret=false;
			}
                        }
                        else
                        {
                                strcat(errText,R_A);
                                ret=false;
                        }

		}
		else if (p==g_blockPaddingChar)
		{
                        if(checkAlphabet(arg))
                        {
                                strcat(errText,R_G);
                                ret=false;
                        }
			else if(checkNumeric(arg))
                        {
			if (parValue<MIN_BLOCKPADDINGCHAR || parValue>MAX_BLOCKPADDINGCHAR)
			{
				strcat(errText,R_G);
				ret=false;
			}
                        }
                        else
                        {
                                strcat(errText,R_G);
                                ret=false;
                        }
		}
		else if (p==h_fileSize)
		{
			if(checkAlphabet(arg))
                        {
                                strcat(errText,R_H);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_FILESIZE || parValue>MAX_FILESIZE)
			{
				strcat(errText,R_H);
				ret=false;
			}
			}
			else
                        {
                                strcat(errText,R_H);
                                ret=false;
                        }
		}
		else if (p==j_fileHoldTime)
		{
			if(checkAlphabet(arg))
                        {
                                strcat(errText,R_J);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_FILEHOLDTIME || parValue>MAX_FILEHOLDTIME)
			{
				strcat(errText,R_J);
				ret=false;
			}
			}
			else
                        {
                                strcat(errText,R_J);
                                ret=false;
                        }
		}
		else if (p==n_fileRecordLength)
		{
			if(checkAlphabet(arg))
                        {
                                strcat(errText,R_N);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_FILERECLENGTH || parValue>MAX_FILERECLENGTH)
			{
				strcat(errText,R_N);
				ret=false;
			}
			}
			else
                        {
                                strcat(errText,R_N);
                                ret=false;
                        }
		}
		else if (p==o_filePaddingChar)
		{
			if(checkAlphabet(arg))
                        {
                                strcat(errText,R_O);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_FILEPADDINGCHAR || parValue>MAX_FILEPADDINGCHAR)
			{
				strcat(errText,R_O);
				ret=false;
			}
			}
			else
                        {
                                strcat(errText,R_O);
                                ret=false;
                        }
		}
		else if (p==p_minFileSize)
		{
			if(checkAlphabet(arg))
                        {
                                strcat(errText,R_P);
                                ret=false;
                        }
                        else if(checkNumeric(arg))
                        {
			if (parValue<MIN_MINFILESIZE || parValue>MAX_MINFILESIZE)
			{
				strcat(errText,R_P);
				ret=false;
			}
			}
			else
                        {
                                strcat(errText,R_P);
                                ret=false;
                        }
		}
		else 
		{
			strcpy(errText,"internal program error");
			ret= false;
		}
	} // end number parameter.

	else if (p==d_blockLengthType)
	{
                if(checkNumeric(arg))
                {
			strcat(errText,R_D);
                        ret=false;
                }
                else if(checkAlphabet(arg))
                {
		if (stricmp(arg,"FIXED") && stricmp(arg,"VARIABLE") && stricmp(arg,"EVEN"))
		{
			strcat(errText,R_D);
			ret=false;
		}
                }
                else
                {
                	strcat(errText,R_D);
                        ret=false;
                }
	}
	else if (p==m_fixedFileRecords)
	{
		if(checkNumeric(arg))
                {
                        strcat(errText,R_M);
                        ret=false;
                }
                else if(checkAlphabet(arg))
                {
		if (stricmp(arg,"YES") && stricmp(arg,"NO"))
		{
			strcat(errText,R_M);
			ret=false;
		}
                }
		else
                {
                        strcat(errText,R_M);
                        ret=false;
                }
	}
	else if (p==c_key)
	{
		if (stricmp(arg,"UNDEFINED") && !isHexString(strlen(arg),(unsigned char*)arg))
		{
			strcat(errText,R_C);
			ret=false;
		}
	}
	else if (p==b_cdrFileFormat)
	{
		if(checkNumeric(arg))
                {
                        strcat(errText,R_B);
                        ret=false;
                }
                else if(checkAlphabet(arg))
                {
		if (stricmp(arg,"ENABLE") && stricmp(arg,"DISABLE"))
		{
			strcat(errText,R_B);
			ret=false;
		}
		}
		else
                {
                        strcat(errText,R_B);
                        ret=false;
                }
	}
	else 
	{
		strcpy(errText,"internal program error");
		ret= false;
	}
	return ret;
}

//*************************************************************************
// incorrectForm
//
// Prints usage information for rtrch command.

// return values:
//   -
//*************************************************************************
void incorrectForm()
{

    cout<<left;
	cout<<"Incorrect usage"<<endl;
    cout<<"Usage:"<<endl;
        cout<<"rtrch ";
        cout<<"[-ap apidentity]";
        cout<<"[-a blocklength]";
        cout<<"[-d blocklengthtype] ";
        cout<<"[-e blockholdtime] ";
        cout<<"[-g blockpaddingchar] ";
        cout<<"[-f ] ";
        cout<<"messagestore"<<endl;
        cout<<"rtrch ";
        cout<<"[-ap apidentity] ";
        cout<<"[-b cdrcounter] ";
        cout<<"[-c key] ";
        cout<<"[-h filesize] ";
        cout<<"[-j fileholdtime] ";
        cout<<"[-m fixedfilerecords] ";
        cout<<"[-n filerecordlength] ";
        cout<<"[-o filepaddingchar] ";
        cout<<"[-p minfilesize] ";
        cout<<"[-f ] ";
        cout<<"messagestore"<<endl;

}

//*******************************************************************************************
//                                isMultipleCpSystem
//********************************************************************************************
bool isMultipleCpSystem()
{
	bool isMultipleCPSystem = false;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
	if (result == ACS_CS_API_NS::Result_Success)
	{
		return isMultipleCPSystem;
	}
	else
	{//error
		switch (result)
		{
		case ACS_CS_API_NS::Result_NoEntry: 
			{
				throw 56;
			}
		case ACS_CS_API_NS::Result_NoValue:
			{
				throw 55;
			}
		case ACS_CS_API_NS::Result_NoAccess:
			{
				throw 55;
			}
		case ACS_CS_API_NS::Result_Failure:
			{
				throw 55;
			}
		default:
			{
				throw 56;
			}
		}
	}//negative case
}
//***************************************************************************************
//             CheckApName
//***************************************************************************************
void CheckApName(char* apname,unsigned int& apid)
{
	if(apname)
	{
		string ap(apname);
		string str;
		//check len of the name
		if((ap.length() > 5) || (ap.length() < 3))
		{
			throw 113;
		}
		//check first 2 chars
		str = ap.substr(0,2);
		if((str.compare("AP") != 0) && (str.compare("ap") != 0) && (str.compare("Ap") != 0)
			&& (str.compare("aP") != 0))
		{
			throw 113;
		}
		//check last chars
		str = ap.substr(2,(ap.length() - 2));
		for (unsigned int i = 0; i < std::strlen (str.c_str()); i++) {
			if (! std::isdigit (str[i])) {
				throw 113;
			}
		}
		if( ( ACE_OS::atoi(str.c_str()) > 16 ) || ( ACE_OS::atoi(str.c_str()) < 1) )
		{
			throw 113;
		}
		//set id
		apid = 2000 + ACE_OS::atoi(str.c_str());
	}
	else
	{
		throw 113;
	}
}
//******************************************************************************
// get_All_AP_sysId_in_Cluster
//
// return parameters:
// the function returns the number of systemIds it has stored in apSysIds[].
//******************************************************************************
ACE_INT32 get_All_AP_sysId_in_Cluster(unsigned int apSysIds[16])
{
	ACE_INT32 noOfAP=0;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NS::Result_Failure;
    //create HWC instance
	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();
	if (hwc)
	{
		//create boardSearch instance
		ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();  
		if (boardSearch)
		{
			ACS_CS_API_IdList boardList;
			boardSearch->setSysType(ACS_CS_API_HWC_NS::SysType_AP);//set SysType
			//search for all board ids (wildcard search)
			result = hwc->getBoardIds(boardList, boardSearch); 
			if (result == ACS_CS_API_NS::Result_Success)
			{
				for (unsigned int i = 0; i < boardList.size(); i++)//step through all the boards in the hwc table
				{
					BoardID boardId = boardList[i];
					unsigned short sysId = 0;

					result = hwc->getSysId(sysId, boardId);//get the system id of current board
					bool alreadyStored=false;
					if (result == ACS_CS_API_NS::Result_Success)
					{
						//check that sysId is not already in array.
						for (ACE_INT32 j=0;j<noOfAP;j++)
							if (apSysIds[j]==sysId) alreadyStored=true;
						if (alreadyStored==false)
						{
							apSysIds[noOfAP++]=sysId;
						}
					}            
				}
			}//get boardIds
			else
			{
				ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);//delete boardsearch
				ACS_CS_API::deleteHWCInstance(hwc); //delete hwc
				throw 56;
			}
			ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);//delete Bordsearch
		}//boardSearch
		else
		{
			ACS_CS_API::deleteHWCInstance(hwc);//delete hwc
			throw 56;
		}
		ACS_CS_API::deleteHWCInstance(hwc); //delete hwc
	}//hwc
	else
	{
		throw 56;
	}
	return noOfAP;
}
//******************************************************************************
//                         CheckData
//*******************************************************************************
bool CheckData(const ACE_TCHAR* data)
{
	if(data)
	{
		string str(data);
		ACE_INT32 ind = 0;
		if(str.compare("") == 0)
			return false;
		else
		{
			//find char -
			ind = (ACE_INT32)str.find_first_of("-",0);
			if(ind == 0)
				return false;
			else
				return true;
		}
	}
	else
		return false;
}

//*************************************************************************
// M A I N    r t r c h
//*************************************************************************
ACE_INT32 main(ACE_INT32 argc, ACE_TCHAR** argv)
{
	bool fileParameter=false;
	bool blockParameter=false;
	bool twoFileSizeParameters=false;
	ACE_TCHAR errText[500] = {0};
	ACE_TCHAR MessageStoreName[16] = {0};
	ACE_TCHAR ApIdentity[6] = {0};
	bool MCP = false;
	RTRMS_Parameters parameters;
	int parIssued[128];
	int MaxNumberArg = 0;
	int MinNumberArg = 0;
	unsigned int apID = 0;
	memset(parIssued,0,128*sizeof(int));
	//CHECK CPSystem
	try { MCP = isMultipleCpSystem(); }
	catch(ACE_INT32 cod)
	{
		if(cod == 55)
		{
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigConn<<endl;
			return 55;
		}
		if(cod == 56)
		{
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigError<<endl;
			return 56;
		}
	}
	//check number of Arguments on command line
	if(argc < 4)
	{
		incorrectForm();
		return 2;
	}
	//set max and min number of arg on cmd line (Not Ap option)
	MinNumberArg = 4; 
	MaxNumberArg = 23;

	if (argc < MinNumberArg || argc > MaxNumberArg)
	{
		incorrectForm();
		return 2;
	}
	//check command line
	int argCnt = 1;
	int LastArg = argc - 1;
	//Analyze option
	while (argCnt < (LastArg))
	{
		if (!strcmp(argv[argCnt], "-a"))                   //1
		{
			parIssued[a_blockLength] += 1;//set oprion in array
			parameters.BlockLength=atoi(argv[argCnt+1]); //set the parameter
			blockParameter=true;
			if (!checkValidity(a_blockLength,argv[argCnt+1],errText))//CHECK the value
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-b"))              //2
		{
			parIssued[b_cdrFileFormat] += 1; //set option in array
			fileParameter=true;
			if (!checkValidity(b_cdrFileFormat,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
			else
			{
				parameters.FiCdrFormat=false;
				if (!stricmp(argv[argCnt+1],"ENABLE"))
				{
					parameters.FiCdrFormat=true;
				}
			}
		}
		else if (!strcmp(argv[argCnt], "-c"))                   //3
		{
			parIssued[c_key] += 1;
			strcpy(parameters.FiKey,argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(c_key,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
			else
			{
				parameters.HmacMd5=true;
			}
		}
		else if(!strcmp(argv[argCnt], "-d"))	              //4
		{
			parIssued[d_blockLengthType] += 1;				
			if ( !stricmp(argv[argCnt+1],"FIXED") )
				parameters.BlockLengthType = FIXED;
			else if ( !stricmp(argv[argCnt+1],"VARIABLE") )
				parameters.BlockLengthType = VARIABLE;
			else if ( !stricmp(argv[argCnt+1],"EVEN"))
				parameters.BlockLengthType = EVEN;

			blockParameter=true;
			if (!checkValidity(d_blockLengthType,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if(!strcmp(argv[argCnt], "-e"))                   //5
		{
			parIssued[e_blockHoldTime] += 1;
			parameters.BlockHoldTime=atoi(argv[argCnt+1]);
			blockParameter=true;
			if (!checkValidity(e_blockHoldTime,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-f"))	               //6
		{
			parIssued[f_forcedFlag] += 1;
			argCnt--; // this option is without value
		}
		else if (!strcmp(argv[argCnt], "-g"))	               //7
		{
			parIssued[g_blockPaddingChar] += 1;
			parameters.BlPaddingChar=atoi(argv[argCnt+1]);
			blockParameter=true;
			if (!checkValidity(g_blockPaddingChar,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-h"))	              //8
		{

			parIssued[h_fileSize] += 1;
			parameters.FileSize=atoi(argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(h_fileSize,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-j"))                  //9 
		{
			parIssued[j_fileHoldTime] += 1;
			parameters.FiFileHoldTime=atoi(argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(j_fileHoldTime,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-m"))                  //12
		{
			parIssued[m_fixedFileRecords] += 1;
			if ( !stricmp(argv[argCnt+1], "YES"))
				parameters.FixedFileRecords =FIXED;
			else if ( !stricmp(argv[argCnt+1], "NO") )
				parameters.FixedFileRecords = VARIABLE;
			fileParameter=true;
			if (!checkValidity(m_fixedFileRecords,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-n"))                  //13
		{
			parIssued[n_fileRecordLength] += 1;
			parameters.FileRecordLength=atoi(argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(n_fileRecordLength,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-o"))                  //14
		{
			parIssued[o_filePaddingChar] += 1;
			parameters.FiPaddingChar=atoi(argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(o_filePaddingChar,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if (!strcmp(argv[argCnt], "-p"))	                //15
		{
			parIssued[p_minFileSize] += 1;
			parameters.MinFileSize=atoi(argv[argCnt+1]);
			fileParameter=true;
			if (!checkValidity(p_minFileSize,argv[argCnt+1],errText))
			{
				cout<<UNRVAL<<endl<<errText<<endl;
				return 3;
			}
		}
		else if(!strcmp(argv[argCnt],"-ap"))                    //16
		{
			try {
				CheckApName(argv[argCnt + 1],apID);
			}
			catch(int ret) {
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			strcpy(ApIdentity,argv[argCnt + 1]);
			parIssued[ap_apidentity] += 1;
		}
		else
		{//error in command line
			incorrectForm();
			return 2;
		}
		argCnt = argCnt + 2;
	}
	// check that a parameter is not given more than once.
	ACE_INT32 parIndex=0;
	while (parIndex<= 14)
	{
		if (parIssued[parIndex++]>1)
		{
			incorrectForm();
			return 2;
		}
	}
	//check # of arg in cmd lin AP option case
	if((parIssued[ap_apidentity] == 1) && (argc < 6))
	{
		incorrectForm();
		return 2;
	}
	//CHECK last 3 arguments in Multiple cp case
	if((MCP)) 
	{
		if(CheckData(argv[(argc -1)]) && CheckData(argv[(argc -2)]) && CheckData(argv[(argc -3)]))
		{
			incorrectForm();
			return 2;
		}
	}
	//If user tries to change both the FileSize and MinFileSize values in 
	//the same rtrch command then twoFileSizeParameters is set to true, 
	//so that the checkValidity function can ignore comparing these new 
	//values w.r.to old values. The actual comparision is done here to
	//make sure that MinFileSize < FileSize.
	if((parIssued[h_fileSize] == 1 ) && (parIssued[p_minFileSize] == 1))
	{//options h and p (max and min size for a file are set)
		twoFileSizeParameters=true;
		// Converted the Filesize to bytes to check with minimum file size which is in terms of bytes for TR HH71761
		if(parameters.MinFileSize > (parameters.FileSize*1024))	
		{//MIN file size > Max file size,this isn't correct
			cout<<CmdPrintouts::UnReasonval<<endl;
			return 3;
		} 
	}
	if(!CheckData(argv[argc - 1]))
	{
		incorrectForm();
		return 2;
	}
	if(strlen(argv[argc-1]) > 15)
	{
		incorrectForm();
		return 2;
	}
	strcpy(MessageStoreName, strupr(argv[argc-1]));//copy Ms

	if ((strlen(MessageStoreName) > 15)
			|| (MessageStoreName[0]<(char)'A')
			|| (MessageStoreName[0]>(char)'z')
			|| ((MessageStoreName[0]>(char)'Z') && (MessageStoreName[0]<(char)'a')))
	{//error in command line
		incorrectForm();
		return 2;
	}
	//it isn't possible to set both file options and block options 
	if (blockParameter && fileParameter ) 
	{
		incorrectForm();
		return 2;
	}
	if (!parIssued[f_forcedFlag]) //check option f 
	{
		cout<<"Do you really want to change parameters?"<<endl;
		cout<<flush;
		if (!commitCommand())
		{
			cout<<"Command cancelled by operator"<<endl;
			return 0;
		}
	}
	//Connection to DSD
	ACS_DSD_Session ses;
	ACS_DSD_Node node;
	unsigned int APSys[16];
	ACE_INT32 ind = 0;
	ACE_INT32 nAp = 0;
	bool APfound = false;
	if(apID == 0) //LOCAL AP
		node.system_id = acs_dsd::SYSTEM_ID_THIS_NODE;
	else 
	{
		try {
			nAp = get_All_AP_sysId_in_Cluster(APSys);
		}
		catch(int code)
		{
			if(code == 56)
			{
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigError<<endl;
				return 56;
			}
		}
		if(nAp == 0) //no AP in the cluster
		{
			cout<<CmdPrintouts::APundef<<endl;
			return 113;
		}
		else
		{//search for that apid
			for(ind = 0; ind < N_of_AP; ind++)
			{
				if(APSys[ind] == apID)
				{
					APfound =  true;
					break;
				}
			}
		}
		if(APfound == false)//apid not found
		{//not found
			cout<<CmdPrintouts::APundef<<endl;
			return 113;
		}
		node.system_id = apID;
	}
	node.node_state =  acs_dsd::NODE_STATE_ACTIVE;
	ACS_DSD_Client* cl = new ACS_DSD_Client();
	int connected = cl->connect(ses,"ACS_RTR_Server","ACS", node.system_id, node.node_state,5000);
	if (connected >= 0)
	{//connect ok
		rtr_ch_req_t* request;
		request = new rtr_ch_req_t();
		memset(request, 0, sizeof(rtr_ch_req_t));
		request->cmdType = rtrChangeService;
		memcpy(request->opt,parIssued,128*sizeof(int));
		memcpy(&(request->parameters), (void*)&parameters, sizeof(RTRMS_Parameters));
		request->Blockpar = blockParameter;
		request->Filepar = fileParameter;
		strcpy(request->msname,MessageStoreName);
		unsigned int reqSize = sizeof(rtr_ch_req_t);//set size
		ssize_t sent = ses.send(reinterpret_cast<void*>(request),reqSize);	
		if(sent > 0)
		{//sending ok
			ACE_TCHAR response[RESPBUFSIZE] = {0};
			unsigned int respSize = RESPBUFSIZE; // ACS_DSD_MAXBUFFER;
			ssize_t received = ses.recv(response,respSize);//receiving response
			if ((received > 0)&& (respSize != 0))
			{//receiving ok
				const int msglength = sizeof(response);
				char resultCode[msglength+1] = {0};
				memcpy(resultCode, response , msglength);
				if(strcmp(resultCode, "EXECUTED"))
				{
					std::cout<< resultCode <<std::endl;
				}
				if(request)
					delete (request);
				if(cl)
					delete cl;
				return 0;
			}
			else
			{//receiving failed
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
				if(request)
					delete (request);
				if(cl)
					delete cl;
				return 12;
			}
		}
		else
		{//Sending failed
			cout << CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError << endl;
			if(request)
				delete (request);
			if(cl)
				delete cl;
			return 12;
		}
	}
	else
	{//connection failed
		cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoServerAccess<< endl;
		if(cl)
			delete cl;
		return 117;
	}
}

