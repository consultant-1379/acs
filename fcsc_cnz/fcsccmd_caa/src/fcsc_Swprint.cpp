/*=================================================================== */
   /**
   @file fcsc_Swprint.cpp

   Class method implementation for SWUpdate module.

   This module contains the implementation of class declared in
   the SWUpdate module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
#include "fcsc_Swprint.h"

/*===================================================================
   ROUTINE: fcsc_sw_label_print
=================================================================== */
int fcsc_AdminOperations::fcsc_sw_label_print(fcsc_AdminOperations &fcscAdminOperator)
{
        char* nodeVersion;
	
	//cout<<"calling fcsc_AdminOperations::fcsc_sw_label_print"<<endl;

	int res=fcscAdminOperator.getFcscAttribute(SW_CONFIG_OBJ_DNAME,"currentNodeAversion",nodeVersion);

       if(res==0)
	{
        	if(ACE_OS::strcmp(nodeVersion , "") !=0)
        	{
           	cout<<"Software Label of Node A is: "<<nodeVersion<<endl;
        	}
        	else
        	{
           	cout<<"Software Version of Node A is not Available"<<endl;
        	}
	}
	else
	{
		return -1;
	}
        
	res=fcscAdminOperator.getFcscAttribute(SW_CONFIG_OBJ_DNAME,"currentNodeBversion",nodeVersion);
	if(res==0)
	{
        	if(ACE_OS::strcmp(nodeVersion , "") !=0)
        	{
           	cout<<"Software Label of Node B is: "<<nodeVersion<<endl;
        	}
        	else
        	{
          	 cout<<"Software Version of Node B is not Available"<<endl;
        	}
	}
	else
	{
		return -1;
	}
	return 0;
}
/* end fcsc_sw_label_print */
/*===================================================================
   ROUTINE: getFcscAttribute
=================================================================== */
int fcsc_AdminOperations::getFcscAttribute(const char* dn, const char* attrName,char* attrValue)
{
      
        ACS_CC_ImmParameter paramToFind;
        paramToFind.attrName = (char*) attrName;
	OmHandler fcscAttrHandler;
        if( fcscAttrHandler.Init() == ACS_CC_FAILURE )
        {
               cout<<"Error in reading from IMM"<<endl;
		return -1;
        }
	//cout<<"success in INit"<<endl;
        if (fcscAttrHandler.getAttribute(dn, &paramToFind )!=ACS_CC_SUCCESS)
        {
                cout<<"Error in reading from IMM"<<endl;
		return -1;
        }
        else
        {
                if(paramToFind.attrValuesNum == 0)
                {
                  cout<<"The attribute has no value set\n";
                }
                else
                {
                  strcpy(attrValue,(reinterpret_cast<char*>(*(paramToFind.attrValues))));
                //  cout<<"The attribute has value get\n";
                }
	}

	if(fcscAttrHandler.Finalize() != ACS_CC_SUCCESS)
	{

		cout<<"Finalize Failure "<<endl;
	}
	return 0;
}
/* end getFcscAttribute */
/*===================================================================
   ROUTINE: invokeAdminOperation
=================================================================== */
SaAisErrorT  fcsc_AdminOperations::invokeAdminOperation(const std::string & i_dn, unsigned int i_operationId ,
                                const SaImmAdminOperationParamsT_2 ** i_params, SaTimeT i_timeout )
{
        SaAisErrorT returnValue;
        SaAisErrorT rc;
        SaNameT objectName;
        objectName.length = i_dn.length();
        memcpy(objectName.value, i_dn.c_str(), objectName.length);

        const SaNameT *objectNames[2];
        objectNames[0] = &objectName;
        objectNames[1] = NULL;
        SaImmAdminOwnerHandleT m_ownerHandle;
        SaImmHandleT m_omHandle;

        //SaImmAdminOperationParamsT_2 **params;
        i_params = ( const SaImmAdminOperationParamsT_2 ** )realloc(NULL, sizeof(SaImmAdminOperationParamsT_2 *));
        i_params[0] = NULL;

        SaVersionT s_immVersion = { 'A', 2, 1 };



        rc = saImmOmInitialize(&m_omHandle, NULL, &s_immVersion);
        if( rc != SA_AIS_OK)
        {

                syslog(LOG_INFO,"saImmOmInitialize  ERROR CODE %u\n",rc);

                return rc;
        }


        rc = saImmOmAdminOwnerInitialize(m_omHandle, (char*)"FCSCSERVICE", SA_TRUE, &m_ownerHandle);
        if( rc != SA_AIS_OK)
        {

                syslog(LOG_INFO,"saImmOmAdminOwnerInitialize  ERROR CODE %u\n",rc);

                return rc;
        }

        rc = saImmOmAdminOwnerSet(m_ownerHandle, objectNames, SA_IMM_ONE);
        if( rc != SA_AIS_OK)
        {

                syslog(LOG_INFO,"saImmOmAdminOwnerSet  ERROR CODE %u\n",rc);
                rc = saImmOmAdminOwnerFinalize(m_ownerHandle);
                if (SA_AIS_OK != rc) {
                        fprintf(stderr, "error - saImmOmAdminOwnerFinalize FAILED: %d\n", (rc));
                        exit(EXIT_FAILURE);
                }
                rc = saImmOmFinalize(m_omHandle);
                if (SA_AIS_OK != rc) {
                        fprintf(stderr, "error - saImmOmFinalize FAILED: %d\n", (rc));
                        exit(EXIT_FAILURE);
                }



                return rc;
        }
                printf("saImmOmAdminOwnerSet  successful  %u\n",rc);
                syslog(LOG_INFO,"saImmOmAdminOwnerSet  successful  %u\n",rc);


        rc = saImmOmAdminOperationInvoke_2(m_ownerHandle, &objectName, 0, i_operationId,i_params,
                                                           &returnValue, SA_TIME_ONE_SECOND * 60);
        if( rc != SA_AIS_OK)
        {

                syslog(LOG_INFO,"saImmOmAdminOperationInvoke_2  ERROR CODE %u\n",rc);

        }

        // Clean Up the Admin Handle and OM Hanlde

        rc = saImmOmAdminOwnerFinalize(m_ownerHandle);
        if (SA_AIS_OK != rc)
        {
                fprintf(stderr, "error - saImmOmAdminOwnerFinalize FAILED: %d\n", (rc));
                exit(EXIT_FAILURE);
       }
       rc = saImmOmFinalize(m_omHandle);
                if (SA_AIS_OK != rc) {
                        fprintf(stderr, "error - saImmOmFinalize FAILED: %d\n", (rc));
                        exit(EXIT_FAILURE);
                }



        return rc;
}
/* end invokeAdminOperation */

/*===================================================================
   ROUTINE: swprint_usage
=================================================================== */
void swprint_usage()
{
   cout << "Syntax Error :"<< endl;
   cout << "Usage :"<< endl;
   cout << "Format: swprint"<<endl;
}
/* end swprint_usage */

/* Define Operations Id according to the system usecases*/
/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR *argv[])
{
        if(argc != 1)
        {
                swprint_usage();
                return -1;
        }
        if (!strcmp(argv[0], "swprint"))
        {
                swprint_usage();
                return -1;
        }
        fcsc_AdminOperations obj;
        obj.fcsc_sw_label_print(obj);
        return 0;
}

/* end ACE_TMAIN */
