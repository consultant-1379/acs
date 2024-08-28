/*
 * * @file acs_asec_immhandler.cpp
 *	@brief
 *	Class method implementation for ACS_ASEC_ImmHandler.
 *
 *
 *	@author enungai (Nunziante Gaito)
 *	@date 2019-03-21
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2019
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2019-03-21 | enungai      | File created.                       |
 *	+========+============+==============+=====================================+

 *
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_asec_immhandler.h"
#include "acs_asec_oi_ipsecConnection.h"
#include "acs_asec_oi_gsnhConnection.h"
#include "acs_asec_global.h"
#include "ace/OS_NS_poll.h"
#include <ace/Barrier.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <ace/Reactor.h>

#define IPSECSTATUS "ipsec status "
#define IPSECUP "ipsec up "
#define RETRY_TIME  10
#define IPSECCONNFILE   "/storage/system/config/asec/strongswan/connections"
const char IpsecConnectionClass[] = "IpsecConnection";


/*============================================================================
	ROUTINE: ACS_ASEC_ImmHandler
 ============================================================================ */
ACS_ASEC_ImmHandler::ACS_ASEC_ImmHandler()
{


	// create the file descriptor to signal stop
	m_StopEvent = eventfd(0,0);

	// create the Ipsec Connection OI
	m_oi_ipsecConnection = new ACS_ASEC_OI_IpsecConnection();

	// create the Gsnh Connection OI
	m_oi_gsnhConnection = ACS_ASEC_OI_GsnhConnection::getInstance();;

	// Initialize the svc state flag
	svc_run = false;

	acs_asec_immhandlerTrace = new (std::nothrow) ACS_TRA_trace("ACS_ASEC_ImmHandler");

	ASEC_Log.Open("ASECBIN");



}

/*============================================================================
	ROUTINE: ~ACS_ASEC_ImmHandler
 ============================================================================ */
ACS_ASEC_ImmHandler::~ACS_ASEC_ImmHandler()
{
	TRACE(acs_asec_immhandlerTrace, "%s", "~ACS_ASEC_ImmHandler");
	ACE_OS::close(m_StopEvent);

	if(NULL != m_oi_ipsecConnection)
	{
		// remove IPSEC connection OI definition
		m_oiHandler.removeClassImpl(m_oi_ipsecConnection, IpsecConnectionClass);
		delete m_oi_ipsecConnection;
	}

	if(NULL != m_oi_gsnhConnection)
	{
		// remove GSNH connection OI definition
		m_oiHandler.removeObjectImpl(m_oi_gsnhConnection);
//		delete m_oi_gsnhConnection;
		ACS_ASEC_OI_GsnhConnection::finalize();
	}

	//if(NULL != m_ThreadsSyncShutdown)
	//	delete m_ThreadsSyncShutdown;

	if(NULL != acs_asec_immhandlerTrace)
		delete acs_asec_immhandlerTrace;

	ASEC_Log.Close();
}

/*============================================================================
	ROUTINE: open
 ============================================================================ */
int ACS_ASEC_ImmHandler::open(void *args)
{
	TRACE(acs_asec_immhandlerTrace, "%s", "Entering open()");
	ASEC_Log.Write("asec-immhandler-class ... entering open()",LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(args);
	string versionStr;
	p_stopRacoon = "systemctl stop racoon";
	p_startRacoon = "systemctl start racoon";
	p_stopStrongswan = "systemctl stop strongswan";
	p_startStrongswan = "systemctl start strongswan";
	p_stopSetkey = "systemctl stop racoon-setkey";
	p_startSetkey = "systemctl start racoon-setkey";

	string commandOutput = " ";
	string checkRacoonStatus = "systemctl is-active racoon";
	string checkStrongswanStatus = "systemctl is-active strongswan";
	string checkRacoonSetkeyStatus = "systemctl is-active racoon-setkey";

	p_ikeversion = getIkeVersion();
	if(p_ikeversion == 2){
		//stop racoon if active
		if(executeCommand(checkRacoonStatus, commandOutput) == 1){
			executeCommand(p_stopRacoon, commandOutput);
			commandOutput = " ";
		}
		if(executeCommand(checkRacoonSetkeyStatus, commandOutput) == 1){
                        executeCommand(p_stopSetkey, commandOutput);
                        commandOutput = " ";
                }

		//if strongswan is stopped
		if(executeCommand(checkStrongswanStatus, commandOutput) == 0){
			clearIkev2Data();
			// load ipsecConnections from IMM
			int numberOfConnectionsMade = loadIkev2Connections();
			if(numberOfConnectionsMade > 0){
				ASEC_Log.Write("asec-immhandler-class ikev2 connections create success",LOG_LEVEL_INFO);
			}
			else{
				ASEC_Log.Write("asec-immhandler-class no IKEV2 connections made",LOG_LEVEL_ERROR);
			}
			//start strongswan
			executeCommand(p_startStrongswan, commandOutput);
			commandOutput = " ";
		}
	}
	else if(p_ikeversion == 1){
		//stop strongswan if active
		if(executeCommand(checkStrongswanStatus, commandOutput) == 1){
			executeCommand(p_stopStrongswan, commandOutput);
			commandOutput = " ";
		}
		//start racoon if not active
		if(executeCommand(checkRacoonStatus, commandOutput) == 0){
			executeCommand(p_startRacoon, commandOutput);
			commandOutput = " ";
		}
               if(executeCommand(checkRacoonSetkeyStatus, commandOutput) == 0){
                        executeCommand(p_startSetkey, commandOutput);
			commandOutput = " ";
                }
	}
	

        // Reset IMM attributes
        ASEC_Log.Write("ACS_ASEC_ImmHandler::open(), reset temp TLS info", LOG_LEVEL_ERROR);
        m_oi_gsnhConnection->setIsRestartTriggered(false);
        m_oi_gsnhConnection->clearConfTLSciphers();
        m_oi_gsnhConnection->clearConfTLSversion();

        // Initialize IMM and set Implementers in the map
	if(!registerImmOI()){
		ASEC_Log.Write("ACS_ASEC_ImmHandler::open(), error on Implementer set", LOG_LEVEL_ERROR);
		TRACE(acs_asec_immhandlerTrace, "%s", "Leaving open(), error on Implementer set");
		return -1;
	}
	
	//Fetch GSNH security related IMM attribute values
	if(m_oi_gsnhConnection->fetchGsnhImmattribs(gsnhConnectionClass::dnName))
	{
		try
		{
			m_oi_gsnhConnection->performGsnhOperaions();
		}
		catch (int error)
		{
			ASEC_Log.Write("ACS_ASEC_ImmHandler::open(), error on Performing GSNH Security related Operations", LOG_LEVEL_ERROR);
			TRACE(acs_asec_immhandlerTrace, "%s", "Leaving open(), error on Retrieving GSNH security related IMM attribute values");
			return -1;
		}
	}
	else
	{
		ASEC_Log.Write("ACS_ASEC_ImmHandler::open(), error on Retrieving GSNH security related IMM attribute values", LOG_LEVEL_ERROR);
		TRACE(acs_asec_immhandlerTrace, "%s", "Leaving open(), error on Retrieving GSNH security related IMM attribute values");
		return -1;
	}
	TRACE(acs_asec_immhandlerTrace, "%s", "open, Start thread to handler IMM request");
	// Start the thread to handler IMM request
	if( activate() == -1 )
	{
		m_oiHandler.removeClassImpl(m_oi_ipsecConnection, IpsecConnectionClass);

		m_oiHandler.removeObjectImpl(m_oi_gsnhConnection);
		
		ASEC_Log.Write("ACS_ASEC_ImmHandler::open,  error on start svc thread", LOG_LEVEL_ERROR);
		TRACE(acs_asec_immhandlerTrace, "%s", "Leaving open(), error on start svc thread");
		return -1;
	}
        p_restartFlag = 1;
	//start timer
        p_timer = ACS_ASEC_Global::instance()->reactor()->schedule_timer(this,0,ACE_Time_Value(5), ACE_Time_Value(0));
	
	TRACE(acs_asec_immhandlerTrace, "%s", "Leaving open()");
	return 0;
}

/*============================================================================
	ROUTINE: registerImmOI
 ============================================================================ */
bool ACS_ASEC_ImmHandler::registerImmOI()
{
	TRACE(acs_asec_immhandlerTrace, "%s", "Entering registerImmOI()");
	ACS_CC_ReturnType result;

	// Register the OI of Ipsec Connection class
	result = m_oiHandler.addClassImpl(m_oi_ipsecConnection, IpsecConnectionClass);

	if(ACS_CC_FAILURE == result)
	{
		ASEC_Log.Write("ACS_ASEC_ImmHandler::open, error on set ipsec connection Implementer", LOG_LEVEL_ERROR);
		TRACE(acs_asec_immhandlerTrace, "%s", "Leaving registerImmOI(), error on set ipsec connection Implementer");
		return false;
	}
	setenv("IMMA_OI_CALLBACK_TIMEOUT","15",1);
	// Register the OI of Gsnh Connection class
	result = m_oiHandler.addObjectImpl(m_oi_gsnhConnection);

	if(ACS_CC_FAILURE == result)
	{
		ASEC_Log.Write("ACS_ASEC_ImmHandler::open, error on set gsnh connection Implementer", LOG_LEVEL_ERROR);
		TRACE(acs_asec_immhandlerTrace, "%s", "Leaving registerImmOI(), error on set gsnh connection Implementer");
		return false;
	}
	TRACE(acs_asec_immhandlerTrace, "%s", "Leaving registerImmOI()");
	return true;
}



/*============================================================================
	ROUTINE: svc
 ============================================================================ */
int ACS_ASEC_ImmHandler::svc()
{
	TRACE(acs_asec_immhandlerTrace, "%s", "Entering svc() thread");
	// Create a fd to wait for request from IMM
	const nfds_t nfds = 3;

	struct pollfd fds[nfds];

	const size_t msgLength = 255;
	char msg_buff[msgLength+1]={'\0'};

	ACE_INT32 poolResult;
	ACS_CC_ReturnType result;

	// Set the svc thread state to on
	svc_run = true;

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = m_oi_ipsecConnection->getSelObj();
	fds[1].events = POLLIN;

	fds[2].fd = m_oi_gsnhConnection->getSelObj();
	fds[2].events = POLLIN;

	// waiting for IMM requests or stop
	while(svc_run)
	{
		poolResult = ACE_OS::poll(fds, nfds);

		if( FAILURE == poolResult )
		{
			if(errno == EINTR)
			{
				continue;
			}
			snprintf(msg_buff, msgLength, "ACS_ASEC_ImmHandler::svc(), exit after error=%s", strerror(errno) );
			ASEC_Log.Write(msg_buff,	LOG_LEVEL_ERROR);
			TRACE(acs_asec_immhandlerTrace, "%s", "Leaving svc(), exit after poll error");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received a stop request from server
			TRACE(acs_asec_immhandlerTrace, "%s", "Leaving svc(), received a stop request from server");
			break;
		}

		bool needPoll = false;
		if(fds[1].revents & POLLIN)
		{
			// Received a IMM request on a SimpleFile
			//TRACE(acs_asec_immhandlerTrace, "%s","svc, received IMM request on a ipsec connecction");
			result = m_oi_ipsecConnection->dispatch(ACS_APGCC_DISPATCH_ONE);

			if(ACS_CC_SUCCESS != result)
			{
				snprintf(msg_buff, msgLength, "ACS_ASEC_ImmHandler::svc(), error on ipsec connection dispatch event" );
				ASEC_Log.Write(msg_buff,	LOG_LEVEL_ERROR);
				TRACE(acs_asec_immhandlerTrace, "%s", msg_buff);
			}
			//continue;
			needPoll=true;
		}

		if(fds[2].revents & POLLIN)
		{
			// Received a IMM request on a SimpleFile
			//TRACE(acs_asec_immhandlerTrace, "%s","svc, received IMM request on a gsnh connecction");
			result = m_oi_gsnhConnection->dispatch(ACS_APGCC_DISPATCH_ONE);

			if(ACS_CC_SUCCESS != result)
			{
				snprintf(msg_buff, msgLength, "ACS_ASEC_ImmHandler::svc(), error on gsnh connection dispatch event" );
				ASEC_Log.Write(msg_buff,	LOG_LEVEL_ERROR);
				TRACE(acs_asec_immhandlerTrace, "%s", msg_buff);
			}
			//continue;
			needPoll=true;
		}
		if(needPoll)
			continue;

	// Set the svc thread state to off
	svc_run = false;
	TRACE(acs_asec_immhandlerTrace, "%s","Leaving svc() thread");

	}

	return SUCCESS;
}



//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ImmHandler::close(){

	string commandOutput=" ";
	if(NULL != m_oi_gsnhConnection)
	{
		// remove GSNH connection OI definition
		m_oiHandler.removeObjectImpl(m_oi_gsnhConnection);
	}
	ACE_UINT64 stopEvent=1;
	ssize_t numByte;
	int rCode=0;

	if(p_ikeversion == 2){
		executeCommand(p_stopStrongswan, commandOutput);
		commandOutput=" ";
		clearIkev2Data();
	}
	else{
		if(p_ikeversion == 1){
			executeCommand(p_stopRacoon, commandOutput);
			commandOutput=" ";
			executeCommand(p_stopSetkey, commandOutput);
			commandOutput=" ";
		}
	}
	if(NULL != m_oi_ipsecConnection)
	{
		// remove IPSEC connection OI definition
		m_oiHandler.removeClassImpl(m_oi_ipsecConnection, IpsecConnectionClass);
	}
	// Signal to IMM thread to stop
	numByte = ::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));
	if(sizeof(ACE_UINT64) != numByte)
	{
		ASEC_Log.Write("ACS_ASEC_ImmHandler::stopImmHandler, error on stop IMM_Handler", LOG_LEVEL_ERROR);
		rCode = -1;

	}
	else
	{
		ASEC_Log.Write("ACS_ASEC_ImmHandler waiting for the closure of IMM handler ",LOG_LEVEL_ERROR);
		this->wait();
		ASEC_Log.Write("ACS_ASEC_ImmHandler IMM handler closed ",LOG_LEVEL_ERROR);

	}
	//stop timer
	if (p_timer >= 0){
	        ACS_ASEC_Global::instance()->reactor()->cancel_timer(this);
	}
        p_restartFlag = 0;
	return rCode;
}

//----------------------------------------------------------------------------------------------------------------------------------
ACS_ASEC_OI_IpsecConnection*  ACS_ASEC_ImmHandler::getOi(){
	return this->m_oi_ipsecConnection;
}
//----------------------------------------------------------------------------------------------------------------------------------
//result == 1 //SUCCESS
//result == 0 //FAILURE
bool ACS_ASEC_ImmHandler::executeCommand(const std::string& command, std::string& output)
{
        ASEC_Log.Write("acs-asec-class:executeCommand start", LOG_LEVEL_INFO);
        bool result = false;
        FILE* pipe = popen(command.c_str(), "r");
        if(NULL != pipe){
                char rowOutput[1024]={'\0'};
                while(!feof(pipe)){
                        // get the cmd output
                        if(fgets(rowOutput, 1023, pipe) != NULL){
                                std::size_t len = strlen(rowOutput);
                                // remove the newline
                                if( rowOutput[len-1] == '\n' ) rowOutput[len-1] = 0;
                                output.append(rowOutput);
                                }
                        }

                // wait cmd termination
                int exitCode = pclose(pipe);
                // get the exit code from the exit status
                result = (WEXITSTATUS(exitCode) == 0);
        }
        ASEC_Log.Write("acs-asec-class:executeCommand end",LOG_LEVEL_INFO);
        syslog(LOG_INFO, "ACS_ASEC_Adm::svc(): result:%s\n, command : %s\n, command output:%s\n", (result ? "TRUE" : "FALSE"), command.c_str(), output.c_str());
        return result;
}

void ACS_ASEC_ImmHandler::clearIkev2Data()
{
	ofstream ipsecConfFile,ipsecSecretsFile;
	//clean conf files
	ipsecConfFile.open("/etc/ipsec.conf",ios::out|ios::trunc);
	ipsecSecretsFile.open("/etc/ipsec.secrets", ios::out|ios::trunc);
	ipsecSecretsFile.close();
	ipsecConfFile.close();
	m_oi_ipsecConnection->clearAllSoftLinks();
}
int ACS_ASEC_ImmHandler::loadIkev2Connections()
{
	OmHandler immHandle;
	int numberOfConnections = 0;
	const size_t buffLength = 255;
	char buff[buffLength+1]={'\0'};
	vector<string> ipsec_vector;
	string v_authentication,v_ipsecConnectionId,v_type,v_remoteIP,v_lifetime,v_ikelifetime,v_protocol,v_remoteCertDn,v_key,v_remoteVirtualIP,v_ikeEncryptAlg,v_ipsecParam;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	vector<string> vecObj;
	int v_nodeCredentialId = 0;
	int v_trustCategoryId = 0;

	vector<ACS_APGCC_ImmAttribute *> vecAttr;
	ACS_APGCC_ImmAttribute authentication;
	ACS_APGCC_ImmAttribute ipsecConnectionId;
	ACS_APGCC_ImmAttribute type;
	ACS_APGCC_ImmAttribute remoteIP;
	ACS_APGCC_ImmAttribute lifetime;
	ACS_APGCC_ImmAttribute ikelifetime;
	ACS_APGCC_ImmAttribute protocol;
	ACS_APGCC_ImmAttribute remoteCertDn;
	ACS_APGCC_ImmAttribute key;
	ACS_APGCC_ImmAttribute nodeCredentialId;
	ACS_APGCC_ImmAttribute trustCategoryId;
	ACS_APGCC_ImmAttribute remoteVirtualIP;
	ACS_APGCC_ImmAttribute ikeEncryptAlg;
	ACS_APGCC_ImmAttribute ipsecParam;


	authentication.attrName = const_cast<char*>("authentication");
	ipsecConnectionId.attrName = const_cast<char*>("ipsecConnectionId");
	type.attrName = const_cast<char*>("type");
	remoteIP.attrName = const_cast<char*>("remoteIP");
	lifetime.attrName = const_cast<char*>("lifetime");
	ikelifetime.attrName = const_cast<char*>("ikelifetime");
	protocol.attrName = const_cast<char*>("protocol");
	remoteCertDn.attrName = const_cast<char*>("remoteCertDn");
	key.attrName = const_cast<char*>("key");
	nodeCredentialId.attrName = const_cast<char*>("nodeCredentialId");
	trustCategoryId.attrName = const_cast<char*>("trustCategoryId");
	remoteCertDn.attrName = const_cast<char*>("remoteCertDn");
	remoteVirtualIP.attrName = const_cast<char*>("remoteVirtualIP");
	ikeEncryptAlg.attrName = const_cast<char*>("ikeEncryptAlg");
	ipsecParam.attrName = const_cast<char*>("ipsecParam");


	vecAttr.push_back(&authentication);
	vecAttr.push_back(&ipsecConnectionId);
	vecAttr.push_back(&type);
	vecAttr.push_back(&remoteIP);
	vecAttr.push_back(&lifetime);
	vecAttr.push_back(&ikelifetime);
	vecAttr.push_back(&protocol);
	vecAttr.push_back(&remoteCertDn);
	vecAttr.push_back(&key);
	vecAttr.push_back(&remoteVirtualIP);
	vecAttr.push_back(&ikeEncryptAlg);
	vecAttr.push_back(&ipsecParam);
	vecAttr.push_back(&nodeCredentialId);
	vecAttr.push_back(&trustCategoryId);


	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		snprintf(buff, buffLength, "ACS_ASEC_Adm::createIkev2Connections(), immHandle.Init failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
		ASEC_Log.Write(buff, LOG_LEVEL_INFO);
		return -1;
	}
	result = immHandle.getClassInstances("IpsecConnection",vecObj);
	if ( result != ACS_CC_SUCCESS ){
		snprintf(buff, buffLength, "ACS_ASEC_Adm::createIkev2Connections(), immHandle.getClassInstances failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
		ASEC_Log.Write(buff, LOG_LEVEL_INFO);
		immHandle.Finalize();
		return -1;
	}
	numberOfConnections = vecObj.size();
	//loop for each object instance
	for ( unsigned int i = 0; i < vecObj.size(); i++){
		ipsec_vector.clear();
		result = immHandle.getAttribute(vecObj[i].c_str(), vecAttr);
		char *tmp = '\0';if( result != ACS_CC_SUCCESS ){
			snprintf(buff, buffLength, "ACS_ASEC_Adm::createIkev2Connections(), immHandle.getAttribute failed - Error Code : %i - Error Text : %s", immHandle.getInternalLastError(), immHandle.getInternalLastErrorText());
			ASEC_Log.Write(buff, LOG_LEVEL_INFO);
			immHandle.Finalize();
			return -1;
		}
		if(authentication.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(authentication.attrValues)));
			v_authentication = tmp;
		}
		if(ipsecConnectionId.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ipsecConnectionId.attrValues)));
			v_ipsecConnectionId = tmp;
		}
		if(type.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(type.attrValues)));
			v_type = tmp;
		}
		if(remoteIP.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteIP.attrValues)));
			v_remoteIP = tmp;
		}
		if(lifetime.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(lifetime.attrValues)));
			v_lifetime = tmp;
		}
		if(ikelifetime.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ikelifetime.attrValues)));
			v_ikelifetime = tmp;
		}
		if(protocol.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(protocol.attrValues)));
			v_protocol = tmp;
		}
		if(remoteCertDn.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteCertDn.attrValues)));
			v_remoteCertDn = tmp;
		}
		if(key.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(key.attrValues)));
			v_key = tmp;
		}
		if(nodeCredentialId.attrValuesNum !=0){
			v_nodeCredentialId = *(reinterpret_cast<int*>(*(nodeCredentialId.attrValues)));
		}
		if(trustCategoryId.attrValuesNum !=0){
			v_trustCategoryId = *(reinterpret_cast<int*>(*(trustCategoryId.attrValues)));
		}
		if(remoteVirtualIP.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(remoteVirtualIP.attrValues)));
			v_remoteVirtualIP = tmp;
		}
		if(ikeEncryptAlg.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ikeEncryptAlg.attrValues)));
			v_ikeEncryptAlg = tmp;
		}
		if(ipsecParam.attrValuesNum !=0){
			tmp = (reinterpret_cast<char *>(*(ipsecParam.attrValues)));
			v_ipsecParam = tmp;
		}

		ipsec_vector.push_back(v_authentication);
		ipsec_vector.push_back(v_ipsecConnectionId);
		ipsec_vector.push_back(v_type);
		ipsec_vector.push_back(v_remoteIP);
		ipsec_vector.push_back(v_lifetime);
		ipsec_vector.push_back(v_ikelifetime);
		ipsec_vector.push_back(v_protocol);
		ipsec_vector.push_back(v_remoteCertDn);
		ipsec_vector.push_back(v_key);
		ipsec_vector.push_back(v_remoteVirtualIP);
		ipsec_vector.push_back(v_ikeEncryptAlg);
		ipsec_vector.push_back(v_ipsecParam);


		result = m_oi_ipsecConnection->makeIkev2Connection(ipsec_vector,v_nodeCredentialId,v_trustCategoryId);
		if(result != ACS_CC_SUCCESS){
			numberOfConnections = -1;
		}
	}
	return numberOfConnections;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ImmHandler::getIkeVersion()
{
        string versionStr = " ";
        std::ifstream verStream("/cluster/etc/ikeversion", ios::in);
        while (verStream >> versionStr);

        if(!strcmp(versionStr.c_str(),"IKEV2"))
                return 2;
        else
                return 1;
}
//---------------------------------------------------------------------------------------------------------------
// 0 = inactive
// 1 = active
int ACS_ASEC_ImmHandler::getStrongswanStatus()
{
        int result = 0;
        std::string commandOutput = " ";
        string checkStrongswanStatus = "systemctl is-active strongswan";
        result = executeCommand(checkStrongswanStatus, commandOutput);
        return result;
}

//----------------------------------------------------------------------------------------------------------------------------------
//0 if down
//1 if up
int ACS_ASEC_ImmHandler::getIpsecStatus(std::string command)
{
        ASEC_Log.Write("ACS_ASEC_ImmHandler::getIpsecStatus start",LOG_LEVEL_INFO);
        int result = 0;
        string output= " ";
        string verifyStr = "no match";
        FILE* pipe = popen(command.c_str(), "r");
        if(NULL != pipe){
                char rowOutput[1024]={'\0'};
                while(!feof(pipe)){
                        // get the cmd output
                        if(fgets(rowOutput, 1023, pipe) != NULL){
                                output = rowOutput;
                                if(output.find(verifyStr) != string::npos){
                                        return 0;
                                }
                                else {
                                        result = 1;
                                }
                        }
                }
                // wait cmd termination
                int exitCode = pclose(pipe);
                // get the exit code from the exit status
                bool res = (WEXITSTATUS(exitCode) == 0);
                TRACE(acs_asec_immhandlerTrace,"exit code for getIpsecStatus:%d\n",res);
        }
        ASEC_Log.Write("ACS_ASEC_ImmHandler::getIpsecStatus end",LOG_LEVEL_INFO);
        return result;
}
//----------------------------------------------------------------------------------------------------------------------------------
void ACS_ASEC_ImmHandler::readIpsecConnFromFile(){
        ASEC_Log.Write("ACS_ASEC_ImmHandler::readIpsecConnFromFile start",LOG_LEVEL_INFO);
        ifstream ipsecConnFile(IPSECCONNFILE,ios::in);
        std::string line,connName,value,cmdString;
        int pos;
        if(ipsecConnFile.is_open()) {
                while(ipsecConnFile >> line){
                        pos = line.find(":");
                        connName = line.substr(0,pos);
                        value = line.substr(pos+1);
                        if(strcmp(value.c_str(),"1") == 0){
                                cmdString = IPSECUP + connName;
                                if(getIpsecStatus(cmdString.c_str()) == 1){
                                        syslog(LOG_INFO,"Ipsec connection %s is made up",cmdString.c_str());
                                }
                        }
                }
        }
        ASEC_Log.Write("ACS_ASEC_ImmHandler::readIpsecConnFromFile end",LOG_LEVEL_INFO);
        ipsecConnFile.close();
}
//-------------------------------------------------------------------------------------------------------------------------------
void ACS_ASEC_ImmHandler::writeIpsecConnToFile()
{
        ASEC_Log.Write("ACS_ASEC_ImmHandler::writeIpsecConntoFile start",LOG_LEVEL_INFO);
        ofstream ipsecConnFile(IPSECCONNFILE,ios::out|ios::trunc);
        std::string commandOutput = " ";
        std::string connName;
        std::vector<std::string> connList;
        std::string str1 = "\"";
        int status;

        std::map<std::string, int>::iterator it;
        if( ipsecConnFile.is_open())
        {
                std::string cmdString = "ipsecls | grep " + str1 + "Policy name" + str1 + " | cut -d: -f 2";
                if(executeCommand(cmdString.c_str(),commandOutput) == 1){
                        char* str = strtok((char*)commandOutput.c_str()," ");
                        while (str != NULL)
                        {
                                connList.push_back(str);
                                str = strtok(NULL," ");
                        }
                }
                for(int i=0;i<connList.size();i++)
                {
                        connName = connList[i];
                        cmdString = IPSECSTATUS + connName;
                        status = getIpsecStatus(cmdString.c_str()); //1 = up
                        if(status == 1){
                                ipsecConnFile << connName.c_str() << ":"<< status << "\n";
                        }
                }
        }
        ipsecConnFile.close();
        ASEC_Log.Write("ACS_ASEC_ImmHandler::writeIpsecConntoFile end",LOG_LEVEL_INFO);
}
//-------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ImmHandler::handle_timeout(const ACE_Time_Value& , const void*)
{
        ASEC_Log.Write("ACS_ASEC_ImmHandler::handle timeout start",LOG_LEVEL_INFO);
	p_ikeversion = getIkeVersion();
        if((p_ikeversion == 2) && (getStrongswanStatus() == 1)) {
                if(p_restartFlag == 1)
                {
                        readIpsecConnFromFile();
                        p_restartFlag = 0;
                }
                else
                {
                        writeIpsecConnToFile();
                }
        }
	else {
		ASEC_Log.Write("ACS_ASEC_ImmHandler::handle timeout ike version is 1 or strongswan is dead",LOG_LEVEL_ERROR);
	}

        // reschedule the timer
        p_timer = ACS_ASEC_Global::instance()->reactor()->schedule_timer(this,0,ACE_Time_Value(RETRY_TIME), ACE_Time_Value(0));
        ASEC_Log.Write("ACS_ASEC_ImmHandler::handle timeout end",LOG_LEVEL_INFO);
        return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
