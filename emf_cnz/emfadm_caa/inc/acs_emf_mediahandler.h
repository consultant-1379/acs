#ifndef ACS_EMF_MEDIAHANDLER_H
#define ACS_EMF_MEDIAHANDLER_H

#include <acs_emf_defs.h>
#include <acs_apgcc_omhandler.h>
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
#include <saAis.h>
#include <acs_emf_tra.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_DSD_MacrosConstants.h>

#define CHECK_ENDSIGNAL()			if(m_stopFlag == true){\
																ERROR(1,"%s","EMF Service has received Stop Signal!");\
																modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);\
																copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);\
																modifyStructObjFinalAttr(FAILURE,"Media operation is interrupted because of service is stopped");

#define IF_ENDSIGNAL_EXIT()			return EMF_RC_NOK;}

#define EXIT_IF_ENDSIGNAL()		CHECK_ENDSIGNAL()\
															IF_ENDSIGNAL_EXIT()

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*=================================================================== */
class ACS_EMF_MEDIAHandler
{
protected:

	static ACE_INT32 cntValue;
public:

	enum  EMFMediaHandler_constant
		{
			EMFMEDIAHANDLER_OK = 0,
			EMFMEDIAHANDLER_ERROR = 1,
			EMFMEDIAHANDLER_STOPRECVD = 2,
			EMFMEDIAHANDLER_MOUNTFAILURE = -1,
		};
        /*=================================================================== */
        /**
                        @brief       Default constructor for ACS_EMF_MEDIAHandler

                        @par         None

                        @pre         None

                        @post        None

                        @param       None

                        @exception   None
         */
        /*=================================================================== */

	ACS_EMF_MEDIAHandler();
        /*=================================================================== */
        /**
                        @brief       virtual destructor for ACS_EMF_MEDIAHandler

                        @par         None

                        @pre         None

                        @post        None

                        @param       None

                        @exception   None
         */
        /*=================================================================== */

	virtual ~ACS_EMF_MEDIAHandler();
        /*=================================================================== */
        /**
                        @brief      mediaOperation   Methods used to Perform the operations(copy to nanousb,copy from nanousb) based on the data received.

                        @par         None

                        @pre         None

                        @post        None

                        @param          pData
                                                                COntains the EMF Data based on the operation performed

                        return          0 on success
                                                                        otherwise failed
                        @exception   None
         */
        /*=================================================================== */

	virtual ACE_UINT32 mediaOperation(ACS_EMF_DATA* pData)=0; //to copy data to and from external media

        /*=================================================================== */
        /**
                        @brief       GetMediaInformation                  :This method gets information(Used Space,Free Space etc.,) about the USB

                        @param      mediaType                             :ACE_INT32

                        @param      freeSpace                             :ACE_UINT16

                        @param      usedSpace                             :ACE_UINT16

                        @param      totalSpace                             :ACE_INT32

                        return      ACE_UINT32
         */
        /*=================================================================== */

	virtual ACE_UINT32 GetMediaInformation(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace)=0;

        /*=================================================================== */
        /**
                        @brief      importFiles                          :This method imports files from media

                        @param      pData                                 :ACS_EMF_DATA

                        return      ACE_UINT32
         */
        /*=================================================================== */

	virtual ACE_UINT32 ImportFiles(ACS_EMF_DATA* pData)= 0;
        /*=================================================================== */
        /**
                        @brief       ExportFiles                          :This method exports files to media

                        @param      sysId                                 :int32_t

                        @param      nodeSide                              :acs_dsd::NodeSideConstants

                        @param      pData                                 :ACS_EMF_DATA

                        return      ACE_UINT32
         */
        /*=================================================================== */

	virtual ACE_UINT32 ExportFiles(int32_t sysId, acs_dsd::NodeSideConstants nodeSide, ACS_EMF_DATA* pData)=0;
         /*=================================================================== */
         /**
                 @brief       stopMediaOperation

                 @par         None

                 @return       None

          */
         /*=================================================================== */

	//virtual void stopMediaOperation()= 0;

        /*=================================================================== */
        /**
                        @brief      GetUSBINTInformation                  :This method gets information(Used Space,Free Space etc.,) about the USB

                        @param      mediaType                             :ACE_INT32

                        @param      freeSpace                             :ACE_UINT16

                        @param      usedSpace                             :ACE_UINT16

                        @param      totalSpace                             :ACE_INT32

                        return      ACE_UINT32
         */
        /*=================================================================== */

	virtual int GetUSBINTInformation(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace)
	{
		mediaType = 0;
		freeSpace = 0;
		usedSpace = 0;
		totalSpace = 0;
		return 0;

	}
	virtual ACE_INT32 syncMediaOnActive(){return 0;}
	virtual ACE_INT32 syncMediaOnPassive(){return 0;}
	virtual ACE_INT32 formatMediaInUnlock(){return 0;}
	virtual ACE_INT32 enableMediaOnAccessibleNode(int localMediaState)
	{
		localMediaState = 0;
		return 0;
	} 
	virtual ACE_UINT32 GetMediaInfoOnPassive(std::string& mediaType,std::string& freeSpace,std::string& usedSpace,std::string& totalSpace)
	{
		mediaType = "";
		freeSpace = "";
		usedSpace = "";
		totalSpace = "";
		return 0;
	}
/* ============================================================================== */
//  The following methods are common for both nanousb and dvd, so included in parent class
//*============================================================================== */
        /*=================================================================== */
        /**
                        @brief       incr_cntValue

                        @param      none

                        return      none
         */
        /*=================================================================== */

	void inline incr_cntValue(){cntValue ++; if (cntValue > 20) cntValue = 20;}
        /*=================================================================== */
        /**
                        @brief       get_cntValue

                        @param      none

                        return      ACE_INT32
         */
        /*=================================================================== */

	inline ACE_INT32 get_cntValue()const {return cntValue;}
        /*=================================================================== */
        /**
                        @brief       initEMFHistRuntimeHandler

                        @param      none

                        return      none
         */
        /*=================================================================== */

	void initEMFHistRuntimeHandler();
	//void finalizeEMFHistRuntimeHandler();
        /*=================================================================== */
        /**
                        @brief       setLastHistOperStateToIDLEIfPROCESSING

                        @param      none

                        return      none
         */
        /*=================================================================== */

	void setLastHistOperStateToIDLEIfPROCESSING();
        /*=================================================================== */
        /**
                        @brief       isObjectExiststheEMFInfoClassName

                        @param      none

                        return      bool
         */
        /*=================================================================== */

	bool isObjectExiststheEMFInfoClassName(std::string aObjectRdn , std::string dn,bool& isObjectAvailable);
        /*=================================================================== */
        /**
                        @brief       createRuntimeObjectsforEmfHistoryInfoClass

                        @param      none

                        return      bool
         */
        /*=================================================================== */

	bool createRuntimeObjectsforEmfHistoryInfoClass(std::string& attrRDN);
        /*=================================================================== */
        /**
                        @brief       deleteAllRuntimeObjects

                        @param      none

                        return      bool
         */
        /*=================================================================== */

	bool deleteAllRuntimeObjects();
        /*=================================================================== */
        /**
                        @brief       deleteAllRuntimeObjects

                        @param      aObjectRdn1                        :ACE_TCHAR

			@param      aObjectRdn2			       :ACE_TCHAR

                        return      bool
         */
        /*=================================================================== */

	bool copyObjToObj(ACE_TCHAR* aObjectRdn1,ACE_TCHAR* aObjectRdn2);
        /*=================================================================== */
        /**
                        @brief       moveObjectsOneLevelDown

                        @param      countValue				:int

                        return      bool
         */
        /*=================================================================== */

	bool moveObjectsOneLevelDown(int countValue);
        /*=================================================================== */
        /**
                        @brief       modifyInitialHistoryAttrs

                        @param       attrValue                         :ACE_UINT32

                        @param       fileNames                         :std::string

                        return      none
         */
        /*=================================================================== */

	void modifyInitialHistoryAttrs(ACE_UINT32 attrValue,std::string fileNames);
        /*=================================================================== */
        /**
                        @brief       modifyEndHistoryAttrs

                        @param      attribute                          :ACE_TCHAR

                        return      none
         */
        /*=================================================================== */

	void modifyEndHistoryAttrs(ACE_TCHAR* attribute);
        /*=================================================================== */
        /**
                        @brief       getHistoryInstancesCnt

                        @param      none

                        return      ACE_INT32
         */
        /*=================================================================== */

	ACE_INT32 getHistoryInstancesCnt();
        /*=================================================================== */
        /**
                        @brief       modifyStructObjInitialAttr

                        @param      operationType                          :int

                        return      ACE_INT32
         */
        /*=================================================================== */

	ACE_INT32 modifyStructObjInitialAttr(int operationType);
        /*=================================================================== */
        /**
                        @brief       modifyStructObjFinalAttr

                        @param      resultOfaction                          :int

			@param      reasonForFailure			    :std::string			

                        return      ACE_INT32
         */
        /*=================================================================== */

	ACE_INT32 modifyStructObjFinalAttr(int resultOfaction, std::string reasonForFailure);
	/*=================================================================== */
	/**
		  @brief           Modifys the run time history info Attribute in IMM

		  @param  		   aObjectDN :   char*

		  @param  		   attribute :   char *

		  @param  		   value :   int

		  @return		   bool

		  @exception       none
	 */
	/*=================================================================== */
	bool modifyEMFHistoryIntegerAttribute(ACE_TCHAR* aObjectDN , ACE_TCHAR* attribute,int value);
	/*=================================================================== */
	/**
		  @brief           Modifys the run time history info Attribute in IMM

		  @param  		   aObjectDN :   char*

		  @param  		   attribute :   char *

		  @param  		   value :   char *

		  @return		   bool

		  @exception       none
	 */
	/*=================================================================== */
	bool modifyEMFHistoryAttribute(char* aObjectDN , char* attribute,char* value);
	/*=================================================================== */
	/**
                                        @brief       Method to modify the state attribute in structure.

                                        @par         None

                                        @pre         None

                                        @post        None

                                        return          0 on success
                                                                                        otherwise failed
                                        @exception   None
	 */
	/*=================================================================== */
	ACE_INT32 modifyStructObjState();

	/*=================================================================== */
	/**
	      			@brief       Method to create structure object.

	      			@par         None

	      			@pre         None

	      			@post        None

	      			return		no of instances on success
	      									otherwise 0 on failure
	      			@exception   None
	 */
	/*=================================================================== */
	ACE_INT32 createStructObj();
        /*=================================================================== */
        /**
                        @brief       create

                        @param      hwVersion                          :int

			@param      endEvent			       :ACE_HANDLE		

                        return      ACS_EMF_MEDIAHandler
         */
        /*=================================================================== */

	static ACS_EMF_MEDIAHandler* create(int hwVersion,ACE_HANDLE endEvent);
				
		/*=================================================================== */
    /**
                 @brief       stopMediaOperation

                 @par         None

                 @return       None
                                  
     */
     /*=================================================================== */
		void stopMediaOperation();

        /*=================================================================== */
        /**
                        @brief      mmHandle 
         */
        /*=================================================================== */
	OmHandler immHandle;
        /*=================================================================== */
        /**
                        @brief      m_stopFlag
         */
        /*=================================================================== */
	/*private:*/
	bool m_stopFlag;	// Exit signal for all media thread. If true stop all thread.
	bool m_mediaAttachedLocally;	
	
};

#endif
