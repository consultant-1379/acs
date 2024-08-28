#include <sys/time.h>
#include <ace/ACE.h>
#include <ace/Handle_Set.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>

#include "fcsc_application.h"
#include "fcscadm_Common.h"
#include "fcsc_OI_Handler.h"
#define SELECT_TIMEOUT_SEC 100
#define SELECT_TIMEOUT_USEC 0


ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR **argv)
{
	ACE_Handle_Set fd_set;
	ACE_INT32 retval = ACS_APGCC_SUCCESS;
	ACS_APGCC_ReturnType rc = ACS_APGCC_SUCCESS;
	ACE_Time_Value timeout;
	int length = strlen("FCSCHK1");
#if 1
	ACS_APGCC_AMF_HealthCheckKeyT healthcheckkey = {"FCSCHK1",length};


	myAPGApp1Application *myApplication = new myAPGApp1Application;

	mysyslog(LOG_INFO, " apg_app1 service is about to start!!!    ");

	// initialize application 
	rc = myApplication->myAppInitialize();
	if(rc != ACS_APGCC_SUCCESS){
		mysyslog(LOG_ERR, "Application Initialization Failed FAILED - %u", retval);
		return rc;
	}

	mysyslog(LOG_INFO, "Application Initialization done!!!   ");

	rc = myApplication->coreMWInitialize(&healthcheckkey);
	if(rc != ACS_APGCC_SUCCESS){
		mysyslog(LOG_ERR, "coreMWInitialize FAILED - %u", rc);
		return rc;
	}
	mysyslog(LOG_INFO, "Application coreMWInitialize done!!!   \n ");

	// Set the timeout values
	timeout.set(SELECT_TIMEOUT_SEC,SELECT_TIMEOUT_USEC);
	fd_set.set_bit((ACE_HANDLE)myApplication->getSelObj());  // getSelObj returns int handle. typecast to ACE_HANDLE
       
//     fcsc_OI_Admin_Operator adminOperator;
//     adminOperator.register_object();

	while(1) {

		retval = ACE::select(fd_set.max_set()+1, &fd_set, NULL,NULL,NULL);
		if(retval == -1){
			if (errno == EINTR)
				continue;
			mysyslog(LOG_ERR,"Select Failed - %s",strerror(errno));
			break;
		}else if(retval == 0){
                        /*  timeout happens on select*/
                        // You can continue or do application specific stuff
						mysyslog(LOG_INFO, "timeout on ACE::select");
						continue;
                }
		if(fd_set.is_set((ACE_HANDLE)myApplication->getSelObj())){
			//dispatch all the AMF pending function 
			retval = myApplication->dispatch(ACS_APGCC_AMF_DISPATCH_ALL);
			if(retval != ACS_APGCC_SUCCESS)
				mysyslog(LOG_ERR, "myApplication dispatch FAILED - %u", retval);
		}



	}
	

	// Time to release the handles we won

	fd_set.clr_bit( (ACE_HANDLE)myApplication->getSelObj() );

	if ((rc = myApplication->finalize()) != ACS_APGCC_SUCCESS ) {
		mysyslog(LOG_ERR, "myApplication finalize FAILED");
	}
#endif
	return rc;

}

