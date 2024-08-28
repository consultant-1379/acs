//******************************************************************************
//
//  NAME
//   rtrfe.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 2006. All rights reserved.
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
//  This file implements the 'forced file end' command for RTR which
//  forcefully ends the RTR file before the file holding timer expires
//  or the RTR file attains the specified File size

// 	 
//  DOCUMENT NO
//	   190 89-CAA 109 0499
//
//  AUTHOR 
//     2006-05-22 EAB/UZ/TI TCSVEPA First Version 
//	CHANGE HISTORY
//  2007-02-01		XCSVAAN			For TR HH50071
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
using namespace std;
#define RESPBUFSIZE             128
#define N_of_AP        16

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
//************************************************************************
//                               Usage
//************************************************************************
void Usage()
{
                cout<<"Incorrect usage"<<endl;
                cout<<"Usage:"<<endl;
		cout<<"rtrfe [-ap apidentity] messagestore"<<endl;
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
				throw 53;
			}
			ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
		}//boardsearch
		else
		{
			ACS_CS_API::deleteHWCInstance(hwc);
			throw 53;
		}
		ACS_CS_API::deleteHWCInstance(hwc);
	}//hwc
	else
	{
		throw 53;
	}
	return noOfAP;
}
//****************************************************************************
// M A I N    r t r f e
//****************************************************************************
ACE_INT32 main(ACE_INT32 argc, ACE_TCHAR** argv)
{
	ACE_TCHAR MSname[16] = {0};
	ACE_TCHAR Ap[5] = {0};
	unsigned int apid = 0;
	bool MCP = false;

	//check the cpsystem
	try {
		MCP = isMultipleCpSystem();
	}
	catch(int cod)
	{
		if(cod == 55)
		{
			cout <<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigConn<< endl;
			return 55;
		}
		if(cod == 56)
		{
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::ConfigError<< endl;
			return 56;
		}
	}

	//Cmd Line options
	//rtrfe messagestore        (single CP)          #2
	//rtrfe messagestore        (multiple CP)        #2
	//rtrfe -ap AP messagestore (multiple CP)        #4

    //cmd line check
	if ((argc != 2) && (argc != 4))
	{
		Usage();
		return 2;	
	}
	else 
	{
		if(argc == 2)
		{//rtrfe messagestore ( single & multiple ) 
			if(!CheckData(argv[1]))
			{
				Usage();
				return 2;
			}
			if(strlen(argv[1]) > 15)
			{
				Usage();
				return 2;
			}
			strcpy(MSname,strupr(argv[1]));
		}
		else if((argc == 4) && (!strcmp(argv[1],"-ap")))
		{//rtrfe -ap AP messagestore
				if(!CheckData(argv[3]))
				{
					Usage();
					return 2;
				}
				if(strlen(argv[3]) > 15)
				{
					Usage();
					return 2;
				}
				try {
					CheckApName(argv[2],apid);
				}
				catch(int ret)
				{
					if(ret == 113)
					{
						cout<<CmdPrintouts::APundef<<endl;
						return 113;
					}
				}
				strcpy(Ap,argv[2]);
				strcpy(MSname,strupr(argv[3]));
		}
		else
		{
			Usage();
			return 2;
		}
	}
	//check msname, mslen should be ok
	if (strlen(MSname)>15 
		|| MSname[0]<(char)'A'
		|| MSname[0]>(char)'z'
		|| ((MSname[0]>(char)'Z') && (MSname[0]<(char)'a')))
	{//error in command line
		Usage();
		return 2;
	}

	////////////CONNECTION TO DSD////////////
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
	int connected = cl->connect(ses,"ACS_RTR_Server","ACS",node.system_id,node.node_state,5000);
	if (connected >= 0)
	{//connect ok
		rtr_fe_req_t* request = 0;
		request = new rtr_fe_req_t();
		memset(request, 0, sizeof(rtr_fe_req_t));
		request->cmdType = rtrForcedFileEnd; //set RTRFE command
		strcpy(request->msname, MSname); //copy Msname
		unsigned int reqSize = sizeof(rtr_fe_req_t);//set size
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
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError << endl;
				if(request)
					delete (request);
				if(cl)
					delete cl;
				return 12;//TO VERIFY
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
