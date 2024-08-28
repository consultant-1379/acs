//******************************************************************************
//
//  NAME
//   rtrls.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 2013. All rights reserved.
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
//  This file implements the 'list' command for RTR which outputs
//  configuration settings.

// 	 
//  DOCUMENT NO
//	   190 89-CAA 109 0573
//
//  AUTHOR 
//     2013-02-26 by XLANSRI
//
// CHANGES
// RELEASE REVISION HISTORY
// DATE		NAME			DESCRIPTION
// 2007-05-2	GP/AP/A QALEDIR	Blade Cluster adaptations
//******************************************************************************
//#include "ACS_DSD_DSA2.h"  //check this file not available in cm4ap dir
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <acs_rtr_cmd_util.h>
#include <iostream>
#include <acs_rtr_lib.h>
#include <acs_rtr_global.h>
#include "ACS_CS_API.h" 
#include <ace/ACE.h>
#include<string>
using namespace std;
#define RESPBUFSIZE             128
#define No_of_AP          16

//*************************************************************************
// incorrectForm
//
// Prints usage information for rtrch command.

// return values:
//   -
//*************************************************************************
void incorrectForm()
{
                cout<<"Incorrect usage"<<endl;
		cout<<"Usage:"<<endl;
		cout<<"rtrls"<<endl;
		cout<<"rtrls [-ap apidentity][-s] messagestore" << endl;
                cout<<"rtrls [-ap apidentity] -d" << endl;
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
		}//board search instance
		else
		{
			ACS_CS_API::deleteHWCInstance(hwc);
			throw 56;
		}
		ACS_CS_API::deleteHWCInstance(hwc);
	}//hwc instance
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
// M A I N    r t r l s
//List specified Message Store definition or all existing ones
//*************************************************************************
ACE_INT32 main(ACE_INT32 argc, ACE_TCHAR** argv)
{
	bool		    statRequested = false;
	ACE_TCHAR	    MSname[16] = {0};
	ACE_TCHAR	    Sitename[16] = {0};
	ACE_TCHAR           APIdentity[5] = {0};
	bool                MCP = false;
	bool                DefOpt = false;
	unsigned int        apid = 0;
	bool                data = true;
	bool                ALLAP = false;
	bool                NOMSOPTION = false;
	//CPSystem CHECK
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
	//FORMAT FOR SINGLE CP SYSTEM          #Arguments on command line
	//rtrls                                1
	//rtrls -s                             2 
	//rtrls -s MS                          3	
	//rtrls MS                             2	
	//rtrls -d                             2
	//FORMAT FOR MULTIPLE CP SYSTEM
	//rtrls                                1
	//rtrls MS                             2
	//rtrls -s MS                          3
	//rtrls -ap AP MS                      4
	//rtrls -ap AP -s MS                   5
	//rtrls -d                             2
	//rtrls -ap AP -d                      4

	//cmd line options flags
	bool ap_opt = false;
	bool s_opt = false;
	bool d_opt = false;
	ACE_INT32 ap_ind = 0;
	ACE_INT32 s_ind = 0;
	ACE_INT32 d_ind = 0;
	ACE_INT32 argInd = 1;
	//Look for options on cmd line(only several options)
	while(argInd < argc)
	{
		if(!ACE_OS::strcmp(argv[argInd],"-s"))
		{
			s_opt = true;
			s_ind = argInd;
		}
		else if(!ACE_OS::strcmp(argv[argInd],"-ap"))
		{
			ap_opt = true;
			ap_ind = argInd;
		}
		else if(!ACE_OS::strcmp(argv[argInd],"-d"))
		{
			d_opt = true;
			d_ind = argInd;
		}
		argInd++;
	}
	//check cmd line
	switch (argc)
	{
		case 1:
			// rtrls
		//	if(MCP)
				ALLAP = true;
			NOMSOPTION = true;
			break;
		case 2:
			// rtrls -d,rtrls MS
			if(!ACE_OS::strcmp(argv[1], "-d"))
			{//rtrls -d (single and multiple)
				DefOpt = true;
				NOMSOPTION = true;
			}
			else
			{//rtrls MS ( multiple & Single CP)
				if(!CheckData(argv[1])) //check msname
				{
					incorrectForm();
					return 2;
				}
				if(ACE_OS::strlen(argv[1]) > 15) //check mslen
				{
					incorrectForm();
					return 2;
				}
				ACE_OS::strcpy(MSname,strupr(argv[1]));
				ACE_OS::strcpy(Sitename,"cp0ex"); //set sitename
			}
			break;
		case 3:
			//rtrls -s MS
			if(!ACE_OS::strcmp(argv[1],"-s"))
			{//rtrls -s msname (multiple & Single)
				if(!CheckData(argv[2])) //check msname
				{
					incorrectForm();
					return 2;
				}
				if(ACE_OS::strlen(argv[2]) > 15) //check mslen
				{
					incorrectForm();
					return 2;
				}
				ACE_OS::strcpy(MSname,strupr(argv[2]));
				statRequested = true;
			}
			break;
		case 4:
			//rtrls -ap AP MS   
			//rtrls -ap AP -d
			if((ap_opt == true) && (d_opt == true))
			{//rtrls -ap AP -d (only multiple) // Make it compatible in SCP as well

				//check errors
				if((ap_ind == (argc -1)))
				{
					incorrectForm();
					return 2;
				}
				if(!CheckData(argv[ap_ind + 1]))
				{
					incorrectForm();
					return 2;
				}
				//no errors	get and check ap		
				try {
					CheckApName(argv[ap_ind + 1],apid);
				}
				catch(ACE_INT32 ret)
				{
					if(ret == 113)
					{
						cout<<CmdPrintouts::APundef<<endl;
						return 113;
					}
				}
				ACE_OS::strcpy(APIdentity,argv[ap_ind + 1]); //APname ok
				DefOpt = true;
				NOMSOPTION = true;
			}
			else if(!ACE_OS::strcmp(argv[1],"-ap") && ACE_OS::strcmp(argv[3],"-d"))
			{//rtrls -ap AP MS (only multiple) // Make it compatible to SCP as well
				try {
					CheckApName(argv[2],apid);//check apname
				}
				catch(ACE_INT32 ret)
				{
					if(ret == 113)
					{
						cout<<CmdPrintouts::APundef<<endl;
						return 113;
					}
				}
				if(!CheckData(argv[3])) //check msname
				{
					incorrectForm();
					return 2;
				}
				if(ACE_OS::strlen(argv[3]) > 15) //check mslen
				{
					incorrectForm();
					return 2;
				}
				ACE_OS::strcpy(APIdentity,argv[2]);
				ACE_OS::strcpy(MSname,strupr(argv[3]));
			}

			else
			{
				incorrectForm();
				return 2;
			}
			break;
		case 5:
			//rtrls -ap AP -s MS (only multiple) //Make it compatible to SCP as well
			if((ap_opt == true) && (s_opt == true))
			{
				//check errors
				if((ap_ind == (argc - 1)) || (s_ind == (argc - 1)))
				{				
					incorrectForm();
					return 2;
				}
				if(!CheckData(argv[ap_ind + 1]) || !CheckData(argv[4]))
				{
					incorrectForm();
					return 2;
				}
				try {
					CheckApName(argv[ap_ind + 1],apid); //check Ap
				}
				catch(ACE_INT32 ret)
				{
					if(ret == 113)
					{
						cout<<CmdPrintouts::APundef<<endl;
						return 113;
					}
				}
				if(ACE_OS::strlen(argv[4]) > 15)//check mslen
				{
					incorrectForm();
					return 2;
				}
				ACE_OS::strcpy(APIdentity,argv[ap_ind + 1]);
				ACE_OS::strcpy(MSname,strupr(argv[4]));
				statRequested = true;
			}
			else
			{
				incorrectForm();
				return 2;
			}
			break;
		default:
			incorrectForm();
			return 2;
	}
	//check Msname  (mslen checked)
	if(!NOMSOPTION) 
	{
		if ((ACE_OS::strlen(MSname) > 15)
				|| (MSname[0]<(ACE_TCHAR)'A')
				|| (MSname[0]>(ACE_TCHAR)'z')
				|| ((MSname[0]>(ACE_TCHAR)'Z') && (MSname[0]<(ACE_TCHAR)'a')))
		{//error in command line
			incorrectForm();
			return 2;
		}
	}
	//command line ok it'time to connect to dsd
	unsigned int APSys[16] = {0};
	ACE_INT32 ind = 0;
	ACE_INT32 nAp = 0;
	bool END_LIST = false;
	bool not_found = true;
	bool header = false;
	bool multiAp =false;
	///////////////////////ALLAP//////////////////////////////////////////////
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
                if(nAp == 0) //no AP in the cluster
                {
                        cout<<CmdPrintouts::APundef<<endl;
                        return 113;
                }
		multiAp=(nAp > 1);

	if(ALLAP || (multiAp && argc==1) )
	{//ALLAP case
		{
			for(ACE_INT32 ind = 0; ind < No_of_AP; ind++)
			{
				unsigned int SysAP = APSys[ind];
				if (SysAP == 0)	break;//no others AP in array
				ACS_DSD_Session ses;
				ACS_DSD_Node nd;
				nd.system_id = SysAP; //set systemId
				nd.node_state = acs_dsd::NODE_STATE_ACTIVE; //set state
				//make AP name to print
				ACE_TCHAR APstr1[3] = "AP";
				ACE_TCHAR APstr2[3] = {0};
				ACE_OS::itoa((SysAP - 2000),APstr2,10); 
				ACE_OS::strcat(APstr1,APstr2);
				ACS_DSD_Client* cl = new ACS_DSD_Client();
				//connected to DSD
				int ret = cl->connect(ses,"ACS_RTR_Server","ACS",nd.system_id,nd.node_state,5000);
				if(ret >= 0)
				{
					not_found = false;
					END_LIST = false;
					rtr_ls_req_t* request = 0;
					request = new rtr_ls_req_t();
					ACE_OS::memset(request, 0, sizeof(rtr_ls_req_t));
					//building rquest
					request->cmdType = rtrListStatistics;
					request->mAPOpt = multiAp;
					unsigned int reqSize = sizeof(rtr_ls_req_t);//set size
					ssize_t sent = ses.send(reinterpret_cast<void*>(request),reqSize);
					if(sent > 0)
					{
						while(!END_LIST)
						{
							ACE_TCHAR response[RESPBUFSIZE] = {0};
							unsigned int respSize = RESPBUFSIZE;
							ssize_t received = ses.recv(response,respSize);//receiving response
							if ((received > 0) && (respSize != 0))
							{//receiving ok
								respCode code = (respCode)response[0];
								if(code == END_MESSAGE)
								{
									END_LIST = true;
									continue;
								}
								else if(code == NODATA)
								{
									if(!header)
									{
										cout<<"RTR TABLE"<<endl<<endl;
										cout<<"NODE MESSAGE STORE    TRANSFER QUEUE"<<endl;
										header = true;
									}
									END_LIST = true;
									continue;
								}
								else if(code == READ_DATA)
								{
									if(!header) //print header
									{
										//HK54866
										cout<<"RTR TABLE"<<endl<<endl;
										cout<<"NODE MESSAGE STORE    TRANSFER QUEUE"<<endl;
										header = true;
									}
									ACE_TCHAR bufRead[(RESPBUFSIZE - sizeof(respCode))] ={0};
									ACE_INT32 len = (RESPBUFSIZE - sizeof(respCode));
									ACE_OS::memcpy(bufRead,response + sizeof(respCode),len);//copy message
									ACE_TCHAR bufWrite[(RESPBUFSIZE - sizeof(respCode))] ={0};
									ACE_OS::sprintf(bufWrite,"%-5s",APstr1);
									ACE_OS::strcat(bufWrite,bufRead);
									cout<<bufWrite;
								}
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
						}//end while
					}
					else
					{//sending failed
						cout <<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
						if(request)
							delete (request);
						if(cl)
							delete cl;
						return 12; 
					}
					if(request)
						delete request;
				}
				if(cl)
					delete cl;
			}//FOR ALL AP
			if(not_found)
			{//connect failed towards all AP
				cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoServerAccess<< endl;
				return 117;
			}
		}//nAP != 0
	}//ALLAP
	////////////////////////////OTHERS CASES//////////////////////////////////
	else
	{
		ACS_DSD_Session ses;
		ACS_DSD_Node node;		
		bool APfound = false;
		//check apid
		if(apid == 0)
		{
			//set systemid for connection
			node.system_id = acs_dsd::SYSTEM_ID_THIS_NODE;
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
			if(nAp == 0) //no AP in the cluster
			{
				cout<<CmdPrintouts::APundef<<endl;
				return 113;
			}
			else
			{//look for that apid
				for(ind = 0; ind < 16; ind++)
				{
					if(APSys[ind] == apid)
					{
						APfound = true;
						break;
					}
				}
			}
			if(APfound == false)
			{//not found
				cout<<CmdPrintouts::APundef<<endl;
				return 113;
			}
			node.system_id = apid; //SPECIFIC AP
		}
		node.node_state =  acs_dsd::NODE_STATE_ACTIVE;
		ACS_DSD_Client* cl = new ACS_DSD_Client();
		int ret  = cl->connect(ses,"ACS_RTR_Server","ACS",node.system_id,node.node_state,5000);
		if(ret >= 0)//connection ok
		{
			rtr_ls_req_t* request = 0;
			request = new rtr_ls_req_t;
			ACE_OS::memset(request, 0, sizeof(rtr_ls_req_t));
			//building rquest
			request->cmdType = rtrListStatistics;
			request->DefOpt = DefOpt;
			request->StatOpt = statRequested;
			request->mAPOpt= multiAp;
			ACE_OS::strcpy(request->apIdentity,APIdentity);
			ACE_OS::strcpy(request->msname,MSname);
			//sending message
			unsigned int reqSize = sizeof(rtr_ls_req_t);//set size
			ssize_t sent = ses.send(reinterpret_cast<void*>(request),reqSize);
			if(sent > 0)
			{//sending ok
				while(data)
				{
					const int RESPBUFSIZE_= 65000;
					ACE_TCHAR response[RESPBUFSIZE_] = {0};
					unsigned int respSize = RESPBUFSIZE_;
					ssize_t received = ses.recv(response,respSize);//receiving response
					if ((received > 0) && (respSize != 0))
					{//receiving ok
						respCode code;
						memcpy(&code,response,sizeof(respCode));//copy first 4 bytes
						if(code == END_MESSAGE)
						{

							data = false;
							continue;
						}
						else if(code == READ_DATA)
						{
							ACE_TCHAR bufRead[(RESPBUFSIZE_ - sizeof(respCode))] = {0};
							ACE_INT32 len = (RESPBUFSIZE_ - sizeof(respCode));
							ACE_OS::memcpy(bufRead,response + sizeof(respCode),len);
							cout<<bufRead;
						}
						else if(code == NODATA)
						{
							if(!header)
							{
								cout<<"RTR TABLE"<<endl;
								//HK54866
								header = true;
								data = false;
								continue;
							}
						}
						else
						{
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
				}//while
			}
			else
			{//sending failed
				cout <<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::CommError<< endl;
				if(request)
					delete (request);
				if(cl)
					delete cl;
				return 12; 
			}
			if(cl)
				delete cl;
			if(request)
				delete request;
		}
		else
		{//connection failed
			cout<<CmdPrintouts::IntProgFault<<": "<<CmdPrintouts::NoServerAccess<< endl;
			if(cl)
				delete cl;
			return 117;
		}
	}
	return 0;
}
