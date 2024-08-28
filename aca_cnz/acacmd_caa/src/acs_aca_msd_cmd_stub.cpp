#include <sys/types.h>
#include <sys/socket.h>

#include "acs_aca_api_tracer.h"
#include "acs_aca_msd_cmd_stub.h"
#include "acs_aca_utility.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_MSDCMD_Stub)

class ACS_ACA_StructuredRequestBuilder;

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_MSDCMD_Stub::ACS_ACA_MSDCMD_Stub (void)
	: ALLAP(false), CURRENTAP(false), SPECIFICAP(false), MCP(false) {
	ACS_ACA_TRACE_FUNCTION;
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_MSDCMD_Stub::~ACS_ACA_MSDCMD_Stub (void) {
	ACS_ACA_TRACE_FUNCTION;
	DeleteList();
}

//------------------------------------------------------------------------------
//      Command request
//msdls request building,interaction with DSD,receiving data
//------------------------------------------------------------------------------
void ACS_ACA_MSDCMD_Stub::msdls (unsigned short opt, ACE_TCHAR * ap, unsigned int APid, ACE_TCHAR * cpsource, ACE_TCHAR * ms, ACE_TCHAR * data) {
	ACS_ACA_TRACE_FUNCTION;

	ACS_ACA_StructuredRequestBuilder req_builder;
	void * request = 0;
	ACE_UINT32 reqSize = 0;
	bool EndList = false;
	//get CpId for request
	CPID cpID = 0;
	const unsigned timeout = 6 * 1000;	// 6 seconds

	if (MCP) {
		if (ACE_OS::strcmp(cpsource, "") != 0) //check cpsource
			GetCPID(cpsource, cpID);
	}

	//request building
	request = ACS_ACA_MSGDirector::buildStructuredRequest(&req_builder, reqSize, opt, cpsource, ms, data, cpID);

	if (!request) { // ERROR: on building request structure
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'ACS_ACA_MSGDirector::buildStructuredRequest' returned NULL, throwing GENERAL_EXCEPTION!");
		throw GENERAL_EXCEPTION;
	}

	aca_msdls_req_t * rq = reinterpret_cast<aca_msdls_req_t *>(request);
	aca_msdls_resp_t * rs = 0;

	ACS_DSD_Client cl;

	//check AP options
	if (ALLAP) { //msdls
		ACS_ACA_TRACE_MESSAGE("The command must be executed for ALL the configured AP!");

		std::set<uint16_t> ap_system_ids;
		get_All_AP_sysId_in_Cluster(ap_system_ids);

		if (ap_system_ids.size() == 0) { // ERROR: Retrieving all AP system in cluster
			ACS_ACA_TRACE_MESSAGE("ERROR: the number of AP found is 0, throwing GENERAL_EXCEPTION!");
			DeleteList();
			throw GENERAL_EXCEPTION;
		}

		bool not_found = true;

		for ( std::set<uint16_t>::iterator it = ap_system_ids.begin(); it != ap_system_ids.end(); it++ ) {
			uint16_t SysAP = *it;
			ACS_DSD_Session ses;
			ACE_TCHAR APstr1 [32] = {0};
			::snprintf(APstr1, sizeof(APstr1) - 1, "AP%u", (SysAP - 2000));

			ACS_ACA_TRACE_MESSAGE("Trying to connect to the service ACS_MSD_Server@ACS with the following parameters: "
					"system_id = %u, timeout = %u, system_name = %s", SysAP, timeout, APstr1);

			if (cl.connect(ses, "ACS_MSD_Server", "ACS", SysAP, acs_dsd::NODE_STATE_ACTIVE, timeout)) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Call 'connect' failed, trying the next AP! - "
						"last_error = %d, error_message: '%s'", cl.last_error(), cl.last_error_text());
				continue;
			}

			not_found = false;
			EndList = false;
			ACS_ACA_TRACE_MESSAGE("Sending request to ACA server running on %s with timeout %u", APstr1, timeout);

			//sending request
			if (ses.sendf(request, reqSize, timeout, MSG_NOSIGNAL) < 0) { // ERROR: sending request
				ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendf' failed, throwing SEND_EXCEPTION! - "
						"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
				DeleteList();
				ses.close();
				throw SEND_EXCEPTION;
			}
			ACS_ACA_TRACE_MESSAGE("Receiving messages from ACA server running on %s with timeout %u", APstr1, timeout);

			while (!EndList) {
				//receiving
				ACE_TCHAR resp [16 * 1024];  // 16 KB receiver data buffer
				ACE_UINT32 respSize = ACS_ACA_ARRAY_SIZE(resp);

				if (ses.recv(resp, respSize, timeout) <= 0) { // ERROR: on receiving response from the ACA server
					ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed, throwing RECV_EXCEPTION! - "
							"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
					DeleteList();
					ses.close();
					throw RECV_EXCEPTION;
				}

				rs = reinterpret_cast<aca_msdls_resp_t *>(resp);

				//check error (end list?)
				if (rs->error == END) {
					ACS_ACA_TRACE_MESSAGE("Last message received from ACA server running on %s!", APstr1);
					EndList = true;
					continue;
				}

				if (rs->error != OK) {
					ACS_ACA_TRACE_MESSAGE("ERROR: Message with error received from ACA server running on %s! "
							"- error = %hu", APstr1, rs->error);
					DeleteList();
					ses.close();
					ErrorCheck(rs);
				}

				if (MCP) { // In MCP environment, print AP, CP, MS
					ACS_ACA_TRACE_MESSAGE("Adding the message received (MCP system) from %s into the final list!", APstr1);

					//no error
					CPID cpid = rs->cpID;//read cpID
					std::string NODE = "";

					//get Cp name for that cpid
					ACE_INT32 ret = GetCPName(cpid, NODE);

					if (ret) { //CS error
						ACS_ACA_TRACE_MESSAGE("ERROR: Call 'GetCPName' returned %d!", ret);
						DeleteList();
						ses.close();
						throw ret;
					}

					//push in list message
					size_t dLen = rs->data_lenght + 32; //32 bytes for node & ap len
					ACE_TCHAR * str = new ACE_TCHAR[dLen];
					ACE_OS::memset(str, 0, dLen);
					::sprintf(str, "%-7s %-7s %-15s", APstr1, NODE.c_str(), rs->data);
					lprint.push_back(str);
				}
				else { // In SCP environment, print AP and MS only
					ACS_ACA_TRACE_MESSAGE("Adding the message received (MCP system) from %s into the final list!", APstr1);
					size_t str_len = rs->data_lenght + 32;
					char * str = new char[str_len];
					::sprintf(str, "%-7s %-15s", APstr1, rs->data);
					lprint.push_back(str);
				}
			} //end while recv

			ses.close();
		} //for AP

		if (not_found) { //connect failed towards all AP
			ACS_ACA_TRACE_MESSAGE("ERROR: connection failed for all the APs (n_of_AP = %zd)", ap_system_ids.size());
			throw CONNECT_EXCEPTION;
		}
	} //end case msdls (all AP)

	else if (CURRENTAP) { //all options for local ap
		ACS_ACA_TRACE_MESSAGE("The command must be executed only for the CURRENT AP!");

		ACS_DSD_Session ses;
		ACS_DSD_Node nd;
		nd.system_id = acs_dsd::SYSTEM_ID_THIS_NODE;
		nd.node_state = acs_dsd::NODE_STATE_ACTIVE; //Only active node

		//get AP name
		std::string strAP = "";
		ACE_TCHAR tmpAP [16] = {0};
		ses.get_local_node(nd);
		ACE_OS::strcpy(tmpAP, nd.node_name);
		strAP = tmpAP;
		strAP = strAP.substr(0,(strAP.length() - 1));

		ACS_ACA_TRACE_MESSAGE("Trying to connect to the service ACS_MSD_Server@ACS with the following parameters: "
				"system_id = %u, timeout = %u, system_name = %s", acs_dsd::SYSTEM_ID_THIS_NODE, timeout, strAP.c_str());

		if (cl.connect(ses, "ACS_MSD_Server", "ACS", acs_dsd::SYSTEM_ID_THIS_NODE, acs_dsd::NODE_STATE_ACTIVE, timeout)) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'connect' failed, throwing CONNECT_EXCEPTION! - "
					"last_error = %d, error_message: '%s'", cl.last_error(), cl.last_error_text());
			DeleteList();
			throw CONNECT_EXCEPTION;
		}
		ACS_ACA_TRACE_MESSAGE("Sending request to ACA server with timeout %u", timeout);

		if (ses.sendf(request, reqSize, timeout, MSG_NOSIGNAL) < 0) { // ERROR: sending request
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendf' failed, throwing SEND_EXCEPTION! - "
					"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
			DeleteList();
			ses.close();
			throw SEND_EXCEPTION;
		}
		ACS_ACA_TRACE_MESSAGE("Receiving messages from ACA server with timeout %u", timeout);

		while (!EndList) {
			ACE_TCHAR resp [16 * 1024];  // 16 KB receiver data buffer
			ACE_UINT32 respSize = ACS_ACA_ARRAY_SIZE(resp);

			if (ses.recv(resp, respSize, timeout) <= 0) { // ERROR: on receiving response from the ACA server
				ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed, throwing RECV_EXCEPTION! - "
						"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
				DeleteList();
				ses.close();
				throw RECV_EXCEPTION;
			}

			rs = reinterpret_cast<aca_msdls_resp_t *>(resp);

			//chek error (end List?)
			if (rs->error == END) {
				ACS_ACA_TRACE_MESSAGE("Last message received from ACA server!");
				EndList = true;
				continue;
			}

			//check error
			if (rs->error != OK) { //error
				ACS_ACA_TRACE_MESSAGE("ERROR: Message with error received from ACA server! - error = %hu", rs->error);
				DeleteList();
				ses.close();
				ErrorCheck(rs);
			}
			ACS_ACA_TRACE_MESSAGE("Adding the message received from ACA server into the final list! - optmask = %hu", rq->optmask);

			//No error check options
			if ((rq->optmask == 2) || (rq->optmask == 4)) { //msdls -cp CP,mslds -m MS (multiple)
				std::string NODE = "";
				//get Cp name
				ACE_INT32 ret = GetCPName(rs->cpID, NODE);

				if (ret != 0) {//CS error
					ACS_ACA_TRACE_MESSAGE("ERROR: Call 'GetCPName' returned %d!", ret);
					DeleteList();
					ses.close();
					throw ret;
				}

				ACE_TCHAR * str = new char[4096];
				//rs->data_lenght + 1];
				//memset(str, 0, (rs->data_lenght + 1));
				ACE_OS::memset(str, 0, 4096);
				::sprintf(str, "%-7s %-7s %-15s", strAP.c_str(), NODE.c_str(), rs->data);
				lprint.push_back(str);
			}
			else if ((rq->optmask == 22) || (rq->optmask == 276)) { //msdls -m MS -cp CP -d data
				ACE_TCHAR str[65000];
				ACE_OS::memset(str, 0, 65000);
				if (rs->data_lenght < 65000) ACE_OS::memcpy(str, rs->data, rs->data_lenght);
				else ACE_OS::memcpy(str, rs->data, 65000);
				::printf("%s", str);
			}
			else { //all others option for Current AP Case (Multiple and Single)
				ACE_TCHAR * str = new char[rs->data_lenght];
				ACE_OS::memset(str, 0, rs->data_lenght);
				ACE_OS::memcpy(str, rs->data, rs->data_lenght);
				lprint.push_back(str);
			}
		} //while recv

		ses.close();
	} //case CURRENTAP

	else if (SPECIFICAP) { //Specific AP
		ACS_ACA_TRACE_MESSAGE("The command must be executed only for the AP having system_id = %u!", APid);

		std::set<uint16_t> ap_system_ids;
		get_All_AP_sysId_in_Cluster(ap_system_ids);

		if (ap_system_ids.size() == 0) {
			ACS_ACA_TRACE_MESSAGE("ERROR: the number of AP found is 0, throwing GENERAL_EXCEPTION!");
			throw APNAME_EXCEPTION;
		}

		std::set<uint16_t>::iterator it = ap_system_ids.find(APid);

		if (it == ap_system_ids.end()) {
			ACS_ACA_TRACE_MESSAGE("ERROR: The AP with the given system_id (%u) was not found, throwing GENERAL_EXCEPTION!", APid);
			throw APNAME_EXCEPTION;
		}

		ACS_DSD_Session ses;
		uint16_t system_id = *it;

		ACS_ACA_TRACE_MESSAGE("Trying to connect to the service ACS_MSD_Server@ACS with the following parameters: "
				"system_id = %u, timeout = %u", system_id, timeout);

		if (cl.connect(ses, "ACS_MSD_Server", "ACS", system_id, acs_dsd::NODE_STATE_ACTIVE, timeout)) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'connect' failed, throwing CONNECT_EXCEPTION! - "
					"last_error = %d, error_message: '%s'", cl.last_error(), cl.last_error_text());
			DeleteList();
			throw CONNECT_EXCEPTION;
		}
		ACS_ACA_TRACE_MESSAGE("Sending request to ACA server running on %u with timeout %u", system_id, timeout);

		//sending Message
		if (ses.sendf(request, reqSize, timeout, MSG_NOSIGNAL) < 0) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendf' failed, throwing SEND_EXCEPTION! - "
					"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
			DeleteList();
			ses.close();
			throw SEND_EXCEPTION;
		}
		ACS_ACA_TRACE_MESSAGE("Receiving messages from ACA server running on %u with timeout %u", system_id, timeout);

		//receiving msg
		while (!EndList) {
			ACE_TCHAR resp [16 * 1024];  // 16 KB receiver data buffer
			ACE_UINT32 respSize = ACS_ACA_ARRAY_SIZE(resp);

			if (ses.recv(resp, respSize, timeout) <= 0) { // ERROR: on receiving response from the ACA server
				ACS_ACA_TRACE_MESSAGE("ERROR: Call 'recv' failed, throwing RECV_EXCEPTION! - "
						"last_error = %d, error_message: '%s'", ses.last_error(), ses.last_error_text());
				DeleteList();
				ses.close();
				throw RECV_EXCEPTION;
			}

			rs = reinterpret_cast<aca_msdls_resp_t *>(resp);

			//check error (end List?)
			if (rs->error  == END) {
				ACS_ACA_TRACE_MESSAGE("Last message received from ACA server running on %u!", system_id);
				EndList = true;
				continue;
			}

			//check error
			if (rs->error != OK) { //error
				ACS_ACA_TRACE_MESSAGE("ERROR: Message with error received from ACA server running on %u! "
						"- error = %hu", system_id, rs->error);
				DeleteList();
				ses.close();
				ErrorCheck(rs);
			}
			ACS_ACA_TRACE_MESSAGE("Adding the message received from ACA server on %u into the final list!"
					" - optmask = %hu", system_id, rq->optmask);

			//no error check options
			if (((rq->optmask == 1) && MCP) ||(rq->optmask == 3) || (rq->optmask == 5)) { //msdls -ap AP, msdls -ap AP -m MS,msdls -ap AP -cp CP
				std::string NODE = "";
				//get Cp name
				ACE_INT32 ret = GetCPName(rs->cpID, NODE);

				if (ret != 0) { //CS error
					ACS_ACA_TRACE_MESSAGE("ERROR: Call 'GetCPName' returned %d!", ret);
					DeleteList();
					ses.close();
					throw ret;
				}

				size_t dLen = rs->data_lenght + 32; //32 bytes for node & ap len
				ACE_TCHAR * str = new char[dLen];
				ACE_OS::memset(str, 0, dLen);
				::sprintf(str, "%-7s %-7s %-15s", ap, NODE.c_str(), rs->data);
				lprint.push_back(str);
			}
			else { //all others options for Specific AP case (only multiple)
				if (rq->optmask == 1) {	// msdls -ap AP
					int lenght = rs->data_lenght + 16;
					char * str = new char[lenght];
					::memset(str, 0, lenght);
					::snprintf(str, lenght, "%-7s %-15s", ap, rs->data);
					lprint.push_back(str);
				}
				else {
					ACE_TCHAR * str = new char[rs->data_lenght];
					ACE_OS::memset(str, 0, rs->data_lenght);
					ACE_OS::strcpy(str, rs->data);
					lprint.push_back(str);
				}

			}
		} //end while

		ses.close();
	} //end case SPECIFIC AP

	if ((rq->optmask != 22) && ((rq->optmask != 276)))
			PrintList(rq); //print data
}
//*****************************************************************************
//                              GetCPName
//*****************************************************************************
int ACS_ACA_MSDCMD_Stub::GetCPName (unsigned short id, string & name) {
	ACS_ACA_TRACE_FUNCTION;
	ACS_ACA_TRACE_MESSAGE("Trying to retrieve the name for the CP/BC having ID = %hu", id);

	ACS_CS_API_Name Name;
	ACS_CS_API_CP * CP = ACS_CS_API::createCPInstance();//create a CP instance

	//calling to getCPName
	bool result = false;
	ACS_CS_API_NS::CS_API_Result returnValue = CP->getCPAliasName(id, Name, result);

	//check return value
	if (returnValue != ACS_CS_API_NS::Result_Success) { //error
		switch (returnValue) {
		case ACS_CS_API_NS::Result_NoAccess:
			ACS_CS_API::deleteCPInstance(CP);
			return CS_EXCEPTION55;
		case ACS_CS_API_NS::Result_NoEntry:
			ACS_CS_API::deleteCPInstance(CP);
			return CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_NoValue:
			ACS_CS_API::deleteCPInstance(CP);
			return CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_Failure:
			ACS_CS_API::deleteCPInstance(CP);
			return CS_EXCEPTION56;
		default:
			ACS_CS_API::deleteCPInstance(CP);
			return CS_EXCEPTION56;
		}//switch
	}

	ACS_CS_API::deleteCPInstance(CP);

	//result for GetCpName ok, get name string
	ACE_TCHAR * node = new char[Name.length()];

	size_t Len = Name.length();

	//calling to getName
	ACS_CS_API_NS::CS_API_Result Value = Name.getName(node,Len); 

	//check return value
	if (Value != ACS_CS_API_NS::Result_Success) { //error
		delete[] node;
		switch (Value) {
		case ACS_CS_API_NS::Result_NoAccess:
			return CS_EXCEPTION55;
		case ACS_CS_API_NS::Result_NoEntry:
			return CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_NoValue:
			return CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_Failure:
			return CS_EXCEPTION56;
		default:
			return CS_EXCEPTION56;
		} //switch
	}

	//return value for get name, ok copy node string
	name = node;

	delete[] node;

	ACS_ACA_TRACE_MESSAGE("The retrieved name for the CP/BC having ID = %hu is %s", id, name.c_str());
	return 0; //Success
}

//**************************************************************************************
//                                          PrintList
//**************************************************************************************
void ACS_ACA_MSDCMD_Stub::PrintList(aca_msdls_req_t * req) {
	ACS_ACA_TRACE_FUNCTION;

	std::list<char *>::iterator itr;

	if (MCP) { //multiple cp case
		//check options
		if ((ALLAP) || ((CURRENTAP) && ((req->optmask ==2) || (req->optmask == 4)))
			|| ((SPECIFICAP) && ((req->optmask == 1) || (req->optmask == 3) || (req->optmask == 5)))) {
			//print header (format 1)
			printf("\n%-7s %-7s %-15s\n", "NODE", "CPNAME", "MESSAGESTORE");
			//printf("%s\n","");
		} else if (((CURRENTAP) && (req->optmask == 134)) || ((SPECIFICAP) && (req->optmask == 135))) {
			//print header(format 4)
			::printf("%s '%s' %s '%s'\n", "Statistics of message store", req->msname, "at CP", req->cpsource);
		}
	} //end multiple cp case

	else { //single cp case
		//check options (only CURRENTAP)
		if ((req->optmask == 0) || (req->optmask == 1)) {
			//print header (format1)
			::printf("\nExisting Message Stores\n\n");
			::printf("%-7s %-15s\n", "NODE", "MESSAGESTORE");
		}
		else if (req->optmask == 388) {
			//print header (format4)
			::printf("%s '%s' \n", "Statistics of message store", req->msname);
		}
	}//end single cp case

	//print data
	if (!lprint.empty()) {
		for (itr = lprint.begin(); itr != lprint.end(); ++itr) {
			::printf("%s\n", *itr);
		}
	}
}

//****************************************************************************
//             DeleteList
//****************************************************************************
void ACS_ACA_MSDCMD_Stub::DeleteList () {
	ACS_ACA_TRACE_FUNCTION;

	std::list<char *>::iterator it, front, end;

	if (!lprint.empty()) {
		for (it = lprint.begin(); it != lprint.end(); ++it) {
			if (*it) delete[] *it; //delete items in global list
		}

		front = lprint.begin();
		end = lprint.end();
		lprint.erase(front, end);
	}
}

//***************************************************************************
//             GetCPID
//***************************************************************************
void ACS_ACA_MSDCMD_Stub::GetCPID (char * cpname, CPID & id) {
	ACS_ACA_TRACE_FUNCTION;
	ACS_ACA_TRACE_MESSAGE("Trying to retrieve the ID for the CP/BC %s", cpname);

	//cp instance
	ACS_CS_API_CP * cp = ACS_CS_API::createCPInstance();
	ACS_CS_API_Name cpName(cpname);

	//calling to getCPId method
	ACS_CS_API_NS::CS_API_Result returnValue = cp->getCPId(cpName,id);

	//check return value
	if (returnValue != ACS_CS_API_NS::Result_Success) { //error
		switch (returnValue) {
		case ACS_CS_API_NS::Result_NoAccess:
			ACS_CS_API::deleteCPInstance(cp);
			throw CS_EXCEPTION55;
		case ACS_CS_API_NS::Result_NoEntry:
			ACS_CS_API::deleteCPInstance(cp);
			throw CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_NoValue:
			ACS_CS_API::deleteCPInstance(cp);
			throw CS_EXCEPTION118;
		case ACS_CS_API_NS::Result_Failure:
			ACS_CS_API::deleteCPInstance(cp);
			throw CS_EXCEPTION56;
		default:
			ACS_CS_API::deleteCPInstance(cp);
			throw CS_EXCEPTION56;
		}//switch
	}

	ACS_ACA_TRACE_MESSAGE("The retrieved ID for the CP/BC %s is %u", cpname, id);
	//return value OK
	ACS_CS_API::deleteCPInstance(cp);
}

//************************************************************************************
//                           CheckError
//************************************************************************************
void ACS_ACA_MSDCMD_Stub::ErrorCheck (aca_msdls_resp_t * rsp) {
	ACS_ACA_TRACE_FUNCTION;
	ACS_ACA_TRACE_MESSAGE("ERROR: A message with error has been received from ACA server, "
			"throwing the correct exception, error = %hu", rsp->error);

	switch (rsp->error) {
	case CS_ERROR55://CSERROR
		throw CS_EXCEPTION55;
	case CS_ERROR56://CSERROR
		throw CS_EXCEPTION56;
	case CS_ERROR118://CSERROR
		throw CS_EXCEPTION118;
	case GENERAL_FAULT://GENERAL FAULT
		throw GENERAL_EXCEPTION;
	case INVALID://MSTORE ERROR
		throw MSNAME_EXCEPTION;
	case DATA_FILE_ERROR://DATAFILE ERROR
		throw DATA_FILE_EXCEPTION;
	case COMMIT_ERROR://COMMIT FILE ERROR
	case EOF_FILE: //EOF ERROR
		throw EOF_FILE_EXCEPTION;
	case STATISTICS_ERROR: //STATISTICS FILE ERROR
		throw STAT_FILE_EXCEPTION;
	default:
		throw GENERAL_FAULT;
	}//switch
}

void ACS_ACA_MSDCMD_Stub::get_All_AP_sysId_in_Cluster(std::set<uint16_t> & ap_system_ids) {
	ACS_ACA_TRACE_FUNCTION;

	if (ACS_CS_API_HWC * hwc = ACS_CS_API::createHWCInstance()) {
		if (ACS_CS_API_BoardSearch * boardSearch = ACS_CS_API_HWC::createBoardSearchInstance()) {
			ACS_CS_API_IdList boardList;
			boardSearch->setSysType(ACS_ACA_CS_API::SysType_AP);

			if (hwc->getBoardIds(boardList, boardSearch) == ACS_CS_API_NS::Result_Success) {
				for (unsigned int i = 0; i < boardList.size(); i++) { //step through all the boards in the hwc table
					BoardID boardId = boardList[i];
					uint16_t sysId = 0;

					if (hwc->getSysId(sysId, boardId) == ACS_CS_API_NS::Result_Success)
						ap_system_ids.insert(sysId);
				}
			}
			ACS_CS_API_HWC::deleteBoardSearchInstance(boardSearch);
		}
		ACS_CS_API::deleteHWCInstance(hwc);
	}

	ACS_ACA_TRACE_MESSAGE("Retrieved %zd different APs from the CS!", ap_system_ids.size());
}
