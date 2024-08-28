//******************************************************************************
//
//  NAME
//  acs_rtr_lib.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 20012. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//
//  AUTHOR 
//     2012-12-05 by XSAMECH	 
//
// RELEASE REVISION HISTORY
//	REV NO          DATE          NAME 	DESCRIPTION
//	A               2012-12-05    XSAMECH	Product release
//******************************************************************************
#include "acs_rtr_lib.h"
#include "acs_rtr_tracer.h"
#include "acs_rtr_defaultvalues.h"
#include "acs_rtr_systemconfig.h"
#include "ACS_APGCC_CommonLib.h"

#include "ace/Recursive_Thread_Mutex.h"
#include <boost/filesystem.hpp>

#include <cstring>
#include <iostream>
#include <crypto_status.h>
#include <crypto_api.h>

#define rtrEcimPasswordAttr "password"
#define MAXSTRLEN(s) ( sizeof(s) - strlen(s) - 1 )
#define messageStoreNameAttr "messageStoreName"
#define BLD_SITE "message_store"
#define RPT_SITE "transfer_store"
#define OPEN 0
struct string64
{
	char str[64];
};
struct string128
{
	char str[128];
};

ACS_RTR_TRACE_DEFINE(ACS_RTR_API);

ACE_Recursive_Thread_Mutex _tqCriticalSection;

//*************************************************************************
// getRTRdir
//
// Fetches the RTR base directory parameter using the commonDLL.

// return values:
//   -
//*************************************************************************
int getRTRdir(char* RTRdataHome, unsigned cpSystemId)
{
	(void)cpSystemId;
	char szPath[FILENAME_MAX] = {0};
	int  dwLen = 512;
	int returnCode = 0;

	ACS_APGCC_DNFPath_ReturnTypeT retCode = ACS_APGCC_DNFPATH_FAILURE;
	ACS_APGCC_CommonLib oComLib;
	string fileMFuncName("sourceCallRecords");

	retCode = oComLib.GetFileMPath( fileMFuncName.c_str(),szPath,dwLen );
	if (retCode == ACS_APGCC_DNFPATH_SUCCESS)
	{
		strncpy(RTRdataHome,szPath, FILENAME_MAX - 1);
	}
	else
		returnCode=-1;

	return returnCode; 	

}

//*************************************************************************
// getACAparameters
//
// Fetches the MS-record size parameter from ACABIN and it also checks
// whether the specified Message store exists.

// return values:
//   true  : ok
//   false : Message store not defined in ACABIN.
//*************************************************************************
bool getACAparameters(const char*	 msName,
		const char*	 siteName,
		unsigned short &recSize)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_tqCriticalSection);

	(void)siteName;
	ACS_CC_ReturnType returnCode;
	ACS_APGCC_ImmAttribute attribute_1;
	OmHandler immHandler;
	char msgStoreN[16] = {0};
	returnCode=immHandler.Init();

	if(returnCode!=ACS_CC_SUCCESS){
	}
	std::vector<std::string> pd_dnList;

	char *classNamepd =const_cast<char*>("AxeDataRecordMessageStore");
	returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);
	if(returnCode != ACS_CC_SUCCESS){
	}
	sort (pd_dnList.begin(), pd_dnList.end());
	for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end();it2++)
	{
		string dobjname=*it2;
		ACS_CC_ImmParameter paramToFind;
		ACS_CC_ReturnType returnCode;


		paramToFind.attrName = (char *)messageStoreNameAttr;
		if (immHandler.getAttribute(dobjname.c_str(), &paramToFind ) == ACS_CC_FAILURE)
		{
			return false;
		}
		else
		{
			strcpy(msgStoreN , reinterpret_cast<char*>(*(paramToFind.attrValues)));
		}


		if ((std::strcmp(msgStoreN,msName)== 0 ))// && (std::strcmp(cpName.c_str(),siteName )== 0))
			{
			char attrname[]="recordSize";
			attribute_1.attrName = attrname;
			std::vector<ACS_APGCC_ImmAttribute *> attributes;
			attributes.push_back(&attribute_1);
			returnCode = immHandler.getAttribute(dobjname.c_str(), attributes);
			recSize=*((unsigned short *)(attribute_1.attrValues[0]));
			break;
			}
	}
	//finalize
	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS)
	{
		return false;
	}
	return true;


}

//*************************************************************************
// getACASite
//
// Fetches the Site parameter from ACABIN and it also checks
// whether the specified Message store exists.

// return values:
//   true  : ok
//   false : Message store not defined in ACABIN.
//*************************************************************************
bool getACASite(const char*	 msName, char* siteName, size_t count)
{
	(void)msName;
	(void)count;
	ACS_RTR_TRACE_MESSAGE("In - msName == %s, siteName == %s, count == %zu", msName, siteName, count);

	std::strcpy(siteName,"cp0ex");
	//Adding check to validate messageStore defined is a valid or not
	(void)siteName;
	ACS_CC_ReturnType returnCode;
	ACS_APGCC_ImmAttribute attribute_1;
	OmHandler immHandler;
	char msgStoreN[16] = {0};
	returnCode=immHandler.Init();

	std::vector<std::string> pd_dnList;

	char *classNamepd =const_cast<char*>("AxeDataRecordMessageStore");
	returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);
	if(returnCode != ACS_CC_SUCCESS)
	{
		ACS_RTR_TRACE_MESSAGE("Error: Cannot read message stores from AxeDataRecordMessageStore");
		return false;
	}
	sort (pd_dnList.begin(), pd_dnList.end());
	bool found = false;
	for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end(); ++it2)
	{
		string dobjname = *it2;
		ACS_CC_ImmParameter paramToFind;
		ACS_CC_ReturnType returnCode;

		paramToFind.attrName = (char *)messageStoreNameAttr;
		if (immHandler.getAttribute(dobjname.c_str(), &paramToFind ) == ACS_CC_FAILURE)
		{
			returnCode=immHandler.Finalize();
			ACS_RTR_TRACE_MESSAGE("Error: Cannot read 'messageStoreName' attribute");
			return false;
		}
		else
		{
			strcpy(msgStoreN , reinterpret_cast<char*>(*(paramToFind.attrValues)));
		}


		if ((std::strcmp(msgStoreN,msName)== 0 ))
		{
			returnCode=immHandler.Finalize();
			found = true;
			return true;
		}
	}
	//finalize
	returnCode=immHandler.Finalize();
	if(found == false)
		return false;

	return true;

}

//*************************************************************************
// getRTRparameters
//
// Fetches the RTR parameters from the configuration file for the
// specified Message store exists.
// The configuration file name is: %APG43_RTR_HOME%\RTR_Configuration.%msName%

// return values:
//   true  : ok
//   false : cannot retrieve the RTR parameters.
//*************************************************************************
bool getRTRparameters(const char*		msName,
		RTRMS_Parameters*	parameters,
		bool&				suppAlarm,
		char*				errstr,
		unsigned			errLen,
		const char*		siteName,bool jobDefine)
{
	//--------Fetching RTR info for a specified Message Store from IMM-------------------//
	(void)jobDefine;
	(void)suppAlarm;
	(void)errLen;
	(void)errstr;
	(void)siteName;

	ACS_CC_ReturnType returnCode;
	ACS_APGCC_ImmAttribute attribute_1;
	OmHandler immHandler;
	char MessageStoreName[16] = {};
	returnCode=immHandler.Init();
	bool jobknown = false;
	if(returnCode!=ACS_CC_SUCCESS){
	}

	bool found = false;
	//	if((parameters->OutputType == FileOutput) || ((jobDefine == true) && (jobknown == false)))
	{
		std::vector<std::string> pd_dnList;
		char *classNamepd =const_cast<char*>("AxeDataRecordFileBasedJob");
		returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS){

			sort (pd_dnList.begin(), pd_dnList.end());
			for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end();it2++)
			{

				string dobjname=*it2;
				// Extract the MS name form the Full DN
				size_t posMsNameEnd=0;
				size_t posMsName = 0;	
				posMsName = dobjname.find(",",posMsName+1);
				string  parentDN = dobjname.substr(posMsName+1,(posMsNameEnd - (posMsName+1)));

				ACS_CC_ImmParameter paramToFind;
				ACS_CC_ReturnType returnCode;

				paramToFind.attrName = (char*)messageStoreNameAttr;
				if (immHandler.getAttribute(parentDN.c_str(), &paramToFind ) == ACS_CC_FAILURE)
				{
					found = false;
				}
				else
				{
					found = true;
					strcpy(MessageStoreName , reinterpret_cast<char*>(*(paramToFind.attrValues)));
				}


				if(!strcmp(MessageStoreName, msName))
				{
					ACS_APGCC_ImmAttribute attribute_1;
					ACS_APGCC_ImmAttribute attribute_2;
					ACS_APGCC_ImmAttribute attribute_3;
					ACS_APGCC_ImmAttribute attribute_4;
					ACS_APGCC_ImmAttribute attribute_5;
					ACS_APGCC_ImmAttribute attribute_6;
					ACS_APGCC_ImmAttribute attribute_7;
					ACS_APGCC_ImmAttribute attribute_8;
					ACS_APGCC_ImmAttribute attribute_9;
					ACS_APGCC_ImmAttribute attribute_10;
					ACS_APGCC_ImmAttribute attribute_11;


					attribute_1.attrName = "transferQueue";
					attribute_2.attrName = "cdrCounterFlag";
					attribute_3.attrName = "holdTime";
					attribute_4.attrName = "paddingChar";
					attribute_5.attrName = "recordlength";
					attribute_6.attrName = "fileSize";
					attribute_7.attrName = "fixedFileRecordsFlag";
					attribute_8.attrName = "minFileSize";
					attribute_9.attrName = "hashKey";


					std::vector<ACS_APGCC_ImmAttribute *> attributes;
					attributes.push_back(&attribute_1);
					attributes.push_back(&attribute_2);
					attributes.push_back(&attribute_3);
					attributes.push_back(&attribute_4);
					attributes.push_back(&attribute_5);
					attributes.push_back(&attribute_6);
					attributes.push_back(&attribute_7);
					attributes.push_back(&attribute_8);
					attributes.push_back(&attribute_9);

					returnCode = immHandler.getAttribute(dobjname.c_str(), attributes);
					parameters->FileSize	= (*(int *)(attribute_6.attrValues[0]));
					strcpy(parameters->TransferQueue, reinterpret_cast<char*>(attribute_1.attrValues[0]));
					parameters->FiCdrFormat	= (*(int *)(attribute_2.attrValues[0]));
					parameters->FiPaddingChar	= (*(int *)(attribute_4.attrValues[0]));
					parameters->FiFileHoldTime	= (*(int *)(attribute_3.attrValues[0]));
					parameters->FileRecordLength= (*(int *)(attribute_5.attrValues[0]));
					parameters->FixedFileRecords= (*(int *)(attribute_7.attrValues[0]));
					parameters->MinFileSize	= (*(int *)(attribute_8.attrValues[0]));
					parameters->OutputType = FileOutput;	

					if(attribute_9.attrValuesNum != 0)
						parameters->HmacMd5=true;
					else
						parameters->HmacMd5=false;

					jobknown = true;
					break;


				}
			}


		}
	}

	//	if((parameters->OutputType == BlockOutput) || ((jobknown == false) && (jobDefine == true)))
	{
		std::vector<std::string> pd_dnList;
		char *classNamepd =const_cast<char*>("AxeDataRecordBlockBasedJob");
		returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);
		if(returnCode == ACS_CC_SUCCESS){
			sort (pd_dnList.begin(), pd_dnList.end());
			for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end();it2++)
			{
				string dobjname=*it2;
				// Extract the MS name form the Full DN
				size_t posMsNameEnd = 0;
				size_t posMsName = 0;	

				posMsName = dobjname.find(",",posMsName+1);
				string  parentDN = dobjname.substr(posMsName+1,(posMsNameEnd - (posMsName+1)));

				ACS_CC_ImmParameter paramToFind;
				ACS_CC_ReturnType returnCode;


				paramToFind.attrName =(char*) messageStoreNameAttr;
				if (immHandler.getAttribute(parentDN.c_str(), &paramToFind ) == ACS_CC_FAILURE)
				{
					found = false;
					continue;
				}
				else
				{
					found = true;
					strcpy(MessageStoreName , reinterpret_cast<char*>(*(paramToFind.attrValues)));
				}

				if(!strcmp(MessageStoreName, msName))
				{

					ACS_APGCC_ImmAttribute attribute_1;
					ACS_APGCC_ImmAttribute attribute_2;
					ACS_APGCC_ImmAttribute attribute_3;
					ACS_APGCC_ImmAttribute attribute_4;
					ACS_APGCC_ImmAttribute attribute_5;

					attribute_1.attrName = "transferQueue";
					attribute_2.attrName = "length";
					attribute_3.attrName = "holdTime";
					attribute_4.attrName = "paddingChar";
					attribute_5.attrName = "lengthType";


					std::vector<ACS_APGCC_ImmAttribute *> attributes;
					attributes.push_back(&attribute_1);
					attributes.push_back(&attribute_2);
					attributes.push_back(&attribute_3);
					attributes.push_back(&attribute_4);
					attributes.push_back(&attribute_5);

					returnCode = immHandler.getAttribute(dobjname.c_str(), attributes);
					parameters->BlockLength		= (*(int *)(attribute_2.attrValues[0]));
					strcpy(parameters->TransferQueue, reinterpret_cast<char*>(attribute_1.attrValues[0]));
					parameters->BlockLengthType	= (*(int *)(attribute_5.attrValues[0]));
					parameters->BlPaddingChar	= (*(int *)(attribute_4.attrValues[0]));
					parameters->BlockHoldTime	= (*(int *)(attribute_3.attrValues[0]));
					parameters->OutputType = BlockOutput;
					jobknown = true;
					break;
				}
			}
		}
	}
	if(found == false)
	{
		return false;
	}

	//finalize
	returnCode=immHandler.Finalize();
	if(returnCode!=ACS_CC_SUCCESS){
		return false;
	}	
	return true;
}

bool validateParameters(RTRMS_Parameters* parameters) {
	(void) parameters;
	//Not used anymore to be removed

	return true;
}

//*************************************************************************
// setRTRparameters
//
// Stores the RTR parameters in the configuration file for the
// specified Message store.

// return values:
//   true  : ok
//   false : cannot store the RTR parameters.
//*************************************************************************
bool setRTRparameters(const char*		msName,
		RTRMS_Parameters* parameters,
		const char*		siteName)
{

	(void)msName;
	(void)parameters;
	(void)siteName;
	return true;
}

//*************************************************************************
// setFileRights
//
// Sets file permissions.

// return values:
//   -
//*************************************************************************
void setFileRights(const char* f)
{
	(void)f;
	// The following function's return values are ignored
}

//*************************************************************************
// commitCommand
//
// Handles the commit (Y\N) for a command.

// return values:
//   true  : command committed.
//   false : command cancelled.
//*************************************************************************
bool commitCommand(void)
{
	char okString[5]="No";
	std::cout<<"Enter y(es) or n(o) [default: n] \03:";
	std::cout<<std::flush;
	//gets(okString);
	if (fgets( okString, sizeof(okString), stdin) == 0)
	{
		std::cout<<"Error has occurred while reading from standard input";
		return false;
	}
	if (!strcasecmp(okString, "Y\n") || !strcasecmp(okString, "YES\n")) 
	{
		return true;
	}
	return false;
}

//*******************************************************************************
// setDefaultParValue()
//*******************************************************************************
bool setDefaultParValue(int* parsIssued,RTRMS_Parameters* parValue, char* errstr)
{
	char RTRdataHome[FILENAME_MAX] = {0};
	int  err = 0;

	if (getRTRConfigDir(RTRdataHome) < 0) return false;

	//if it's necessart create directory for default files
	if(!createDir(RTRdataHome,err))
	{
		//check error
		if(err != EEXIST)
			return false;
	}

	defFile* df=new defFile(RTRdataHome);
	if(df == 0)
	{
		if (errstr) sprintf(errstr,"Allocation of memory failed in funtion setDefaultParValue");
		return false;
	}
	if (!df->init(errstr))
	{
		delete df;
		return false;
	}

	if (parsIssued[a_blockLength])
	{
		df->setDefaultValue(a_blockLength,parValue->BlockLength);
	}
	if (parsIssued[b_cdrFileFormat])
	{
		df->setDefaultValue(b_cdrFileFormat,parValue->FiCdrFormat);
	}
	if (parsIssued[c_key])
	{
		df->setDefaultValue(c_key,parValue->FiKey);
	}
	if (parsIssued[d_blockLengthType])
	{
		df->setDefaultValue(d_blockLengthType,parValue->BlockLengthType);
	}
	if (parsIssued[e_blockHoldTime])
	{
		df->setDefaultValue(e_blockHoldTime,parValue->BlockHoldTime);
	}
	if (parsIssued[g_blockPaddingChar])
	{
		df->setDefaultValue(g_blockPaddingChar,parValue->BlPaddingChar);
	}
	if (parsIssued[h_fileSize])
	{
		df->setDefaultValue(h_fileSize,parValue->FileSize);
	}
	if (parsIssued[j_fileHoldTime])
	{
		df->setDefaultValue(j_fileHoldTime,parValue->FiFileHoldTime);
	}
	if (parsIssued[k_generationTime])
	{
		df->setDefaultValue(k_generationTime,parValue->GenerationTime);
	}
	if (parsIssued[l_resetFlag])
	{
		df->setDefaultValue(l_resetFlag,parValue->ResetFlag);
	}
	if (parsIssued[m_fixedFileRecords])
	{
		df->setDefaultValue(m_fixedFileRecords,parValue->FixedFileRecords);
	}
	if (parsIssued[n_fileRecordLength])
	{
		df->setDefaultValue(n_fileRecordLength,parValue->FileRecordLength);
	}
	if (parsIssued[o_filePaddingChar])
	{
		df->setDefaultValue(o_filePaddingChar,parValue->FiPaddingChar);
	}
	if (parsIssued[p_minFileSize])
	{
		df->setDefaultValue(p_minFileSize,parValue->MinFileSize);
	}

	if (df) delete df;
	return true;
}



//*************************************************************************
// getDefaultParValues
//
// Returns the default parameter values, i.e those used at startup of the
// RTR service.

// return values:
//   -
//*************************************************************************
bool getDefaultParValues(struct RTRMS_Parameters& bpar, char* errstr)
{
	bool ret=true;
	char RTRdataHome[FILENAME_MAX] = {0};
	if (getRTRConfigDir(RTRdataHome) < 0) 
	{
		return false;
	} 	
	defFile* df=new defFile(RTRdataHome); 
	if (!df || !df->init(errstr))
	{
		ret=false;
	}
	else
	{
		df->getDefaultValues(bpar); //set the parameters to fefault values
	}

	if (df) delete df;
	return ret;
}

//*************************************************************************
// suppressFile
//
// Handles the alarm suppression file, i.e a file indicating whether
// suppression of alarm 'CHARGING DESTINATION FAULT' should be done or not.

// return values:
//   true: alarm suppression file created, deleted or existed.
//   false: requested task failed.
//*************************************************************************
bool suppressFile(fileRequests fr)
{
	ACE_HANDLE h;
	h=ACE_INVALID_HANDLE;
	if (fr==createSF)
	{
		h = ACE_OS::open(alarmSuppFile,/*_WRONLY|*/O_RDWR, ACE_DEFAULT_OPEN_PERMS);

		if(h == ACE_INVALID_HANDLE) return false;
		close(h);
	}
	else if (fr==removeSF)
	{
		ACE_OS::unlink(alarmSuppFile);

	}
	else // existSF
	{

		if (h == ACE_INVALID_HANDLE) return false;
		close(h);
	}
	return true;
}

//*************************************************************************
// decr
//
//*************************************************************************
void decr(unsigned char d[32])
{
	int st;
	for (int i=0;i<32;i++)
	{
		st=twk[i];
		while(st)
		{
			if (d[i]!='A' && d[i]!='a' && d[i]!='0') d[i]-=1;
			else if (d[i]=='0') d[i]+=9;
			else d[i]+=5;
			st--;
		}
	}
}

//*************************************************************************
// encr
//
//*************************************************************************
void encr(unsigned char e[32])
{
	int st;
	for (int i=0;i<32;i++)
	{
		st=twk[i];
		while(st)
		{
			if (e[i]!='F' && e[i]!='f' && e[i]!='9') e[i]+=1;
			else if (e[i]=='9') e[i]-=9;
			else e[i]-=5;
			st--;
		}
	}
}

//*************************************************************************
// packHexString
//
//*************************************************************************
void packHexString(int ln,char* hexStr,char* pHexStr)
{
	int ind=0;
	for (int i=0;i<ln;i++)
	{
		ind=i/2;
		if (isdigit(hexStr[i])) hexStr[i]-='0';
		else if (hexStr[i]>='a' && hexStr[i]<='f') hexStr[i]=hexStr[i]-'a' +10;
		else hexStr[i]=hexStr[i]-'A' +10;
		if (!(i%2)) pHexStr[ind]=hexStr[i] << 4;
		else pHexStr[ind]|=hexStr[i];
	}
}

//*************************************************************************
// get_HMACMD5Keys
//
// Fetches the RTR ipadd opad parameters from IMM .

// return values:
//   -
//*************************************************************************
bool get_HMACMD5Keys(const char* msName,
					const char*	siteName,
					HMACMD5_keys&	ck,
					const char* _filejobbasedDN,
					char*			errstr,
					unsigned		errLen)
{
	(void)errstr;
	(void)errLen;
	(void)siteName;
	(void) msName;	
	bool ret=false;
	ACS_CC_ReturnType returnCode;                                                                      
	ACS_APGCC_ImmAttribute attribute_1;                                                                
	OmHandler immHandler;                                                                              

	returnCode=immHandler.Init();                                                                      

	if(returnCode!=ACS_CC_SUCCESS){                                                                   
	       	
	}                                                                                                  
	std::vector<std::string> pd_dnList;                                                                

	char *classNamepd =const_cast<char*>("AxeDataRecordDataRecordM");                          
	returnCode = immHandler.getClassInstances(classNamepd, pd_dnList);                                 
	if(returnCode != ACS_CC_SUCCESS){                                                                
		ret=false;
		 immHandler.Finalize(); 
		return ret;
	}                                                                                                  

	sort (pd_dnList.begin(), pd_dnList.end());                                                         
	for(vector<string>::iterator it2=pd_dnList.begin();it2!=pd_dnList.end();it2++)                     
	{                                                                                                  
		string dobjname=*it2;                                                                              
		ACS_APGCC_ImmAttribute attribute_2;                                                                
		ACS_APGCC_ImmAttribute attribute_3;                                                              
		char attrname1[]="iPadding";                                                                       
		char attrname2[]="oPadding";                                                                     
		attribute_2.attrName = attrname1;                                                                  
		attribute_3.attrName = attrname2;                                                                
		std::vector<ACS_APGCC_ImmAttribute *> attributes1;                                                 
		attributes1.push_back(&attribute_2);                                                               
		attributes1.push_back(&attribute_3);                                                             
		returnCode = immHandler.getAttribute(dobjname.c_str(),attributes1);                                
		memcpy(ck.ipad,(reinterpret_cast<char*>(attribute_2.attrValues[0])),64);
		memcpy(ck.opad,(reinterpret_cast< char*>(attribute_3.attrValues[0])),64);                         
	}                                                                                                  

	ACS_CC_ImmParameter attrPasswd;
	string hashKey,hashKeyValue;
	SecCryptoStatus decryptStatus;
	syslog(LOG_INFO,rtrEcimPasswordAttr);
	attrPasswd.attrName = (char *)rtrEcimPasswordAttr;
	std::string dnPrefix("id=hashKey_0,");

	std::string dnName(_filejobbasedDN);
	std::string passDn=dnPrefix+dnName;	
	//retrieving the password from ECIM password struct
	syslog(LOG_INFO,passDn.c_str());
	ACS_CC_ReturnType returncode;
	returncode=immHandler.getAttribute( passDn.c_str(), &attrPasswd);
	if ( returncode == ACS_CC_SUCCESS )
	{
		hashKeyValue = reinterpret_cast<char*>(attrPasswd.attrValues[0]);
		hashKey=hashKeyValue;
		if(!hashKeyValue.empty())
		{
			char * plaintext = NULL;
			string ciphertext = hashKeyValue;
			decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
			if(decryptStatus != SEC_CRYPTO_OK )
			{
				memcpy(ck.skey,hashKeyValue.c_str(),16);

			}
			else
			{
				memcpy(ck.skey,plaintext,16);
			}
			if(plaintext !=0 )
				free(plaintext);
		}

	}
	else
	{
		ret=false; 
	}
	immHandler.Finalize();
	return ret;
}
//********************************************************************************
// getRTRConfigDir
//   Fetches the RTR base directory parameter from RTRBIN.
//   In blade cluster release, rtrConfigDir contains RTRConfigurations file and
//   statistics file for all the message stores.
//   RETURN:
//	-1 on failure
//	 1 on success
//********************************************************************************
int getRTRConfigDir(char* RTRdataHome)
{
	char szPath[FILENAME_MAX] = {0};
	int  dwLen = 512;
	int returnCode = 0;
	int err=0;
	ACS_APGCC_DNFPath_ReturnTypeT retCode = ACS_APGCC_DNFPATH_FAILURE;
	ACS_APGCC_CommonLib oComLib;
	string fileMFuncName("sourceCallRecords");
	retCode = oComLib.GetFileMPath( fileMFuncName.c_str(),szPath,dwLen );
	if (retCode == ACS_APGCC_DNFPATH_SUCCESS)
	{
		strncpy(RTRdataHome,szPath, FILENAME_MAX - 1);
		returnCode=1;
		createDir(RTRdataHome, err);
	}
	return returnCode;
}

//********************************************************************************
//                          create a directory
// 
//********************************************************************************
bool createDir (const char* dirpath, int& err)
{
	bool result = true;
	err = 0;
	std::string inputFolder(dirpath);
	boost::filesystem::path inputFolderPath(inputFolder);

	try
	{
		// check if dirpath exists
		if( !boost::filesystem::exists(inputFolderPath) )
		{
			// create the folders
			boost::filesystem::create_directories(inputFolderPath);
			syslog(LOG_INFO, "acs_rtrd created folder:<%s>", inputFolder.c_str());
		}
	}
	catch(const boost::filesystem::filesystem_error& ex)
	{
		syslog(LOG_ERR, "acs_rtrd cannot create folder:<%s>. error:<%s>", inputFolder.c_str(), ex.what());
		err = errno;
		result = false;
	}
	return result;
}

//***************************************************************************************
// getStatiticsFileAbsolutePath
//
// Returns the absolute path for a statistics file name in filepath output parameter.
// If the cpSite input parameter is NULL, it creates the path for a multiple CP system.
// If the cpSite input parameter isn't NULL, it creates the path for a single CP system.
//
// Take care: we have two kind of statistics file for a multiple cp system. There's a
// statistics file at 'message store-global level', and there's another statistics file
// at the 'message store-cp level'. This function returns the absolute path for the first one.
//***************************************************************************************
bool getStatiticsFileAbsolutePath(const char* msName,
		const char* cpSite1,
		char* filepath)
{
	int  err = 0;
	char RTRdataHome[FILENAME_MAX] = {0};
	if (msName == 0) return false;

	if (getRTRConfigDir(RTRdataHome) < 0) return false; //get RTR Configuration directory
	strcpy(filepath, RTRdataHome);//, FILENAME_MAX - 1);
	if (cpSite1 == 0) //multiple cp system
	{
		bool create = false;
		for(int i =0;( ( i < 10) && (create != true));i++)
		{
			create = createDir(filepath, err);
			sleep(1);
		}

		strncat(filepath, "/statistics.", (FILENAME_MAX - strlen(filepath) - 1));
		strncat(filepath, msName, (FILENAME_MAX - strlen(filepath) - 1));
	}
	else //single cp system
	{
		strncat(filepath, "/message_store/", (FILENAME_MAX - strlen(filepath) - 1));
		strncat(filepath, msName, (FILENAME_MAX - strlen(filepath) - 1));
		bool create = false;
		for(int i =0;( ( i < 10) && (create != true));i++)
		{
			create = createDir(filepath, err);
			sleep(1);
		}
		strncat(filepath, "/statistics", (FILENAME_MAX - strlen(filepath) - 1));
	}
	return true;
}


//***************************************************************************************
// getCPlevelStatiticsAbsPath
//
// It's used only for multiple cp system.
// It returns the absolute path for a statistics file name related to the
// 'message store-cp level' in filepath output parameter.
//***************************************************************************************
bool getCPlevelStatiticsAbsPath(const char* msName,
		unsigned cpSystemId,
		char* filepath)
{
	int  err = 0;
	char RTRdataHome[FILENAME_MAX] = {0};

	string _cpName;	
	if (cpSystemId < (SysType_BC + 1000)) { // Single sided CP (blade)
		stringstream defNameStringStream;
		defNameStringStream << "BC" << cpSystemId;            // For example "BC0"
		_cpName = defNameStringStream.str().c_str();
	} else if ((cpSystemId >= SysType_CP) && (cpSystemId < (SysType_CP + 1000))) { // Double sided CP (SPX)
		stringstream defNameStringStream;
		defNameStringStream << "CP" << (cpSystemId - 1000);   // For example "CP0"
		_cpName = defNameStringStream.str().c_str();
	}

	cout<<"AFter calling cs_getCPName"<<endl;
	if (msName == 0) return false;
	if (getRTRdir(RTRdataHome, cpSystemId) < 0) return false;
	ACE_OS::snprintf(filepath, FILENAME_MAX - 1, "%s/", RTRdataHome); //added x CR32
	strncat(filepath, "/message_store/", (FILENAME_MAX - strlen(filepath) - 1));
	strncat(filepath, _cpName.c_str(),(FILENAME_MAX - strlen(filepath) - 1));
	strncat(filepath,"/",(FILENAME_MAX - strlen(filepath) - 1));
	strncat(filepath, msName, (FILENAME_MAX - strlen(filepath) - 1));
	createDir(filepath, err);
	strncat(filepath, "/statistics", (FILENAME_MAX - strlen(filepath) - 1));
	return true;
}

//***************************************************************************************
// deleteConfigurationFile
//***************************************************************************************
bool deleteConfigurationFile(const char* msName,
		const char* cpSite)
{
	char RTRdataHome[FILENAME_MAX] = {0};
	char RTRconfigPath[FILENAME_MAX] = {0};
	int err = 0;
	if (getRTRConfigDir(RTRdataHome) < 0) return false;

	strncpy(RTRconfigPath, RTRdataHome, sizeof(RTRconfigPath) - 1);
	if (cpSite == 0) { //multiple cp system
		createDir(RTRconfigPath, err);
		strncat(RTRconfigPath, "/RTRconfigFile.", MAXSTRLEN(RTRconfigPath));
		strncat(RTRconfigPath, msName, MAXSTRLEN(RTRconfigPath));
	}
	else //single cp system
	{
		strncat(RTRconfigPath, "/", MAXSTRLEN(RTRconfigPath));
		strncat(RTRconfigPath, msName, MAXSTRLEN(RTRconfigPath));
		strncat(RTRconfigPath, "_", MAXSTRLEN(RTRconfigPath));
		strncat(RTRconfigPath, cpSite, MAXSTRLEN(RTRconfigPath));
		createDir(RTRconfigPath, err);
		strncat(RTRconfigPath, "/RTRconfigFile", MAXSTRLEN(RTRconfigPath));
	}

	if(ACE_OS::unlink(RTRconfigPath))
		return true;
	else
		return false;
}

