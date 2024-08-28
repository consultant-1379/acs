/*
 * ACS_CS_SNMPManager.h
 *
 *  Created on: Nov 17, 2011
 *      Author: teiscud
 */

#ifndef ACS_CS_SNMPMANAGER_H_
#define ACS_CS_SNMPMANAGER_H_


#include <ace/Task.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/mib_api.h>
#include <net-snmp/library/mib.h>

#include <string.h>
#include <sstream>
#include <iostream>

//#include "FIXS_CCH_SNMPTrapReceiver.h"

#include "ACS_TRA_trace.h"

#include "ACS_CS_Registry.h"
//#include "FIXS_CCH_Util.h"
//#include "FIXS_CCH_Event.h"

class ACS_CS_SNMPManager //: public ACE_Task_Base
{

public:

	static const int GETSET_SNMP_OK = 0;
	static const int GETSET_SNMP_OK_LEN_ZERO = 1;
	static const int OPEN_SESS_INIT_ERROR = 2;
	static const int OPEN_SESS_ERROR = 3;
	static const int GETSET_SNMP_PARSE_OID_ERROR = 4;
	static const int GETSET_SNMP_PDU_CREATE_ERROR = 5;
	static const int GETSET_SNMP_ADD_NULL_VAR = 6;
	static const int GETSET_SNMP_RESPONSE_ERROR = 7;
	static const int GETSET_SNMP_STATUS_ERROR = 8;
	static const int GETSET_SNMP_STATUS_TIMEOUT = 9;
	static const int GETSET_SNMP_BUFFERSMALL = 10;

	enum {
		LOADRELEASE = 1,
		STARTRELEASE = 2,
		MAKEDEFAULT = 3,
		REMOVERELEASE = 4
	};

	enum {
		NOATTRIBUTE = 0,
		CURRENTSW = 1,
		DEFAULTSW = 2,
		CURRENTDEFAULTSW = 3
	};

	enum {
		FAILSAFE = 1,
		EXCHANGEBLE = 2,
		PROGRAM = 3
	};

	enum {
		SNMPGET_STD_STRING = 0,
		SNMPGET_HEX_STRING = 1,
		SNMPGET_ASCII_STRING = 2
	};


	ACS_CS_SNMPManager(std::string ipAddress);
	~ACS_CS_SNMPManager();

	/* enable this methods if you want an SNMP Thread


	virtual int close (u_long flags = 0);

	int initialize(void);
	bool isRunning(void);
	*/
	// bool isCBAenvironment(void);
	// bool startTrapReceiver (void);
	// bool stopTrapReceiver (void);
	std::string getIpAddress(void);
	void setIpAddress(std::string ip);

	int snmpGet(const char* community, const char* peername,const char* usrOID, int type, void* out, unsigned int iBufSize, int strType = 0);

	int snmpSet(const char* community, const char* peername, const char* usrOID,  char type, const char *value);
//	bool queryBoardPresencePower(int type, std::string &oidShelfpresence);
//	int getBoardPower (std::string shelfMgrIp,int slot);
//	int getBoardPresence (std::string shelfMgrIp,int slot);

	/************************************************************************************/
	//SCX
	// bool setSWMAction (unsigned int action);
	// bool setSWMActionVersion (std::string version);
	// bool setSWMActionURL (std::string url);
	// bool getSwmActionStatus (int &actStatus);

	//Not used
	// bool getSwmLoadedVersion (unsigned int index, std::string &loadVer);
	//bool getSwmLoadedType (unsigned int index, int *loadType);
	//bool getSwmLoadedAttribute (unsigned int index, int *loadAttr);
	//bool getSwmLoadedAttributeIndex (unsigned int attribute, int *kIndex);

	// bool getCurrentLoadedVersion (std::string &loadVer);
	// bool checkLoadedVersion (std::string loadVer);
	// bool resetSwitch (unsigned int slot);
	// bool getCurrentDefaultVersion (std::string &loadVer);

	/************************************************************************************/
	//IPMI
	// bool getIpmiUpgStatus (std::string slot, int &input);
	// bool setIpmiUpg (std::string slot, std::string valueToSet);
	bool isMaster ();
	int isMasterSCX (std::string ip);
	int isMasterSCB(std::string ip, int slot);
	// bool getIpmiUpgStatus (std::string slot, std::string &input);
	// bool getIPMIBoardData(std::string slot, std::string &running, std::string &product, std::string &revision);
	bool isPassiveDeg ();

	/************************************************************************************/
	//PFM
	// bool setCurrentPfmScanningState (int scanningState);
	// bool setPfmFWUpgrade (std::string fwUpgrade, std::string typeIndex, std::string typeInstance);
	// bool getPfmFWUpgrade (std::string *installedFWUpgrade, int typeIndex, int typeInstance);
	// bool getPfmFWProductNumber (std::string *installedFWProductNumber, int typeIndex, int typeInstance);
	// bool getPfmFWProductRevisionState (std::string *installedFWProductRevisionState, int typeIndex, int typeInstance);
	// bool getPfmFWUpgradeStatus (int *actFwStatus, int typeIndex, int typeInstance);
	// bool getPfmModuleDeviceType (unsigned int index, std::string *ModuleType);

	//Non utilizzata
	//bool getCurrentPfmScanningState (std::string *scanningState);


	//Could be delete
	//Check utilizer and modify the code !
	//bool sendSetSNMPpfm (smiVALUE &smival, smiOID &xshmcName, smiUINT32 xshmcVector[], int typeIndex, int typeInstance);
	//bool sendGetSNMPpfm (smiOID &xshmcName, smiUINT32 xshmcVector[], int typeIndex, int typeInstance);

	//Check who use it !
	// bool isWaiting (); //to be remove


	/************************************************************************************/
	//IPT
//	bool getXshmcGPRData (std::string *xshmcGPRDataValue, int slot, int typeInstance);
//	bool setXshmcGPRData (std::string xshmcGPRDataVal, std::string slot, std::string typeInstance);
//	bool setXshmcBoardReset (unsigned int action, unsigned int index);
//	bool getXshmcBoardPwrOff (unsigned int index, int* pwrOffVal);
//	bool getXshmcBoardPwrOn (unsigned int index, int *pwrOnVal);
//	bool setXshmcBoardPwrOn (unsigned int action, unsigned int index);
//	bool setXshmcBoardPwrOff (unsigned int action, unsigned int index);
//	bool getGprDataValue (std::string *gprDataVal, int slot, int typeInstance);
//	bool setGprDataValue (unsigned char *gprDataVal, std::string slot, std::string typeInstance);
//	bool setBladeReset (unsigned int action, unsigned int index);
//	bool isBladePower (unsigned int index, int *pwrVal);

	//Not used
	//bool getXshmcBoardReset (unsigned int index, int *resetVer);
	//bool getMasterSCB (unsigned int index, int *resetVer);

	//Could be delete
	//Check utilizer and modify the code !
	//bool sendGetSNMPGPR (smiOID &xshmcName, smiUINT32 xshmcVector[], int slot, int typeInstance);
	//bool sendSetSNMPGPR (smiVALUE &smival, smiOID &xshmcName, smiUINT32 xshmcVector[], int slot, int typeInstance);


	/************************************************************************************/


private:

	bool bExit;
	bool bRun;
	std::string ipAddr;

	//CCH_Util::EnvironmentType m_cba_environment;

};

#endif /* ACS_CS_SNMPMANAGER_H_ */
