//******************************************************************************
//
//  NAME
//   rtrdef.cpp
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
//  This file implements the 'define' command for RTR which
//  allows a user to start-up file/block reporting to GOH.
// 	 
//  DOCUMENT NO
//	   190 89-CAA 109 0499
//
//  AUTHOR 
//     2005-08-02 by EAB/UZ/DG UABCAJN
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
// 2007-05-2	GP/AP/A QALEDIR	Blade Cluster adaptations
// 2013-02-27 XLANSRI
//************************************************************************************
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <ace/ACE.h>

#include <iostream>
#include <acs_rtr_lib.h>
#include <acs_rtr_global.h>
#include <acs_rtr_cmd_util.h>
#include "aes_ohi_extfilehandler2.h"
#include "aes_ohi_blockhandler2.h"

#include "ACS_CS_API.h" 
using namespace std;
#define RESPBUFSIZE             128

//**************************************************************************************
//                                   printUsage
//**************************************************************************************
void printUsage()
{
                cout<<"Incorrect usage"<<endl;
		cout<<"Usage:"<<endl;
		cout<<"rtrdef [-ap apidentity] -a file_dest messagestore"<<endl;
		cout<<"rtrdef [-ap apidentity] -b block_dest messagestore"<<endl;
}
//*****************************************************************************************
//                                    decodePar
//*****************************************************************************************
ACE_INT32 decodePar(ACE_TCHAR* supp)
{
	if (!ACE_OS::strcmp(strlwr(supp),"no")) return 0;
	else if (!ACE_OS::strcmp(strlwr(supp),"yes")) return 2;
	else return -1;
}
//*****************************************************************************************
//                                isMultipleCpSystem
//*****************************************************************************************
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
void CheckApName(ACE_TCHAR* apname,unsigned int& apid)
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
		for (int i = 0; i < std::strlen (str.c_str()); i++) {
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
    //create hwc instance
	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();
	if (hwc)
	{
		//create boardsearch instance
		ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();  
		if (boardSearch)
		{
			ACS_CS_API_IdList boardList;
			boardSearch->setSysType(ACS_CS_API_HWC_NS::SysType_AP);
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
            }//getBoardIds
			else
			{
				ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
				ACS_CS_API::deleteHWCInstance(hwc);
				throw 56;
			}
			ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
		}//boardsearch
		else
		{
			ACS_CS_API::deleteHWCInstance(hwc);
			throw 56;
		}
		ACS_CS_API::deleteHWCInstance(hwc);
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
			ind = (int)str.find_first_of("-",0);
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
// M A I N    r t r d e f
//*************************************************************************
ACE_INT32 main(ACE_INT32 argc,ACE_TCHAR** argv)
{
	bool fileOutput;
	ACE_TCHAR TransQueue[33] = {0};
	ACE_TCHAR MessageStoreName[16] = {0};
	bool MCP = false;
	bool Sopt = false;
	ACE_INT32 addInd = 0;
	ACE_TCHAR apName[5] = {0};
	unsigned apID = 0;
	
	//MULTIPLE CPSYSTEM CHECK
	try {
		MCP = isMultipleCpSystem();
	}
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
	//COMMAND LINE CHECK
	// rtrdef -a  TQ messagestore (file Transfer) MultipleCpSystem 
	// rtrdef -b  TQ messagestore (block transfer) MultipleCpSystem
	// rtrdef -ap apidentity -a TQ messagestore (file Transfer) MultipleCpSystem
	// rtrdef -ap apidentity -b TQ messagestore (block transfer)MultipleCpSystem
	// rtrdef -a TQ messagestore site (file transfer) SingleCpSystem
	// rtrdef -b TQ messagestore site (block transfer) SingleCpSystem

	
	//Check commandline
		if(argc != 4 && argc != 6)
                {//check options number on the command line
                        printUsage();
                        return 2;
                }


        //rtrdef -b TQ MS
	if(!ACE_OS::strcmp(argv[1],"-b") && argc == 4)
	{
		if((!CheckData(argv[2])) || (!CheckData(argv[3])))
		{
			printUsage();
			return 2;
		}
		if(ACE_OS::strlen(argv[2]) > 32 || ACE_OS::strlen(argv[3]) > 15)
		{
			printUsage();
			return 2;
		}
		fileOutput = false; //block tr.
	}
	//rtrdef -a TQ MS
	else if(!ACE_OS::strcmp(argv[1],"-a") && argc == 4)
	{
		if(!CheckData(argv[2]) || !CheckData(argv[3]))
		{
			printUsage();
			return 2;
		}
		if(ACE_OS::strlen(argv[2]) > 32 || ACE_OS::strlen(argv[3]) > 15)
		{
			printUsage();
			return 2;
		}
		fileOutput = true; //File tr
	}
	else if(argc == 6 && !ACE_OS::strcmp(argv[1],"-ap") && !ACE_OS::strcmp(argv[3],"-a"))
	{//rtrdef -ap AP -a TQ MS
			if(!CheckData(argv[4]) || !CheckData(argv[5]))
			{
				printUsage();
				return 2;
			}
			if(strlen(argv[4]) > 32 || strlen(argv[5]) > 15)
			{
				printUsage();
				return 2;
			}
			try {
				CheckApName(argv[2],apID);
			}
			catch(int ret)
			{
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			fileOutput = true;
			addInd = 2;
			strcpy(apName,argv[2]);
	}
	else if(argc == 6 && !ACE_OS::strcmp(argv[1],"-ap") && !ACE_OS::strcmp(argv[3],"-b"))
	{//rtrdef -ap AP -b TQ Ms
			if(!CheckData(argv[4]) || !CheckData(argv[5]))
			{
				printUsage();
				return 2;
			}
			if(strlen(argv[4]) > 32 || strlen(argv[5]) > 15)
			{
				printUsage();
				return 2;
			}
			try {
				CheckApName(argv[2],apID);
			}
			catch(int ret)
			{
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			fileOutput = false;
			addInd = 2;
			strcpy(apName,argv[2]);
	}

	else //error in command line 
	{
		printUsage();
		return 2;
	}
	//SET TQ,MS
	ACE_OS::strcpy(TransQueue,argv[2+addInd]);//set TQ
	ACE_OS::strcpy(MessageStoreName,strupr(argv[3+addInd]));//set MS

    
	//CHECK MS Len and MS 
	if (ACE_OS::strlen(MessageStoreName)>15 //|| ACE_OS::strlen(Site)>15
		|| MessageStoreName[0]<(char)'A'
		|| MessageStoreName[0]>(char)'z'
		|| ((MessageStoreName[0]>(char)'Z') && (MessageStoreName[0]<(char)'a'))
		)
	{//error in command line
		printUsage();
		return 2;
	}

	//TR HJ81456. Only check if transfer queue is defined if on LOCAL AP
	if (apID == 0 && fileOutput) //transfer file
	{
		//CHECK if file transfer queue is defined (also in SERVER)
		ACE_INT32	errcode;
		AES_OHI_ExtFileHandler2* mySenderFile;
		mySenderFile = new AES_OHI_ExtFileHandler2("ACS","ACS_RTR_service");
		switch (errcode = mySenderFile->attach()) //attach
		{//check attach operation
		case AES_OHI_NOPROCORDER:
			cout<<CmdPrintouts::NoTQDef<<endl;
			delete mySenderFile;
			return 4;
		case AES_OHI_NOSERVERACCESS:
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoDatatransferAccess<<endl;
			delete mySenderFile;
			return 5;
		case AES_OHI_NOERRORCODE:
			break;
		default:
			cout<<"Internal error code "<<errcode<<endl;
			delete mySenderFile;
			return 6;
		}
		//check that transfer queue is defines
		switch (mySenderFile->fileTransferQueueDefined(TransQueue))
		{
		case AES_OHI_NOERRORCODE:
			break;
		case AES_OHI_NOSERVERACCESS:
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoDatatransferAccess<<endl;
			delete mySenderFile;
			return 5;
		default:
			cout<<CmdPrintouts::NoTQDef<<endl;
			delete mySenderFile;
			return 4;
		}
		mySenderFile->detach(); //detach
		delete mySenderFile;
	}
	else if(apID == 0) // block transfer outpu
	{
		//  Check if block transfer queue is defined
		switch (AES_OHI_BlockHandler2::blockTransferQueueDefined(TransQueue))
		{
		case AES_OHI_NOERRORCODE:
			break;
		case AES_OHI_NOSERVERACCESS:
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoDatatransferAccess<<endl;
			return 10;
		default:
			cout<<CmdPrintouts::NoTQDef<<endl;
			return 4;
		}
	}
 	//CONNECT to ACS_RTR_Server
	ACS_DSD_Session ses;
	ACS_DSD_Node node;
	unsigned int APSys[16];
	ACE_INT32 ind = 0;
        ACE_INT32 nAp = 0;
	bool APfound = false;
	//check apid
	if(apID == 0)
	{
		node.system_id = acs_dsd::SYSTEM_ID_THIS_NODE; //LOCAL AP
	}
	else
	{
		//CHECK if AP is in the Cluster before of connection
		try {
			nAp = get_All_AP_sysId_in_Cluster(APSys);}
		catch(ACE_INT32 code)
		{
			if(code == 56)
			{
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigError<<endl;
				return 56;
			}
		}
		if(nAp == 0)
		{
			cout<<CmdPrintouts::APundef<<endl;
			return 113;
		}
		else
		{
			for(ind = 0; ind < 16; ind++)
			{
				if(APSys[ind] == apID)
				{
					APfound = true;
					break;
				}
			}
		}
		if(APfound == false)
		{
			cout<<CmdPrintouts::APundef<<endl;
			return 113;
		}
		node.system_id = apID; //SPECIFIC AP
	}
	node.node_state = acs_dsd::NODE_STATE_ACTIVE;
	ACS_DSD_Client* cl = new ACS_DSD_Client();
	int ret = cl->connect(ses,"ACS_RTR_Server","ACS",node.system_id,node.node_state,5000);
	if (ret >= 0)
	{//connect ok
		rtr_def_req_t* request;
		request = new rtr_def_req_t();
		ACE_OS::memset(request, 0, sizeof(rtr_def_req_t));
		request->cmdType = rtrStartService; //set RTRDEF command
		ACE_OS::strcpy(request->msname, MessageStoreName); //copy Msname
		ACE_OS::strcpy(request->TransferQueue,TransQueue);//copy tq
                if(fileOutput)
			request->OutputType = FileOutput;
		else
			request->OutputType = BlockOutput;
		request->S_opt = Sopt; //set S option
		unsigned int reqSize = sizeof(rtr_def_req_t);//set size
		ssize_t sent = ses.send(reinterpret_cast<void*>(request),reqSize);
		if(sent > 0)
		{//sending ok
			char response[RESPBUFSIZE] = {0};
			unsigned int respSize = RESPBUFSIZE;
			ssize_t received = ses.recv(response,respSize);//receiving response
			if ((received > 0) && (respSize != 0))
			{//receiving ok
				const int msglength = sizeof(response);
				char resultCode[msglength+1] = {0};
				memcpy(resultCode, response , msglength);
				if(strcmp(resultCode, "EXECUTED"))
				{
					std::cout<< resultCode <<std::endl;
				}
				if(request)
					delete request;
				if(cl)
					delete cl;
				return 0;	
			}
			else
			{//receiving failed
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
				if(request)
					delete request;
				if(cl)
					delete cl;
				return 12;  
			}
		}//sending
		else
		{//sending failed
			cout <<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
			if(request)
				delete (request);
			if(cl)
				delete cl;
			return 12; 
		}
	}//connect
	else
	{//connect failed
		cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoServerAccess<< endl;
		if(cl)
			delete cl;
		return 117;
	}
}

