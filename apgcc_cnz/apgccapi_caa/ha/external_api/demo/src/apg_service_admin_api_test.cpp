/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "stdio.h"
#include "stdlib.h"
#include "ace/ACE.h"
#include "APGCC_Service_Operation.h"

void fflushstdin( void )
{
	int c;
	while((c = fgetc( stdin )) != EOF && c != '\n') ;
}

/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN()
{
	ACE_INT32 option;
	ACE_INT32 rc = 0;
	ACE_TCHAR service[256] = {'\0'};

	APGCC_ServiceAdminOps apg_service_adminop;

	while(1){
		ACE_OS::printf("\nChoose the operation to be performed:\n");
		ACE_OS::printf("1:Stop APG service\n");
		ACE_OS::printf("2:Start APG service\n");
		ACE_OS::printf("3:Reboot passive APG node\n");
		ACE_OS::printf("4:Exit\n");

		option = ACE_OS::fgetc(stdin);
		fflushstdin();

		switch(option){
			
			case '1': ACE_OS::printf("Enter name of APG service to be stopped:\n");
				scanf("%s",service);
				fflushstdin();
				rc = apg_service_adminop.apgServiceStop(service);
				if(!rc)
					ACE_OS::printf("Service \"%s\" is down now\n",service);
				else 
					ACE_OS::printf("Service \"%s\" stop failed\n",service);
				break;
			case '2': ACE_OS::printf("Enter name of APG service to be started:\n");
				scanf("%s",service);
				fflushstdin();
				rc = apg_service_adminop.apgServiceStart(service);
				if(!rc)
					ACE_OS::printf("Service \"%s\" is up now\n",service);
				else 
					ACE_OS::printf("Service \"%s\" start failed\n",service);
				break;
			case '3': 
				rc = apg_service_adminop.apgPassiveNodeRestart();
				if(!rc)
					ACE_OS::printf("Passive node reboot initiated\n",service);
				else 
					ACE_OS::printf("Passive node reboot failed\n",service);
				break;
			case '4': 
				ACE_OS::exit(0);
				break;
			default: 
				ACE_OS::printf("Invalid option chosen\n");
				break;
		}
	}

	return rc;
}

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
