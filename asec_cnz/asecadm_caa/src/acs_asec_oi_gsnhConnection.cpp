/*
 * * @file acs_asec_oi_gsnhConnection.cpp
 *	@brief
 *	Class method implementation for ACS_ASEC_OI_SimpleFile.
 *
 *  This module contains the implementation of class declared in
 *  the acs_asec_oi_gsnhConnection.h module
 *
 *
 *	COPYRIGHT Ericsson AB, 2010
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
        +          04/23/2020     ZCHIROS      Updated modify function to handle   +
	+				       NodeCredential and TrustCategory    +
	+					 in case of a string               +
 *      +          02/03/2022     XSIGANO      GSNH security enhancements          +
 *	+========+============+==============+=====================================+
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_asec_oi_gsnhConnection.h"


#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include "ACS_APGCC_Util.H"

#include <boost/format.hpp>
#include<fstream>
#include <sstream>
#include <list>
#include <iterator>
#include <acs_apgcc_oihandler_V3.h>
#include "acs_apgcc_omhandler.h"
class OmHandler;


ACS_ASEC_OI_GsnhConnection* ACS_ASEC_OI_GsnhConnection::gsnhInstance = NULL;
/*============================================================================
	ROUTINE: ACS_ASEC_OI_GsnhConnection
 ============================================================================ */
ACS_ASEC_OI_GsnhConnection::ACS_ASEC_OI_GsnhConnection() :

			acs_apgcc_objectimplementerinterface_V3(gsnhConnectionClass::dnName,gsnhConnectionClass::ImmImplementerName,ACS_APGCC_ONE),
			_secHandle(0),
			_nodecredentialSubscription(0),
			_trustcategorySubscription(0),
			_tCategory(NULL),
			_nodeCredentialId(""),
			_trustCategoryId(""),
			_fd(-1),
			_trustCatDirName(""),
			_trustCatFileName(""),
			_updated(false),
			m_EnabledTLSVersion(""),
			m_EnabledTLSCiphers(""),
			m_IsRestartTriggered(false)
{
	acs_asec_oi_gsnhConnectionTrace = new (std::nothrow) ACS_TRA_trace("ACS_ASEC_OI_GsnhConnection");
	ASEC_Log.Open("ASECBIN");
	is_security_toggled=false;
}

/*============================================================================
	ROUTINE: create
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_GsnhConnection::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering in create(...) callback");

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	
	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	UNUSED(className);
	UNUSED(parentname);
	UNUSED(attr);

	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving create(...)");
	return result;
}

/*============================================================================
	ROUTINE: deleted
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_GsnhConnection::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering deleted(...)");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	UNUSED(objName);

	

	//TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving deleted(...)");
	return result;
}

/*============================================================================
	ROUTINE: modify
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_GsnhConnection::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	const size_t msgLength = 255;
	char msg_buff[msgLength+1]={'\0'};

	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering modify(...)");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	ASEC_Log.Write(" ACS_ASEC_OI_GsnhConnection::modify(...) ", LOG_LEVEL_INFO);

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	UNUSED(objName);
	UNUSED(attrMods);

	fetchGsnhImmattribs(gsnhConnectionClass::dnName);	
	std::string security=string(m_security);
	if(attrMods[1]){
		int error=EXIT_GENERAL_FAULT;
		snprintf(msg_buff,msgLength,"Multiple attributes cannot be modified at a time ");
		ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
		syslog(LOG_ERR,"Multiple attributes cannot be modified at a time ");
		setExitCode(error, msg_buff);
		return ACS_CC_FAILURE;
	}
	if(security=="disabled" ||  ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"security") == 0 )
	{
		snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() after while() before if TrustCategoryId= %s NodeCredentialId=%s", m_TrustCategoryId.c_str(),m_NodeCredentialId.c_str());
		ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
		if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName, "nodeCredentialId") == 0 )
		{
			m_NodeCredentialId = reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]);
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() NodeCredentialId=%s", m_NodeCredentialId.c_str());
			ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"trustCategoryId") == 0 )
		{
			m_TrustCategoryId = reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]);
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() TrustCategoryId= %s NodeCredentialId=%s", m_TrustCategoryId.c_str(),m_NodeCredentialId.c_str());
			ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"security") == 0 )
		{	
			is_security_toggled=true;
			ACE_OS::strcpy(m_security , reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]));
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() security=%s m_security=%s", security.c_str(),m_security);
			ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
			if(ACE_OS::strcmp(m_security,security.c_str()) != 0)
			{
				//Check if security attribute is Enabled
				if(isSecurityEnabled())
				{
					snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() security=%s", m_security);
					ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
				}
				//Check if Security attribute is Disabled
				else 
				{
					snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() security=%s", m_security);
					ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
				}
			}
			else
			{
				is_security_toggled=false;
				snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() security is already %s", m_security);
				ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
				result = ACS_CC_SUCCESS;
			}
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"confTLSversion") == 0 || ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"isRestartTriggered") == 0|| ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"confCiphersList") == 0 )
		{
			int error=EXIT_GENERAL_FAULT;
			snprintf(msg_buff,msgLength,"Failed to update attribute  since security is in disabled state ");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
			syslog(LOG_ERR, "Failed to update attribute  since security is in disabled state ");
			result = ACS_CC_FAILURE;
			setExitCode(error, msg_buff);
		}
                else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"enabledTLSversion") == 0 ||  ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"enabledCiphersList") == 0)
                {
                        return ACS_CC_SUCCESS;
                }

		snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() After while() TrustCategoryId= %s NodeCredentialId=%s", m_TrustCategoryId.c_str(),m_NodeCredentialId.c_str());
		ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);

		TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving modify(...)");
	}
	else
	{
		if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"enabledTLSversion") == 0 )
		{
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() m_EnabledTLSVersion= %s",reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]));

			if(reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]))
			{
				m_EnabledTLSVersion = reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]);
			}
			else
			{
				m_EnabledTLSVersion.clear();
			}
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() m_EnabledTLSVersion= %s",m_EnabledTLSVersion.c_str());
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
                        is_security_toggled=false;
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"enabledCiphersList") == 0 )
		{
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() enabledCiphersList= %s",reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]));
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);

			if(reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]))
			{
				m_EnabledTLSCiphers = reinterpret_cast<char *>(attrMods[0]->modAttr.attrValues[0]);
			}
			else
			{
				m_EnabledTLSCiphers.clear();
			}
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() enabledCiphersList= %s",m_EnabledTLSCiphers.c_str());
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
                        is_security_toggled=false;
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"isRestartTriggered") == 0 )
		{
                        is_security_toggled=false;
			m_IsRestartTriggered = *(reinterpret_cast<int *>(attrMods[0]->modAttr.attrValues[0]));
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify() isRestartTriggered= %d",m_IsRestartTriggered);
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
			setExitCode(0);
			return ACS_CC_SUCCESS;
		}
		else if ( ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"confTLSversion") == 0 ||  ACE_OS::strcmp(attrMods[0]->modAttr.attrName,"confCiphersList") == 0)
		{
                        is_security_toggled=false;
			return ACS_CC_SUCCESS;
		}
		else
		{
			int error=EXIT_GENERAL_FAULT;
			snprintf(msg_buff,msgLength,"Failed to update attribute  since security is in enabled state ");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
			syslog(LOG_ERR, "Failed to update attribute  since security is in enabled state ");
			result = ACS_CC_FAILURE;
			setExitCode(error, msg_buff);
		}
	}
	return result;
}

/*============================================================================
	ROUTINE: complete
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_GsnhConnection::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	const size_t msgLength = 255;
	char msg_buff[msgLength+1]={'\0'};
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering complete(...)");
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);

	snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::complete() in complete");
	ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);

	if(is_security_toggled)
	{
		//is_security_toggled=false;
		if(isSecurityEnabled())
		{
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::complete() iin if enabled");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);
			try{
				performGsnhOperaions();
			}
			catch(int error)
			{
				is_security_toggled=false;
				result = ACS_CC_FAILURE;
				snprintf(msg_buff,msgLength,"Failed to perform Gsnh Operation gsnh_error_code=%d",error);
				ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
				//setExitCode(error code, errorDetail);
				setExitCode(error, msg_buff);
			}
		}
		else
		{
			if(isSubscribed())
			{
				//Unsubscribe to SecApi
				unsubscribe();
				restartApacheServer();		
			}
		}
	}
	else if (m_IsRestartTriggered)
	{
		snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::complete() is Restart triggered");
		ASEC_Log.Write(msg_buff, LOG_LEVEL_WARN);

		try{
			snprintf(msg_buff, msgLength, "calling perform GSNH");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_WARN);
			unsubscribe();
			performGsnhOperaions();
			m_IsRestartTriggered=false;
		}
		catch(int error)
		{
			m_IsRestartTriggered=false;
			result = ACS_CC_FAILURE;
			snprintf(msg_buff,msgLength,"Failed to perform Gsnh Operation gsnh_error_code=%d",error);
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
			setExitCode(error, msg_buff);
		}

	}
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving complete(...)");
	return result;
}
/*============================================================================
	ROUTINE: abort
 ============================================================================ */
void ACS_ASEC_OI_GsnhConnection::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering abort(...)");
	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	if(is_security_toggled && isSubscribed())
	{
		is_security_toggled=false;
		//Unsubscribe to SecApi
		unsubscribe();
	}
        if(m_IsRestartTriggered)
        {
                m_IsRestartTriggered=false;
        }


	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving abort(...)");
}

/*============================================================================
	ROUTINE: apply
 ============================================================================ */
void ACS_ASEC_OI_GsnhConnection::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering apply(...)");
	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(ccbId);
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving apply(...)");
}



/*============================================================================
	ROUTINE: updateRuntime
 ============================================================================ */
ACS_CC_ReturnType ACS_ASEC_OI_GsnhConnection::updateRuntime(const char* p_objName, const char** p_attrName)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace,"%s","Entering in updateRuntime()");

	// To avoid warning about unused parameter
	UNUSED(p_attrName);
	UNUSED(p_objName);

	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving updateRuntime");

	return result;
}



/*============================================================================
ROUTINE: adminOperationCallback
============================================================================ */
void ACS_ASEC_OI_GsnhConnection::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering adminOperationCallback(...)");

	// To avoid warning about unused parameter
	UNUSED(oiHandle);
	UNUSED(invocation);
	UNUSED(p_objName);
	UNUSED(operationId);
	UNUSED(paramList);
	// No actions are defined in CompositeFile class
	//adminOperationResult(oiHandle, invocation, actionResult::NOOPERATION);

	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving adminOperationCallback(...)");
}


void ACS_ASEC_OI_GsnhConnection::writeToSslConf(string certFileName,string keyFileName,string trustCategoryFileName,string confTLSVersion,string confTLSCiphers)
{
	string sslconf = "/etc/apache2/conf.d/ssl.conf";
	ofstream confile;
	if(confile)
	{
		confile.open("/etc/apache2/conf.d/ssl.conf");

		confile << "<IfDefine SSL>" << "\n";
		confile << "\t"<<"SSLEngine on"<<"\n";
		confile << "\t"<<"SSLCertificateFile "<< "\""<<certFileName<<"\"" <<"\n";
		confile << "\t"<<"SSLCertificateKeyFile "<< "\""<<keyFileName<<"\"" <<"\n";
		confile << "\t"<<"SSLCACertificateFile "<< "\""<<trustCategoryFileName<<"\"" <<"\n";
		confile << "\t"<<"SSLVerifyClient require"<<"\n";
		if (!confTLSVersion.empty())
			confile << "\t"<<"SSLProtocol "<<get_TLS_string(confTLSVersion)<<"\n";
		if (!confTLSCiphers.empty())
			confile << "\t"<<"SSLCipherSuite "<<confTLSCiphers<<"\n";

		confile << "</IfDefine>"<<"\n";

		confile.close();
		ASEC_Log.Write("Write to SSL.CONF file is sucessful", LOG_LEVEL_INFO);
	}
	else
	{
		ASEC_Log.Write("Failed to Write SSL.CONF file", LOG_LEVEL_ERROR);
		throw(EXIT_GENERAL_FAULT);	
	}
}

void ACS_ASEC_OI_GsnhConnection::updateApacheServiceFile()
{
        string pszReadLine,findstr,replacestr,fileName;
        vector <string> contentsBlock;
        vector<string>::iterator anotherHelper;
        fileName="/usr/lib/systemd/system/apache2.service";
        ifstream myfile(fileName.c_str());
        if(myfile.is_open())
        {
                while(getline(myfile,pszReadLine))
                {
                        contentsBlock.push_back(pszReadLine);
                }
        	myfile.close();
        }
        else
        {
                ASEC_Log.Write("Failed to open apache.service file inorder to update PrivateTmp value ", LOG_LEVEL_ERROR);
                throw(EXIT_GENERAL_FAULT);
        }
        findstr="PrivateTmp=true";
        replacestr="PrivateTmp=false";
  	bool need_to_update=false;
        for(unsigned int i=0;i<contentsBlock.size();i++)
        {
                if(strstr(contentsBlock[i].c_str(),findstr.c_str())!=NULL)
                {
                        contentsBlock[i]=replacestr;
                  	need_to_update=true;
                  	break;
                }
        }
  	if(need_to_update)
        {
        	ofstream output_file(fileName.c_str());
        	ostream_iterator<std::string> output_iterator(output_file, "\n");
        	copy(contentsBlock.begin(), contentsBlock.end(), output_iterator);
        	output_file.close();
        }
        contentsBlock.clear();

}


//Validate Node Cert files using openssl command
void ACS_ASEC_OI_GsnhConnection::validateNodeCertFile(string FileName)
{        const size_t msgLength = 300;
        char msg_buff[msgLength+1]={'\0'};

        snprintf(msg_buff, msgLength, "varify file=%s",FileName.c_str());
        ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);	
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering Cert validation");
	string validateCmd="openssl x509 -in ";
	validateCmd = validateCmd + FileName + " -text -noout";
        snprintf(msg_buff, msgLength, "varify cmd=%s",validateCmd.c_str());
        ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);	
	if(executeCmd(validateCmd))
	{
		ASEC_Log.Write("Validation of NodeCredential file is sucessful",LOG_LEVEL_INFO);
	}
	else
	{
		ASEC_Log.Write("Validation of NodeCredential file failed",LOG_LEVEL_ERROR);
		throw(EXIT_SSL_NOT_VALID);
	}
}
//Validate Trust Category files using openssl command
void ACS_ASEC_OI_GsnhConnection::validateTrustCatFile(string FileName)
{        const size_t msgLength = 300;
        char msg_buff[msgLength+1]={'\0'};

        snprintf(msg_buff, msgLength, "varify file=%s",FileName.c_str());
        ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);	
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering Cert validation");
	string validateCmd="openssl x509 -in ";
	validateCmd = validateCmd + FileName + " -text -noout";
                                snprintf(msg_buff, msgLength, "varify cmd=%s",validateCmd.c_str());
                                ASEC_Log.Write(msg_buff, LOG_LEVEL_INFO);	
	if(executeCmd(validateCmd))
	{
		ASEC_Log.Write("Validation of Trust Category file is sucessful",LOG_LEVEL_INFO);
	}
	else
	{
		ASEC_Log.Write("Validation of Trust Category file failed",LOG_LEVEL_ERROR);
		throw(EXIT_TRUSTED_CERTIFICATE_NOT_VALID);
	}
}

//Validate Key files using openssl command
void ACS_ASEC_OI_GsnhConnection::validateKeyFile(string FileName)
{	
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering Cert validation");
	string validateCmd="/usr/bin/openssl rsa -in ";
	validateCmd = validateCmd + FileName + " -text -noout";
	if(executeCmd(validateCmd))
	{
		ASEC_Log.Write("Validation of Gsnh Certificate KEY is sucessful",LOG_LEVEL_INFO);
	}
	else
	{
		ASEC_Log.Write("Validation of Gsnh Certificate KEY failed",LOG_LEVEL_ERROR);
		throw(EXIT_SSL_NOT_VALID);
	}
}

//Restart Apache Server
void ACS_ASEC_OI_GsnhConnection::restartApacheServer()
{	
	TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Restarting Apache Server");	
	string restartCmd="/usr/sbin/rcapache2 restart";
	if(executeCmd(restartCmd))
	{
		ASEC_Log.Write("Apache server restarted succesfully",LOG_LEVEL_INFO);
	}
	else
	{
		ASEC_Log.Write("Failed to restart Apache server",LOG_LEVEL_ERROR);
		throw(EXIT_GENERAL_FAULT);
	}

}

//Execute a Command
bool ACS_ASEC_OI_GsnhConnection::executeCmd(string command)
{
        TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Entering in executeCmd");
	string output;
        bool result = false;
        FILE* pipe = popen(command.c_str(), "r");
        if(NULL != pipe)
        {
                char rowOutput[1024]={'\0'};
                while(!feof(pipe))
        {
                        // get the cmd output
            if(fgets(rowOutput, 1023, pipe) != NULL)
            {
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

        TRACE(acs_asec_oi_gsnhConnectionTrace,"result:%s\n, command : %s\n, command output:%s\n", (result ? "TRUE" : "FALSE"), command.c_str(), output.c_str());
        TRACE(acs_asec_oi_gsnhConnectionTrace, "%s", "Leaving restartApacheServer");

        return result;
}

//Fetch GSNH Security realted Attributes from IMM
bool ACS_ASEC_OI_GsnhConnection::fetchGsnhImmattribs(string dnName)
{
	const size_t msgLength = 255;
	char msg_buff[msgLength+1]={'\0'};
	bool res = true;
	ACS_CC_ReturnType result;
	OmHandler omHandler;
	result = omHandler.Init();
	if (result != ACS_CC_SUCCESS)
	{
		res = false;
		snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::fetchImmattribs() failed to perform omHandler.Init()");
		ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
	}
	else
	{
		std::vector<ACS_APGCC_ImmAttribute *> gsnhAttributes;
		ACS_APGCC_ImmAttribute nodeCertId;
		nodeCertId.attrName="nodeCredentialId";
		gsnhAttributes.push_back(&nodeCertId);

		ACS_APGCC_ImmAttribute trustCatId;
		trustCatId.attrName="trustCategoryId";
		gsnhAttributes.push_back(&trustCatId);

		ACS_APGCC_ImmAttribute security;
		security.attrName="security";
		gsnhAttributes.push_back(&security);

		ACS_APGCC_ImmAttribute enabledTLSversion;
		enabledTLSversion.attrName="enabledTLSversion";
		gsnhAttributes.push_back(&enabledTLSversion);

		ACS_APGCC_ImmAttribute enabledCiphersList;
		enabledCiphersList.attrName="enabledCiphersList";
		gsnhAttributes.push_back(&enabledCiphersList);

		result = omHandler.getAttribute(dnName.c_str(), gsnhAttributes);
		if ( (result == ACS_CC_SUCCESS) && (0 !=nodeCertId.attrValuesNum) && (0 != trustCatId.attrValuesNum) && (0 != security.attrValuesNum))
		{
			if (( 0 != enabledTLSversion.attrValuesNum) && ( 0 != enabledCiphersList.attrValuesNum))
			{
				m_NodeCredentialId = reinterpret_cast<char *>(nodeCertId.attrValues[0]);
				m_TrustCategoryId = reinterpret_cast<char *>(trustCatId.attrValues[0]);
				strcpy(m_security,reinterpret_cast<char *>(security.attrValues[0]));
				m_EnabledTLSVersion = reinterpret_cast<char *>(enabledTLSversion.attrValues[0]);
				m_EnabledTLSCiphers = reinterpret_cast<char *>(enabledCiphersList.attrValues[0]);

				snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::fetchImmattribs() succesful to perform omHandler.getAttribute() Nodecredential=%s,Trustcaterory=%s,security=%s",m_NodeCredentialId.c_str(),m_TrustCategoryId.c_str(),m_security);
				ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);

				snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::fetchImmattribs() succesful to perform omHandler.getAttribute() enabledTLSversion=%s,enabledTLSciphers=%s",m_EnabledTLSVersion.c_str(),m_EnabledTLSCiphers.c_str());
				ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
			}
		}
		else
		{
			res = false;
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::fetchImmattribs() failed to perform omHandler.getAttribute()");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);

		}

		result = omHandler.Finalize();
		if (result != ACS_CC_SUCCESS)
		{
			res = false;
			snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::fetchImmattribs() failed to perform omHandler.Finalize()");
			ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
		}
	}

	return res;
}

//Performs required operations for Secured web connection
void ACS_ASEC_OI_GsnhConnection::performGsnhOperaions()
{
	try{
		if(isSecurityEnabled())
		{
			string certFileName,keyFileName,trustCategoryDir,trustCategoryFileName;

			//Subscribe NodeCredentialId and TrustCategoryId with SecApi
			subscribe(m_NodeCredentialId,m_TrustCategoryId);
			
			//Fetch NodeCredential File Name from SecApi
			certFileName=getNodeCertFileName();
			validateNodeCertFile(certFileName);
			
			//Fetch Key file Name from SecApi
			keyFileName=getNodeKeyFileName();
			validateKeyFile(keyFileName);
			
			//Fetch TrustCategory Directory and file Name from SecAPI
			trustCategoryDir=getTrustCatDirName();
			trustCategoryFileName=getTrustCatFileName();
			validateTrustCatFile(trustCategoryFileName);

			//Update ssl.conf file
			writeToSslConf(certFileName,keyFileName,trustCategoryFileName,m_EnabledTLSVersion,m_EnabledTLSCiphers);
			
			//Update privateTmp value in apache.service file
			updateApacheServiceFile();
				
			
			//Restar ApacaheServer
			restartApacheServer();	
		}


	}
	catch(int error)
	{
		if(isSubscribed())
		{
			//Unsubscribe to SecApi
			unsubscribe();
		}
		throw;
	}

}
void GsnhCertificate::clear()
{
        _pem.clear();
        _filename.clear();
}
const std::vector<GsnhCertificate>& ACS_ASEC_OI_GsnhConnection::getTrustCategory() const
{
        return _trustCategory;
}

const GsnhCertificate& ACS_ASEC_OI_GsnhConnection::getNodeCredentialCert() const
{
        return _nodeCredentialCert;
}

const GsnhCertificate& ACS_ASEC_OI_GsnhConnection::getNodeCredentialKey() const
{
        return _nodeCredentialKey;
}

bool ACS_ASEC_OI_GsnhConnection::isSubscribed() const
{

        if(_secHandle == 0)
                return false;
        return true;
}
int ACS_ASEC_OI_GsnhConnection::subscribe(const std::string& nodeCredentialId, const std::string& trustCategoryId)
{
        char trace[256] = {0};
        syslog(LOG_INFO,"Entering subscribe");
        if(isSubscribed()) {
                TRACE(acs_asec_oi_gsnhConnectionTrace,"%s", "subscribe called, but sec_credu api is not available");
		throw(EXIT_GENERAL_FAULT);
                //return -1;
        }
        snprintf(trace, sizeof(trace) - 1, "_fd=%d _TrustCategory = %s  _NodeCredential = %s;;;;NodeCredentialId=%s,TrustCategoryId=%s",_fd,_nodeCredentialId.c_str(),_trustCategoryId.c_str(),nodeCredentialId.c_str(),trustCategoryId.c_str());
        syslog(LOG_INFO,"%s",trace);

        if(_fd != -1 && _nodeCredentialId==nodeCredentialId && _trustCategoryId==trustCategoryId) {
                        TRACE(acs_asec_oi_gsnhConnectionTrace,"%s", "Already subscribed");
                return _fd;  // already subscribed
        } else {
                syslog(LOG_INFO, "unsubscribing inside subscribing");
                unsubscribe();
                if(nodeCredentialId.empty() || trustCategoryId.empty()) {
                        TRACE(acs_asec_oi_gsnhConnectionTrace,"%s", "nodeCredentialId or trustCategoryId is empty, so not calling secapi subscribe again");
                        _fd = -1;
			throw(EXIT_GENERAL_FAULT);
                        //return _fd;
                }
        }

        snprintf(trace, sizeof(trace) - 1, "TrustCategory = %s  NodeCredential = %s",
                        trustCategoryId.c_str() ,nodeCredentialId.c_str());
        syslog(LOG_INFO, "%s", trace);

        SecCreduVersion version;
        version.release_code  = 'A';
        version.major_version = 0x01;
        version.minor_version = 0x01;

        // Parameters to be used.
        SecCreduParameters parameters;
        parameters.nodecredential_change_callback = nodeCredentialChangeCallback;
        parameters.trustcategory_change_callback  = trustCategoryChangeCallback;

        // Initialize the API.
        SecCreduStatus retval;
        if (SEC_CREDU_OK == (retval=sec_credu_initialize(&_secHandle, &parameters, &version))) {
                // Subscribe
                syslog(LOG_INFO,"nodeCredentialId=%s ,trustCategoryId=%s",nodeCredentialId.c_str(),trustCategoryId.c_str());
                if (SEC_CREDU_OK != (retval=sec_credu_nodecredential_subscribe(_secHandle, nodeCredentialId.c_str(), &_nodecredentialSubscription))) {
                        syslog(LOG_INFO, "sec_credu_nodecredential_subscribe failed, retval=%s, nodeCredentialId %s ",sec_credu_status_string(retval), nodeCredentialId.c_str());
                } else if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_subscribe(_secHandle, trustCategoryId.c_str(), &_trustcategorySubscription))) {
                        syslog(LOG_INFO, "sec_credu_trustcategory_subscribe failed, retval=%s, trustCategoryId %s",sec_credu_status_string(retval), trustCategoryId.c_str() );
                } else if (SEC_CREDU_OK != (retval=sec_credu_selectionobject_get(_secHandle, &_fd))) {
                        syslog(LOG_INFO, "sec_credu_selectionobject_get failed, retval=%s",sec_credu_status_string(retval));
                } else { // success
                        fetchTrustCategory();
                        fetchNodeCert();

                        _nodeCredentialId=nodeCredentialId;
                        _trustCategoryId=trustCategoryId;
                }
        } else {
                syslog(LOG_INFO, "sec_credu_initialize failed: retval %s",sec_credu_status_string(retval));
                _secHandle = 0;
                _fd = -1;
		throw(EXIT_GENERAL_FAULT);
                //return _fd;
        }

        if(SEC_CREDU_OK != retval) {
                syslog(LOG_INFO, "Failed to subscribe sec_credu api: retval %s",sec_credu_status_string(retval));
                unsubscribe();
                _fd = -1;
		throw(EXIT_GENERAL_FAULT);
        }

        syslog(LOG_INFO,"Exiting subscribe");

        return _fd;
}


void ACS_ASEC_OI_GsnhConnection::unsubscribe()
{

        bool success = true;
        SecCreduStatus retval;

	syslog(LOG_INFO,"Entering unsubscribe");
	
	string sslconf = "/etc/apache2/conf.d/ssl.conf";
	ofstream confile;
	//To clear ssl.conf file
	confile.open(sslconf.c_str());
	if(confile)
	{
		confile.close();
	}

        if(_secHandle != 0) {
                _nodeCredentialId.clear();
                _trustCategoryId.clear();
                _nodeCredentialCert.clear();
                _nodeCredentialKey.clear();
                _trustCategory.clear();
                _trustCatDirName.clear();
                _trustCatFileName.clear();


                // Free the allocated memory.
                if (_tCategory) {
                        if ( (SEC_CREDU_OK != (retval = sec_credu_trustcategory_free(_secHandle, &_tCategory)))) {
                                syslog(LOG_INFO, "sec_credu_trustcategory_free failed, retval=%s",sec_credu_status_string(retval));
                        }
                }

                if (SEC_CREDU_OK != sec_credu_nodecredential_unsubscribe(_secHandle, _nodecredentialSubscription)) success=false;
                _nodecredentialSubscription = 0;

                if (SEC_CREDU_OK != sec_credu_trustcategory_unsubscribe(_secHandle, _trustcategorySubscription)) success=false;
                _trustcategorySubscription = 0;

                if (SEC_CREDU_OK != sec_credu_finalize(_secHandle)) success=false;
                _secHandle = 0;
                _fd = -1;
                if(!success) {
                        TRACE(acs_asec_oi_gsnhConnectionTrace,"%s", "failed to unsubscribe SEC subscription");
                }
        }
        syslog(LOG_INFO,"Exiting unsubscribe");
}
void ACS_ASEC_OI_GsnhConnection::fetchNodeCert()
{
  SecCreduStatus retval;

	TRACE(acs_asec_oi_gsnhConnectionTrace,"%s", "fetchNodeCert");

  if(_secHandle != 0){
    _nodeCredentialCert.clear();
    _nodeCredentialKey.clear();

    char *certContent = NULL;
    char *certFilename = NULL;
    char *keyContent = NULL;
    char *keyFilename = NULL;

    if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle, _nodecredentialSubscription,
        SEC_CREDU_FILENAME, &certFilename))) {
      syslog(LOG_INFO, "sec_credu_nodecredential_cert_get FILENAME failed, retval=%s",sec_credu_status_string(retval));
    } else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_cert_get(_secHandle,
        _nodecredentialSubscription, SEC_CREDU_PEM, &certContent))) {
      syslog(LOG_INFO, "sec_credu_nodecredential_cert_get PEM failed, retval=%s",sec_credu_status_string(retval));
    } else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
        _nodecredentialSubscription, SEC_CREDU_FILENAME, &keyFilename))) {
      syslog(LOG_INFO, "sec_credu_nodecredential_key_get FILENAME failed, retval=%s",sec_credu_status_string(retval));
    } else if (SEC_CREDU_OK != (retval= sec_credu_nodecredential_key_get(_secHandle,
        _nodecredentialSubscription, SEC_CREDU_PEM, &keyContent))) {
      syslog(LOG_INFO, "sec_credu_nodecredential_key_get PEM failed, retval=%s",sec_credu_status_string(retval));
    }

    if(retval == SEC_CREDU_OK) {
      if((certFilename != NULL)&&(certContent!=NULL))
      {
      _nodeCredentialCert = GsnhCertificate(certFilename, certContent);
      }
      if((keyFilename != NULL)&&(keyContent!=NULL))
      {
      _nodeCredentialKey = GsnhCertificate(keyFilename, keyContent);
      }
    free(certContent);
    free(certFilename);
    free(keyFilename);
    free(keyContent);
   }
}
}
void ACS_ASEC_OI_GsnhConnection::fetchTrustCategory()
{
        bool success = true;

        SecCreduTrustCategory *category = NULL;
        SecCreduStatus retval;
        syslog(LOG_INFO, " Inside fetchTrustCategory");

        if(_secHandle == 0){
                return;
        }
        _trustCategory.clear();
        _trustCatDirName.clear();
        _trustCatFileName.clear();
        char *trustCatDirName = NULL;

        // Fetch information on the TrustCategory instance.
        if (SEC_CREDU_OK != (retval= sec_credu_trustcategory_get(_secHandle, _trustcategorySubscription, &category))) {
                syslog(LOG_INFO, "sec_credu_trustcategory_get failed, retval=%s",sec_credu_status_string(retval));
                success = false;
        } else if(SEC_CREDU_OK != (retval= sec_credu_trustcategory_dirname_get(category, &trustCatDirName))) {
                syslog(LOG_INFO, "sec_credu_trustcategory_dirname_get failed, retval=%s",sec_credu_status_string(retval));
                trustCatDirName = NULL;
                success = false;
        } else {
                char* trustedCertFilename = NULL;
                char* trustedCertContent = NULL;
                char* trustedCertId = NULL;
                char* trustedCertId2 = NULL;

                _trustCatDirName = (trustCatDirName)?std::string(trustCatDirName):std::string();

                syslog(LOG_INFO, " fetching directory");
                size_t numTrustCategoryCerts=0;
                sec_credu_trustcategory_cert_count_get(category, &numTrustCategoryCerts);

                for (size_t i = 0; i < numTrustCategoryCerts; ++i) {
                        bool ok = true;
                        if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_FILENAME, i,
                                        &trustedCertFilename,
                                        &trustedCertId))) {
                                syslog(LOG_INFO, "sec_credu_trustcategory_cert_get FILENAME failed, retval=%s",sec_credu_status_string(retval));
                                trustedCertFilename=NULL;
                                trustedCertId=NULL;
                                ok=false;
                        }

                        if (SEC_CREDU_OK != (retval=sec_credu_trustcategory_cert_get(category, SEC_CREDU_PEM, i,
                                        &trustedCertContent,
                                        &trustedCertId2))) {
                                syslog(LOG_INFO, "sec_credu_trustcategory_cert_get PEM failed, retval=%s",sec_credu_status_string(retval));
                                trustedCertContent=NULL;
                                trustedCertId2=NULL;
                                ok=false;
                        }

                        if(ok) {
                                _tCategory = category;
                                _trustCategory.push_back(GsnhCertificate(trustedCertFilename, trustedCertContent));
                                _trustCatFileName=string(trustCatDirName+string("/")+trustedCertFilename);

                        } else {
                                success = false;
                        }
                        free(trustedCertFilename);
                        free(trustedCertContent);
                        free(trustedCertId);
                        free(trustedCertId2);
                }

                free(trustCatDirName);
        }
        if(success) {
                syslog(LOG_INFO, " fetching Success");

                return;
        }

        syslog(LOG_INFO, "fetchTrustCategory failed");
        _trustCategory.clear();
}
ACS_ASEC_OI_GsnhConnection* ACS_ASEC_OI_GsnhConnection::getInstance()
{

        if(!gsnhInstance)
                gsnhInstance = new ACS_ASEC_OI_GsnhConnection();

        return gsnhInstance;
}

std::string ACS_ASEC_OI_GsnhConnection::getNodeKeyFileName() const
{
        return _nodeCredentialKey.getFilename();
}

std::string ACS_ASEC_OI_GsnhConnection::getNodeCertFileName() const
{
        return _nodeCredentialCert.getFilename();
}

std::string ACS_ASEC_OI_GsnhConnection::getTrustCatDirName() const
{
        return _trustCatDirName;
}

std::string ACS_ASEC_OI_GsnhConnection::getTrustCatFileName() const
{
        return _trustCatFileName;
}
void ACS_ASEC_OI_GsnhConnection::nodeCredentialChangeCallback(
                SecCreduHandle /*handle*/, SecCreduSubscription /*nodecredential_subscription*/, const char */*nodecredential_id*/)
{
        syslog(LOG_INFO, "Modify nodeCredentialChangeCallback");
        ACS_ASEC_OI_GsnhConnection *pinstance = ACS_ASEC_OI_GsnhConnection::getInstance();
        if (pinstance) {
                pinstance->fetchNodeCert();
                pinstance->_updated=true;
        }
}

void ACS_ASEC_OI_GsnhConnection::trustCategoryChangeCallback(
                SecCreduHandle /*handle*/, SecCreduSubscription /*trustcategory_subscription*/, const char */*trustcategory_id*/)
{


        syslog(LOG_INFO, "Modify trustCategoryChangeCallback");
        ACS_ASEC_OI_GsnhConnection *pinstance = ACS_ASEC_OI_GsnhConnection::getInstance();
        if (pinstance) {
                pinstance->fetchTrustCategory();
                pinstance->_updated=true;

        }
}
void ACS_ASEC_OI_GsnhConnection::finalize()
{
        if (gsnhInstance)
                delete(gsnhInstance);
        gsnhInstance = 0;
}
bool ACS_ASEC_OI_GsnhConnection::isSecurityEnabled()
{

        if(ACE_OS::strcmp(m_security,"enabled") == 0)
        {
                return true;
        }
        return false;
}


/*===========================================================================
ROUTINE: setIsRestartTriggered
 ============================================================================*/
bool ACS_ASEC_OI_GsnhConnection::setIsRestartTriggered(int isRestartTriggered)
{
             bool res = true;

         ACS_CC_ImmParameter parToModify = defineParameterInt(gsnhConnectionClass::ATTR_ISRESTARTED,ATTR_INT32T,&isRestartTriggered,1);
         if (modify_OM_ImmAttr(gsnhConnectionClass::dnName,parToModify)== ACS_CC_FAILURE) res = false;

         return res;
}
/*============================================================================
        ROUTINE: clearConfTLSversion
 ============================================================================*/
bool ACS_ASEC_OI_GsnhConnection::clearConfTLSversion()
{
             std::string confTLSversion;
             confTLSversion.clear();
             bool res = true;

         ACS_CC_ImmParameter parToModify = defineParameterString(gsnhConnectionClass::ATTR_CONFTLSVERSION,ATTR_STRINGT,confTLSversion.c_str(),1);
         if (modify_OM_ImmAttr(gsnhConnectionClass::dnName,parToModify)== ACS_CC_FAILURE) res = false;

         return res;
}
/*============================================================================
        ROUTINE: clearConfTLSciphers
 ============================================================================*/
bool ACS_ASEC_OI_GsnhConnection::clearConfTLSciphers()
{
             std::string clearConfTLSciphers;
             clearConfTLSciphers.clear();
             bool res = true;

         ACS_CC_ImmParameter parToModify = defineParameterString(gsnhConnectionClass::ATTR_CONFTLSCIPHERS,ATTR_STRINGT,clearConfTLSciphers.c_str(),1);
         if (modify_OM_ImmAttr(gsnhConnectionClass::dnName,parToModify)== ACS_CC_FAILURE) res = false;

         return res;
}
/*============================================================================
        ROUTINE: modify_OM_ImmAttr
 ============================================================================*/
bool ACS_ASEC_OI_GsnhConnection::modify_OM_ImmAttr(const char *object, ACS_CC_ImmParameter parameter)
{
        const size_t msgLength = 255;
        char msg_buff[msgLength+1]={'\0'};
        {
                snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify_OM_ImmAttr() object name=%s",object);
                ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
        }

        ACS_CC_ReturnType result;
        bool res = true;
        OmHandler omHandler;

        result = omHandler.Init();
        if (result != ACS_CC_SUCCESS)
        {
                res = false;
        }
        else
        {
                result = omHandler.modifyAttribute(object,&parameter);
                if (result != ACS_CC_SUCCESS)
                {
                        int id = 0;
                        char * text;
                        id = omHandler.getInternalLastError();
                        text = omHandler.getInternalLastErrorText();
                        snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify_OM_ImmAttr() failed to perform modify attribute id=%d,Text=%s",id, text);
                        ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
                        res = false;
                }
                snprintf(msg_buff, msgLength, "ACS_ASEC_OI_GsnhConnection::modify_OM_ImmAttr() succesful to perform modify attribute");
                ASEC_Log.Write(msg_buff, LOG_LEVEL_ERROR);
                result = omHandler.Finalize();
                if (result != ACS_CC_SUCCESS)
                {
                        res = false;
                }
        }

        return res;
}

/*==========================================================================
        ROUTINE: defineParameterInt
 ============================================================================ */
ACS_CC_ImmParameter ACS_ASEC_OI_GsnhConnection::defineParameterInt(const char* attributeName, ACS_CC_AttrValueType type, int *value, unsigned int numValue)
{
        ACS_CC_ImmParameter parToModify;

        char *name_attrUnsInt32 = const_cast<char*>(attributeName);
        parToModify.attrName = name_attrUnsInt32;
        parToModify.attrType = type;
        parToModify.attrValuesNum = numValue;

        if (parToModify.attrValuesNum == 0)     parToModify.attrValues = 0;
        else
        {
                parToModify.attrValues=new void*[parToModify.attrValuesNum];
                parToModify.attrValues[0] =reinterpret_cast<void*>(value);
        }

        return parToModify;

}
/*==========================================================================
        ROUTINE: defineParameterString
 ============================================================================ */
ACS_CC_ImmParameter ACS_ASEC_OI_GsnhConnection::defineParameterString(const char* attributeName, ACS_CC_AttrValueType type, const char* value, unsigned int numValue)
{
        ACS_CC_ImmParameter parToModify;

        char* stringValue = const_cast<char *>(value);
        char *name_attrString = const_cast<char*>(attributeName);
        parToModify.attrName = name_attrString;
        parToModify.attrType = type;
        parToModify.attrValuesNum = numValue;

        if (parToModify.attrValuesNum == 0)     parToModify.attrValues = 0;
        else
        {
                parToModify.attrValues=new void*[parToModify.attrValuesNum];
                parToModify.attrValues[0] =reinterpret_cast<void*>(stringValue);
        }


        return parToModify;

}

/*==========================================================================
        ROUTINE: defineParameterString
 ============================================================================ */
std::string ACS_ASEC_OI_GsnhConnection::get_TLS_string(std::string tls_input_str)  
{
  
   std::string delim = " "; // delimiter  
   size_t pos = 0;  
   std::string token1;      // define a string variable  
   std::string tls_formatted_string;

  // use find() function to get the position of the delimiters  
  while (( pos = tls_input_str.find (delim)) != std::string::npos)  
  {  
   token1 = tls_input_str.substr(0, pos); // store the substring 
   token1 = "+" + token1 + " ";
   tls_formatted_string = tls_formatted_string.append(token1);
   tls_input_str.erase(0, pos + delim.length());  
   /* erase() function store the current positon and move to next token. */   
  }  
  
  if (!tls_input_str.empty())
  {
    token1 = "+" + tls_input_str;
    tls_formatted_string = tls_formatted_string.append(token1);
  } 
 
  return tls_formatted_string;

}
/*==========================================================================
	ROUTINE: ~ACS_ASEC_OI_GsnhConnection
 ============================================================================ */
ACS_ASEC_OI_GsnhConnection::~ACS_ASEC_OI_GsnhConnection()
{
	if(isSubscribed())
	{
		//Unsubscribe to SecApi
		unsubscribe();
	}
	ASEC_Log.Close();
	if(NULL != acs_asec_oi_gsnhConnectionTrace)
		delete acs_asec_oi_gsnhConnectionTrace;
}
