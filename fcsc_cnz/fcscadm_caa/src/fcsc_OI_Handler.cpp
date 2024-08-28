
#include "fcsc_OI_Handler.h"

#include <syslog.h>
#include "ACS_CC_Types.h"
#include "fcscadm_Common.h"
#include<iostream>
using namespace std;
extern int flag;


SaAisErrorT fcsc_OI_Admin_Operator::register_object()
{
	mysyslog(LOG_INFO,"in fcsc_OI_Admin_Operator::register_object\n");
	SaAisErrorT rc;

	SaImmOiHandleT immOiHandle;

	fd_set readfds;
	int retval = -1; 
	
	static SaVersionT immVersion = { 'A', 2, 1 };
	
	rc = saImmOiInitialize_2(&immOiHandle, &callbacks, &immVersion);
        if( rc != SA_AIS_OK)
	{

		mysyslog(LOG_INFO,"saImmOiInitialize_2  ERROR CODE %u\n",rc);

		return rc;
        }


	//obj->setImmOiHandle(immOiHandle);

	// TBD
	SaImmOiImplementerNameT implementerName;

        implementerName = const_cast<char*>("ACS_SW_CONFIG");

        /**Set the implementer name specified in the implementerName for the handle immOihandle
         * and registers the invoking process as Object  implementer having the name which is
         * specified in the implementerName parameter.  **/
        SaAisErrorT errorCode = saImmOiImplementerSet(immOiHandle, implementerName);

        if( errorCode != SA_AIS_OK)
	{

		mysyslog(LOG_INFO,"saImmOiImplementerSet  ERROR CODE %u\n",errorCode);

		return errorCode;
        }

/*
        SaNameT objName;
        objName.length = obj->getObjName().length();
        objName.value[obj->getObjName().copy(reinterpret_cast<char *>(objName.value), SA_MAX_NAME_LENGTH)] = '\0';

        SaImmScopeT scope = (SaImmScopeT) obj->getScope();
*/

	rc = saImmOiClassImplementerSet( immOiHandle, (SaImmClassNameT) "APGSWUpdate");
        if( rc != SA_AIS_OK)
        {

                syslog(LOG_INFO,"saImmOiClassImplementerSet  ERROR CODE %d\n",rc);
                mysyslog(LOG_INFO,"saImmOiClassImplementerSet  ERROR CODE %u\n",rc);

                return rc;
        }

        SaSelectionObjectT selObj;

        /** this function returns the operating system handle associated with the handle
         * immOiHandle**/
        errorCode = saImmOiSelectionObjectGet(immOiHandle, &selObj);	
        if( errorCode != SA_AIS_OK ){

 		mysyslog(LOG_INFO,"aImmOiSelectionObjectGet RROR CODE %u\n",errorCode);

                return errorCode;
        }
	
	//obj->setSelObj(selObj);



	FD_ZERO(&readfds);
	FD_SET(selObj, &readfds);
	while(flag) {
		retval = select(selObj +1, &readfds, NULL,NULL,NULL);
		if(retval == -1)
			perror("select()");
		if(retval){
			if(FD_ISSET(selObj, &readfds)){

 				mysyslog(LOG_INFO,"select  ");

				 saImmOiDispatch(immOiHandle, SA_DISPATCH_ALL);
				//dispatch all the AMF pending function
			}
		}
	}
        return SA_AIS_OK; 

}

void* fcscactivate(void*)
{
   mysyslog(LOG_INFO, "!!! fcscactivate invoked--------");
}

void* fcscprepare(void*)
{
   mysyslog(LOG_INFO, "!!! fcscprepare invoked----------");
}

void* fcsctransfer(void*)
{
   mysyslog(LOG_INFO, "!!! fcsctransfer invoked----------");
}

void fcsc_OI_Admin_Operator::saImmOiAdminOperationCallback(SaImmOiHandleT immOiHandle,
					  SaInvocationT invocation,
					  const SaNameT *objectName,
					  SaImmAdminOperationIdT opId, const SaImmAdminOperationParamsT_2 **params)
{



	mysyslog(LOG_INFO,"saImmOiAdminOperationCallback");
	int worker_status;
	void* (* start_worker)(void *);
	pthread_t worker_thread;
	pthread_attr_t worker_attr;
	switch (opId)
	{
		case 1:
			mysyslog(LOG_INFO,"performing fcscactivate...");
			start_worker = &fcscactivate;
			break;

		case 2 :
			mysyslog(LOG_INFO,"performing fcscprepare...");
			start_worker = &fcscprepare;
			break;
                
                case 3:
			mysyslog(LOG_INFO,"performing fcsctransfer...");
			start_worker = &fcsctransfer;
			break;
		default:
		 	break;
	};

	pthread_attr_init(&worker_attr);
	pthread_attr_setdetachstate(&worker_attr, PTHREAD_CREATE_DETACHED);

    cout << "Spawning worker thread" << endl;
    char *msg1 = "Spawning worker thread";
    worker_status = pthread_create(&worker_thread, &worker_attr, start_worker, (void *)msg1);
    if (worker_status)
    {
       printf("ERROR; return code from pthread_create() is %d\n", worker_status);
       exit(-1);
    }
    pthread_attr_destroy(&worker_attr);
//TODO : Catch SIGUSR2 in worker, clean up & exit

	SaAisErrorT rc = saImmOiAdminOperationResult(immOiHandle, invocation, SA_AIS_OK);
	if(rc != SA_AIS_OK)
	{
		mysyslog(LOG_INFO,"Sending Result1  ...\n");

	}
	mysyslog(LOG_INFO,"Operation Success\n");

}

