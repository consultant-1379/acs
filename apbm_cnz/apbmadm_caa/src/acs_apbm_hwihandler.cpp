/** @file acs_apbm_hwihandler.cpp
 *      @brief
 *      @author xlucdor
 *      @date 2011-10-20
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 *      REVISION INFO
 *      +=======+============+==============+=====================================+
 *      | REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *      +=======+============+==============+=====================================+
 *      | R-001 | 2011-10-20 | xlucdor      | Creation and first revision.        |
 *      +=======+============+==============+=====================================+
 *      |       | 2014-06-17 | xsunach      | TR HS54933                          |
 *      +=======+============+==============+=====================================+
 */

#include "acs_apbm_hwihandler.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_csreader.h"
#include "acs_apbm_serverworkingset.h"
#include <boost/algorithm/string.hpp>
/*tr HR55108 added*/
ACE_Recursive_Thread_Mutex  ACS_APBM_HWIHandler::_acs_hwi_mutex_m_map;


ACS_APBM_HWIHandler::ACS_APBM_HWIHandler(acs_apbm_serverworkingset * serverWorkingSet) {

	m_serverWorkingSet = NULL;
    if (serverWorkingSet != NULL) {
    	m_serverWorkingSet = serverWorkingSet;
    }

    m_snmpManager = NULL;
	immImplementerThread = NULL;
	m_collectionOngoing = false;
	m_stopCollection = false;
	m_openDone = false;
	m_immRunTimeCloseOK = false;
	m_immConfingCloseOK = false;
	/*TR HR55108- Added*/
	m_storeMagazineOrBoardInProgress = false;
}


ACS_APBM_HWIHandler::~ACS_APBM_HWIHandler() {
	//To close HWI use following methods :
	//closeListener();
	//closeImmConfigHandler();
	//closeImmRunTimeHandler();
}

int ACS_APBM_HWIHandler::openHwiHandler(){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	if (m_openDone) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Open already done !");
		return acs_apbm::ERR_NO_ERRORS;
	}

	if (m_serverWorkingSet != NULL) {
		if (m_serverWorkingSet->snmp_manager != NULL) {
			m_snmpManager = m_serverWorkingSet->snmp_manager;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP configured");
		} else {
			ACS_APBM_LOG(LOG_LEVEL_WARN, "SNMP is NULL !!!");
		}
	}

	int res = acs_apbm::ERR_NO_ERRORS;

	do {
		res = this->openImmConfigHandler();
		if (res) { // ERROR: cannot install the HWI configuration object implementer
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'openImmConfigHandler' failed: res == %d", res);
			break;
		}
/*PFM Config now*/
//3		res = this->openImmRunTimeHandler();
//3		if (res) { // ERROR: cannot install the HWI runtime object implementer
//3			this->closeImmConfigHandler();
//3			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'openImmRunTimeHandler' failed: res == %d", res);
//3			break;
//3		}

		res = this->openListener();
		if (res) { // ERROR:
			this->closeImmRunTimeHandler();
			this->closeImmConfigHandler();
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'openListener' failed: res == %d", res);
			break;
		}
	} while (0); //dummy while

	//Start up is completely done!
	if (res == acs_apbm::ERR_NO_ERRORS) {
		m_immRunTimeCloseOK = false;
		m_immConfingCloseOK = false;
		m_openDone = true;
	}

	return res;
}

int ACS_APBM_HWIHandler::closeHwiHandler(){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	if (! m_openDone) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HWI is already closed !");
		return acs_apbm::ERR_NO_ERRORS;
	}

	int res = acs_apbm::ERR_NO_ERRORS;

	do {
		res = this->closeListener();
		if (res) { // ERROR:
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'openListener' failed: res == %d", res);
			break;
		}

		if (! m_immConfingCloseOK) {
			res = this->closeImmConfigHandler();
			if (res) {
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'closeImmConfigHandler' failed: res == %d", res);
				break;
			} else {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "closeImmConfigHandler done !");
				m_immConfingCloseOK = true;
			}
		} //if (! m_immConfingCloseOK)
/*PFM CONFIG*/
//3		if (! m_immRunTimeCloseOK) {
//3			res = this->closeImmRunTimeHandler();
//3			if (res) {
//3				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'closeImmRunTimeHandler' failed: res == %d", res);
//3				break;
//3			} else {
//3				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "closeImmRunTimeHandler done !");
//3				m_immRunTimeCloseOK = true;
//3			}
//3		} //if (! m_immRunTimeCloseOK

	} while (0); //dummy while

	if (res == acs_apbm::ERR_NO_ERRORS)
		m_openDone = false;

	return res;
}

int ACS_APBM_HWIHandler::openListener() {

	int res = 0;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");
	//Start thread listener
	immImplementerThread = new implementerThread(&immHandlerConfigObj);

	if (immImplementerThread->activate() >= 0 )
		res = acs_apbm::ERR_NO_ERRORS;
	else
		res = acs_apbm::ERR_HWI_IMMLISTENER_FAILED;

	return res;
}

int ACS_APBM_HWIHandler::openImmRunTimeHandler() {
	//Add Runtime owner implementer on PFM class

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");
	int returnCode = ACS_CC_FAILURE;
	for (int i = 0 ; i < 3 ; i++) {
		returnCode = immHandlerRunTimeObj.init(IMPLEMENTER_NAME_PFM);
		if (returnCode == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "%s@%d - init failed, retry %d", __FUNCTION__, __LINE__, i);
                          usleep(50000);
		}
	}

	if (returnCode == ACS_CC_FAILURE) {
		returnCode = immHandlerRunTimeObj.getInternalLastError();

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "init failed, error: %d %s", returnCode, immHandlerRunTimeObj.getInternalLastErrorText());

		switch (returnCode) {
			case 4:
			case 5:
			case 6:
			case 10:
			case 11:
			case 31:
			case 39:
				returnCode = acs_apbm::ERR_HWI_IMMRT_RETRY;
				break;

			default:
				returnCode = acs_apbm::ERR_HWI_IMMRT_FAILED;
				break;
		}
	} else {
		returnCode = acs_apbm::ERR_NO_ERRORS;
	}
	return returnCode;
}

int ACS_APBM_HWIHandler::openImmConfigHandler() {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	//Set implementer
	immHandlerConfigObj.setImpName(IMPLEMENTER_NAME_CFG);
	immHandlerConfigObj.setScope(ACS_APGCC_ONE);

	std::vector<std::string> vecClassName;
	vecClassName.push_back(IMMCLASSNAME_HWISHELF);
	vecClassName.push_back(IMMCLASSNAME_BOARD);
	/*3 added to be compliant to new model PFM config -Start*/
	vecClassName.push_back(IMMCLASSNAME_PFM);
	/*3 added to be compliant to new model PFM config -End  */


	//retry loop
	ACS_CC_ReturnType returnCode = ACS_CC_FAILURE;
	for (int i = 0 ; i < 3 ; i++) {
		returnCode = oiHandler.addMultipleClassImpl(&immHandlerConfigObj, vecClassName);
		if (returnCode == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
                          usleep(500000);

		}
	}
	if (returnCode == ACS_CC_FAILURE) {
		return acs_apbm::ERR_HWI_IMMCFG_FAILED;
	}
	return acs_apbm::ERR_NO_ERRORS;
}


int ACS_APBM_HWIHandler::closeListener() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");
	if (immImplementerThread != NULL) {
		immImplementerThread->stop();
		usleep(HWI_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS * 1000 + 500000);
		delete immImplementerThread;
		immImplementerThread = NULL;
	}
	return 0;
}

int ACS_APBM_HWIHandler::closeImmRunTimeHandler() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");
	int res = ACS_CC_FAILURE;
	for (int i = 0 ; i < 3 ; i++) {
		res = immHandlerRunTimeObj.finalize();
		if (res == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
                    usleep(500000);

		}
	}
	if (res == ACS_CC_FAILURE) {
		res = immHandlerRunTimeObj.getInternalLastError();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "finalize failed, error: %d %s", res, immHandlerRunTimeObj.getInternalLastErrorText());

		switch (res) {
			case 4:
			case 5:
			case 6:
			case 10:
			case 11:
			case 31:
			case 39:
				res = acs_apbm::ERR_HWI_IMMRT_RETRY;
				break;

			default:
				res = acs_apbm::ERR_HWI_IMMRT_FAILED;
				break;
		}
	} else {
		res = acs_apbm::ERR_NO_ERRORS;
	}

	return res;
}

int ACS_APBM_HWIHandler::closeImmConfigHandler() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");
	ACS_CC_ReturnType res = ACS_CC_FAILURE;
	std::vector<std::string> vecClassName;
	vecClassName.push_back(IMMCLASSNAME_HWISHELF);
	vecClassName.push_back(IMMCLASSNAME_BOARD);
	/*3 added to be compliant to new model PFM config -Start*/
	vecClassName.push_back(IMMCLASSNAME_PFM);
	/*3 added to be compliant to new model PFM config -End  */

	//retry loop
	for (int i = 0 ; i < 3 ; i++) {
		res = oiHandler.removeMultipleClassImpl(&immHandlerConfigObj, vecClassName);
		if (res == ACS_CC_SUCCESS) {
			i = 3;
		}
		else {
                             usleep(500000);

		}
	}
	if (res == ACS_CC_FAILURE) {
		return acs_apbm::ERR_HWI_IMMCFG_FAILED;
	}
	return acs_apbm::ERR_NO_ERRORS;
}

void ACS_APBM_HWIHandler::createIMMClasses() {
	//TODO remove when model will delivery
	//Create all classes
	immHandlerConfigObj.createIMMTables();
}

int ACS_APBM_HWIHandler::createHWIRootClass() {

	OmHandler omHandler;
	ACS_CC_ReturnType res;

	res = omHandler.Init();
	if (res == ACS_CC_FAILURE) {
		std::cout << "HWIROOT INIT failure" << std::endl;
		return -1;
	}

	//char* nomeParent = const_cast<char *>(IMMROOT.c_str());
	char* nomeParent = 0; //root !

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> attrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;

	/*Fill the rdn Attribute */
//7	char attrdn[]= "hardwareInventoryId";
	char attrdn[]= "AxeHardwareInventoryhardwareInventoryMId";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;
	char* rdnValue = const_cast<char *>(IMMHWIROOT.c_str());
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);

	attrList.push_back(attrRdn);

	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_HWI.c_str()), nomeParent, attrList );
	if (res == ACS_CC_FAILURE) {
		std::cout << "HWIROOT creation failure" << std::endl;
		omHandler.Finalize();
		return -1;
	}

	omHandler.Finalize();

	return 0;
}

void ACS_APBM_HWIHandler::setSNMPManager(acs_apbm_snmpmanager *snmpManager) {

	m_snmpManager = snmpManager;
}

void ACS_APBM_HWIHandler::addMagazine(const std::string &magazineAddress,
			const std::string &egem2L2Switch0EthA,
			const std::string &egem2L2Switch0EthB,
			const std::string &egem2L2Switch25EthA,
			const std::string &egem2L2Switch25EthB) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s -- IP0 %s %s -- IP25 %s %s",
			magazineAddress.c_str(), egem2L2Switch0EthA.c_str(), egem2L2Switch0EthB.c_str(),
			egem2L2Switch25EthA.c_str(),	egem2L2Switch25EthB.c_str());

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;

	it = m_mapMagazine.find(magazineAddress);

	if (it == m_mapMagazine.end()) {
		std::cout << "ok !" << std::endl;

		ACS_APBM_HWIData* magazine = new ACS_APBM_HWIData;
		magazine->setMagName(magazineAddress);
		magazine->setEgem2L2Switch0EthA(egem2L2Switch0EthA);
		magazine->setEgem2L2Switch0EthB(egem2L2Switch0EthB);
		magazine->setEgem2L2Switch25EthA(egem2L2Switch25EthA);
		magazine->setEgem2L2Switch25EthB(egem2L2Switch25EthB);

		m_mapMagazine.insert(std::make_pair<std::string, ACS_APBM_HWIData*>(magazineAddress, magazine));
	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Magazine %s already exist", magazineAddress.c_str());
	}

}

int ACS_APBM_HWIHandler::modifyMagazineIPSwitch(const std::string &magazineAddress,
		const int switchSlot,
		const std::string &egem2L2SwitchEthA,
		const std::string &egem2L2SwitchEthB) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s -- switch %d -- IP %s %s",
				magazineAddress.c_str(), switchSlot,
				egem2L2SwitchEthA.c_str(), egem2L2SwitchEthB.c_str());

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it != m_mapMagazine.end()) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " IP " << egem2L2SwitchEthA << " " << egem2L2SwitchEthB << std::endl;

		if (switchSlot == 0) {
			it->second->setEgem2L2Switch0EthA(egem2L2SwitchEthA);
			it->second->setEgem2L2Switch0EthB(egem2L2SwitchEthB);
		}
		else {
			it->second->setEgem2L2Switch25EthA(egem2L2SwitchEthA);
			it->second->setEgem2L2Switch25EthB(egem2L2SwitchEthB);
		}
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Magazine %s not found ", magazineAddress.c_str());
	}
	return 0;
}



int ACS_APBM_HWIHandler::startCollection() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	if (m_snmpManager == NULL) {
		ACS_APBM_LOG(LOG_LEVEL_WARN, "SNMP Manager is NULL !!! ");
		return -1;
	}
	if (m_mapMagazine.size() == 0) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "No magazine configured !");
		return 0;
	}

	if (! m_collectionOngoing && !m_storeMagazineOrBoardInProgress) {	//added for TR HR5508

		/*tr HR55108 added*/
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);

		m_collectionOngoing = true;

		for (std::map<std::string, ACS_APBM_HWIData*>::iterator it = m_mapMagazine.begin();
				it != m_mapMagazine.end() ; it++) {

			if (! m_stopCollection){
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection for magazine: %s", it->first.c_str() );
				readSNMPData(it->second);
				readIMMPersistentData(it->first, it->second);
			}
			else {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "Collection interrupted on magazine %s ", it->first.c_str());
				break;
			}

		} //end for

		m_stopCollection = false;
		m_collectionOngoing = false;
	}//end if added TR HR5508
	return 0;
}


int ACS_APBM_HWIHandler::startCollectionOn(const std::string &magazineAddress,
											bool checkValidMaster) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection on magazine %s", magazineAddress.c_str() );

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it != m_mapMagazine.end()) {

		if (checkValidMaster) {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Verify master on MAG " << magazineAddress << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Check master before to proceed with collection");
			int checkRes = -1;
			for ( int i = 0 ; i < 3 ; i++) {

				it->second->getEgem2L2Switch0EthA();
				checkRes = verifyMaster(magazineAddress,
										it->second->getEgem2L2Switch0EthA(), it->second->getEgem2L2Switch0EthB(),
										it->second->getEgem2L2Switch25EthA(), it->second->getEgem2L2Switch25EthB());

				if (checkRes == 0) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master OK ! ");
					break;
				} else if (checkRes == -1) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master not ready !");
				} else if (checkRes == -2) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master not found !");
				}

				std::cout << "Verify = " << checkRes << std::endl;
			}
			if (checkRes != 0) return checkRes;
		}

		std::cout << __FUNCTION__ << "@" << __LINE__ << " Start collection for magazineAddress " << magazineAddress << std::endl;

		if (! m_collectionOngoing ) {

			m_collectionOngoing = true;

			readSNMPData(it->second);
			readIMMPersistentData(it->first, it->second);

			m_stopCollection = false;
			m_collectionOngoing = false;
		}
	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Magazine %s not found ", magazineAddress.c_str());
		//return 0;
	}

	return 0;

}

int ACS_APBM_HWIHandler::startCollectionOnBoard(const std::string &magazineAddress,
												const int &slot) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Magazine %s - Board %d", magazineAddress.c_str(), slot);

	/*tr HR55108 added*/
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);


	if (it != m_mapMagazine.end()) {


		//std::cout << __FUNCTION__ << "@" << __LINE__ << " magazineAddress found ! " << magazineAddress << std::endl;

		if (! m_collectionOngoing && !m_storeMagazineOrBoardInProgress) {


			/*tr HR55108 added*/
			m_collectionOngoing = true;

			OmHandler omHandle;

			ACS_CC_ReturnType result;

			//connect to IMM
			result = omHandle.Init();
			if ( result != ACS_CC_SUCCESS ){
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandler INIT failure ");
				cout << "ERROR: Initialization FAILURE!!!\n";
				/*TR HR55108 : ADDED the following line*/
				m_collectionOngoing = false;
				return -1;
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection");


			//readSNMPBoardData(slot, it->second);
ACS_APBM_LOG(LOG_LEVEL_ERROR, "before readSNMP ");
                                readSNMPData(it->second);
				readIMMPersistentData(it->first, it->second);
ACS_APBM_LOG(LOG_LEVEL_ERROR, "after  readSNMP ");
/*			//Store in IMM
			for (int x = 0 ; x < 3 ; x++) { //retry 3 times

				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Get info from IMM - retry: %d", x);
				int res = getBoardPosFromIMM(omHandle, magazineAddress, slot, it->second);
				std::cout << "Mag: " << magazineAddress.c_str()
						<< " - Slot: " << slot
						<< " - Result: " << res << std::endl;

				switch (res) {
					case 0:
						x = 3; //no more retries
						break;

					case -12: //NOT EXIST
						x = 3; //no more retries
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "Element not exist");
						break;

					case -10: //BUSY
						std::cout << " IMM BUSY getBoardPosFromIMM res: " << res << std::endl;
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "IMM BUSY ");
						break;

					default:
						std::cout << " getBoardPosFromIMM res: " << res << std::endl;
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "UNKNOWN res:%d", res);
						break;
				} //switch (res) {
			} //retry for (int x = 0 ; x < 3 ; x++)
*/
			/*TR HR55108 : ADDED the following line*/
			m_collectionOngoing = false;

			result = omHandle.Finalize();

		} //if (! m_collectionOngoing )
	} //if (it != m_mapMagazine.end())
	else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Magazine %s not found ", magazineAddress.c_str());
		return -1;
	}
	return 0;
}

bool ACS_APBM_HWIHandler::isCollectionOngoing() const {
	return m_collectionOngoing;
}

void ACS_APBM_HWIHandler::stopCollection() {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Stop collection requested !!!");
	if (m_collectionOngoing)
		m_stopCollection = true;
}

int ACS_APBM_HWIHandler::removeAllMagazine() {
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Removing all magazine");
	ACS_APBM_LOG(LOG_LEVEL_ERROR, "removeAllMagazine::Called");

	if (m_mapMagazine.size() == 0) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "No magazine configured !");
		return 0;
	}

	/*tr HR55108 added*/
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);

	for (std::map<std::string, ACS_APBM_HWIData*>::iterator it = m_mapMagazine.begin();
			it != m_mapMagazine.end() ; it++) {

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "delete magazine: %s ", it->first.c_str());
		delete it->second;
		//m_mapMagazine.erase(it);

	} //end for
	/*TR HR55108 here clear the map*/
	m_mapMagazine.clear();
	return 0;
}

int ACS_APBM_HWIHandler::removeMagazine(const std::string &magazineAddress){

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Remove magazine: %s ", magazineAddress.c_str());

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it != m_mapMagazine.end()) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Deletion of " << magazineAddress << std::endl;

		delete it->second;
		m_mapMagazine.erase(it);
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s not found ", magazineAddress.c_str());
	}

	return 0;
}

int ACS_APBM_HWIHandler::removeBoard(const std::string &magazineAddress,
									const int &slot) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Remove board: %d magazine: %s ", slot, magazineAddress.c_str());

	/*tr HR55108 added*/
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it != m_mapMagazine.end()) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Deletion board " << slot << " in " << magazineAddress << std::endl;

		if (it->second->removeBoardData(slot) == ACS_APBM_HWIData::DATA_SUCCESS) {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " board deleted !!! " << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Board successfully removed");
		}
	} else {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s not found ", magazineAddress.c_str());
	}

	return 0;
}

int ACS_APBM_HWIHandler::verifyMaster(const std::string &mag,
										const std::string &ip0A,
										const std::string &ip0B,
										const std::string &ip25A,
										const std::string &ip25B) {

	int res = 0;
	bool bExit = false;
	std::string ipA = ip0A;
	std::string ipB = ip0B;
	int retry = 0;
	bool checkNeighbour = false;

	while (! bExit) {
		std::string egem2L2Switch = getMaster(mag, ipA, ipB);
		if (atoi(egem2L2Switch.c_str()) == 3) { //Master
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Master ! " << std::endl;
			if (egem2L2Switch.compare("03 01 01") > 0) { //mask
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Master OK " << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master found on ip %s - %s with right configuration ", ipA.c_str(), ipB.c_str());
				bExit = true;
			} else {
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Master NOK ! " << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master found on ip %s - %s INVALID CONFIGURATION, wait before retry %d ", ipA.c_str(), ipB.c_str(), retry);
 usleep(500000);
			
	if ( (retry++) > 10) {
					bExit = true;
					res = -1;
					ACS_APBM_LOG(LOG_LEVEL_ERROR, "Time Expired, Master not ready - ip %s - %s ", ipA.c_str(), ipB.c_str());
				}
			}
		} else {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Passive ! " << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Master NOT found on ip %s - %s ", ipA.c_str(), ipB.c_str());
			if (! checkNeighbour) {
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Passive, change IP ! " << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Change IP - ip %s - %s ", ipA.c_str(), ipB.c_str());
				checkNeighbour = true;
				ipA = ip25A;
				ipB = ip25B;
				retry = 0;
			} else {
				//No Master found !
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Neighbour is passive too ! ip %s - %s ", ipA.c_str(), ipB.c_str());
				res = -2;
				bExit = true;
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Master not found !! " << std::endl;
			}
		}
	}

	return res;
}

std::string ACS_APBM_HWIHandler::getMaster(const std::string &mag,
											const std::string &ipA,
											const std::string &ipB) {
	int res = 0;
	int oid_ctrlState[] = {1,3,6,1,4,1,193,177,2,2,1,2,1,8,0};

	for (int i = 0 ; i < 3 ; i++) {

		char tmp[SNMP_MAXARRAY];
		int size = sizeof oid_ctrlState/sizeof(int);
		res = m_snmpManager->getHWIData(
							mag.c_str(), 0,
							ipA.c_str(), ipB.c_str(),
							&SNMP_TIMEOUT,
							oid_ctrlState, size,
							tmp, SNMP_MAXARRAY, true);

		if (res >= 0) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Value %s ", tmp);
			return std::string(tmp);

		} else {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "SNMP Error on ip %s - %s with right configuration ", ipA.c_str(), ipB.c_str());
			usleep(500000);
		}
	}
	return "-1";
}


int ACS_APBM_HWIHandler::readSNMPData(ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start SNMP collection");
	//std::string ipA = "";
	//std::string ipB = "";
	//int res;

	/*
	   Possible values egem2L2Switch0/egem2L2Switch25:

	   3 = Master
	   2 = PassiveDeg
	   1 = Passive
	   0 = IDLE
	   -1 = SNMP Failure
	   -2 = IPs not presents
	 */
	int egem2L2Switch0 = -2;
	int egem2L2Switch25 = -2;

	/*
	   EGEM2L2SWITCH_0_MASTER = 0,
	   EGEM2L2SWITCH_25_MASTER,
	   EGEM2L2SWITCH_0_ONLY,
	   EGEM2L2SWITCH_25_ONLY,
	   EGEM2L2SWITCH_0_25_NO_MASTER
	 */
	ACS_APBM_HWIHandler::egem2L2SwitchStateConfiguration egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_MASTER;

	//int oid_ctrlState[] = {1,3,6,1,4,1,193,177,2,2,1,2,1,8,0};

	int checkCase = 0;
	switch (checkCase) {

		case 0:
			if (hwiData->getEgem2L2Switch0EthA().size() > 0 && hwiData->getEgem2L2Switch0EthB().size() > 0 ) {

				std::string strTmp = getMaster(hwiData->getMagName().c_str(),
						hwiData->getEgem2L2Switch0EthA().c_str(),
						hwiData->getEgem2L2Switch0EthB().c_str());

				egem2L2Switch0 = atoi(strTmp.c_str());
				std::cout << "strTmp : " << strTmp << "    egem2L2Switch0 : "<< egem2L2Switch0 <<std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 - Get value: %s", strTmp.c_str());

				if (egem2L2Switch0 != 3) {
					//SNMP failure, IDLE, Passive, PassiveDeg
					checkCase = 1;
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MASTER NOT FOUND on SCX/SMX 0");
				}
				else { //Master found !
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MASTER FOUND on SCX/SMX 0");
					break;
				}
			}
			else checkCase = 1;

		case 1:
			if (hwiData->getEgem2L2Switch25EthA().size() > 0 && hwiData->getEgem2L2Switch25EthB().size() > 0 ) {
				//SCX 25 IP OK !

				std::string strTmp = getMaster(hwiData->getMagName().c_str(),
						hwiData->getEgem2L2Switch25EthB().c_str(),
						hwiData->getEgem2L2Switch25EthA().c_str());

				egem2L2Switch25 = atoi(strTmp.c_str());
				std::cout << "strTmp : " << strTmp << "    egem2L2Switch25 : "<< egem2L2Switch0 <<std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 - Get value: %s", strTmp.c_str());

				if (egem2L2Switch25 != 3) {
					//SNMP failure, IDLE, Passive, PassiveDeg
					checkCase = 2;
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MASTER NOT FOUND on SCX/SMX 25");
				}
				else { //Master found !
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MASTER FOUND on SCX/SMX 25");
					break;
				}
			}
			else checkCase = 2;

		case 2:
			//No master available;
			ACS_APBM_LOG(LOG_LEVEL_WARN, "NO master available");
			break;
	} //switch (checkCase)

	if (checkCase == 0 && egem2L2Switch0 == 3) {
		//SCX 0 is Master
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 is Master");
		egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_MASTER;
	}
	else if (checkCase == 1 && egem2L2Switch25 == 3) {
		//SCX 25 is master !
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 is Master");
		egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_25_MASTER;
	}
	else if (egem2L2Switch0 < 0 && (egem2L2Switch25 >= 0 && egem2L2Switch25 < 3)) {
		//SCX 0 is faulty/present not master/absent
		//SCX 25 is present but not Master

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 not available - SCX/SMX 25 present ");
		egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_25_ONLY;
	}
	else if (egem2L2Switch25 < 0 && ( egem2L2Switch0 >= 0 && egem2L2Switch0 < 3)) {
		//SCX 25 is faulty/present not master/absent
		//SCX 0 is present but not Master

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 present - SCX/SMX 25 not available");
		egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_ONLY;
	}
	else if ((egem2L2Switch0 >= 0 && egem2L2Switch0 <3) &&
			(egem2L2Switch25 >= 0 && egem2L2Switch25 <3) ) {

		//SCX 0 and SCX 25 are presents but NO Master configured !

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 present - SCX/SMX 25 present");
		egem2L2SwitchStateConf = ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_25_NO_MASTER;
	}
	else if ((egem2L2Switch0 < 0 && egem2L2Switch25 < 0)) {
		//NO Switch available

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "NO SWITCH BOARD AVAILABLE !!");
		std::cout << __FUNCTION__ << "@" << __LINE__ << "No switch Available!" << std::endl;

		return ACS_APBM_HWIHandler::NO_SWITCH_BOARD;
	}
	
	int env=0;
	bool isSMX = false;
        m_serverWorkingSet->cs_reader->get_environment(env);
	if(env == ACS_CS_API_CommonBasedArchitecture::SMX)
		isSMX = true;
		
	switch (egem2L2SwitchStateConf)
	{
		case  ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_MASTER:

			//collect backplane !!
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 Master, start backplane collection");
			std::cout << __FUNCTION__ << "@" << __LINE__ << " SCX_0_MASTER" << std::endl;
			this->snmpBackPlane(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), hwiData);

			//collect all board on master!!
			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				if(s == 27)
				   continue;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 Master, collect ipmi board: %d", s);
				this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), s, hwiData);
			}

			//collect MBUS on board 25
			if (hwiData->getEgem2L2Switch25EthA().size() > 0 && hwiData->getEgem2L2Switch25EthB().size() > 0 ) {
				for (int s = 13 ; s <= 25 ; s++) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 Master, collect m-bus board: %d", s);
					std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
					this->snmpBoardData(hwiData->getEgem2L2Switch25EthB(), hwiData->getEgem2L2Switch25EthA(), s, hwiData);
				}
			}
			break;
		case  ACS_APBM_HWIHandler::EGEM2L2SWITCH_25_MASTER:
			std::cout << __FUNCTION__ << "@" << __LINE__ << " SCX_25_MASTER" << std::endl;
			//collect backplane !!
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 Master, start backplane collection");
			this->snmpBackPlane(hwiData->getEgem2L2Switch25EthB(), hwiData->getEgem2L2Switch25EthA(), hwiData);

			//collect all board on master!!
			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				if(s == 27)
					continue;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 Master, collect ipmi board: %d", s);
				std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
				this->snmpBoardData(hwiData->getEgem2L2Switch25EthB(), hwiData->getEgem2L2Switch25EthA(), s, hwiData);
			}

			//collect MBUS on board 0
			if (hwiData->getEgem2L2Switch0EthA().size() > 0 && hwiData->getEgem2L2Switch0EthB().size() > 0 ) {
				for (int s = 0 ; s <= 12 ; s++) {
					ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 Master, collect m-bus board: %d", s);
					std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
					this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), s, hwiData);
				}
			}
			break;

		case  ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_ONLY :

			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				if(s == 27)
					continue;
				std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 present, collect board: %d", s);
				this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), s, hwiData);
			}
			break;

		case  ACS_APBM_HWIHandler::EGEM2L2SWITCH_25_ONLY :

			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				 if(s == 27)
				    continue;			
				std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 25 present, collect board: %d", s);
				this->snmpBoardData(hwiData->getEgem2L2Switch25EthB(), hwiData->getEgem2L2Switch25EthA(), s, hwiData);
			}

			break;

		case  ACS_APBM_HWIHandler::EGEM2L2SWITCH_0_25_NO_MASTER :

			//Try to fetch data on all SCX
			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				if(s == 27)
				   continue;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 and 25 present, collect board %d on SCX/SMX 0", s);
				std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
				this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), s, hwiData);
			}
			for (int s = 0 ; (s <= (isSMX? 25:28)) ; s++) {
				if(s == 27)
				   continue;
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SCX/SMX 0 and 25 present, collect board %d on SCX/SMX 25", s);
				std::cout << __FUNCTION__ << "@" << __LINE__ << " - Slot: " << s << std::endl;
				this->snmpBoardData(hwiData->getEgem2L2Switch25EthA(), hwiData->getEgem2L2Switch25EthB(), s, hwiData);
			}
			break;

	} //switch (egem2L2SwitchStateConf)

	//collect upper
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection on PFM Upper");
	this->snmpPFMModule(hwiData, true);

	//collect lower
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection on PFM Lower");
	this->snmpPFMModule(hwiData, false);
	//get SubRackName used for XML

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection on SubRackName");
	this->snmpSubRackName(hwiData);


	//TODO to delete !
	//DUMMY DATA !!!!
	//hwiData->printData();

	/*
	hwiData->addMagazinePositionData(55,56,12,15,"EGEM2 40");
	hwiData->addBackplaneData("EGEM240GBP", "ROJ6091001/2", "R2A", "A034105816", "Ericsson AB","20100415");

	hwiData->addPFMUpperLowerData("FAN UNIT","BFB14013/1","R5A","BN73621534","hod","1","20100204");
	hwiData->addPFMUpperLowerData("FAN UNIT","BFB14013/1","R5A","BN73621534","hod","1","20100204", false);

	hwiData->addSlotPosData(0,33,44);

	hwiData->addSlotPosData(2,55,66);

	hwiData->addBoardData(0,"SCXB", "ROJ123456/1", "R1A",
				"A270000141000" , "Ericsson", "IPMI", "20111111");

	hwiData->addBoardData(25,"SCXB", "ROJ123456/1", "R1A",
				"A270000141000" , "Ericsson", "IPMI", "20081012");

	hwiData->addBoardData(2,"CP", "ROJ123456/1", "R1A",
				"A270000141000" , "Ericsson", "IPMI", "19990101");

	hwiData->printData();
	*/

	return ACS_APBM_HWIHandler::SUCCESS;
}

void ACS_APBM_HWIHandler::readSNMPBoardData(const int &slot,
											ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Start collection on board: %d", slot);
	//ACS_APBM_LOG(LOG_LEVEL_ERROR, "readSNMPBoardData hwiData : %d", hwiData);
	if (slot >= 0 && slot <= 12) {
		//use IP slot 0
		if (this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), slot, hwiData) != 0) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "collection failed on board %d, SCX 0", slot);
			if (this->snmpBoardData(hwiData->getEgem2L2Switch25EthA(), hwiData->getEgem2L2Switch25EthB(), slot, hwiData) != 0) {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "collection failed on board %d, SCX 25", slot);
			}
		}
	}
	else {
		//use IP slot 25
		if (this->snmpBoardData(hwiData->getEgem2L2Switch25EthA(), hwiData->getEgem2L2Switch25EthB(), slot, hwiData) != 0) {
			ACS_APBM_LOG(LOG_LEVEL_INFO, "collection failed on board %d, SCX 25", slot);
			if (this->snmpBoardData(hwiData->getEgem2L2Switch0EthA(), hwiData->getEgem2L2Switch0EthB(), slot, hwiData) != 0) {
				ACS_APBM_LOG(LOG_LEVEL_INFO, "collection failed on board %d, SCX 0", slot);
			}
		}
	}
}

int ACS_APBM_HWIHandler::snmpBackPlane(
		const std::string &masterIPA,
		const std::string &masterIPB,
		ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "BackPlane collection, IP: %s %s ",
						masterIPA.c_str(), masterIPB.c_str() );

	int res = 0;
	std::string prodName = "";
	std::string prodNo = "";
	std::string prodRev = "";
	std::string prodSerialNo = "";
	std::string prodSupplier = "";
	std::string prodManufactureDate = "";


	//char tmp[SNMP_MAXARRAY];
	int size;

	for (int i = 10 ; i <= 14 ; i++) {
		char tmp[SNMP_MAXARRAY];
		int shelfOID[] = {1,3,6,1,4,1,193,177,2,2,1,2,1,i,0};
		size = sizeof shelfOID/sizeof(int);
		res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
									masterIPA.c_str(),
									masterIPB.c_str(),
									&SNMP_TIMEOUT,
									shelfOID, size,
									tmp, SNMP_MAXARRAY, false);
		if (res >= 0) {

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP i = %d - Val = %s ", i, tmp);
			std::string trimString = trimStr(std::string(tmp));
						

			switch (i) {
				case 10:
					prodNo = trimString;
					break;

				case 11:
					prodRev = trimString;
					break;

				case 12:
					prodName = std::string(tmp);
					break;

				case 13:
					prodSerialNo = trimString;
					break;

				case 14:
					prodManufactureDate = trimString;
					break;
			}
		}
	}

	prodSupplier = BACKPLANEVENDOR;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodName = %s ", prodName.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodNo = %s ", prodNo.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodRev = %s ", prodRev.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodSerialNo = %s ", prodSerialNo.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodSupplier = %s ", prodSupplier.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "prodManufactureDate = %s ", prodManufactureDate.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneName " << prodName.c_str() << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneProdNo " << prodNo.c_str() << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneProdRev " << prodRev.c_str() << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneSerialNo " << prodSerialNo.c_str() << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneProdSupplier " << prodSupplier.c_str() << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBackplaneProdManDate " << prodManufactureDate.c_str() << std::endl;

	res = hwiData->addBackplaneData(prodName,
									prodNo,
									prodRev,
									prodSerialNo,
									prodSupplier,
									prodManufactureDate);

	return res;
}

std::string ACS_APBM_HWIHandler::trimStr(const std::string &strToTrim) {

	std::cout << "strToTrim |" << strToTrim << "|" << std::endl;
	std::string tmp = "";
	for (unsigned int i = 0 ; i < strToTrim.size() ; i++) {
		if (strToTrim[i] != ' ') {
			tmp += strToTrim[i];
		}
	}
	std::cout << "tmp |" << tmp << "|" << std::endl;
	return tmp;
}


int ACS_APBM_HWIHandler::snmpPFMModuleCheck(ACS_APBM_HWIData* hwiData,
											const bool &isUpperModule){

	//result:
	//-2 = error
	//-1 = no info about pfm
	//0 = data on board 0
	//25 = data on board 25

	int res = 0;
	int size = 0;
	bool errEgem2L2Switch0 = false;
	//lower index = 0
	//upper index = 1
	int pfmIndex = isUpperModule ? 1 : 0;
	//char tmp[SNMP_MAXARRAY];

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Check PFM Collection %s ", isUpperModule ? "UPPER" : "LOWER");


	if (hwiData->getEgem2L2Switch0EthA().size() > 0 && hwiData->getEgem2L2Switch0EthB().size() > 0 ) {
		char tmp[SNMP_MAXARRAY];
		int pfmPresentsOID[] = {1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,9,pfmIndex};
		size = sizeof pfmPresentsOID/sizeof(int);
		res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
									hwiData->getEgem2L2Switch0EthA().c_str(),
									hwiData->getEgem2L2Switch0EthA().c_str(),
									&SNMP_TIMEOUT,
									pfmPresentsOID, size,
									tmp, SNMP_MAXARRAY, false);

		if (res >= 0) {
			if (atoi(tmp) == 0) {
				//check other board !
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM not present on SCX 0");
				std::cout << __FUNCTION__ << "@" << __LINE__ << " PFM Board 0 not present " << std::endl;
			}
			else {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM found on SCX 0");
				return 0;
			}
		} else {
			errEgem2L2Switch0 = true;
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM Check snmp SCX 0 error: %d", res);
			std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMP ERROR PFM Board 0" << std::endl;
		}
	}

	if (hwiData->getEgem2L2Switch25EthA().size() > 0 && hwiData->getEgem2L2Switch25EthB().size() > 0 ) {
		char tmp[SNMP_MAXARRAY];
		int pfmPresentsOID[] = {1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,9,pfmIndex};
		size = sizeof pfmPresentsOID/sizeof(int);
		res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
									hwiData->getEgem2L2Switch25EthA().c_str(),
									hwiData->getEgem2L2Switch25EthA().c_str(),
									&SNMP_TIMEOUT,
									pfmPresentsOID, size,
									tmp, SNMP_MAXARRAY, false);

		if (res >= 0) {
			if (atoi(tmp) == 0) {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM not present on SCX 25");
				std::cout << __FUNCTION__ << "@" << __LINE__ << " PFM Board 25 not present " << std::endl;
			}
			else {
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM found on SCX 25");
				return 25;
			}
		} else {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMP ERROR PFM Board 25" << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM Check snmp SCX 25 error: %d", res);
			//all SCX failed !!!
			if (errEgem2L2Switch0) return -2;
		}
	}

	return -1;
}


int ACS_APBM_HWIHandler::snmpPFMModule(
				ACS_APBM_HWIData* hwiData,
				const bool &isUpperModule) {


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM Collection %s ", isUpperModule ? "UPPER" : "LOWER");

	int res = 0;

	std::string pfmProductName = "";
	std::string pfmProductNumber = "";
	std::string pfmProductRevision = "";
	std::string pfmHwVersion = "";
	std::string pfmSerialNo = "";
	std::string pfmDeviceType = "";
	std::string pfmManufactureDate = "";
	//std::string pfmInstance;

	std::string masterIPA;
	std::string masterIPB;

	//lower index = 0
	//upper index = 1
	int pfmIndex = isUpperModule ? 1 : 0;
	int size;


	int egem2L2Switch = snmpPFMModuleCheck(hwiData, isUpperModule);

	if (egem2L2Switch == 0) {
		masterIPA = hwiData->getEgem2L2Switch0EthA();
		masterIPB = hwiData->getEgem2L2Switch0EthB();
	} else if (egem2L2Switch == 25) {
		masterIPA = hwiData->getEgem2L2Switch25EthA();
		masterIPB = hwiData->getEgem2L2Switch25EthB();
	} else if (egem2L2Switch == -1) {
		//Not found !
		res = hwiData->addPFMUpperLowerData("","","","","","","", isUpperModule);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM not found");
		return 0;
	}
	else {
		//Error
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM Error !");
		return -1;
	}


	for (int i = 2 ; i <= 7 ; i++) {
		char tmp[SNMP_MAXARRAY];
		int pfmOID[] = {1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,i,pfmIndex};
		size = sizeof pfmOID/sizeof(int);
		res = m_snmpManager->getHWIData( (hwiData->getMagName()).c_str(), 0,
									masterIPA.c_str(),
									masterIPB.c_str(),
									&SNMP_TIMEOUT,
									pfmOID, size,
									tmp, SNMP_MAXARRAY, false);
		if (res >= 0) {
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SNMP i = %d - Val = %s ", i, tmp);
			std::string trimString = trimStr(std::string(tmp));
			switch (i) {
				case 2:
					pfmHwVersion = trimString;
					break;

				case 3:
					pfmProductNumber = trimString;
					break;

				case 4:
					pfmProductRevision = trimString;
					break;

				case 5:
					pfmProductName = std::string(tmp);
					break;

				case 6:

					//jiira case AXE-2517 SerialNumber i_len=10
					if ( allPrintableChars ( tmp ,  PFM_SERIALNO_LEN ))
					{
						std::string tempVar = std::string(tmp);
						boost::algorithm::trim_right(tempVar);
						pfmSerialNo = tempVar; 
					}
					else
						ACS_APBM_LOG(LOG_LEVEL_ERROR, "ACS_APBM_HWIHandler::snmpPFMModule PFM serial Number has not printable values ");
					break;

				case 7:
					pfmManufactureDate = trimString;
					break;
			}
		}
	}

	//Device Type !
	char tmp[SNMP_MAXARRAY];

	int pfmOID[] = {1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,11,pfmIndex};
	size = sizeof pfmOID/sizeof(int);
	res = m_snmpManager->getHWIData( (hwiData->getMagName()).c_str(), 0,
								masterIPA.c_str(),
								masterIPB.c_str(),
								&SNMP_TIMEOUT,
								pfmOID, size,
								tmp, SNMP_MAXARRAY, false);

	if (res >= 0) {
		if (atoi(tmp) == 0)
			pfmDeviceType = PFM_LOD;
		else
			pfmDeviceType = PFM_HOD;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmProductName = %s ", pfmProductName.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmProductNumber = %s ", pfmProductNumber.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmProductRevision = %s ", pfmProductRevision.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmHwVersion = %s ", pfmHwVersion.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmSerialNo = %s ", pfmSerialNo.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmDeviceType = %s ", pfmDeviceType.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "pfmManufactureDate = %s ", pfmManufactureDate.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMProdName " << pfmProductName << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMProdName " << pfmProductNumber << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMProdRev " << pfmProductRevision << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMHardwareVer " << pfmHwVersion << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMSerialNo " << pfmSerialNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMDeviceType " << pfmDeviceType << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getPFMManufactureDate " << pfmManufactureDate << std::endl;

	res = hwiData->addPFMUpperLowerData(pfmProductName,
										pfmProductNumber,
										pfmProductRevision,
										pfmSerialNo,
										pfmDeviceType,
										pfmHwVersion,
										pfmManufactureDate,
										isUpperModule);

	return res;
}

int ACS_APBM_HWIHandler::snmpSubRackName(ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	int val = -1;
	int res = 0;
	int size = 0;

	if (hwiData->getEgem2L2Switch0EthA().size() > 0 && hwiData->getEgem2L2Switch0EthB().size() > 0 ) {
		char tmp[SNMP_MAXARRAY];
		int shelfNameOID[] = {1,3,6,1,4,1,193,177,2,2,1,2,1,16,0};
		size = sizeof shelfNameOID/sizeof(int);
		res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
									hwiData->getEgem2L2Switch0EthA().c_str(),
									hwiData->getEgem2L2Switch0EthA().c_str(),
									&SNMP_TIMEOUT,
									shelfNameOID, size,
									tmp, SNMP_MAXARRAY, false);

		if (res >= 0) {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " Value on Board 0: " << tmp << std::endl;
			val = atoi(tmp);
			if (val < 0) {
				val = -1;
			}
		} else {
			val = -1;
			std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMP ERROR Board 0" << std::endl;
		}
	}

	if (val == -1) { //try on board 25
		if (hwiData->getEgem2L2Switch25EthA().size() > 0 && hwiData->getEgem2L2Switch25EthB().size() > 0 ) {
			char tmp[SNMP_MAXARRAY];
			int shelfNameOID[] = {1,3,6,1,4,1,193,177,2,2,1,2,1,16,0};
			size = sizeof shelfNameOID/sizeof(int);
			res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
										hwiData->getEgem2L2Switch0EthA().c_str(),
										hwiData->getEgem2L2Switch0EthA().c_str(),
										&SNMP_TIMEOUT,
										shelfNameOID, size,
										tmp, SNMP_MAXARRAY, false);
			if (res >= 0) {
				std::cout << __FUNCTION__ << "@" << __LINE__ << " Value on Board 25: " << tmp << std::endl;
				val = atoi(tmp);
				if (val < 0) {
					val = -1;
				}
			} else {
				std::cout << __FUNCTION__ << "@" << __LINE__ << " SNMP ERROR Board 25" << std::endl;
				val = -1;
			}
		}
	} //if (errEgem2L2Switch0)

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SubRackName = %d ", val);
	switch (val) {
		case 0:
			hwiData->addSubRackName(::SHELFNAME_0);
			break;
		case 1:
			hwiData->addSubRackName(::SHELFNAME_1);
			break;
		case 2:
			hwiData->addSubRackName(::SHELFNAME_2);
			break;
		default:
			hwiData->addSubRackName(::SHELFNAME_NA);
			break;
	} //switch (val)

	return 0;
}




int ACS_APBM_HWIHandler::snmpBoardData(
						const std::string &masterIPA,
						const std::string &masterIPB,
						const int &slotNo,
						ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	std::string boardName = "";
	std::string boardProdNo = "";
	std::string boardProdRev = "";
	std::string boardSerialNo = "";
	std::string boardBusType = "";
	std::string boardSupplier = "";
	std::string boardManufactureDate = "";

	int size;

	char tmp[SNMP_MAXARRAY];
	int boardPresOID[] = {1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,19,slotNo};
	size = sizeof(boardPresOID)/sizeof(int);
	int res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
									masterIPA.c_str(),
									masterIPB.c_str(),
									&SNMP_TIMEOUT,
									boardPresOID, size,
									tmp, SNMP_MAXARRAY, false);

	if (res >= 0) {
		int present = atoi (tmp);
		std::cout << "tmp : " << tmp << " --- p " << present << std::endl;

		if (present == 1 || present==3) {
			for (int i = 2 ; i <= 8 ; i++) {
				char tmpB[SNMP_MAXARRAY];
				int boardOID[] = {1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,i,slotNo};
				size = sizeof(boardOID)/sizeof(int);

				res = m_snmpManager->getHWIData(hwiData->getMagName().c_str(), 0,
												masterIPA.c_str(),
												masterIPB.c_str(),
												&SNMP_TIMEOUT,
												boardOID, size,
												tmpB, SNMP_MAXARRAY, false);

				if (res >= 0) {
					std::string trimString = trimStr(std::string(tmpB));

					switch (i) {
						case 2:
							boardProdNo = trimString;
							break;
						case 3:
							boardProdRev = trimString;
							break;
						case 4:
							boardName = std::string(tmpB);
							break;
						case 5:
							boardSerialNo = trimString;
							break;
						case 6:
							boardManufactureDate = trimString;
							break;
						case 7:
							//jiira case AXE-2517
							if ( allPrintableChars ( tmpB, BLADE_SUPPLIER_LEN))
							{
								std::string tempVar = std::string(tmpB);
								boost::algorithm::trim_right(tempVar);
								boardSupplier = tempVar; 
							}
							else
								ACS_APBM_LOG(LOG_LEVEL_ERROR, "ACS_APBM_HWIHandler::snmpBoardData Blade Supplier has not printable values ");
							break;
						case 8:
							boardBusType = (atoi(tmpB) == 0) ? BUSTYPE_IPMI : BUSTYPE_MBUS;
							break;
					} //switch (i)
				} //if (res >= 0)
			} //for (int i = 2 ; i <= 8 ; i++)
		} //if (present == 1 || present==3) {
		else {
			std::cout << " __________________________________________________ "<< std::endl;
			std::cout << " BOARD " << slotNo << " not present" << std::endl;
			std::cout << " __________________________________________________ "<< std::endl;

			return -1;
		}
	} else {
		return -1;
	}

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardName = %s ", boardName.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardProdNo = %s ", boardProdNo.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardProdRev = %s ", boardProdRev.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardSerialNo = %s ", boardSerialNo.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardBusType = %s ", boardBusType.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardSupplier = %s ", boardSupplier.c_str());
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardManufactureDate = %s ", boardManufactureDate.c_str());


	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardName " << boardName << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardProdNo " << boardProdNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardProdRev " << boardProdRev << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardSerialNo " << boardSerialNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardBusType " << boardBusType << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardSupplier " << boardSupplier << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardManufactureDate " << boardManufactureDate << std::endl;

	hwiData->addBoardData(slotNo,
						boardName,
						boardProdNo,
						boardProdRev,
						boardSerialNo,
						boardSupplier,
						boardBusType,
						boardManufactureDate,
						true);

	//Create an entry for position list
	hwiData->addSlotPosData(slotNo,
							-1,
							-1,
							true);

	return 0;
}


int ACS_APBM_HWIHandler::readIMMPersistentData(
									const std::string &magazineAddress,
									ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin %s", magazineAddress.c_str());

	int exitCode = -1;
	OmHandler omHandle;

	ACS_CC_ReturnType result;

	//connect to IMM
	result = omHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Initialization FAILURE!!!\n";
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		return exitCode;
	}

	//Try to fetch information
	int res = 0;
	for (int x = 0 ; x < 3 ; x++) { //retry 3 times
		res = getShelfPosFromIMM(omHandle, magazineAddress, hwiData);
		switch (res) {
			case 0:
				x = 3; //no more retries
				break;

			case -12: //NOT EXIST
				x = 3; //no more retries
				ACS_APBM_LOG(LOG_LEVEL_WARN, "getShelfPosFromIMM - Entry not exist in IMM !!!");
				break;

			case -10: //BUSY
				std::cout << " IMM BUSY getShelfPosFromIMM res: " << res << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_INFO, "getShelfPosFromIMM - IMM BUSY !!!");
				break;

			default:
				std::cout << " getShelfPosFromIMM res: " << res << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_WARN, "getShelfPosFromIMM - unknown error: %d", res);
				break;
		}
	}

	if (res != -10 && res != -12) {

		if (hwiData->getBoardNumber() > 0) {

			std::vector<int> lst;
			//List will contain all board present
			hwiData->getBoardList(lst);

			//for each board found !
			for (unsigned int i = 0 ; i < lst.size() ; i++) {

				for (int x = 0 ; x < 3 ; x++) { //retry 3 times
					res = getBoardPosFromIMM(omHandle, magazineAddress, lst[i], hwiData);
					std::cout << "Mag: " << magazineAddress.c_str()
							<< " - Slot: " << lst[i]
							<< " - Result: " << res << std::endl;

					ACS_APBM_LOG(LOG_LEVEL_DEBUG, " Mag %s -- slot lst[i] %d -- Res %d",
								magazineAddress.c_str(), lst[i], res);


					switch (res) {
						case 0:
							x = 3; //no more retries
							break;

						case -12: //NOT EXIST
							x = 3; //no more retries
							ACS_APBM_LOG(LOG_LEVEL_WARN, "getBoardPosFromIMM - Entry not exist in IMM !!!");
							break;

						case -10: //BUSY
							std::cout << " IMM BUSY getBoardPosFromIMM res: " << res << std::endl;
							ACS_APBM_LOG(LOG_LEVEL_INFO, "getBoardPosFromIMM - IMM BUSY !!!");
							break;

						default:
							std::cout << " getBoardPosFromIMM res: " << res << std::endl;
							ACS_APBM_LOG(LOG_LEVEL_WARN, "getBoardPosFromIMM - unknown error: %d", res);
							break;
					} //switch (res) {
				} //retry for (int x = 0 ; x < 3 ; x++)
			} //for (unsigned int i = 0 ; i < lst.size() ; i++)
		} //if (hwiData->getBoardNumber() > 0) {
		exitCode = 0;
		/*3 added to be compliant to new model : PFM configuration - START*/
		if(hwiData->isPFMUpperPresent()){
			/*todo add retry*/
			res = getPFMPosFromIMM(omHandle, magazineAddress, true, hwiData);
		}
		if(hwiData->isPFMUpperPresent()){
			/*todo add retry*/
			res = getPFMPosFromIMM(omHandle, magazineAddress, false, hwiData);
		}
		/*3 added to be compliant to new model : PFM configuration - END */
	} //if (res != -10 && res != -12) {
	else {
		std::cout << "Impossible to retrieve information from IMM - Res: " << res << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Impossible to retrieve information from IMM - Res: %d", res);
		exitCode = -1;
	}

	result = omHandle.Finalize();
	if ( result != ACS_CC_SUCCESS ){
		cout << "ERROR: Finalization FAILURE!!!\n";
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Finalize error ");
	}

	return exitCode;
}

int ACS_APBM_HWIHandler::getShelfPosFromIMM(
		OmHandler &immHandle,
		const std::string &magazineAddress,
		ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s", magazineAddress.c_str());

	int exitCode = -1;
	ACS_CC_ReturnType result;

	std::vector<ACS_APGCC_ImmAttribute *> vecAttr;
	ACS_APGCC_ImmAttribute row;
	ACS_APGCC_ImmAttribute num;
	ACS_APGCC_ImmAttribute xPos;
	ACS_APGCC_ImmAttribute yPos;

//1row.attrName = "row";
//1num.attrName = "number";
	row.attrName = "cabinetRow";
	num.attrName = "cabinetNumber";
	xPos.attrName = "xPosition";
	yPos.attrName = "yPosition";
	vecAttr.push_back(&row);
	vecAttr.push_back(&num);
	vecAttr.push_back(&xPos);
	vecAttr.push_back(&yPos);

	//std::string distName = "shelfId=" + magazineAddress + ",hardwareInventoryId=1,safApp=safImmService";
	//1std::string distName = "shelfId=" + magazineAddress + IMMHWIPATH;
	//4std::string distName =  magazineAddress + IMMHWIPATH;              // now the DN is 1.2.0.4,hardwareInventoryId=1
	std::string distName =  "shelfInfoId="+magazineAddress + IMMHWIPATH;

	std::cout << "DN distName = " << distName.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN %s", distName.c_str());

	result = immHandle.getAttribute(const_cast<char*>(distName.c_str()), vecAttr);

	if (result == ACS_CC_SUCCESS) {

		int iRow = -1, iNum = -1, iXPos = -1, iYPos = -1;

		if (row.attrValuesNum != 0) {
//1			char *tmp = (reinterpret_cast<char*>(*(row.attrValues)));
//1			iRow = atoi(tmp);
//1			cout << " INT Row " << iRow << endl;
			/*now it is an int*/
			iRow = *(reinterpret_cast<int*>(*(row.attrValues)));

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore Shelf iRow = %d", iRow);
		}
		if (num.attrValuesNum != 0) {
//1			char *tmp = (reinterpret_cast<char*>(*(num.attrValues)));
//1			iNum = atoi(tmp);
//1			cout << " INT NUM " << iNum << endl;
			/*now it is an int*/
			iNum = *(reinterpret_cast<int*>(*(num.attrValues)));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore Shelf : iNum %d", iNum);

		}
		if (xPos.attrValuesNum != 0) {
//1			char *tmp = (reinterpret_cast<char*>(*(xPos.attrValues)));
//1			iXPos = atoi(tmp);
//1			cout << " INT XPOS " << iXPos << endl;
			/*now it is an int*/
			iXPos = *(reinterpret_cast<int*>(*(xPos.attrValues)));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore Shelf : iXPos %d", iXPos);
		}
		if (yPos.attrValuesNum != 0) {
//1			char *tmp = (reinterpret_cast<char*>(*(yPos.attrValues)));
//1			iYPos = atoi(tmp);
//1			cout << " INT YPOS " << iYPos << endl;
			/*now it is an int*/
			iYPos = *(reinterpret_cast<int*>(*(yPos.attrValues)));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore Shelf : iYPos %d", iYPos);
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MagazinePosition Row %d, Num %d, X %d, Y %d ",
															iRow, iNum, iXPos, iYPos);

		hwiData->addMagazinePositionData(iRow, iNum, iXPos, iYPos);

		exitCode = 0;
	}
	else {
		std::cout << "DN distName = " << distName.c_str() << "  NOT FOUND !!!" << std::endl;
		exitCode = immHandle.getInternalLastError();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "getAttribute error %d on DN %s", exitCode, distName.c_str());
	}
	//else = probably the class is not present

	vecAttr.clear();
	return exitCode;
}

/**3 Added to be compliant to new model PFM configuration -Start *********/
int ACS_APBM_HWIHandler::getPFMPosFromIMM(OmHandler &immHandle,
					const std::string &magazineAddress,
					const bool isUpper,
					ACS_APBM_HWIData* hwiData){

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "getPFMPosFromIMM PFM CONFIG");

	int exitCode = -1;
	ACS_CC_ReturnType result;

	std::vector<ACS_APGCC_ImmAttribute *> vecAttr;
	ACS_APGCC_ImmAttribute xPos;
	ACS_APGCC_ImmAttribute yPos;


	xPos.attrName = "xPosition";
	yPos.attrName = "yPosition";

	vecAttr.push_back(&xPos);
	vecAttr.push_back(&yPos);

	std::string instancePFM = "";
	if (isUpper)
		instancePFM = "UPPER";
	else
		instancePFM ="LOWER";



	//4std:: string distName =  "powerFanModuleId="+ instancePFM + ","+ magazineAddress + IMMHWIPATH;
	std:: string distName =  "powerFanModuleInfoId="+ instancePFM + ",shelfInfoId="+ magazineAddress + IMMHWIPATH;
	std::cout << "PFM CONFIG DN distName = " << distName.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN %s", distName.c_str());

	result = immHandle.getAttribute(const_cast<char*>(distName.c_str()), vecAttr);

	if (result == ACS_CC_SUCCESS) {

		int iXPos = -1, iYPos = -1;

		if (xPos.attrValuesNum != 0) {

			/*now it is an int*/
			iXPos = *(reinterpret_cast<int*>(*(xPos.attrValues)));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore PFM : iXPos %d", iXPos);
		}
		if (yPos.attrValuesNum != 0) {

			/*now it is an int*/
			iYPos = *(reinterpret_cast<int*>(*(yPos.attrValues)));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore PFM : iYPos %d", iYPos);
		}

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFMPosition  X = %d, Y = %d ",
															 iXPos, iYPos);

		hwiData->addPFMPositionData(isUpper, iXPos, iYPos);

		exitCode = 0;
	}
	else {
		std::cout << "DN distName = " << distName.c_str() << "  NOT FOUND !!!" << std::endl;
		exitCode = immHandle.getInternalLastError();
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "getAttribute error %d on DN %s", exitCode, distName.c_str());
	}


	vecAttr.clear();
	return exitCode;

}
/**3 Added to be compliant to new model PFM configuration -End   *********/
int ACS_APBM_HWIHandler::getBoardPosFromIMM(
		OmHandler &immHandle,
		const std::string &magazineAddress,
		const int &slot,
		ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Mag %s - slot %d", magazineAddress.c_str(), slot);

	int exitCode = -1;
	ACS_CC_ReturnType result;

	std::vector<ACS_APGCC_ImmAttribute *> vecAttr;
	ACS_APGCC_ImmAttribute xPos;
	ACS_APGCC_ImmAttribute yPos;

	xPos.attrName = "xPosition";
	yPos.attrName = "yPosition";
	vecAttr.push_back(&xPos);
	vecAttr.push_back(&yPos);

	char s[4]= {0};
	sprintf(s, "%d", slot);

	//2std::string distName =  "boardId=" + std::string(s) + ",shelfId=" + magazineAddress + IMMHWIPATH;
	//4std::string distName =  std::string(s) + "," + magazineAddress + IMMHWIPATH;
	std::string distName =  "bladeInfoId=" + std::string(s) + ",shelfInfoId=" + magazineAddress + IMMHWIPATH;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " getBoardPosFromIMM ... DN distName = " << distName.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN %s", distName.c_str());
	for (int x = 0 ; x < 3 ; x++) {

		result = immHandle.getAttribute(const_cast<char*>(distName.c_str()), vecAttr);
		if (result == ACS_CC_SUCCESS) {

			int iXPos = -1, iYPos = -1;

			if (xPos.attrValuesNum != 0) {
//2				char *tmp = (reinterpret_cast<char*>(*(xPos.attrValues)));
//2				iXPos = atoi(tmp);
//2				cout << " INT XPOS " << iXPos << endl;
				/*now it is int*/
				iXPos = *(reinterpret_cast<int*>(*(xPos.attrValues)));
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore : iXPos %d", iXPos);
			}

			if (yPos.attrValuesNum != 0) {
//2				char *tmp = (reinterpret_cast<char*>(*(yPos.attrValues)));
//2				iYPos = atoi(tmp);
//2				cout << " INT YPOS " << iYPos << endl;
				iYPos = *(reinterpret_cast<int*>(*(yPos.attrValues)));
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Valore : iYPos %d", iYPos);
			}

			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "MagazinePosition  X %d, Y %d ", iXPos, iYPos);

			hwiData->addSlotPosData(slot, iXPos, iYPos, true);

			x = 3; //no more retries !
			exitCode = 0;
		}
		else {
			exitCode = immHandle.getInternalLastError();
			if (exitCode == -12) { //NOT EXIST
				x = 3; //no more retries !
			}
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "getAttribute error %d on DN %s", exitCode, distName.c_str());
		}
	} //for (int x = 0 ; x < 3 ; x++) {

	vecAttr.clear();
	return exitCode;

}

int ACS_APBM_HWIHandler::storeAllData() {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	//Before to store all data in IMM we will drop all previous element
	if (this->removeAllMagazineFromIMM() == -1) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " deletion of all magazine failed ! " << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "deletion of all magazine failed !");
		return -1;
	}

	//************************************************************
	//Create and Store all data in IMM
	//************************************************************

	if (m_mapMagazine.size() == 0) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " Magazine list is empty !" << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Magazine list empty");
		return 0;
	}

	this->immHandlerConfigObj.setIMMOperationOnGoing(true);

	//for each magazine
	/*
	 * TR HR55108- Start
	 * this flag is set to true when a store is in progress so to avoid that
	 * a StartCollection() or StartCollectionOnBoad() could access the m_mapMagazine map and
	 * modify it while a store operation is reading the map to store data in Imm
	 *  */
	m_storeMagazineOrBoardInProgress = true;
	bool save_failed = false;
	for (std::map<std::string, ACS_APBM_HWIData*>::iterator it = m_mapMagazine.begin();
			it != m_mapMagazine.end() && !save_failed; ++it) {

		std::cout << __FUNCTION__ << "@" << __LINE__ << " Store Magazine: " << it->first << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Store Magazine: %s", it->first.c_str());
		if (this->storeIMMData(it->first, it->second))
		{
			save_failed = true;
		}

	} //end for

	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	/*TR HR55108- Added 1 line reset the flag*/
	m_storeMagazineOrBoardInProgress = false;

	return (save_failed)? -1: 0;
}


int ACS_APBM_HWIHandler::storeMagazine(const std::string &magazineAddress) {

	/*
	 * TR HR55108- Start
	 * this flag is set to true when a store is in progress so to avoid that
	 * a StartCollection() or StartCollectionOnBoad() could access the m_mapMagazine map and
	 * modify it while a store operation is reading the map to store data in Imm
	 *  */
	std::string myMagazineAddress = magazineAddress;
	m_storeMagazineOrBoardInProgress = true;
	/*TR HR55108- END*/
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", myMagazineAddress.c_str());

	//std::cout << __FUNCTION__ << "@" << __LINE__ << " Store data for magazineAddress " << myMagazineAddress << std::endl;

	/*tr HR55108 added: wait access to m_map completed*/
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(myMagazineAddress);

	if (it == m_mapMagazine.end()) {
		//std::cout << __FUNCTION__ << "@" << __LINE__ << " magazineAddress " << magazineAddress << " not found !" << std::endl;
		/*TR HR55108- Added*/
		m_storeMagazineOrBoardInProgress = false;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s not found ", myMagazineAddress.c_str());
		return -1;
	}

	//TR HR55108 if (this->removeMagazineFromIMM(it->first) == -1) {
	if (this->removeMagazineFromIMM(myMagazineAddress) == -1) {
		//std::cout << __FUNCTION__ << "@" << __LINE__ << " removeMagazineFromIMM failure !" << std::endl;
		/*TR HR55108- Added*/
		m_storeMagazineOrBoardInProgress = false;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s removal failure", myMagazineAddress.c_str());
		return -1;
	}



	this->immHandlerConfigObj.setIMMOperationOnGoing(true);
	this->storeIMMData(it->first, it->second);
	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	/*TR HR55108- Added 1 line*/
	m_storeMagazineOrBoardInProgress = false;

	return 0;
}

int ACS_APBM_HWIHandler::removeBoardFromIMM(const std::string &magazineAddress,
											const int &slot) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s - slot %d", magazineAddress.c_str(), slot);

	OmHandler omHandler;
	ACS_CC_ReturnType result;
	int res;

	result = omHandler.Init();
	if (result == ACS_CC_FAILURE) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " INIT failure" << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		return -1;
	}

	this->immHandlerConfigObj.setIMMOperationOnGoing(true);
	if (deleteBoardDataFromIMM(omHandler, magazineAddress, slot) != 0) {

		std::cout << __FUNCTION__ << "@" << __LINE__ << " remove failure" << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s, slot %d - removal failure",
											magazineAddress.c_str(), slot);
		res = -1;
	}
	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	omHandler.Finalize();

	return res;
}


int ACS_APBM_HWIHandler::storeBoard(const std::string &magazineAddress,
									const int &slot) {

	/*
	 * TR HR55108- Start
	 * this flag is set to true when a store is in progress so to avoid that
	 * a StartCollection() or StartCollectionOnBoad() could access the m_mapMagazine map and
	 * modify it while a store operation is reading the map to store data in Imm
	 *  */
	m_storeMagazineOrBoardInProgress = true;
	/*TR HR55108- END*/

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s - slot %d", magazineAddress.c_str(), slot);

	OmHandler omHandler;
	ACS_CC_ReturnType res;

	/*tr HR55108 added*/
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard_hwi_m_map_file(_acs_hwi_mutex_m_map);


	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it == m_mapMagazine.end()) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s not found ", magazineAddress.c_str());
		std::cout << __FUNCTION__ << "@" << __LINE__ << " magazineAddress " << magazineAddress << " not found !" << std::endl;
		/*TR HR55108- Start*/
		m_storeMagazineOrBoardInProgress = false;
		/*TR HR55108- END*/
		return -1;
	}

	res = omHandler.Init();
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		std::cout << __FUNCTION__ << " storeBoard INIT failure" << std::endl;
		/*TR HR55108- Start*/
		m_storeMagazineOrBoardInProgress = false;
		/*TR HR55108- END*/
		return -1;
	}


	this->immHandlerConfigObj.setIMMOperationOnGoing(true);

	deleteBoardDataFromIMM(omHandler, magazineAddress, slot);

	storeBoardDataToIMM(omHandler, magazineAddress, slot, it->second);

	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	omHandler.Finalize();

	/*TR HR55108- Start*/
	m_storeMagazineOrBoardInProgress = false;
	/*TR HR55108- END*/

	return 0;
}

int ACS_APBM_HWIHandler::removeAllMagazineFromIMM(void) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Begin");

	OmHandler omHandle;
	ACS_CC_ReturnType result;

	result = omHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		cout << "ERROR: Initialization FAILURE!!!\n";
		return -1;
	}

	this->immHandlerConfigObj.setIMMOperationOnGoing(true);

//	//**************************************************************************
//	/*
//		Delete PFM classes
//	*/
//	std::vector<std::string> vecShelf;
//	omHandle.getClassInstances(IMMCLASSNAME_PFM.c_str(), vecShelf);
//	cout << "vecShelf size " << vecShelf.size() << std::endl;
//	if(vecShelf.size() != 0) {
//		for (std::vector<std::string>::iterator it = vecShelf.begin() ; it != vecShelf.end() ; it++ ) {
//
//			std::cout << " -> delete this DN = " << (*it).c_str() << std::endl;
//			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "delete DN = %s", (*it).c_str());
////3			result = immHandlerRunTimeObj.deleteRuntimeObj((*it).c_str());
//			result = omHandle.deleteObject( const_cast<char*>((*it).c_str()));
//
//			if (result != ACS_CC_SUCCESS) {
//				ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM deleteRuntimeObj failed");
//				cout << "delete faimmHandlerRunTimeObj: " << immHandlerRunTimeObj.getInternalLastErrorText() << std::endl;
//			}
//		}
//	}
//
//	//TODO, to be delete !!!
//	//**************************************************************************
//	/*
//		Delete Board classes
//	*/
//	vecShelf.clear();
//	cout << "vecShelf size " << vecShelf.size() << std::endl;
//	result = omHandle.getClassInstances(IMMCLASSNAME_BOARD.c_str(), vecShelf);
//	if(vecShelf.size() != 0) {
//		for (std::vector<std::string>::iterator it = vecShelf.begin() ; it != vecShelf.end() ; it++ ) {
//
//			std::cout << " -> delete this DN = " << (*it).c_str() << std::endl;
//			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "delete DN = %s", (*it).c_str());
//			result = omHandle.deleteObject( const_cast<char*>((*it).c_str()));
//
//			if (result != ACS_CC_SUCCESS) {
//				ACS_APBM_LOG(LOG_LEVEL_ERROR, "BOARD omHandle.deleteObject failed");
//				cout << "omHandle: " << omHandle.getInternalLastErrorText() << std::endl;
//			}
//		}
//	}
//	else {
//		std::cout << " NO ELEMENTS !!!!!!!! " << std::endl;
//	}

	//**************************************************************************
	/*
		HWIShelf Board classes
	*/
	//std::string rootName = IMMHWIROOT + "," + IMMROOT;
	std::string rootName = IMMHWIROOT;
	std::vector<std::string> vecShelf;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "HWIShelf DN = %s", rootName.c_str());
	cout << "rootName: " << rootName << std::endl;
	vecShelf.clear();
	result = omHandle.getChildren( const_cast<char*>(rootName.c_str()) , ACS_APGCC_SUBLEVEL, &vecShelf);

	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.getChildren error ");
		//cout << "ERROR: getChildren FAILURE!!!\n";
		this->immHandlerConfigObj.setIMMOperationOnGoing(false);
		omHandle.Finalize();
		return -1;
	}

	if(vecShelf.size() != 0) {
		for (std::vector<std::string>::iterator it = vecShelf.begin() ; it != vecShelf.end() ; ++it ) {

			//std::cout << " -> delete this DN = " << (*it).c_str() << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "removeAllMagazineFromIMM DELETING ENTIRE SUBTREE ROOTED BY  DN = %s", (*it).c_str());

			//TODO enable it with new lib and remove the previous deletion code!
			//OK now the lib is available deleting one shot all subtree rooted by HardwareInventoryId=1
			result = omHandle.deleteObject( const_cast<char*>((*it).c_str()), ACS_APGCC_SUBTREE);
			//result = omHandle.deleteObject( const_cast<char*>((*it).c_str()));
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "removeAllMagazineFromIMM deleted object=%s SUCCESS", (*it).c_str());

			if (result != ACS_CC_SUCCESS) {
				//cout << "deleteObject FAILURE!!! "  << std::endl;
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "removeAllMagazineFromIMM HWISHELF omHandle.deleteObject failed");
				//cout << "omHandle: " << omHandle.getInternalLastErrorText() << std::endl;
			}
		}
	}
	else {
		//std::cout << " NO ELEMENTS !!!!!!!! " << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "removeAllMagazineFromIMM HWISHELF NO ELEMENT");
	}

	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	omHandle.Finalize();

	return 0;
}

int ACS_APBM_HWIHandler::removeMagazineFromIMM(const std::string &magazineAddress) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s", magazineAddress.c_str());
	std::cout << __FUNCTION__ << "@" << __LINE__ << " remove data for magazineAddress " << magazineAddress << std::endl;

	std::map<std::string, ACS_APBM_HWIData*>::iterator it;
	it = m_mapMagazine.find(magazineAddress);

	if (it == m_mapMagazine.end()) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "magazine %s not found ", magazineAddress.c_str());
		std::cout << __FUNCTION__ << "@" << __LINE__ << " magazineAddress " << magazineAddress << " not found !" << std::endl;
		return -1;
	}

	OmHandler omHandler;
	ACS_CC_ReturnType result;

	result = omHandler.Init();
	if (result == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		std::cout << __FUNCTION__ << " deleteBoard INIT failure" << std::endl;
		return -1;
	}

	//**************************************************************************
	/*
		Delete PFM classes
	*/
	/*to be compliant to new model PFM CONFIG Class so they are deleted when deleting Shelf
	 * using the flag ACS_APGCC_SUBLEVEL in delete Object*/
//3	deletePFMDataFromIMM(magazineAddress, true);
//3	deletePFMDataFromIMM(magazineAddress, false);

	//TODO, to be delete !!!
	//**************************************************************************
	/*
		Delete Board classes
	*/

	this->immHandlerConfigObj.setIMMOperationOnGoing(true);

	std::vector<int> boardList;

	it->second->getBoardList(boardList);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "boardList.size %d ", boardList.size());
	if (boardList.size() > 0) {
		for (unsigned int i = 0 ; i < boardList.size() ; i++) {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " board i = " << i << std::endl;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "deleting board= %d ", boardList[i]);
			deleteBoardDataFromIMM(omHandler, magazineAddress, boardList[i]);
		}
	}
	else {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " No board present" << std::endl;
	}

	//**************************************************************************
	/*
		Delete Shelf classes
	*/
	deleteShelfDataFromIMM(omHandler, magazineAddress);


	omHandler.Finalize();

	//this->storeIMMData(it->first, it->second);

	this->immHandlerConfigObj.setIMMOperationOnGoing(false);

	return 0;
}

int ACS_APBM_HWIHandler::deleteShelfDataFromIMM(OmHandler omHandler,
											const std::string &magazineAddress) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());

//1	std::string tmpDN = "shelfId=" + magazineAddress + IMMHWIPATH;
//4	std::string tmpDN =  magazineAddress + IMMHWIPATH;                       /*now the DN is 1.2.0.4,hardwareInventoryId=1*/
	std::string tmpDN = "shelfInfoId=" + magazineAddress + IMMHWIPATH;
//1	int result = omHandler.deleteObject( const_cast<char*>(tmpDN.c_str()));
	int result = omHandler.deleteObject( const_cast<char*>(tmpDN.c_str()), ACS_APGCC_SUBLEVEL);
	if (result != ACS_CC_SUCCESS) {

		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandler.deleteObject error DN = %s", tmpDN.c_str());

		return -1;
	}
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "deleting Shelf= %s ", tmpDN.c_str());
	return 0;
}

int ACS_APBM_HWIHandler::deletePFMDataFromIMM(const std::string &magazineAddress,
											const bool &isUpper) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());

	if (! isUpper) {
		std::string tmpDN = "powerFanModuleId=LOWER,shelfId=" + magazineAddress + IMMHWIPATH;
		std::cout << "DN tmpDN = " << tmpDN.c_str() << std::endl;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "LOWER DN = %s ", tmpDN.c_str());
		int result = immHandlerRunTimeObj.deleteRuntimeObj(tmpDN.c_str());
		if (result != ACS_CC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM deleteRuntimeObj error ");
			std::cout << __FUNCTION__ << "@" << __LINE__ << " immHandlerRunTimeObj: " << immHandlerRunTimeObj.getInternalLastErrorText() << std::endl;
			return -1;
		}
	}
	else {
		std::string tmpDN = "powerFanModuleId=UPPER,shelfId=" + magazineAddress + IMMHWIPATH;
		std::cout << "DN tmpDN = " << tmpDN.c_str() << std::endl;

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "UPPER DN = %s ", tmpDN.c_str());
		int result = immHandlerRunTimeObj.deleteRuntimeObj(tmpDN.c_str());
		if (result != ACS_CC_SUCCESS) {
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "PFM deleteRuntimeObj error ");
			std::cout << __FUNCTION__ << "@" << __LINE__ << " immHandlerRunTimeObj: " << immHandlerRunTimeObj.getInternalLastErrorText() << std::endl;
			return -1;
		}
	}

	return 0;
}

int ACS_APBM_HWIHandler::deleteBoardDataFromIMM(OmHandler omHandler,
												const std::string &magazineAddress,
												const int &slot) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s - slot %d ", magazineAddress.c_str(), slot);

	//TR HR55108 - START - the following line commented and statically allocated an array of char
	//max dn length is 256 char
	//char *idValue = new char();
	char idValue[257] = {0};
	//TR HR55108 - END   -

//2	sprintf(idValue, "boardId=%d", slot);
//4	sprintf(idValue, "%d", slot);
	sprintf(idValue, "bladeInfoId=%d", slot);
//2	std::string tmpDN = std::string(idValue) + ",shelfId=" + magazineAddress + IMMHWIPATH;
//4	std::string tmpDN = std::string(idValue) + "," + magazineAddress + IMMHWIPATH; /*now the DN is 2,1.2.0.4,hardwareInventoryId=1*/
	std::string tmpDN = std::string(idValue) + ",shelfInfoId=" + magazineAddress + IMMHWIPATH;

	std::cout << "DN tmpDN = " << tmpDN.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "DN = %s ", tmpDN.c_str());
//2	int result = omHandler.deleteObject(const_cast<char*>( tmpDN.c_str()));
	int result = omHandler.deleteObject(const_cast<char*>( tmpDN.c_str()), ACS_APGCC_SUBLEVEL);
	if (result != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Board deleteObject error ");
		cout << "omHandle: " << omHandler.getInternalLastErrorText() << std::endl;
		return -1;
	}
	return 0;
}

int ACS_APBM_HWIHandler::storeIMMData(const std::string &magazineAddress,
									ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " Begin "<< std::endl;

	OmHandler omHandler;
	ACS_CC_ReturnType res;

	res = omHandler.Init();
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "omHandle.Init error ");
		std::cout << __FUNCTION__ << " storeIMMData INIT failure" << std::endl;
		return -1;
	}

	//Store Shelf
	if (storeShelfDataToIMM(omHandler, magazineAddress, hwiData) != 0) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM error ");
		std::cout << __FUNCTION__ << "@" << __LINE__ << " storeShelfDataToIMM FAILED !!" << std::endl;
		omHandler.Finalize();
		return -1;
	}

	//Store PFM UPPER
	if (hwiData->isPFMUpperPresent()) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " isPFMUpperPresent !!" << std::endl;
//3		storePFMDataToIMM(magazineAddress, hwiData, true);
		if (storePFMDataToIMM(omHandler, magazineAddress, hwiData, true) != 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storePFMDataToIMM error ");
			omHandler.Finalize();
			return -1;
		}
	}

	//Store PFM LOWER
	if (hwiData->isPFMLowerPresent()) {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " isPFMLowerPresent !!" << std::endl;
//3		storePFMDataToIMM(magazineAddress, hwiData, false);
		if (storePFMDataToIMM(omHandler, magazineAddress, hwiData, false) != 0)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storePFMDataToIMM error ");
			omHandler.Finalize();
			return -1;
		}
	}

	//Store Board
	std::vector<int> boardList;
	hwiData->getBoardList(boardList);
	if (boardList.size() > 0) {
		for (unsigned int i = 0 ; i < boardList.size() ; i++) {
			std::cout << __FUNCTION__ << "@" << __LINE__ << " board i = " << i << std::endl;
			if (storeBoardDataToIMM(omHandler, magazineAddress, boardList[i] , hwiData) != 0)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeBoardDataToIMM error ");
				omHandler.Finalize();
				return -1;
			}
		}
	}
	else {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " No board present" << std::endl;
	}

	omHandler.Finalize();

	return 0;
}


int ACS_APBM_HWIHandler::storeShelfDataToIMM(OmHandler omHandler,
											const std::string &magazineAddress,
											ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());
	int err_count = 0;

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> attrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;

	ACS_CC_ValuesDefinitionType attr1;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;
	ACS_CC_ValuesDefinitionType attr5;
//1	ACS_CC_ValuesDefinitionType attr6;
//1	ACS_CC_ValuesDefinitionType attr7;
	ACS_CC_ValuesDefinitionType attr8;
	ACS_CC_ValuesDefinitionType attr9;
	ACS_CC_ValuesDefinitionType attr10;
	ACS_CC_ValuesDefinitionType attr11;


	int magRow, magNo, magXpos, magYpos;
	std::string subRackName;

	std::string prodName, prodNo, prodRev, prodSerialNo, prodSupplier, prodManufactureDate;

	hwiData->getMagazinePositionData(magRow, magNo, magXpos, magYpos, subRackName);
	hwiData->getBackplaneData(prodName, prodNo, prodRev, prodSerialNo, prodSupplier, prodManufactureDate);


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "BackplanePositionData row=%d Num=%d X=%d Y=%d ",
													magRow, magNo, magXpos, magYpos);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "BackplaneData %s - %s - %s - %s - %s - %s - %s ",
							subRackName.c_str(), prodName.c_str(), prodNo.c_str(), prodRev.c_str(),
							prodSerialNo.c_str(), prodSupplier.c_str(), prodManufactureDate.c_str());

	/*Fill the rdn Attribute */
	//1char attrdn[]= "shelfId";
	char attrdn[]= "shelfInfoId";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;

	//1std::string tmpRdn = "shelfId=" + magazineAddress;
	//4std::string tmpRdn = magazineAddress;
	std::string tmpRdn = "shelfInfoId=" + magazineAddress;
	char* rdnValue = new char[tmpRdn.size()];
	sprintf(rdnValue, "%s", tmpRdn.c_str());
	std::cout << __FUNCTION__ << "@" << __LINE__ << " rdnValue: " << rdnValue << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "rdnValue %s ", rdnValue);

	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	reinterpret_cast<void*>(rdnValue);



	//1char name_attr1[]= "row";
	char name_attr1[]= "cabinetRow";
	attr1.attrName = name_attr1;
	//1attr1.attrType = ATTR_STRINGT;
	attr1.attrType = ATTR_INT32T;
	if (magRow >= 0) {
		attr1.attrValuesNum = 1;
		/*1
		char *strVal1 = new char();
		sprintf(strVal1, "%d", magRow);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal1: " << strVal1 << std::endl;
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*>(strVal1);
		*/
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*>(&magRow);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magRow in IMM  %d ", magRow);
	}
	else {
		attr1.attrValuesNum = 0;
		attr1.attrValues = NULL;
	}


	//1char name_attr2[]= "number";
	char name_attr2[]= "cabinetNumber";
	attr2.attrName = name_attr2;
	//1attr2.attrType = ATTR_STRINGT;
	attr2.attrType = ATTR_INT32T;
	if (magNo >= 0) {
		attr2.attrValuesNum = 1;
		/*1
		char *strVal2 = new char[2];
		sprintf(strVal2, "%d", magNo);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal2: " << strVal2 << std::endl;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] =	reinterpret_cast<void*>(strVal2);
		*/
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magNo in IMM  %d ", magNo);
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] =	reinterpret_cast<void*>(&magNo);

	}
	else {
		attr2.attrValuesNum = 0;
		attr2.attrValues = NULL;
	}


	char name_attr3[]= "xPosition";
	attr3.attrName = name_attr3;
	//1attr3.attrType = ATTR_STRINGT;
	attr3.attrType = ATTR_INT32T;
	if (magXpos >=0) {
		attr3.attrValuesNum = 1;
		/*1
		char *strVal3 = new char[2];
		sprintf(strVal3, "%d", magXpos);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal3: " << strVal3 << std::endl;
		attr3.attrValues = new void*[1];
		attr3.attrValues[0] =	reinterpret_cast<void*>(strVal3);
		*/
		attr3.attrValues = new void*[1];
		attr3.attrValues[0] =	reinterpret_cast<void*>(&magXpos);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magXpos in IMM  %d ", magXpos);
	}
	else {
		attr3.attrValuesNum = 0;
		attr3.attrValues = NULL;
	}


	char name_attr4[]= "yPosition";
	attr4.attrName = name_attr4;
	//1attr4.attrType = ATTR_STRINGT;
	attr4.attrType = ATTR_INT32T;
	if (magYpos >= 0) {
		attr4.attrValuesNum = 1;
		/*1
		char *strVal4 = new char[2];
		sprintf(strVal4, "%d", magYpos);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal1: " << strVal4 << std::endl;
		attr4.attrValues = new void*[1];
		attr4.attrValues[0] =	reinterpret_cast<void*>(strVal4);
		*/
		attr4.attrValues = new void*[1];
		attr4.attrValues[0] =	reinterpret_cast<void*>(&magYpos);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magYpos in IMM  %d ", magYpos);
	}
	else {
		attr4.attrValuesNum = 0;
		attr4.attrValues = NULL;
	}

/*1*/
//	char name_attr5[]= "productName";
//	attr5.attrName = name_attr5;
//	attr5.attrType = ATTR_STRINGT;
//	attr5.attrValuesNum = 0;
//	//attr5.attrValues = new void*[1];
//	//attr5.attrValues[0] = 0;
//	attr5.attrValues = NULL;
//
//	char name_attr6[]= "productNumber";
//	attr6.attrName = name_attr6;
//	attr6.attrType = ATTR_STRINGT;
//	attr6.attrValuesNum = 0;
//	//attr6.attrValues = new void*[1];
//	//attr6.attrValues[0] = 0;
//	attr6.attrValues = NULL;
//
//	char name_attr7[]= "productRevision";
//	attr7.attrName = name_attr7;
//	attr7.attrType = ATTR_STRINGT;
//	attr7.attrValuesNum = 0;
//	//attr7.attrValues = new void*[1];
//	//attr7.attrValues[0] = 0;
//	attr7.attrValues = NULL;

	//manage productIdentity
	char name_attr5[]= "productIdentity";
	attr5.attrName = name_attr5;
	attr5.attrType = ATTR_NAMET;
	attr5.attrValuesNum = 0;
	attr5.attrValues = NULL;

	char name_attr8[]= "serialNumber";
	attr8.attrName = name_attr8;
	attr8.attrType = ATTR_STRINGT;
	attr8.attrValuesNum = 0;
	//attr8.attrValues = new void*[1];
	//attr8.attrValues[0] = 0;
	attr8.attrValues = NULL;

	char name_attr9[]= "supplier";
	attr9.attrName = name_attr9;
	attr9.attrType = ATTR_STRINGT;
	attr9.attrValuesNum = 0;
	//attr9.attrValues = new void*[1];
	//attr9.attrValues[0] = 0;
	attr9.attrValues = NULL;

	char name_attr10[]= "manufacturingDate";
	attr10.attrName = name_attr10;
	attr10.attrType = ATTR_STRINGT;
	attr10.attrValuesNum = 0;
	//attr10.attrValues = new void*[1];
	//attr10.attrValues[0] = 0;
	attr10.attrValues = NULL;

	char name_attr11[]= "name";
	attr11.attrName = name_attr11;
	attr11.attrType = ATTR_STRINGT;
	attr11.attrValuesNum = 0;
	//attr11.attrValues = new void*[1];
	//attr11.attrValues[0] = 0;
	attr11.attrValues = NULL;

	attrList.push_back(attrRdn);
	attrList.push_back(attr1);
	attrList.push_back(attr2);
	attrList.push_back(attr3);
	attrList.push_back(attr4);
	/**/
	attrList.push_back(attr5);
//1	attrList.push_back(attr6);
//1	attrList.push_back(attr7);
	attrList.push_back(attr8);
	attrList.push_back(attr9);
	attrList.push_back(attr10);
	attrList.push_back(attr11);
	/**/

	std::string parentName = IMMHWIROOT;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Config DN %s ", parentName.c_str());

	ACS_CC_ReturnType res;
	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_HWISHELF.c_str()),
			const_cast<char *>(parentName.c_str()), attrList );

	if (res == ACS_CC_FAILURE) {
		std::cout << "Shelf creation failure on:" << parentName.c_str() << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CreateObject Failure on DN %s ", parentName.c_str());
		return -1;
	}

	//__________________
	//set runtime values

	//1std::string tmpDN = "shelfId=" + magazineAddress + IMMHWIPATH;
	//4std::string tmpDN =  magazineAddress + IMMHWIPATH;
	std::string tmpDN = "shelfInfoId=" + magazineAddress + IMMHWIPATH;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Runtime DN %s ", tmpDN.c_str());
	std::cout << "Shelf Runtime DN = " << tmpDN.c_str() << std::endl;

	ACS_CC_ImmParameter rtAttr5;
	ACS_CC_ImmParameter rtAttr6;
	ACS_CC_ImmParameter rtAttr7;
	ACS_CC_ImmParameter rtAttr8;
	ACS_CC_ImmParameter rtAttr9;
	ACS_CC_ImmParameter rtAttr10;
	ACS_CC_ImmParameter rtAttr11;

	/*1data stored inproductIdentity*/
//	if (prodName.size() > 0) {
//		rtAttr5.attrName = name_attr5;
//		rtAttr5.attrType = ATTR_STRINGT;
//		rtAttr5.attrValuesNum = 1;
//		rtAttr5.attrValues = new void*[1];
//		rtAttr5.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodName.c_str()));
//
//		immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr5);
//	}
//
//	if (prodNo.size() > 0) {
//		rtAttr6.attrName = name_attr6;
//		rtAttr6.attrType = ATTR_STRINGT;
//		rtAttr6.attrValuesNum = 1;
//		rtAttr6.attrValues = new void*[1];
//		rtAttr6.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodNo.c_str()));
//
//		immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr6);
//	}
//
//	if (prodRev.size() > 0) {
//		rtAttr7.attrName = name_attr7;
//		rtAttr7.attrType = ATTR_STRINGT;
//		rtAttr7.attrValuesNum = 1;
//		rtAttr7.attrValues = new void*[1];
//		rtAttr7.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodRev.c_str()));
//
//		immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr7);
//	}

	if (prodSerialNo.size() > 0) {
		rtAttr8.attrName = name_attr8;
		rtAttr8.attrType = ATTR_STRINGT;
		rtAttr8.attrValuesNum = 1;
		rtAttr8.attrValues = new void*[1];
		rtAttr8.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodSerialNo.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr8) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr8.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (prodSupplier.size() > 0) {
		rtAttr9.attrName = name_attr9;
		rtAttr9.attrType = ATTR_STRINGT;
		rtAttr9.attrValuesNum = 1;
		rtAttr9.attrValues = new void*[1];
		rtAttr9.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodSupplier.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr9) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr9.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (prodManufactureDate.size() > 0) {
		rtAttr10.attrName = name_attr10;
		rtAttr10.attrType = ATTR_STRINGT;
		rtAttr10.attrValuesNum = 1;
		rtAttr10.attrValues = new void*[1];
		rtAttr10.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (prodManufactureDate.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr10) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr10.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (subRackName.size() > 0) {
		rtAttr11.attrName = name_attr11;
		rtAttr11.attrType = ATTR_STRINGT;
		rtAttr11.attrValuesNum = 1;
		rtAttr11.attrValues = new void*[1];
		rtAttr11.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (subRackName.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr11) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr11.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}
	/*todo
	 * manage productIdentity object creation
	 * if product name, number revision are present create the object then set the related DN in attribute
	 * productIdentity attr5
	 */
	int result;
	std::string dnProdIdentityObject = "";
	result = storeProductIdentityDataToIMM( omHandler, tmpDN, prodName, prodNo, prodRev, dnProdIdentityObject );
	if (result == 0)
	{
		//on succes: the object has been created successfully : add the DN else the productIdentity is empty
		rtAttr5.attrName = name_attr5;
		rtAttr5.attrType = ATTR_NAMET;
		rtAttr5.attrValuesNum = 1;
		rtAttr5.attrValues = new void*[1];
		rtAttr5.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (dnProdIdentityObject.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr5) != ACS_CC_SUCCESS){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM : setting ProductIdentity DN FAILED error code=%d error message ", omHandler.getInternalLastError(), omHandler.getInternalLastErrorText());
			err_count++;
		}
	}
	else
	{
		err_count++;
	}
	/*end manage product Identity object*/

	return err_count;
}


int ACS_APBM_HWIHandler::storePFMDataToIMM(const std::string &magazineAddress,
											ACS_APBM_HWIData* hwiData,
											const bool &isUpper) {


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());

	ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attr1;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;
	ACS_CC_ValuesDefinitionType attr5;
	ACS_CC_ValuesDefinitionType attr6;
	ACS_CC_ValuesDefinitionType attr7;


	std::string pfmProductName;
	std::string pfmProductNumber;
	std::string pfmProductRevision;
	std::string pfmHwVersion;
	std::string pfmSerialNo;
	std::string pfmDeviceType;
	std::string pfmManufactureDate;


	std::string pfmInstance;

	if (isUpper) {
		pfmInstance = "UPPER";
	}
	else {
		pfmInstance = "LOWER";
	}

	hwiData->getPFMUpperLowerData(pfmProductName,
							pfmProductNumber,
							pfmProductRevision,
							pfmHwVersion,
							pfmSerialNo,
							pfmDeviceType,
							pfmManufactureDate,
							isUpper);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM %s - %s - %s - %s - %s - %s - %s - %s ",
				pfmInstance.c_str(), pfmProductName.c_str(), pfmProductNumber.c_str(),
				pfmProductRevision.c_str(), pfmHwVersion.c_str(), pfmSerialNo.c_str(),
				pfmDeviceType.c_str(), pfmManufactureDate.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductName: " << pfmProductName << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductNumber: " << pfmProductNumber << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductRevision: " << pfmProductRevision << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmHwVersion: " << pfmHwVersion << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmSerialNo: " << pfmSerialNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmDeviceType: " << pfmDeviceType << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmManufactureDate: " << pfmManufactureDate << std::endl;

	char attrdn[]= "powerFanModuleId";
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;

	//std::string tmpRdn = "powerFanModuleId=" + pfmInstance;
	std::string tmpRdn = "powerFanModuleInfoId=" + pfmInstance;

	char* rdnValue = new char[tmpRdn.size()];
	sprintf(rdnValue, "%s", tmpRdn.c_str());
	attributeRDN.attrValues = new void*[1];
	attributeRDN.attrValues[0] = reinterpret_cast<void*>(rdnValue);


	/*Fill the attribute ATTRIBUTE_1*/
	char name_attr1[]= "productName";
	attr1.attrName = name_attr1;
	attr1.attrType = ATTR_STRINGT;
	attr1.attrValuesNum = 1;
	attr1.attrValues = new void*[1];
	attr1.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductName.c_str()));

	char name_attr2[]= "productNumber";
	attr2.attrName = name_attr2;
	attr2.attrType = ATTR_STRINGT;
	attr2.attrValuesNum = 1;
	attr2.attrValues = new void*[1];
	attr2.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductNumber.c_str()));

	char name_attr3[]= "productRevision";
	attr3.attrName = name_attr3;
	attr3.attrType = ATTR_STRINGT;
	attr3.attrValuesNum = 1;
	attr3.attrValues = new void*[1];
	attr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductRevision.c_str()));

	char name_attr4[]= "productSerialNumber";
	attr4.attrName = name_attr4;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 1;
	attr4.attrValues = new void*[1];
	attr4.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmSerialNo.c_str()));

	char name_attr5[]= "deviceType";
	attr5.attrName = name_attr5;
	attr5.attrType = ATTR_STRINGT;
	attr5.attrValuesNum = 1;
	attr5.attrValues = new void*[1];
	attr5.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmDeviceType.c_str()));

	char name_attr6[]= "hardwareVersion";
	attr6.attrName = name_attr6;
	attr6.attrType = ATTR_STRINGT;
	attr6.attrValuesNum = 1;
	attr6.attrValues = new void*[1];
	attr6.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmHwVersion.c_str()));

	char name_attr7[]= "manufacturingDate";
	attr7.attrName = name_attr7;
	attr7.attrType = ATTR_STRINGT;
	attr7.attrValuesNum = 1;
	attr7.attrValues = new void*[1];
	attr7.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmManufactureDate.c_str()));


	AttrList.push_back(attributeRDN);
	AttrList.push_back(attr1);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);
	AttrList.push_back(attr5);
	AttrList.push_back(attr6);
	AttrList.push_back(attr7);

	std::string tmpDN = "shelfId=" + magazineAddress + IMMHWIPATH;
	std::cout << "PFM Runtime DN = " << tmpDN.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM Runtime DN %s ", tmpDN.c_str());

	returnCode = immHandlerRunTimeObj.createRuntimeObj(IMMCLASSNAME_PFM.c_str(), tmpDN.c_str(), AttrList);

	if(returnCode != ACS_CC_SUCCESS) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "createRuntimeObj FAILED on DN = %s ", tmpDN.c_str());
		std::cout << __FUNCTION__ << "@" << __LINE__ << " createRuntimeObj FAILED !!!" << std::endl;
		return -1;
	}

	return 0;
}
/*added to be compliant new model       */
int ACS_APBM_HWIHandler::storePFMDataToIMM( OmHandler omHandler,
											const std::string &magazineAddress,
											ACS_APBM_HWIData* hwiData,
											const bool &isUpper) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "-------------storePFMDataToIMM new method ");
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s ", magazineAddress.c_str());

	int err_count = 0;
	//ACS_CC_ReturnType returnCode;

	/*The list of attributes*/
	vector<ACS_CC_ValuesDefinitionType> AttrList;

	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attr1;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
	ACS_CC_ValuesDefinitionType attr4;
	ACS_CC_ValuesDefinitionType attr5;
	ACS_CC_ValuesDefinitionType attr6;
	ACS_CC_ValuesDefinitionType attr7;
	/*X Y POS management*/
	ACS_CC_ValuesDefinitionType attr8;
	ACS_CC_ValuesDefinitionType attr9;
	int pfmXpos, pfmYpos;


	std::string pfmProductName;
	std::string pfmProductNumber;
	std::string pfmProductRevision;
	std::string pfmHwVersion;
	std::string pfmSerialNo;
	std::string pfmDeviceType;
	std::string pfmManufactureDate;


	std::string pfmInstance;

	if (isUpper) {
		pfmInstance = "UPPER";
	}
	else {
		pfmInstance = "LOWER";
	}

	hwiData->getPFMUpperLowerData(pfmProductName,
							pfmProductNumber,
							pfmProductRevision,
							pfmHwVersion,
							pfmSerialNo,
							pfmDeviceType,
							pfmManufactureDate,
							isUpper);
	/*added to be compliant to new model*/
	hwiData->getPFMPositionData( isUpper, pfmXpos, pfmYpos);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "----- PFMPositionData  pfmXpos=%d pfmYpos=%d ",
			pfmXpos, pfmYpos);

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM %s - %s - %s - %s - %s - %s - %s - %s ",
				pfmInstance.c_str(), pfmProductName.c_str(), pfmProductNumber.c_str(),
				pfmProductRevision.c_str(), pfmHwVersion.c_str(), pfmSerialNo.c_str(),
				pfmDeviceType.c_str(), pfmManufactureDate.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductName: " << pfmProductName << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductNumber: " << pfmProductNumber << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmProductRevision: " << pfmProductRevision << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmHwVersion: " << pfmHwVersion << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmSerialNo: " << pfmSerialNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmDeviceType: " << pfmDeviceType << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " pfmManufactureDate: " << pfmManufactureDate << std::endl;

//	char attrdn[]= "powerFanModuleId";
	char attrdn[]= "powerFanModuleInfoId";
	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;

	//4std::string tmpRdn = "powerFanModuleId=" + pfmInstance;
	std::string tmpRdn = "powerFanModuleInfoId=" + pfmInstance;

	char* rdnValue = new char[tmpRdn.size()];
	sprintf(rdnValue, "%s", tmpRdn.c_str());
	attributeRDN.attrValues = new void*[1];
	attributeRDN.attrValues[0] = reinterpret_cast<void*>(rdnValue);

	/*************************************************************************************************************/
	/*                        X Y Position management - start                                                    */
	/*************************************************************************************************************/
	char name_attr8[]= "xPosition";
	attr8.attrName = name_attr8;
	//2attr1.attrType = ATTR_STRINGT;
	attr8.attrType = ATTR_INT32T;
	if (pfmXpos >= 0) {
		attr8.attrValuesNum = 1;
		attr8.attrValues = new void*[1];
		attr8.attrValues[0] = reinterpret_cast<void*>(&pfmXpos);
	}
	else {
		attr8.attrValuesNum = 0;
		attr8.attrValues = NULL;
	}

	char name_attr9[]= "yPosition";
	attr9.attrName = name_attr9;

	attr9.attrType = ATTR_INT32T;
	if (pfmYpos >=0) {
		attr9.attrValuesNum = 1;
		attr9.attrValues = new void*[1];
		attr9.attrValues[0] = reinterpret_cast<void*>(&pfmYpos);
	}
	else {
		attr9.attrValuesNum = 0;
		attr9.attrValues = NULL;
	}
	/*************************************************************************************************************/
	/*                        X Y Position management - end                                                    */
	/**************************************************************************************************************/


	/*Fill the attribute ATTRIBUTE_1*/
	char name_attr1[]= "productName";
	attr1.attrName = name_attr1;
	attr1.attrType = ATTR_STRINGT;
	attr1.attrValuesNum = 0;
	attr1.attrValues = NULL;
//3	attr1.attrValuesNum = 1;
//3	attr1.attrValues = new void*[1];
//3	attr1.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductName.c_str()));

	char name_attr2[]= "productNumber";
	attr2.attrName = name_attr2;
	attr2.attrType = ATTR_STRINGT;
	attr2.attrValuesNum = 0;
	attr2.attrValues = NULL;
//3	attr2.attrValuesNum = 1;
//3	attr2.attrValues = new void*[1];
//3	attr2.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductNumber.c_str()));

	char name_attr3[]= "productRevision";
	attr3.attrName = name_attr3;
	attr3.attrType = ATTR_STRINGT;
	attr3.attrValuesNum = 0;
	attr3.attrValues = NULL;
//3	attr3.attrValuesNum = 1;
//3	attr3.attrValues = new void*[1];
//3	attr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductRevision.c_str()));

	char name_attr4[]= "productSerialNumber";
	attr4.attrName = name_attr4;
	attr4.attrType = ATTR_STRINGT;
	attr4.attrValuesNum = 0;
	attr4.attrValues = NULL;
//3	attr4.attrValuesNum = 1;
//3	attr4.attrValues = new void*[1];
//3	attr4.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmSerialNo.c_str()));

	char name_attr5[]= "deviceType";
	attr5.attrName = name_attr5;
//3	attr5.attrType = ATTR_STRINGT;
	attr5.attrType = ATTR_INT32T;    /*now it is an enum in IMM*/
	attr5.attrValuesNum = 0;
	attr5.attrValues = NULL;
//3	attr5.attrValuesNum = 1;
//3	attr5.attrValues = new void*[1];
//3	attr5.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmDeviceType.c_str()));

	char name_attr6[]= "hardwareVersion";
	attr6.attrName = name_attr6;
	attr6.attrType = ATTR_STRINGT;
	attr6.attrValuesNum = 0;
	attr6.attrValues = NULL;
//3	attr6.attrValuesNum = 1;
//3	attr6.attrValues = new void*[1];
//3	attr6.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmHwVersion.c_str()));

	char name_attr7[]= "manufacturingDate";
	attr7.attrName = name_attr7;
	attr7.attrType = ATTR_STRINGT;
	attr7.attrValuesNum = 0;
	attr7.attrValues = NULL;
//3	attr7.attrValuesNum = 1;
//3	attr7.attrValues = new void*[1];
//3	attr7.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmManufactureDate.c_str()));


	AttrList.push_back(attributeRDN);
	AttrList.push_back(attr1);
	AttrList.push_back(attr2);
	AttrList.push_back(attr3);
	AttrList.push_back(attr4);
	AttrList.push_back(attr5);
	AttrList.push_back(attr6);
	AttrList.push_back(attr7);

	/*3Added to be compliant to new model : X Y position amnagement*/
	AttrList.push_back(attr8);
	AttrList.push_back(attr9);


	//4std::string parentName = magazineAddress + IMMHWIPATH;
	std::string parentName = "shelfInfoId=" + magazineAddress + IMMHWIPATH;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Config PFM parent DN %s ", parentName.c_str());

	ACS_CC_ReturnType res;
	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_PFM.c_str()),
			const_cast<char *>(parentName.c_str()), AttrList );

	if (res == ACS_CC_FAILURE) {
		std::cout << "Shelf creation failure on:" << parentName.c_str() << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "CreateObject PRM config Failure on Parent DN %s ", parentName.c_str());
		return -1;
	}


//3	std::string tmpDN = "shelfId=" + magazineAddress + IMMHWIPATH;
//3	std::cout << "PFM Runtime DN = " << tmpDN.c_str() << std::endl;
//3	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM Runtime DN %s ", tmpDN.c_str());
//3
//3	returnCode = immHandlerRunTimeObj.createRuntimeObj(IMMCLASSNAME_PFM.c_str(), tmpDN.c_str(), AttrList);
//3
//3	if(returnCode != ACS_CC_SUCCESS) {
//3		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "createRuntimeObj FAILED on DN = %s ", tmpDN.c_str());
//3		std::cout << __FUNCTION__ << "@" << __LINE__ << " createRuntimeObj FAILED !!!" << std::endl;
//3		return -1;
//3	}

	/******************************************************************************************/
	/*3Added to manage PFM configuration object -Start                                         */
	/******************************************************************************************/
	//__________________
	//set runtime values

	//4std::string tmpDN = "powerFanModuleId=" + pfmInstance + "," + parentName;
	std::string tmpDN = "powerFanModuleInfoId=" + pfmInstance + "," + parentName;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PFM CONFIG DN = %s ", tmpDN.c_str());
	std::cout << "Shelf Runtime DN = " << tmpDN.c_str() << std::endl;

	ACS_CC_ImmParameter rtAttr1; //pfmProductName
	ACS_CC_ImmParameter rtAttr2; //pfmProductNumber
	ACS_CC_ImmParameter rtAttr3; //pfmProductRevision
	ACS_CC_ImmParameter rtAttr4; //pfmSerialNo
	ACS_CC_ImmParameter rtAttr5; //pfmDeviceType.c_str
	ACS_CC_ImmParameter rtAttr6; //pfmHwVersion.c_str
	ACS_CC_ImmParameter rtAttr7; //pfmManufactureDate.c_str

	if (pfmProductName.size() > 0) {
		rtAttr1.attrName = name_attr1;
		rtAttr1.attrType = ATTR_STRINGT;
		rtAttr1.attrValuesNum = 1;
		rtAttr1.attrValues = new void*[1];
		rtAttr1.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductName.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr1)  != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr1.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (pfmProductNumber.size() > 0) {
		rtAttr2.attrName = name_attr2;
		rtAttr2.attrType = ATTR_STRINGT;
		rtAttr2.attrValuesNum = 1;
		rtAttr2.attrValues = new void*[1];
		rtAttr2.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductNumber.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr2)  != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr2.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (pfmProductNumber.size() > 0) {
		rtAttr3.attrName = name_attr3;
		rtAttr3.attrType = ATTR_STRINGT;
		rtAttr3.attrValuesNum = 1;
		rtAttr3.attrValues = new void*[1];
		rtAttr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmProductRevision.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr3) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr3.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (pfmSerialNo.size() > 0) {
		rtAttr4.attrName = name_attr4;
		rtAttr4.attrType = ATTR_STRINGT;
		rtAttr4.attrValuesNum = 1;
		rtAttr4.attrValues = new void*[1];
		rtAttr4.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmSerialNo.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr4)  != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr4.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (pfmDeviceType.size() > 0) {
		rtAttr5.attrName = name_attr5;
		rtAttr5.attrType = ATTR_INT32T;
		rtAttr5.attrValuesNum = 1;


		int pfmEnumDeviceType = -1; /*initial value not valid*/
		if ( pfmDeviceType.compare(PFM_HOD) == 0 ){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PPFM CONFIG device type is hod ");
			pfmEnumDeviceType = 1;

		}else if ( pfmDeviceType.compare(PFM_LOD) == 0 ){
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "PPFM CONFIG device type is lod ");
			pfmEnumDeviceType = 0;
		}


		if (pfmEnumDeviceType !=-1 ){ // otherwise the default null value is mantained
			rtAttr5.attrValues = new void*[1];
			rtAttr5.attrValues[0] = reinterpret_cast<void*>(&pfmEnumDeviceType);
			if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr5)  != ACS_CC_SUCCESS)
			{
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr5.attrName ,
						immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
				err_count++;
			}

		}

	}

	if (pfmHwVersion.size() > 0) {
		rtAttr6.attrName = name_attr6;
		rtAttr6.attrType = ATTR_STRINGT;
		rtAttr6.attrValuesNum = 1;
		rtAttr6.attrValues = new void*[1];
		rtAttr6.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmHwVersion.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr6) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr6.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	if (pfmManufactureDate.size() > 0) {
		rtAttr7.attrName = name_attr7;
		rtAttr7.attrType = ATTR_STRINGT;
		rtAttr7.attrValuesNum = 1;
		rtAttr7.attrValues = new void*[1];
		rtAttr7.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (pfmManufactureDate.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(tmpDN.c_str(), &rtAttr7) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr7.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	/******************************************************************************************/
	/*Added to manage PFM configuration object -END                                         */
	/******************************************************************************************/

	return 0;
}
/*
 * the following method is intended to create a product identity objet for shelves or boards.
 * the parent name is provided as input and all information required.
 * As output the DN of the productIdentity object is provided
 */
int ACS_APBM_HWIHandler::storeProductIdentityDataToIMM(OmHandler omHandler,
						const std::string &parentName,
						const std::string &productDesignation,
						const std::string &productNumber,
						const std::string &productRevision,
						std::string &dnProductIdentityObject
						){
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM :parent name = %s", parentName.c_str());

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> attrList;

	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	ACS_CC_ValuesDefinitionType attr1; //designation
	ACS_CC_ValuesDefinitionType attr2; //number
	ACS_CC_ValuesDefinitionType attr3;//revision

	/*Fill the rdn Attribute */
	char attrdn[]= "id";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;
	char rdnValue[] = "id=prodId"; //TR_HU53781   //since each shelf or blade has just one productIdentity object rooted to it the rdn is 1
	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	rdnValue;


	/*productDesignation*/
	char name_attr1[]= "productDesignation";
	attr1.attrName = name_attr1;
	attr1.attrType = ATTR_STRINGT;
	if (productDesignation.size() > 0) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productDesignation = %s", productDesignation.c_str());
		attr1.attrValuesNum = 1;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " productDesignation: " << productDesignation << std::endl;
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (productDesignation.c_str()));

	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productDesignation = EMPTY");
		/* TR HQ23444: Start
		 * The following Two lines commented for TR  */
//		attr1.attrValuesNum = 0;
//		attr1.attrValues = NULL;
		/*when Product number is empty the related value in IMM is set to " " since product identity parameters are mandatory*/
		attr1.attrValuesNum = 1;
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (" "));
		/*TR HQ23444: End */
	}

	/*productNumber*/
	char name_attr2[]= "productNumber";
	attr2.attrName = name_attr2;
	attr2.attrType = ATTR_STRINGT;
	if (productNumber.size() > 0) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productNumber = %s", productNumber.c_str());
		attr2.attrValuesNum = 1;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " productNumber: " << productNumber << std::endl;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (productNumber.c_str()));

	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productNumber = EMPTY");
		/* TR HQ23444: Start
		 * The following Two lines commented for TR  */
//		attr2.attrValuesNum = 0;
//		attr2.attrValues = NULL;
		/*when Product number is empty the related value in IMM is set to " " since product identity parameters are mandatory*/
		attr2.attrValuesNum = 1;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (" "));
		/*TR HQ23444: End */
	}

	/*productRevision*/
	char name_attr3[]= "productRevision";
	attr3.attrName = name_attr3;
	attr3.attrType = ATTR_STRINGT;
	if (productRevision.size() > 0) {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productRevision = %s", productRevision.c_str());
		attr3.attrValuesNum = 1;
		std::cout << __FUNCTION__ << "@" << __LINE__ << " productRevision: " << productRevision << std::endl;
		attr3.attrValues = new void*[1];
		attr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (productRevision.c_str()));

	}
	else {
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  productRevision = EMPTY");
		/* TR HQ23444: Start
		 * The following Two lines commented for TR  */
//		attr3.attrValuesNum = 0;
//		attr3.attrValues = NULL;
		/*when Product number is empty the related value in IMM is set to " " since product identity parameters are mandatory*/
		attr3.attrValuesNum = 1;
		attr3.attrValues = new void*[1];
		attr3.attrValues[0] = reinterpret_cast<void*> (const_cast<char*> (" "));
		/*TR HQ23444: End */
	}

	attrList.push_back(attrRdn);
	attrList.push_back(attr1);
	attrList.push_back(attr2);
	attrList.push_back(attr3);

	/*Create the object*/

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM Creating object, parentName = %s ", parentName.c_str());

	ACS_CC_ReturnType res;
	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_PRODUCTIDENTITY.c_str()), parentName.c_str(), attrList );
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeProductIdentityDataToIMM createObject FAILURE on DN = %s ", parentName.c_str());
		std::cout << "Shelf creation failure" << std::endl;
		return -1;
	}

	dnProductIdentityObject = "id=prodId,"+parentName;  //TR_HU53781
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "storeProductIdentityDataToIMM -  dnProductIdentityObject = %s",dnProductIdentityObject.c_str());
	return 0;

}
/*end added to be compliant to new model*/
int ACS_APBM_HWIHandler::storeBoardDataToIMM(OmHandler omHandler,
											const std::string &magazineAddress,
											const int &slot,
											ACS_APBM_HWIData* hwiData) {

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "magazine %s - slot %d", magazineAddress.c_str(), slot);

	/*The vector of attributes*/
	vector<ACS_CC_ValuesDefinitionType> attrList;
	/*the attributes*/
	ACS_CC_ValuesDefinitionType attrRdn;
	int err_count = 0;

	ACS_CC_ValuesDefinitionType attr1;
	ACS_CC_ValuesDefinitionType attr2;
	ACS_CC_ValuesDefinitionType attr3;
//2	ACS_CC_ValuesDefinitionType attr4;
//2	ACS_CC_ValuesDefinitionType attr5;
	ACS_CC_ValuesDefinitionType attr6;
	ACS_CC_ValuesDefinitionType attr7;
	ACS_CC_ValuesDefinitionType attr8;
	ACS_CC_ValuesDefinitionType attr9;

	int xPos, yPos;

	std::string boardName, boardNo, boardRev, boardSerialNo,
				boardSupplier, boardBusType, boardManufactureDate;


	if (hwiData->getBoardData(slot, boardName, boardNo, boardRev, boardSerialNo,
					boardSupplier, boardBusType, boardManufactureDate)
					!= ACS_APBM_HWIData::DATA_NOT_PRESENT) {

		hwiData->getSlotPosData(slot, xPos, yPos);
	}
	else {
		std::cout << __FUNCTION__ << "@" << __LINE__ << " board " << slot << " not present " << std::endl;
		ACS_APBM_LOG(LOG_LEVEL_WARN, "Slot %d not present ! ", slot);
		return 0;
	}


	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SlotPositionData slot:%d - X=%d Y=%d ", slot, xPos, yPos);
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "SlotData %d - %s - %s - %s - %s - %s - %s - %s ",
								slot, boardName.c_str(), boardNo.c_str(), boardRev.c_str(),
								boardSerialNo.c_str(), boardSupplier.c_str(), boardBusType.c_str(),
								boardManufactureDate.c_str());

	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardName: " << boardName << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardNo: " << boardNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardRev: " << boardRev << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardSerialNo: " << boardSerialNo << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardSupplier: " << boardSupplier << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardBusType: " << boardBusType << std::endl;
	std::cout << __FUNCTION__ << "@" << __LINE__ << " boardManufactureDate: " << boardManufactureDate << std::endl;


	/*Fill the rdn Attribute */
	//2char attrdn[]= "boardId";
	char attrdn[]= "bladeInfoId";
	attrRdn.attrName = attrdn;
	attrRdn.attrType = ATTR_STRINGT;
	attrRdn.attrValuesNum = 1;

	/* TR HR55108 -START - the following line substituted by the next one. Statically allocated memory for a DN */
	//char *rdnValue = new char();
	char rdnValue[257] = {0};
	/* TR HR55108 - END - */

	//2sprintf(rdnValue, "boardId=%d", slot);
	//4sprintf(rdnValue, "%d", slot);
	sprintf(rdnValue, "bladeInfoId=%d", slot);

	attrRdn.attrValues = new void*[1];
	attrRdn.attrValues[0] =	rdnValue;


	char name_attr1[]= "xPosition";
	attr1.attrName = name_attr1;
	//2attr1.attrType = ATTR_STRINGT;
	attr1.attrType = ATTR_INT32T;
	if (xPos >= 0) {
		attr1.attrValuesNum = 1;
		/*2
		char *strVal1 = new char();
		sprintf(strVal1, "%d", xPos);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal1: " << strVal1 << std::endl;
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*>(strVal1);
		*/
		attr1.attrValues = new void*[1];
		attr1.attrValues[0] = reinterpret_cast<void*>(&xPos);
	}
	else {
		attr1.attrValuesNum = 0;
		attr1.attrValues = NULL;
	}

	char name_attr2[]= "yPosition";
	attr2.attrName = name_attr2;
	//2attr2.attrType = ATTR_STRINGT;
	attr2.attrType = ATTR_INT32T;
	if (yPos >=0) {
		attr2.attrValuesNum = 1;
		/*
		char *strVal2 = new char();
		sprintf(strVal2, "%d", yPos);
		std::cout << __FUNCTION__ << "@" << __LINE__ << " strVal2: " << strVal2 << std::endl;
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*>(strVal2);
		*/
		attr2.attrValues = new void*[1];
		attr2.attrValues[0] = reinterpret_cast<void*>(&yPos);
	}
	else {
		attr2.attrValuesNum = 0;
		attr2.attrValues = NULL;
	}

	/*2 substituted by productIdentity*/
//	char name_attr3[]= "productName";
//	attr3.attrName = name_attr3;
//	attr3.attrType = ATTR_STRINGT;
//	attr3.attrValuesNum = 0;
//	//attr3.attrValues = new void*[1];
//	//attr3.attrValues[0] = 0;
//	attr3.attrValues = NULL;
//
//	char name_attr4[]= "productNumber";
//	attr4.attrName = name_attr4;
//	attr4.attrType = ATTR_STRINGT;
//	attr4.attrValuesNum = 0;
//	//attr4.attrValues = new void*[1];
//	//attr4.attrValues[0] = 0;
//	attr4.attrValues = NULL;
//
//	char name_attr5[]= "productRevision";
//	attr5.attrName = name_attr5;
//	attr5.attrType = ATTR_STRINGT;
//	attr5.attrValuesNum = 0;
//	//attr5.attrValues = new void*[1];
//	//attr5.attrValues[0] = 0;
//	attr5.attrValues = NULL;

	//manage productIdentity
	char name_attr3[]= "productIdentity";
	attr3.attrName = name_attr3;
	attr3.attrType = ATTR_NAMET;
	attr3.attrValuesNum = 0;
	attr3.attrValues = NULL;

	char name_attr6[]= "serialNumber";
	attr6.attrName = name_attr6;
	attr6.attrType = ATTR_STRINGT;
	attr6.attrValuesNum = 0;
	//attr6.attrValues = new void*[1];
	//attr6.attrValues[0] = 0;
	attr6.attrValues = NULL;

	char name_attr7[]= "supplier";
	attr7.attrName = name_attr7;
	attr7.attrType = ATTR_STRINGT;
	attr7.attrValuesNum = 0;
	//attr7.attrValues = new void*[1];
	//attr7.attrValues[0] = 0;
	attr7.attrValues = NULL;

	char name_attr8[]= "busType";
	attr8.attrName = name_attr8;
	attr8.attrType = ATTR_STRINGT;
	attr8.attrValuesNum = 0;
	//attr8.attrValues = new void*[1];
	//attr8.attrValues[0] = 0;
	attr8.attrValues = NULL;

	char name_attr9[]= "manufacturingDate";
	attr9.attrName = name_attr9;
	attr9.attrType = ATTR_STRINGT;
	attr9.attrValuesNum = 0;
	//attr9.attrValues = new void*[1];
	//attr9.attrValues[0] = 0;
	attr9.attrValues = NULL;

	attrList.push_back(attrRdn);
	attrList.push_back(attr1);
	attrList.push_back(attr2);

	/**/
	attrList.push_back(attr3);
//2	attrList.push_back(attr4);
//2	attrList.push_back(attr5);
	attrList.push_back(attr6);
	attrList.push_back(attr7);
	attrList.push_back(attr8);
	attrList.push_back(attr9);
	/**/

	//2std::string parentName = "shelfId=" + magazineAddress + IMMHWIPATH;
	//4std::string parentName =  magazineAddress + IMMHWIPATH;
	std::string parentName = "shelfInfoId=" + magazineAddress + IMMHWIPATH;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Config DN = %s ", parentName.c_str());


	ACS_CC_ReturnType res;
	res = omHandler.createObject(const_cast<char *> (IMMCLASSNAME_BOARD.c_str()), parentName.c_str(), attrList );
	if (res == ACS_CC_FAILURE) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "createObject FAILURE on DN = %s ", parentName.c_str());
		std::cout << "Shelf creation failure" << std::endl;
		return -1;
	}

	//__________________
	//set runtime values

	parentName.clear();
	//parentName =  std::string(rdnValue) + ",shelfId=" + magazineAddress + IMMHWIPATH;
	//4parentName =  std::string(rdnValue) + "," + magazineAddress + IMMHWIPATH;
	parentName =  std::string(rdnValue) + ",shelfInfoId=" + magazineAddress + IMMHWIPATH;
	std::cout << " new parentName = " << parentName.c_str() << std::endl;
	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Runtime DN = %s ", parentName.c_str());

	ACS_CC_ImmParameter rtAttr3;
	ACS_CC_ImmParameter rtAttr4;
	ACS_CC_ImmParameter rtAttr5;
	ACS_CC_ImmParameter rtAttr6;
	ACS_CC_ImmParameter rtAttr7;
	ACS_CC_ImmParameter rtAttr8;
	ACS_CC_ImmParameter rtAttr9;

/*2 substituted by productIdentity object*/
//	rtAttr3.attrName = name_attr3;
//	rtAttr3.attrType = ATTR_STRINGT;
//	rtAttr3.attrValuesNum = 1;
//	rtAttr3.attrValues = new void*[1];
//	rtAttr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardName.c_str()));
//
//	immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr3);
//
//	rtAttr4.attrName = name_attr4;
//	rtAttr4.attrType = ATTR_STRINGT;
//	rtAttr4.attrValuesNum = 1;
//	rtAttr4.attrValues = new void*[1];
//	rtAttr4.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardNo.c_str()));
//
//	immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr4);
//
//	rtAttr5.attrName = name_attr5;
//	rtAttr5.attrType = ATTR_STRINGT;
//	rtAttr5.attrValuesNum = 1;
//	rtAttr5.attrValues = new void*[1];
//	rtAttr5.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardRev.c_str()));
//
//	immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr5);

	rtAttr6.attrName = name_attr6;
	rtAttr6.attrType = ATTR_STRINGT;
	rtAttr6.attrValuesNum = 1;
	rtAttr6.attrValues = new void*[1];
	rtAttr6.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardSerialNo.c_str()));

	if (immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr6) != ACS_CC_SUCCESS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr6.attrName ,
				immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
		err_count++;
	}

	rtAttr7.attrName = name_attr7;
	rtAttr7.attrType = ATTR_STRINGT;
	rtAttr7.attrValuesNum = 1;
	rtAttr7.attrValues = new void*[1];
	rtAttr7.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardSupplier.c_str()));

	if (immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr7) != ACS_CC_SUCCESS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr7.attrName ,
				immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
		err_count++;
	}

	rtAttr8.attrName = name_attr8;
	//2rtAttr8.attrType = ATTR_STRINGT;
	rtAttr8.attrType = ATTR_INT32T;/*new type*/

	int busTypeEnumValue = -1;                              /*initial value*/
	if ( boardBusType.compare(BUSTYPE_IPMI) == 0)
		busTypeEnumValue = BUSTYPE_IPMI_ENUM;               /*busType is enumerated in IMM */
	else if (boardBusType.compare(BUSTYPE_MBUS) == 0)
		busTypeEnumValue = BUSTYPE_MBUS_ENUM;               /*busType is enumerated in IMM */

	if (busTypeEnumValue != -1)
	{
		/*only if busType value was retrieved it is inserted in IMM*/
		rtAttr8.attrValuesNum = 1;
		rtAttr8.attrValues = new void*[1];
		//rtAttr8.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardBusType.c_str()));
		rtAttr8.attrValues[0] = reinterpret_cast<void*>( &busTypeEnumValue );

		if (immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr8) != ACS_CC_SUCCESS)
		{
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr8.attrName ,
					immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
			err_count++;
		}
	}

	rtAttr9.attrName = name_attr9;
	rtAttr9.attrType = ATTR_STRINGT;
	rtAttr9.attrValuesNum = 1;
	rtAttr9.attrValues = new void*[1];
	rtAttr9.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (boardManufactureDate.c_str()));

	if (immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr9) != ACS_CC_SUCCESS)
	{
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM: setting %s FAILED error code=%d error message %s", rtAttr9.attrName ,
				immHandlerConfigObj.getInternalLastError(), immHandlerConfigObj.getInternalLastErrorText());
		err_count++;
	}

/*todo manage productIdentity object creation and related attributr update attr3 */
	int result;
	std::string dnProdIdentityObject = "";
	result = storeProductIdentityDataToIMM( omHandler, parentName, boardName, boardNo, boardRev, dnProdIdentityObject );
	if (result == 0){
		//on succes: the object has been created successfully : add the DN else the productIdentity is empty
		rtAttr3.attrName = name_attr3;
		rtAttr3.attrType = ATTR_NAMET;
		rtAttr3.attrValuesNum = 1;
		rtAttr3.attrValues = new void*[1];
		rtAttr3.attrValues[0] = reinterpret_cast<void*>( const_cast<char*> (dnProdIdentityObject.c_str()));

		if (immHandlerConfigObj.modifyRuntimeObj(parentName.c_str(), &rtAttr3) != ACS_CC_SUCCESS){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, "storeShelfDataToIMM : setting ProductIdentity DN FAILED error code=%d error message ", omHandler.getInternalLastError(), omHandler.getInternalLastErrorText());
			err_count++;
		}
	}
	else
	{
		err_count++;
	}
	/*end manage product Identity object*/

	return err_count;
}

bool ACS_APBM_HWIHandler::allPrintableChars( char* vectToCheck, int i_len ){

	bool ret = true;
	int index = 0;

	if (i_len > 15)
		return false; /*returned string from snmp get are 15 char at most*/


	/* go throughout the vector checking if each single char ASCII code is in [32,126] interval or 0 NUL char
	 * when a non printable char is found exit from the while
	 *   */
	while ( index < 15 && (( vectToCheck[index] > 31 && vectToCheck[index]< 127) || vectToCheck[index] == 0) )
		index++;


	if (index == 14)		/* all the vector has been checked and all printable char found : return true*/
		return ret;

	if ( index >= i_len ){	/* found non printable char in a position greater than i_len change the character not printable with NUL return true */
		vectToCheck[index]= 0;
		return ret;
	}else{ /*found non printable char in the first i_len char return false*/
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "allPrintableChars returning false: found not printable char at position  %d ", index );
		ret =false;
	}

	return ret;
}
