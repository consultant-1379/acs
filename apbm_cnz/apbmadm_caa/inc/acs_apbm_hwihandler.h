/*
 * ACS_APBM_HWIHandler.h
 *
 *  Created on: Oct 19, 2011
 *      Author: xlucdor
 */

#ifndef ACS_APBM_HWIHANDLER_H_
#define ACS_APBM_HWIHANDLER_H_


#include "acs_apbm_hwidata.h"
#include "acs_apbm_hwiimmconfobjhandler.h"
#include "acs_apbm_hwiimmruntimeobjhandler.h"
#include <acs_apgcc_oihandler_V2.h>
#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_macrosconstants.h"
#include "acs_apbm_serverworkingset.h"

#include <ace/Task.h>
#include <sys/poll.h>

#define HWI_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS  2000  /* milliseconds */

class implementerThread : public ACE_Task_Base {
public :



	/**Constructor**/
	implementerThread(ACS_APBM_HWIImmConfObjHandler *immHandler){

		implementer = immHandler;
		isStop = false;
	}

	virtual ~implementerThread() {}

	/**This method is used to stop the thread**/
	void stop(){
		isStop = true;
	}

	/** Thread loop **/
	int svc(void){

		int ret;
		struct pollfd fds[1];
		fds[0].fd = implementer->getSelObj();
		fds[0].events = POLLIN;

		while(!isStop) {

			ret = poll(fds, 1, HWI_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS);
			if (ret == 0) {
				//cout<<"timeout off"<<endl;
			}else if (ret == -1){
				//std::cout << " POLL ERROR !" << std::endl;
			}else{
				//std::cout << "Dispatch to IMM Implementer " << std::endl;
				implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
			}
		}
		return 0;
	}

private:

	ACS_APBM_HWIImmConfObjHandler *implementer;
	bool isStop;

};

/*end define thread*/



/*

//----------------------------------------------------------------------------------------
//SHELF NAME
smiUINT32 hwiShelfType[]					=	{1,3,6,1,4,1,193,177,2,2,1,2,1,16};

//----------------------------------------------------------------------------------------
//Backplane data

//SNMP LEAVES +0
smiUINT32 hwiBackPlaneProductNumber[] 		=	{1,3,6,1,4,1,193,177,2,2,1,2,1,10};
smiUINT32 hwiBackPlaneProductRevState[] 	=	{1,3,6,1,4,1,193,177,2,2,1,2,1,11};
smiUINT32 hwiBackPlaneProductName[] 		=	{1,3,6,1,4,1,193,177,2,2,1,2,1,12};
smiUINT32 hwiBackPlaneSerialNumber[] 		=	{1,3,6,1,4,1,193,177,2,2,1,2,1,13};
smiUINT32 hwiBackPlaneManifacturingDate[] 	=	{1,3,6,1,4,1,193,177,2,2,1,2,1,14};

//----------------------------------------------------------------------------------------
//PFM

// Add .0 to read LOWER information ............. Add .1 for UPPER
smiUINT32 HwiPfmModulePresence[]			=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,9};

smiUINT32 HwiPfmModuleHwVersion[]			=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,2};
smiUINT32 HwiPfmModuleProductNumber[]		=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,3};
smiUINT32 HwiPfmModuleProductRevState[]		=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,4};
smiUINT32 HwiPfmModuleProductName[]			=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,5};
smiUINT32 HwiPfmModuleSerialNumber[]		=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,6};
smiUINT32 HwiPfmModuleManufactoringDate[]	=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,7};

smiUINT32 HwiPfmModuleDevicetype[]			=	{1,3,6,1,4,1,193,177,2,2,6,2,1,1,1,11};


//----------------------------------------------------------------------------------------
//BOARD

smiUINT32 hwiBoardPresence[]	 			=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,19};
smiUINT32 hwiBoardProductNumber[] 			=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,2};
smiUINT32 hwiBoardProductRevisionState[] 	=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,3};
smiUINT32 hwiBoardProductName[] 			=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,4};
smiUINT32 hwiBoardSerialNumber[] 			=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,5};
smiUINT32 hwiBoardManifacturingDate[] 		=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,6};
smiUINT32 hwiBoardSupplier[]				=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,7};
smiUINT32 hwiBusType[]						=	{1,3,6,1,4,1,193,177,2,2,1,3,1,1,1,8};

*/

const unsigned SNMP_TIMEOUT = 2500;
const int SNMP_MAXARRAY = 15;
const std::string BACKPLANEVENDOR = "Ericsson AB";
const std::string SHELFNAME_0 = "EGEM";
const std::string SHELFNAME_1 = "EGEM2 40";
const std::string SHELFNAME_2 = "EGEM2 10";
const std::string SHELFNAME_NA = "NOT AVAILABLE";
const std::string PFM_HOD = "hod";
const std::string PFM_LOD = "lod";
const std::string BUSTYPE_IPMI = "ipmi";
const std::string BUSTYPE_MBUS = "mbus";

const int BUSTYPE_MBUS_ENUM = 2;   /*busType is enumerated in IMM  MBUS = 2*/
const int BUSTYPE_IPMI_ENUM = 1;   /*busType is enumerated in IMM  IPMI = 1*/


const std::string IMMROOT = "safApp=safImmService";

//7 const std::string IMMHWIROOT = "hardwareInventoryId=1";
//modified for Gerardo changes
const std::string IMMHWIROOT = "AxeHardwareInventoryhardwareInventoryMId=1";
//const std::string IMMHWIPATH = ",hardwareInventoryId=1,safApp=safImmService";
//7 const std::string IMMHWIPATH = ",hardwareInventoryId=1";
const std::string IMMHWIPATH = ",AxeHardwareInventoryhardwareInventoryMId=1";

//jiira case AXE-2517
const int PFM_SERIALNO_LEN		= 10;
const int BLADE_SUPPLIER_LEN	= 11;


class ACS_APBM_HWIHandler {
public:

	/*tr HR55108 added*/
	static ACE_Recursive_Thread_Mutex _acs_hwi_mutex_m_map;

	enum HWICODE {
			ERROR = -1,
			SUCCESS = 0,
			NO_SWITCH_BOARD
	};

	ACS_APBM_HWIHandler(acs_apbm_serverworkingset *serverWorkingSet);
	~ACS_APBM_HWIHandler();

	//TODO remove following classes!
	void createIMMClasses(void);
	int createHWIRootClass(void);
	//TODO remove end !

	int openHwiHandler();
	int closeHwiHandler();

	int openListener();
	int openImmRunTimeHandler();
	int openImmConfigHandler();
	int closeListener();
	int closeImmRunTimeHandler();
	int closeImmConfigHandler();


	void addMagazine(const std::string &magazineAddress,
					const std::string &egem2L2Switch0EthA,
					const std::string &egem2L2Switch0EthB,
					const std::string &egem2L2Switch25EthA,
					const std::string &egem2L2Switch25EthB);

	int modifyMagazineIPSwitch(const std::string &magazineAddress,
					const int switchSlot,
					const std::string &egem2L2SwitchEthA,
					const std::string &egem2L2SwitchEthB);

	void setSNMPManager(acs_apbm_snmpmanager *snmpManager);

	int startCollection(void);
	int startCollectionOn(const std::string &magazineAddress, bool checkValidMaster = false);
	int startCollectionOnBoard(const std::string &magazineAddress,
					const int &slot);

	void stopCollection(void);
	bool isCollectionOngoing(void) const;

	int removeMagazine(const std::string &magazineAddress);
	int removeMagazineFromIMM(const std::string &magazineAddress);
	int removeAllMagazine(void);
	int removeAllMagazineFromIMM(void);

	int removeBoard(const std::string &magazineAddress,
					const int &slot);

	int removeBoardFromIMM(const std::string &magazineAddress,
								const int &slot);

	int storeAllData(void);
	int storeMagazine(const std::string &magazineAddress);
	int storeBoard(const std::string &magazineAddress,
					const int &slot);

private:

	enum egem2L2SwitchStateConfiguration {
		EGEM2L2SWITCH_0_MASTER = 0,
		EGEM2L2SWITCH_25_MASTER,
		EGEM2L2SWITCH_0_ONLY,
		EGEM2L2SWITCH_25_ONLY,
		EGEM2L2SWITCH_0_25_NO_MASTER
	};

	//static ACS_APBM_HWIHandler* hwiInstance;

	std::map<std::string, ACS_APBM_HWIData*> m_mapMagazine;

	int verifyMaster(const std::string &mag,
						const std::string &ip0A,
						const std::string &ip0B,
						const std::string &ip25A,
						const std::string &ip25B);

	std::string getMaster(const std::string &mag,
								const std::string &ipA,
								const std::string &ipB);

	int readSNMPData(ACS_APBM_HWIData* hwiData);

	int readIMMPersistentData(const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData);

	void readSNMPBoardData(const int &slot,
							ACS_APBM_HWIData* hwiData);

	int snmpBackPlane(const std::string &masterIPA,
							const std::string &masterIPB,
							ACS_APBM_HWIData* hwiData);


	int snmpPFMModule(ACS_APBM_HWIData* hwiData,
							const bool &isUpperModule);

	int snmpPFMModuleCheck(ACS_APBM_HWIData* hwiData,
							const bool &isUpperModule);


	int snmpSubRackName(ACS_APBM_HWIData* hwiData);

	int snmpBoardData(const std::string &masterIPA,
							const std::string &masterIPB,
							const int &slotNo,
							ACS_APBM_HWIData* hwiData);


	int getShelfPosFromIMM(OmHandler &immHandle,
							const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData);


	int getBoardPosFromIMM(OmHandler &immHandle,
							const std::string &magazineAddress,
							const int &slot,
							ACS_APBM_HWIData* hwiData);

/**3 Added to be compliant to new model PFM configuration -Start *********/
	int getPFMPosFromIMM(OmHandler &immHandle,
						const std::string &magazineAddress,
						const bool isUpper,
						ACS_APBM_HWIData* hwiData);
/**3 Added to be compliant to new model PFM configuration -End   *********/

	int storeIMMData(const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData);

	int storeShelfDataToIMM(OmHandler omHandler,
							const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData);


	int storePFMDataToIMM(const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData,
							const bool &isUpper);
	//added to be compliant to new model: now it it is configuration object so OMHANDLER is required -Start
	int storePFMDataToIMM(OmHandler omHandler,
							const std::string &magazineAddress,
							ACS_APBM_HWIData* hwiData,
							const bool &isUpper);
	//added to be compliant to new model: productIdentity object must be created for Shelf and board so new method is required
	int storeProductIdentityDataToIMM(OmHandler omHandler,
							const std::string &parentName,
							const std::string &productDesigantion,
							const std::string &productNumber,
							const std::string &productRevision,
							std::string &dnProductIdentityObject
							);
	//added to be compliant to new model -END
	int storeBoardDataToIMM(OmHandler omHandler,
							const std::string &magazineAddress,
							const int &slot,
							ACS_APBM_HWIData* hwiData);

	int deleteBoardDataFromIMM(OmHandler omHandler,
							const std::string &magazineAddress,
							const int &slot);

	int deletePFMDataFromIMM(const std::string &magazineAddress,
							const bool &isUpper);

	int deleteShelfDataFromIMM(OmHandler omHandler,
							const std::string &magazineAddress);


	std::string trimStr(const std::string &strToTrim);

	/* allPrintableChars :checks if all chars in input vectToCheck within the given i_len
	 * are printable characters. This has been added because of
	 * jiira case AXE-2517.
	 * returns true if all chars in input vector are printable
	 * returns true if a non printable char has been found in a position greater than i_len
	 * 				in this case the non printable char is substituted with NULL char.
	 * returns false if non printable char has been found in the first i_len chars.
	 */
	bool allPrintableChars(char* vectToCheck, int i_len);

	bool m_collectionOngoing;
	bool m_stopCollection;
	bool m_openDone;
	bool m_immRunTimeCloseOK;
	bool m_immConfingCloseOK;
	/*TR HR55108- Added*/
	bool m_storeMagazineOrBoardInProgress;


	acs_apbm_snmpmanager *m_snmpManager;
	acs_apbm_serverworkingset *m_serverWorkingSet;
	ACS_APBM_HWIImmConfObjHandler immHandlerConfigObj;
	implementerThread *immImplementerThread;
	acs_apgcc_oihandler_V2 oiHandler;
	ACS_APBM_HWIImmRuntimeObjHandler immHandlerRunTimeObj;

	//acs_apbm_serverworkingset *server_working_set;
};

#endif /* ACS_APBM_HWIHANDLER_H_ */
