//******************************************************************************
//
//  NAME
//   rtrrm.cpp
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
//  This file implements the 'remove' command for RTR which
//  removes the configuration settings for the specified
//  Message store.

// 	 
//  DOCUMENT NO
//	   190 89-CAA 109 0573
//
//  AUTHOR 
//     2005-10-14 by EAB/UZ/DG UABCAJN
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
// 2007-05-2	GP/AP/A QALEDIR	Blade Cluster adaptations
//******************************************************************************

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
//#include <ACS_PRC_Process.H>
//#include <ACS_ExceptionHandler.H>
using namespace std;
#define RESPBUFSIZE             128
#define N_of_AP           16
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
int get_All_AP_sysId_in_Cluster(unsigned int apSysIds[16])
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
				for (unsigned int i = 0; i < boardList.size(); i++)   //step through all the boards in the hwc table
				{
					BoardID boardId = boardList[i];
					unsigned short sysId = 0;
                  
					result = hwc->getSysId(sysId, boardId);    //get the system id of current board
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
            }//board ids
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
bool CheckData(const char* data)
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
//**************************************************************************
//                          PrintUsage
//**************************************************************************
void PrintUsage()
{
                cout<<"Incorrect usage"<<endl;
		cout<<"Usage:"<<endl;
		cout<<"  rtrrm [-ap apidentity] [-f] messagestore"<<endl;
}

//*************************************************************************
// M A I N    r t r r m
//*************************************************************************
ACE_INT32 main(ACE_INT32 argc,ACE_TCHAR** argv)
{
	bool fflag = false;
	ACE_TCHAR MSname[16] = {0};
	ACE_TCHAR APIdentity[6] = {0};
	bool MCP = false;
	unsigned int apid = 0;
	//CHECK MULTIPLE CPSYSTEM
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

	//LINE COMMAND CHECK                          #Arguments on command line
	//FORMAT FOR SINGLE CP SYSTEM
	//rtrrm Messagestore                          2
	//rtrrm -f Messagestore                       3
	//FORMAT FOR MULTIPLE CP SYSTEM
	//rtrrm Messagestore                          2
	//rtrrm -f Messagestore                       3
	//rtrrm -ap apidentity Messagestore           4
	//rtrrm -f -ap apidentity Messagestore        5


	if(argc!=2 && argc!=3 && argc!=4 && argc!=5)
	{//check options number on the command line
		PrintUsage();
		return 2;
	}

	if(argc == 2)  // NNN
	{//rtrrm MS
		if(!CheckData(argv[1]))
		{
			PrintUsage();
			return 2;
		}
		if(strlen(argv[1]) > 15)
		{
			PrintUsage();
			return 2;
		}
		strcpy(MSname,strupr(argv[1]));
	}//argc = 2 
	else if (argc == 3) // rtrrm -f messagestore // NNN
	{
		if(!strcmp(argv[1],"-f"))
		{//rtrrm -f messagestore
			if(!CheckData(argv[2]))
			{
				PrintUsage();
				return 2;
			}
			if(strlen(argv[2]) > 15)
			{
				PrintUsage();
				return 2;
			}
			fflag = true;
			strcpy(MSname, strupr(argv[2])); //get MSname
		}
	}//argc = 3 


	else if(argc == 4)   // NNN
	{
		if(!strcmp(argv[1],"-ap")) //rtrrm -ap AP messagestore
		{
			try { CheckApName(argv[2],apid); }
			catch(int ret) {
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			if(!CheckData(argv[3]))
			{
				PrintUsage();
				return 2;
			}
			if(strlen(argv[3]) > 15)
			{
				PrintUsage();
				return 2;
			}
			strcpy(APIdentity,argv[2]);
			strcpy(MSname,strupr(argv[3]));
		}
		else
		{//error in command line
			PrintUsage();
			return 2;
		}
	}//argc = 4  


	else if(argc == 5) //rtrrm -f -ap apidentity Messagestore   
	{
		if(!strcmp(argv[1],"-f") && !strcmp(argv[2],"-ap"))
		{

			try { CheckApName(argv[3],apid); }
			catch(int ret) { 
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			if(!CheckData(argv[4]))
			{
				PrintUsage();
				return 2;
			}
			if(strlen(argv[4]) > 15)
			{
				PrintUsage();
				return 2;
			}
			strcpy(APIdentity,argv[3]);
			strcpy(MSname,strupr(argv[4]));
			fflag = true;
		}
		else if(!strcmp(argv[3],"-f") && !strcmp(argv[1],"-ap"))
		{//rtrrm -ap AP -f MS
			try { CheckApName(argv[2],apid); }
			catch(int ret) { 
				if(ret == 113)
				{
					cout<<CmdPrintouts::APundef<<endl;
					return 113;
				}
			}
			if(!CheckData(argv[4]))
			{
				PrintUsage();
				return 2;
			}
			if(strlen(argv[4]) > 15)
			{
				PrintUsage();
				return 2;
			}
			strcpy(APIdentity,argv[3]);
			strcpy(MSname,strupr(argv[4]));
			fflag = true;
		}
		else
		{//error in command line
			PrintUsage();
			return 2;
		}
	}
	 else
	{
		PrintUsage();
		return 2;
	}
	//CHECK MS Len and MS first char
	if (ACE_OS::strlen(MSname)>15  
			||MSname[0]<(char)'A'
			||MSname[0]>(char)'z'
			|| ((MSname[0]>(char)'Z') && (MSname[0]<(char)'a')))
	{//error in command line
		cout<<"Usage:"<<endl;
		cout<<"  rtrrm [-f] messagestore "<<endl;
		return 2;
	}
	//ASK for confirmation
	if (!fflag)
	{
		cout<<"Do you really want to remove data record transfer definition?"<<endl;
		cout<<flush;
		//check YES or NOT
		if (!commitCommand()) 
		{
			//delete confirmed user doesn't want to remove
			cout<<"Command cancelled by operator"<<endl;
			return 0;
		}
	}
	//CONNECT to ACS_RTR_Server
	ACS_DSD_Session ses;
	ACS_DSD_Node node;
	unsigned int APSys[16];
	ACE_INT32 ind = 0;
	ACE_INT32 nAp = 0;
	bool APfound = false;
	if(apid == 0) //LOCAL AP
	{
		node.system_id = acs_dsd::SYSTEM_ID_THIS_NODE; //LOCAL AP
	}
	else 
	{
		try {
			nAp = get_All_AP_sysId_in_Cluster(APSys);}
		catch(ACE_INT32 code)
		{
			if(code == 56)
			{
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigError<<endl;;
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
				if(APSys[ind] == apid)
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
		node.system_id = apid;
	}
	node.node_state = acs_dsd::NODE_STATE_ACTIVE;
	ACS_DSD_Client* cl = new ACS_DSD_Client();
	int ret = cl->connect(ses,"ACS_RTR_Server","ACS",node.system_id,node.node_state,5000);
	if (ret >= 0)
	{//connect ok
		rtr_rm_req_t* request = 0;
		request = new rtr_rm_req_t();
		ACE_OS::memset(request, 0, sizeof(rtr_rm_req_t));
		request->cmdType = rtrStopService; //set RTRRM command
		ACE_OS::strcpy(request->msname, MSname); //copy Msname
		ACE_OS::strcpy(request->apIdentity,APIdentity);//copy AP
		request->f_opt = fflag;
		unsigned int reqSize = sizeof(rtr_rm_req_t);//set size
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
			cout <<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
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
