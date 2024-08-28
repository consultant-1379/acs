/* =================================================================== 
  
   @file ACS_APGCC_ApplicationManager.cpp

   Class method implementation for APG common class API to integrate APG applications with AMF.

   This module contains the implementation of class declared in
   the ACS_APGCC_ApplicationManager.h 

   @version 1.0.0

   HISTORY
   -

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/11/2010     XMALSHA       Initial Release
==================================================================== */


/*=================================================================== 
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "ACS_APGCC_ApplicationManager.h"
#include "nid_start_util.h"
#include "ACS_TRA_trace.h"
#include <sys/file.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <pwd.h>
#include <sys/stat.h>
#include "ace/OS_NS_poll.h"
#include "syslog.h"

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

#define m_SET_AMF_VERSION(amfVersion) \
{ \
	amfVersion.releaseCode='B'; \
	amfVersion.majorVersion=0x01; \
	amfVersion.minorVersion=0x01 ; \
};

//namespace {
//		ACS_TRA_trace trace(const_cast<char*>("ACS_APGCC_ApplicationManager"), const_cast<char *>("C300"));
//}

using namespace std;

/*===================================================================
                        CLASS DECLARATION SECTION
===================================================================== */
/* ================================================================== */
/**
      @brief	The ACS_APGCC_AMFCallbacks class is the internal class, will be 
		used by ACS_APGCC_ApplicationManger common class. This class only 
		defines CoreMW static callbacks.	 
*/
/*=================================================================== */		

class ACS_APGCC_AMFCallbacks {
	
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
	
	private:

		/*===================================================================
	                        PRIVATE ATTRIBUTE DECLARATION
		==================================================================== */
		static ACS_APGCC_ApplicationManager* appMngrObj;
		
		/*===================================================================
	                        PRIVATE METHOD DECLARATION
		==================================================================== */
		static void coremw_csi_set_callback ( 	SaInvocationT ,
							const SaNameT* ,
							SaAmfHAStateT  ,
							SaAmfCSIDescriptorT ) ;
												
		static void coremw_csi_remove_callback(	SaInvocationT,
							const SaNameT*,
							const SaNameT*,
							SaAmfCSIFlagsT);

		static void coremw_csi_terminate_callback( 	SaInvocationT,
								const SaNameT*);
													
		static void coremw_healthcheck_callback(SaInvocationT,
							const SaNameT*,
							SaAmfHealthcheckKeyT*);

	/*=====================================================================
	                        PROTECTED DECLARATION SECTION
	==================================================================== */
	protected:
	
	/*=====================================================================
	                       PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
		/*=====================================================================
                        CLASS CONSTRUCTOR
		==================================================================== */
		ACS_APGCC_AMFCallbacks() {}; // EMPTY
		/*=====================================================================
                        CLASS DESTRUCTOR
		==================================================================== */
		~ACS_APGCC_AMFCallbacks() {}; // EMPTY
		/*=================================================================== */
		/**
		@brief	This routine initializes with AMF of CoreMW
	
		@par	None

		@pre	None

		@pre	None
	
		@param	ACS_APGCC_ApplicationManager
				Application Manager Common Class Object.

		@return	ACS_APGCC_ReturnType
				On success ACS_APGCC_SUCCESS is returned and ACS_APGCC_FAILURE is retuned on failure.
				
		@exception	None
		*/
		/*=================================================================== */
		ACS_APGCC_ReturnType safAMFInitialize ( ACS_APGCC_ApplicationManager *);
		
};	/* end of ACS_APGCC_AMFCallbacks */
		

/* ================================================================== 
			Initialize Static Object with NULL
   ================================================================== */
ACS_APGCC_ApplicationManager* ACS_APGCC_AMFCallbacks::appMngrObj = NULL;
		
/*=================================================================== 
   ROUTINE: safAMFInitialize
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_AMFCallbacks::safAMFInitialize(ACS_APGCC_ApplicationManager *obj){

	SaVersionT version;
	SaAmfCallbacksT callbacks;
	SaAisErrorT errorCode;
	
	/** Set AMF version here **/
	m_SET_AMF_VERSION(version);

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks::safAMFInitialize invoked");
//		trace.ACS_TRA_event(1, buffer);
//	}
	/* Initialize callback structure */
	memset(&callbacks, 0, sizeof(SaAmfCallbacksT) );
	
	/** Fill out the callback structure **/
	callbacks.saAmfCSISetCallback			=	ACS_APGCC_AMFCallbacks::coremw_csi_set_callback ;
	callbacks.saAmfCSIRemoveCallback		=	ACS_APGCC_AMFCallbacks::coremw_csi_remove_callback ;
	callbacks.saAmfHealthcheckCallback		=	ACS_APGCC_AMFCallbacks::coremw_healthcheck_callback ;
	callbacks.saAmfComponentTerminateCallback	=	ACS_APGCC_AMFCallbacks::coremw_csi_terminate_callback ;
	
	/** Initalize application with CoreMW Framework **/
		
	errorCode = saAmfInitialize(	&obj->amfHandle,
					&callbacks,
					&version);
								
//	char buffer[100];
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_AMFCallbacks:safAMFInitialize FAILED with ERROR CODE: %d", errorCode);
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks:safAMFInitialize FAILED with ERROR CODE: %d", errorCode);
//		obj->log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}

//	syslog(LOG_INFO, "ACS_APGCC_AMFCallbacks:amfHandle:[%x]", (unsigned int)obj->amfHandle);
//	sprintf(buffer, "ACS_APGCC_AMFCallbacks:amfHandle:[%x]", (unsigned int)obj->amfHandle);
//	obj->log.Write(buffer, LOG_LEVEL_INFO);
		
	appMngrObj = obj;    /* Assign object to use in callbacks */
		
	return ACS_APGCC_SUCCESS;

} /* end safAMFInitialize */

/*=================================================================== 
   ROUTINE: coremw_csi_set_callback
=================================================================== */
void ACS_APGCC_AMFCallbacks::coremw_csi_set_callback (	SaInvocationT p_Inv, //IN
							const SaNameT *p_compName,// IN
							SaAmfHAStateT p_HAState, // IN
							SaAmfCSIDescriptorT p_csiDesc ) { // IN

	ACS_APGCC_ErrorT errorCode = ACS_APGCC_SUCCESS;
	SaAisErrorT errCode ;
	SaAisErrorT error = SA_AIS_OK;
	ACS_APGCC_AMF_HA_StateT previousHAState=ACS_APGCC_AMF_HA_UNDEFINED;
//	char buff[25];

	/* To surpress warnings */
	(void)p_compName;
	(void)p_csiDesc;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks::coremw_csi_set_callback");
//		trace.ACS_TRA_event(1, buffer);
//	}

//	syslog(LOG_INFO, "ACS_APGCC_AMFCallbacks::coremw_csi_set_callback: ha_state[%d]",p_HAState);
		
	/** Assign current HAState to previousHAState**/
	previousHAState = appMngrObj->HAState;
		
	/** Store the received HAState **/
	appMngrObj->HAState = (ACS_APGCC_AMF_HA_StateT)p_HAState;

	if (p_HAState == SA_AMF_HA_ACTIVE) {
		errorCode = appMngrObj->performStateTransitionToActiveJobs(previousHAState) ;
	}								
					
	if (p_HAState == SA_AMF_HA_STANDBY) {
		errorCode = appMngrObj->performStateTransitionToPassiveJobs(previousHAState) ;
	}
		
	if (p_HAState == SA_AMF_HA_QUIESCED) {
		errorCode = appMngrObj->performStateTransitionToQuiescedJobs(previousHAState);
		appMngrObj->HAState=ACS_APGCC_AMF_HA_UNDEFINED;
	}
		
	if (p_HAState == SA_AMF_HA_QUIESCING) {

		errCode =  saAmfResponse(appMngrObj->amfHandle,
					 p_Inv,
					 error);

		errorCode = appMngrObj->performStateTransitionToQueisingJobs(previousHAState) ;
		appMngrObj->HAState=ACS_APGCC_AMF_HA_UNDEFINED;
		if (errorCode!=ACS_APGCC_SUCCESS)
			error=SA_AIS_ERR_FAILED_OPERATION;

		errCode =  saAmfCSIQuiescingComplete (	appMngrObj->amfHandle,
							p_Inv,
							error );
		
		if ( SA_AIS_OK != errCode ){ 
			syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coremw_csi_set_callback:saAmfCSIQuiescingComplete FAILED");
//			appMngrObj->log.Write("ACS_APGCC_ApplicationManager:coremw_csi_set_callback:saAmfCSIQuiescingComplete FAILED", LOG_LEVEL_INFO);
//		        sprintf(buff, "ERROR CODE:%d", errorCode);			
//			appMngrObj->log.Write(buff, LOG_LEVEL_ERROR);
			appMngrObj->HAState = previousHAState;
		}
		return;
	}

	if (errorCode!=ACS_APGCC_SUCCESS)
		error=SA_AIS_ERR_FAILED_OPERATION;
		
	errCode =  saAmfResponse(appMngrObj->amfHandle,
				p_Inv,
				error);
			
	if ( SA_AIS_OK != errCode ) {
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coremw_csi_set_callback:saAmfResponse FAILED");
//		appMngrObj->log.Write("ACS_APGCC_ApplicationManager:coremw_csi_set_callback:saAmfResponse FAILED", LOG_LEVEL_INFO);
//		sprintf(buff, "ERROR CODE:%d", errorCode);			
//		appMngrObj->log.Write(buff, LOG_LEVEL_ERROR);
		appMngrObj->HAState = previousHAState;
	}
		
} /* end coremw_csi_set_callback */

/*=================================================================== 
   ROUTINE: coremw_csi_remove_callback
=================================================================== */

void ACS_APGCC_AMFCallbacks::coremw_csi_remove_callback (SaInvocationT p_Inv,   	// IN
							const SaNameT *p_compName, // IN
							const SaNameT *p_csiName, // IN
							SaAmfCSIFlagsT p_Flag){ // IN
																
	ACS_APGCC_ErrorT errorCode ;
	SaAisErrorT errCode ;
	SaAisErrorT error = SA_AIS_OK;

	/* To surpress warnings */
	(void)p_compName;
	(void)p_csiName;
	(void)p_Flag;
		
//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks::coremw_csi_remove_callback");
//		trace.ACS_TRA_event(1, buffer);
//	}
//	syslog(LOG_INFO, "ACS_APGCC_AMFCallbacks::coremw_csi_remove_callback:" );

	/** Set our HAState to undefined now **/
	appMngrObj->HAState = ACS_APGCC_AMF_HA_UNDEFINED ;
	
	/** Call remove method now **/	
	errorCode = appMngrObj->performComponentRemoveJobs ( );		
	if (  ACS_APGCC_SUCCESS != errorCode) {	
		error = SA_AIS_ERR_FAILED_OPERATION;
	}
		
	errCode =  saAmfResponse(appMngrObj->amfHandle,
				p_Inv,
				error);
			      
	if (SA_AIS_OK != errCode) {							
//		char buffer[20];
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coremw_csi_remove_callback:saAmfResponse FAILED");
//		appMngrObj->log.Write("ACS_APGCC_ApplicationManager:coremw_csi_remove_callback:saAmfResponse FAILED", LOG_LEVEL_INFO);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);			
//		appMngrObj->log.Write(buffer, LOG_LEVEL_ERROR);
		return;
	}			
		
		
} /* end coremw_csi_remove_callback */

/*=================================================================== 
   ROUTINE: coremw_csi_terminate_callback
=================================================================== */

void ACS_APGCC_AMFCallbacks::coremw_csi_terminate_callback( SaInvocationT p_Inv ,  // IN
							const SaNameT *p_compName) {  // IN
																	
	ACS_APGCC_ErrorT errorCode ;
	SaAisErrorT errCode ;
	SaAisErrorT error = SA_AIS_OK;

	(void)p_compName;
		
//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks::coremw_csi_terminate_callback");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/** Set our HAState to undefined now **/
	appMngrObj->HAState = ACS_APGCC_AMF_HA_UNDEFINED ;		
	
	/** Call terminate method now **/	
	errorCode =	appMngrObj->performComponentTerminateJobs( );		
	if (ACS_APGCC_SUCCESS != errorCode) {
		error = SA_AIS_ERR_FAILED_OPERATION;
	}
		
	errCode =  saAmfResponse(appMngrObj->amfHandle,
				 p_Inv,
				error);
				
	if (SA_AIS_OK != errCode) {							
//		char buffer[20];
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coremw_csi_terminate_callback:saAmfResponse FAILED");
//		appMngrObj->log.Write("ACS_APGCC_ApplicationManager:coremw_csi_terminate_callback:saAmfResponse FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);			
//		appMngrObj->log.Write(buffer, LOG_LEVEL_ERROR);
		return ;
	}	
	/* set the termination flag to true to terminate the daemon */
	appMngrObj->termAppEventReceived = TRUE;
		
}	/* end coremw_csi_terminate_callback */	


/*=================================================================== 
   ROUTINE: coremw_healthcheck_callback
=================================================================== */

void ACS_APGCC_AMFCallbacks::coremw_healthcheck_callback(	SaInvocationT p_Inv,		// IN
								const SaNameT *p_compName,	// IN
								SaAmfHealthcheckKeyT *p_hCheckKey) {	// IN
																
	ACS_APGCC_ErrorT errorCode ;
	SaAisErrorT errCode ;
	SaAisErrorT error = SA_AIS_OK;
	
	(void)p_compName;
		
//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_AMFCallbacks::coremw_healthcheck_callback");
//		trace.ACS_TRA_event(1, buffer);
//	}
						
	/** Do healthcheck now **/
	
	if ( strcmp( (char*)p_hCheckKey->key, (char*)appMngrObj->hCheckKey->key) != 0 ){
		syslog(LOG_ERR, "Healtch Check Key Mismatch Received");
//		appMngrObj->log.Write("Healtch Check Key Mismatch Received", LOG_LEVEL_ERROR);
		return;	
	}	
		
	errorCode = appMngrObj->performComponentHealthCheck ( );
		
	if (  ACS_APGCC_FAILURE != errorCode) {							
		errCode =  saAmfResponse(appMngrObj->amfHandle,
					p_Inv,
					error);
				      
		if (SA_AIS_OK != errCode) {
//			char buffer[20];
			syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coremw_healthcheck_callback:saAmfResponse FAILED");
//			appMngrObj->log.Write("ACS_APGCC_ApplicationManager:coremw_healthcheck_callback:saAmfResponse FAILED", LOG_LEVEL_ERROR);
//			sprintf(buffer, "ERROR CODE:%d",errCode);
//			appMngrObj->log.Write(buffer, LOG_LEVEL_ERROR);
		}		
	}
}	/* end coremw_healthcheck_callback */

/*=================================================================== 
			ACS_APGCC_ApplicationManager CLASS IMPLEMENTATION
=================================================================== */

int ACS_APGCC_ApplicationManager::shutdownPipeFd[2];
char ACS_APGCC_ApplicationManager::__pidfile[PID_FILE_LEN];

/*=================================================================== 
   ROUTINE: ACS_APGCC_ApplicationManager  - Constructor 
=================================================================== */		
ACS_APGCC_ApplicationManager::ACS_APGCC_ApplicationManager(const char* daemon_name,
	       						   const char* user_name) {

	
	
	/* call daemonize to make the application daemon */
	daemonize(daemon_name, user_name);

        /* open log */
//        log.Open("APGCC");
        
	this->HAState = ACS_APGCC_AMF_HA_UNDEFINED;
	this->SelObj = 0;
	this->amfHandle = 0;
	this->daemonName = daemon_name;

} /* end ACS_APGCC_ApplicationManager */

/*=================================================================== 
   ROUTINE:  coreMWInitialize
=================================================================== */	

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::coreMWInitialize( ){

	SaAisErrorT errorCode;
	SaAmfHealthcheckInvocationT    hCheckInv;
	SaAmfRecommendedRecoveryT  rRecovery;
	ACS_APGCC_AMFCallbacks	obj;
//	char buffer[100];

//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::getHAState");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/* Register for shutdown signal handler */
	if ((signal(SIGUSR2, registerApplicationShutdownhandler)) == SIG_ERR) {
		syslog(LOG_ERR, "registerApplicationShutdownhandler FAILED");
//		log.Write("registerApplicationShutdownhandler FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errno);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;	
	}
				
	/* call AMF initialize */
	obj.safAMFInitialize( this );

//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate:saAmfSelectionObjectGet");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	/** Call saAmfSelectionObjectGet to get the operating system handle. 
	  * This handle will be used by applicatio to wait on the callbacks **/
	errorCode = saAmfSelectionObjectGet(this->amfHandle,
					    &this->SelObj);
							
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfSelectionObjectGet FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfSelectionObjectGet FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}
		
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate:saAmfComponentNameGet");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/** Call saAmfCompNameGet to get the name of the component **/
	errorCode = saAmfComponentNameGet(this->amfHandle,
				          &this->compName ) ;
					
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfCompNameGet FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfCompNameGet FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}
		

//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate:saAmfComponentRegister");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/** Call saAmfComponentRegister to register with your component **/
	errorCode = saAmfComponentRegister(this->amfHandle,
					   &this->compName,
					   0);
										
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:componentRegister FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:componentRegister FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}							
		
	/** Start the application healthcheck now **/
	
	hCheckInv   = SA_AMF_HEALTHCHECK_AMF_INVOKED;
	rRecovery   = SA_AMF_COMPONENT_RESTART;

	/* Form the unique health check key */
	this->hCheckKey = new ACS_APGCC_AMF_HealthCheckKeyT;
	int dlen = strlen(this->daemonName);
	memcpy(hCheckKey->key, this->daemonName, dlen);
	strcpy((char*)hCheckKey->key + dlen, "_hck");
	this->hCheckKey->keyLen = strlen((char*)hCheckKey->key) ;
	
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate:saAmfHealthcheckStart");
//		trace.ACS_TRA_event(1, buffer);
//	}

	errorCode = saAmfHealthcheckStart(this->amfHandle,
					  &this->compName,
					  (SaAmfHealthcheckKeyT *)this->hCheckKey,
					  hCheckInv, 
					  rRecovery );
										
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:Application healthcheckStart FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:Application healthcheckStart FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE;
	}							

	return ACS_APGCC_SUCCESS;
		
} /* end coreMWInitialize */

/*=================================================================== 
   ROUTINE:  getHAState
=================================================================== */	

ACS_APGCC_AMF_HA_StateT ACS_APGCC_ApplicationManager::getHAState (void ) const {

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::getHAState");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return this->HAState;
} /* end getHAState */

/*=================================================================== 
   ROUTINE:  getSelObj
=================================================================== */

ACS_APGCC_SelObjT ACS_APGCC_ApplicationManager::getSelObj(void ) const {

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::getSelObj");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return this->SelObj;
} /* end getSelObj */

/*=================================================================== 
   ROUTINE:  componentReportError
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::componentReportError ( ACS_APGCC_AMF_RecommendedRecoveryT rRecovery) { // IN
	
	SaAisErrorT errorCode;
	SaTimeT saTime = 0;
//	char buffer[100];
		
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::componentClearError");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	errorCode = saAmfComponentErrorReport(	this->amfHandle,
						&this->compName,
						saTime,
						(SaAmfRecommendedRecoveryT) rRecovery,
						SA_NTF_IDENTIFIER_UNUSED );
												
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfComponentErrorReport FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfComponentErrorReport FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return  ACS_APGCC_FAILURE ;
	}
		
	return ACS_APGCC_SUCCESS;
} /* end componentReportError */

/*=================================================================== 
   ROUTINE:  componentClearError
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::componentClearError( ){

	SaAisErrorT errorCode;
//	char buffer[100];
		
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::componentClearError");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	errorCode = saAmfComponentErrorClear(	this->amfHandle,
						&this->compName,
						SA_NTF_IDENTIFIER_UNUSED );
												
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfComponentErrorClear FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfComponentErrorClear FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}

	return ACS_APGCC_SUCCESS ;
} /* end componentClearError */

/*=================================================================== 
   ROUTINE:  dispatch
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::dispatch(ACS_APGCC_AMF_DispatchFlagsT p_dFlag){  // IN

	SaAisErrorT errorCode;
//	char buffer[100];
		
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::dispatch");
//		trace.ACS_TRA_event(1, buffer);
//	}

		
	errorCode = saAmfDispatch(this->amfHandle,
				  (SaDispatchFlagsT)p_dFlag );
										
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfDispatch FAILEd");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfDispatch FAILEd", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}
		
	return ACS_APGCC_SUCCESS ;
} /* end dispatch */

/*=================================================================== 
   ROUTINE:  finalize
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::finalize( ) {

	SaAisErrorT errorCode;
//	char buffer[100];

//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::finalize:saAmfHealthcheckStop");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	errorCode = saAmfHealthcheckStop( this->amfHandle,
					  &this->compName,
					  (SaAmfHealthcheckKeyT*) this->hCheckKey);

	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfHealthcheckStop FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfHealthcheckStop FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}

	/* free the allocated memory for health checck key */
	delete[] this->hCheckKey;
	
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::finalize:saAmfComponentUnregister");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	errorCode = saAmfComponentUnregister(	this->amfHandle,
						&this->compName,
						0) ;

	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfComponentUnregister FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfComponentUnregister FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}
	
//	if (trace.ACS_TRA_ON()){
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::finalize:saAmfFinalize");
//		trace.ACS_TRA_event(1, buffer);
//	}
		
	errorCode =  saAmfFinalize( this->amfHandle );
		
	if (SA_AIS_OK != errorCode) {							
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfFinalize FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfFinalize FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}
		
	return ACS_APGCC_SUCCESS;

} /* end finalize */

/*===================================================================
	ROUTINE:  finalize
=================================================================== */
ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::AmfFinalize(){

	SaAisErrorT errorCode;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::AmfFinalize");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/* free the allocated memory for health checck key */
	delete[] this->hCheckKey;

	errorCode =  saAmfFinalize( this->amfHandle );
		
	if (SA_AIS_OK != errorCode) {
//		char buffer[100];
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:saAmfFinalize FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:saAmfFinalize FAILED", LOG_LEVEL_ERROR);
//		sprintf(buffer, "ERROR CODE:%d", errorCode);
//		log.Write(buffer, LOG_LEVEL_ERROR);
		return ACS_APGCC_FAILURE ;
	}

	return ACS_APGCC_SUCCESS;

} /* end AmfFinalize */

/*=================================================================== 
   ROUTINE:  performStateTransitionToActiveJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performStateTransitionToActiveJobs (ACS_APGCC_AMF_HA_StateT prevousHaState) { //IN

	(void)prevousHaState;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performStateTransitionToActiveJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
} // end performStateTransitionToActiveJobs

/*=================================================================== 
   ROUTINE:  performStateTransitionToPassiveJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performStateTransitionToPassiveJobs (ACS_APGCC_AMF_HA_StateT prevousHaState) { //IN

	(void)prevousHaState;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performStateTransitionToPassiveJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
} /* end performStateTransitionToPassiveJobs */

/*=================================================================== 
   ROUTINE:  performStateTransitionToQueisingJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT prevousHaState){ //IN
	
	(void)prevousHaState;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performStateTransitionToQueisingJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
} /* end performStateTransitionToQueisingJobs */

/*===================================================================
   ROUTINE:  performStateTransitionToQuiesedJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT prevousHaState){ //IN

	(void)prevousHaState;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performStateTransitionToQuiescedJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
} /* end performStateTransitionToQueisingJobs */

/*=================================================================== 
   ROUTINE:  performComponentTerminateJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performComponentTerminateJobs( ) {

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performComponentTerminateJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
} /* end performComponentTerminateJobs */

/*=================================================================== 
   ROUTINE:  performComponentRemoveJobs
=================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performComponentRemoveJobs( ) {

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performComponentRemoveJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}
	
	return ACS_APGCC_SUCCESS;
} /* end performComponentRemoveJobs */

/*===================================================================
  ROUTINE: activate
===================================================================== */

ACS_APGCC_HA_ReturnType ACS_APGCC_ApplicationManager::activate(){

	struct pollfd fds[2];
	nfds_t nfds = 2;
	ACE_INT32 retval;
	ACS_APGCC_BOOL Is_haStarted=TRUE;
	termAppEventReceived=FALSE;
//	ACE_TCHAR buffer[100];

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::activate");
//		trace.ACS_TRA_event(1, buffer);
//	}

	/* Initialize CoreMW Framework */
	if (coreMWInitialize() != ACS_APGCC_SUCCESS){
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:coreMWInitialize FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:coreMWInitialize FAILED", LOG_LEVEL_ERROR);
		return ACS_APGCC_HA_FAILURE;
	}

//	log.Write("ACS_APGCC_ApplicationManager:coreMWInitialize Success", LOG_LEVEL_INFO);

	/* create the pipe for shutdown handler */
	if ( (pipe(shutdownPipeFd)) < 0) {
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:pipe creation FAILED");
//		log.Write("ACS_APGCC_ApplicationManager:pipe creation FAILED", LOG_LEVEL_ERROR);
		return ACS_APGCC_HA_FAILURE;
	}
	
	if ( (fcntl(shutdownPipeFd[0], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager: pipe fcntl on readn");
//		log.Write("ACS_APGCC_ApplicationManager: pipe fcntl on readn", LOG_LEVEL_ERROR);
		return ACS_APGCC_HA_FAILURE;
	}

	if ( (fcntl(shutdownPipeFd[1], F_SETFL, O_NONBLOCK)) < 0) {
		syslog(LOG_ERR, "ACS_APGCC_ApplicationManager: pipe fcntl on writen");
//		log.Write("ACS_APGCC_ApplicationManager: pipe fcntl on writen", LOG_LEVEL_ERROR);
		return ACS_APGCC_HA_FAILURE;
	}

	fds[FD_AMF].fd = getSelObj();
	fds[FD_AMF].events = POLLIN;

	fds[FD_USR2].fd = shutdownPipeFd[0];
	fds[FD_USR2].events = POLLIN;

	while (Is_haStarted){
		retval = ACE_OS::poll(fds, nfds, 0) ;

		if(retval == -1){
			if (errno == EINTR)
				continue;
			syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:poll FAILED [%s]",strerror(errno)); 
//			sprintf(buffer, "ACS_APGCC_ApplicationManager:poll FAILED [%s]",strerror(errno)); 
//			log.Write(buffer, LOG_LEVEL_ERROR);
			Is_haStarted = FALSE;
			continue;
		}
		
		if ( fds[FD_AMF].revents & POLLIN){
			if (dispatch(ACS_APGCC_AMF_DISPATCH_ALL) != ACS_APGCC_SUCCESS){
				syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:dispatch FAILED");
//				log.Write("ACS_APGCC_ApplicationManager:dispatch FAILED", LOG_LEVEL_ERROR);
				Is_haStarted = FALSE;
				continue;
			}
		
			if(this->termAppEventReceived == TRUE){
				Is_haStarted = FALSE;
				continue;
			}
		}
		if (fds[FD_USR2].revents & POLLIN){
			if (performApplicationShutdownJobs() != ACS_APGCC_SUCCESS){
				syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:performApplicationShutdownJobs FAILED");
//				log.Write("ACS_APGCC_ApplicationManager:performApplicationShutdownJobs FAILED", LOG_LEVEL_ERROR);
			}
			Is_haStarted = FALSE;
			this->termAppEventReceived=TRUE;
		}

	} /* end while */

	/* close pipe fds first*/
	close(shutdownPipeFd[0]);
	close(shutdownPipeFd[1]);

	if (this->termAppEventReceived == TRUE){
		if (AmfFinalize() != ACS_APGCC_SUCCESS){
			syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:AmfFinalize FAILED");
//			log.Write("ACS_APGCC_ApplicationManager:AmfFinalize FAILED", LOG_LEVEL_ERROR);
			return ACS_APGCC_HA_FAILURE_CLOSE;
		}
	}
	else {
		if (finalize() != ACS_APGCC_SUCCESS){
			syslog(LOG_ERR, "ACS_APGCC_ApplicationManager:finalize FAILED");
//			log.Write("ACS_APGCC_ApplicationManager:finalize FAILED", LOG_LEVEL_ERROR);
			return ACS_APGCC_HA_FAILURE_CLOSE;
		}
	}

	return ACS_APGCC_HA_SUCCESS;
} /* end activate */


/*===================================================================
   ROUTINE:performApplicationShutdownJobs

===================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::performApplicationShutdownJobs(){

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::performApplicationShutdownJobs");
//		trace.ACS_TRA_event(1, buffer);
//	}

	return ACS_APGCC_SUCCESS;
}

/*===================================================================
   ROUTINE: registerApplicationShutdownhandler
===================================================================== */
void ACS_APGCC_ApplicationManager::registerApplicationShutdownhandler(int sig){
	(void)sig;

//	if (trace.ACS_TRA_ON()){
//		char buffer[300];
//		sprintf(buffer, "ACS_APGCC_ApplicationManager::registerApplicationShutdownhandler");
//		trace.ACS_TRA_event(1, buffer);
//	}

	syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::Shutting down the application.( USR2)");
	write(shutdownPipeFd[1], &sig, sizeof(sig));
} /* end registerApplicationShutdownhandler */

/*===================================================================
   ROUTINE: daemonize
===================================================================== */

void ACS_APGCC_ApplicationManager::daemonize(const char* daemon_name, const char* userName){

	pid_t pid, sid;


        /* Already a daemon */
        if (getppid() == 1) return;

	/* Drop user if there is one, and we were run as root */
    	if ( getuid() == 0 || geteuid() == 0 ) {
        	struct passwd *pw = getpwnam(userName);
        	if ( pw ) {
            		//syslog(LOG_INFO, "ACS_APGCC_ApplicationManager::Setting user to [%s]",userName );
            		setuid( pw->pw_uid );
        	}
    	}

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::Fork daemon failed, pid=%d (%s)", pid, strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* If we got a good PID, then we can exit the parent process */
        if (pid > 0)  {
                exit(EXIT_SUCCESS);
        }

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::Create new session failed, sid=%d (%s)", sid, strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* Redirect standard files to /dev/null */
        if (freopen("/dev/null", "r", stdin) == NULL){
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::freopen stdin failed - %s", strerror(errno));
	}
        if (freopen("/dev/null", "w", stdout) == NULL) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::freopen stdout failed - %s", strerror(errno));
	}
        if (freopen("/dev/null", "w", stderr) == NULL) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::freopen stderr failed - %s", strerror(errno));
	}

        /* Change the file mode mask to 0644 */
        umask(022);

        /*
         * Change the current working directory.  This prevents the current
         * directory from being locked; hence not being able to remove it.
         */
        if (chdir("/") < 0) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::unable to change directory, dir=%s (%s)", "/", strerror(errno));
                exit(EXIT_FAILURE);
        }

        snprintf(__pidfile, sizeof(__pidfile),  PIDDIR"/%s.pid", daemon_name);
        /* Create the process PID file */
        if (__create_pidfile(__pidfile) != ACS_APGCC_SUCCESS)
                exit(EXIT_FAILURE);

        /* Cancel certain signals */
        signal(SIGCHLD, SIG_DFL);       /* A child process dies */
        signal(SIGTSTP, SIG_IGN);       /* Various TTY signals */
        signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
        signal(SIGHUP,  SIG_DFL);        /* Ignore hangup signal */
        signal(SIGTERM, SIG_DFL);       /* Die on SIGTERM */
} /* end daemonize */


/*===================================================================
   ROUTINE: __create_pidfile
===================================================================== */

ACS_APGCC_ReturnType ACS_APGCC_ApplicationManager::__create_pidfile(const char* pidfile) {

        FILE *file = NULL;
        int fd, pid;

        /* open the file and associate a stream with it */
        if ( ((fd = open(pidfile, O_RDWR|O_CREAT, 0644)) == -1)
                        || ((file = fdopen(fd, "r+")) == NULL) ) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::Open failed, pidfile:[%s], err:[%s]", pidfile, strerror(errno));
                return ACS_APGCC_FAILURE;
        }

        /* Lock the file */
        if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::flock failed, pidfile:[%s], err:[%s]", pidfile, strerror(errno));
                fclose(file);
                return ACS_APGCC_FAILURE;
        }

        pid = getpid();
        if (!fprintf(file,"%d\n", pid)) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::fprintf failed, pidfile:[%s], err:[%s]", pidfile, strerror(errno));
                fclose(file);
                return ACS_APGCC_FAILURE;
        }
        fflush(file);

        if (flock(fd, LOCK_UN) == -1) {
                syslog(LOG_ERR, "ACS_APGCC_ApplicationManager::flock failed, pidfile:[%s], err:[%s]", pidfile, strerror(errno));
                fclose(file);
                return ACS_APGCC_FAILURE;
        }
        fclose(file);

        return ACS_APGCC_SUCCESS;

} /* end __create_pidfile */

//******************************************************************************

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------


