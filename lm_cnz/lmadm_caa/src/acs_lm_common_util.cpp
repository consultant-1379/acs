//  NAME
//   acs_lm_common_util.cpp
//
//  COPYRIGHT
//     Ericsson AB, 2015. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson  AB, Sweden.
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
//     2015-04-21 by XSUDYAG PA1
//
//  SEE ALSO 
//     -
//
//****************************************************************************
/*=================================================================== */
#include <ace/ACE.h>
#include "acs_lm_common_util.h"
#include <stdio.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_stat.h>
#include "acs_lm_tra.h"
#include <ACS_APGCC_CLibTypes.h>
#include <ACS_APGCC_CommonLib.h>
#include <sstream>
#include <iostream>
#include <string.h>
/*=======OUTINE: ACS_LM_Common constructor
=================================================================== */
ACS_LM_Common_Util::ACS_LM_Common_Util()
{


}//end of constructor
/*=================================================================
        ROUTINE: ACS_LM_Common destructor
=================================================================== */
ACS_LM_Common_Util::~ACS_LM_Common_Util()
{

}//end of destructor

/*=================================================================
        ROUTINE: getClusterIPAddress
=================================================================== */
bool ACS_LM_Common_Util::getClusterIPAddress(std::string &aClusterIPAddress)
{
        DEBUG("ACS_LM_Common_Util::getClusterIPAddress()  %s","Entering");

        ACS_APGCC_CommonLib myAPGCCCommonLib;
        ACS_APGCC_ApgAddresses cluster_ip = {"","","" };
	bool fetched_IPAddr= false;
        ACS_APGCC_IpAddress_ReturnTypeT returnCode = ACS_APGCC_IpAddress_FAILURE;

        returnCode = myAPGCCCommonLib.getIpAddresses(cluster_ip);

        if (returnCode == ACS_APGCC_IpAddress_SUCCESS)
        {
                DEBUG("ACS_LM_Common_Util::getClusterIPAddress() IP1= %s, IP2= %s, IP3= %s", cluster_ip.clusterIp1.c_str(),cluster_ip.clusterIp2.c_str(),cluster_ip.clusterIpAddress.c_str());
                aClusterIPAddress = cluster_ip.clusterIp1;
		fetched_IPAddr = true;
        }
        else if (returnCode == ACS_APGCC_IpAddress_FAILURE)
        {
                DEBUG("ACS_LM_Common_Util::getClusterIPAddress()  %s","failed");
        }
        INFO("ACS_LM_Common_Util::getClusterIPAddress()  aClusterIPAddress = %s, fetched_IPAddr=%d",aClusterIPAddress.c_str(),fetched_IPAddr);
        DEBUG("ACS_LM_Common_Util::getClusterIPAddress()  %s","Leaving");

        return fetched_IPAddr;
}//end of getClusterIPAddress

/*=================================================================
        ROUTINE: getClusterIPAddress_R2
=================================================================== */
bool ACS_LM_Common_Util::getClusterIPAddress_R2(std::string &clusterIPv4, std::string &clusterIPv6)
{
	DEBUG("ACS_LM_Common_Util::getClusterIPAddress_R2()  %s","Entering");

	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_ApgAddresses_R2 apgAddr;
	apgAddr.clusterIp1 = "";
	apgAddr.clusterIp1_IPv6 = "";
	apgAddr.clusterIp2 = "";
	apgAddr.clusterIp2_IPv6 = "";
	apgAddr.clusterIpAddress = "";
	apgAddr.clusterIpAddress_IPv6 = "";
	bool fetched_IPAddr= false;
	ACS_APGCC_IpAddress_ReturnTypeT returnCode = ACS_APGCC_IpAddress_FAILURE;

	returnCode = myAPGCCCommonLib.getIpAddresses(apgAddr);

	if (returnCode == ACS_APGCC_IpAddress_SUCCESS)
	{
		DEBUG("ACS_LM_Common_Util::getClusterIPAddress_R2() IP1= %s, IP2= %s, IP3= %s", apgAddr.clusterIp1.c_str(),apgAddr.clusterIp2.c_str(),apgAddr.clusterIpAddress.c_str());
		DEBUG("ACS_LM_Common_Util::getClusterIPAddress_R2() IP1_v6= %s, IP2_v6= %s, IP3_v6= %s", apgAddr.clusterIp1_IPv6.c_str(),apgAddr.clusterIp2_IPv6.c_str(),apgAddr.clusterIpAddress_IPv6.c_str());
		clusterIPv4 = apgAddr.clusterIp1;
		clusterIPv6 = apgAddr.clusterIp1_IPv6;
		fetched_IPAddr = true;
	}
	else if (returnCode == ACS_APGCC_IpAddress_FAILURE)
	{
		DEBUG("ACS_LM_Common_Util::getClusterIPAddress_R2()  %s","failed");
	}
	INFO("ACS_LM_Common_Util::getClusterIPAddress_R2()  clusterIPv4 = %s, clusterIPv6 = %s, fetched_IPAddr=%d",clusterIPv4.c_str(),clusterIPv6.c_str(),fetched_IPAddr);
	DEBUG("ACS_LM_Common_Util::getClusterIPAddress_R2()  %s","Leaving");

	return fetched_IPAddr;
}//end of getClusterIPAddress


/*=================================================================
        ROUTINE: generateFingerPrint
=================================================================== */
bool ACS_LM_Common_Util::generateFingerPrint(std::string & aFingerPrint,OmHandler* theOmHandlerPtr)
{
        DEBUG("ACS_LM_Common_Util::generateFingerPrint()  %s","Entering");
        std::string myIPClusterAdress("");
        std::string myNodeName("");
        bool myClusterIPAdressFlag= getClusterIPAddress(myIPClusterAdress);
        if(!myClusterIPAdressFlag)
        {//ERROR: Flag is false when not able to fetch the clusterIpAddress
                return false;
        }
        //bool myNodeNameFlag=getNodeName(myNodeName,theOmHandlerPtr);
        bool myNodeNameFlag=ACS_LM_Common::getNodeName(myNodeName,theOmHandlerPtr);
        if(!myNodeNameFlag)
        {//ERROR: Flag is false   not able to fetch the nodeName
                return false;
        }
        //Compare and append the IPAddress and NodeName separated with a '-'
        if(!((strcmp(myIPClusterAdress.c_str(),"")== 0) && (strcmp(myNodeName.c_str(),"") == 0)))
        {
                aFingerPrint = myNodeName;
                aFingerPrint.append("-");
                aFingerPrint.append(myIPClusterAdress);
                INFO("ACS_LM_Common_Util::generateFingerPrint() aFingerPrint =  %s",aFingerPrint.c_str());
        }
        else
        {//ERROR :Not able to print the finger print value
                ERROR("ACS_LM_Common_Util::generateFingerPrint()  %s","Failed to get Fingerpring");
                return false;
        }
        DEBUG("ACS_LM_Common_Util::generateFingerPrint()  %s","Leaving");
        return true;

}//end of generateFingerPrint

/*=================================================================
        ROUTINE: generateFingerPrintForLicenseM
=================================================================== */
bool ACS_LM_Common_Util::generateFingerPrintForLicenseM(std::string & IPv4Fingerprint,std::string & IPv6Fingerprint,OmHandler* theOmHandlerPtr)
{
	DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM()  %s","Entering");
	std::string clusterIPv4("");
	std::string clusterIPv6("");
	std::string myNodeName("");

	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_ApgAddresses_R2 apgAddr;
	apgAddr.clusterIp1 = "";
	apgAddr.clusterIp1_IPv6 = "";
	apgAddr.clusterIp2 = "";
	apgAddr.clusterIp2_IPv6 = "";
	apgAddr.clusterIpAddress = "";
	apgAddr.clusterIpAddress_IPv6 = "";
	bool fetched_IPAddr= false;
	ACS_APGCC_IpAddress_ReturnTypeT returnCode = ACS_APGCC_IpAddress_FAILURE;

	returnCode = myAPGCCCommonLib.getIpAddresses(apgAddr);

	if (returnCode == ACS_APGCC_IpAddress_SUCCESS)
	{
		DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM() IP1= %s, IP2= %s, IP3= %s", apgAddr.clusterIp1.c_str(),apgAddr.clusterIp2.c_str(),apgAddr.clusterIpAddress.c_str());
		DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM() IP1_v6= %s, IP2_v6= %s, IP3_v6= %s", apgAddr.clusterIp1_IPv6.c_str(),apgAddr.clusterIp2_IPv6.c_str(),apgAddr.clusterIpAddress_IPv6.c_str());
		clusterIPv4 = apgAddr.clusterIp1;
		clusterIPv6 = apgAddr.clusterIp1_IPv6;
		fetched_IPAddr = true;
	}
	else if (returnCode == ACS_APGCC_IpAddress_FAILURE)
	{
		DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM()  %s","failed");
	}
	INFO("ACS_LM_Common_Util::generateFingerPrintForLicenseM()  clusterIPv4 = %s, clusterIPv6 = %s, fetched_IPAddr=%d",clusterIPv4.c_str(),clusterIPv6.c_str(),fetched_IPAddr);
	DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM()  %s","Leaving");

	if(!fetched_IPAddr)
	{//ERROR: Flag is false when not able to fetch the clusterIpAddress
		ERROR("ACS_LM_Common_Util::generateFingerPrintForLicenseM() - getClusterIPAddress_R2() returned false. %s","ERROR!");
		return false;
	}
	//bool myNodeNameFlag=getNodeName(myNodeName,theOmHandlerPtr);
	bool myNodeNameFlag=ACS_LM_Common::getNodeName(myNodeName,theOmHandlerPtr);
	if(!myNodeNameFlag || myNodeName.empty())
	{//ERROR: Flag is false   not able to fetch the nodeName
		ERROR("ACS_LM_Common_Util::generateFingerPrintForLicenseM() - getNodeName() returned false. %s","ERROR!");
		return false;
	}

	if(apgAddr.stackType == ACS_APGCC_IPv4_STACK)
	{
		IPv4Fingerprint = myNodeName;
		IPv4Fingerprint.append("-");
		IPv4Fingerprint.append(apgAddr.clusterIp1);
		IPv6Fingerprint = "";
	}
	else if(apgAddr.stackType == ACS_APGCC_IPv6_STACK)
	{
		IPv6Fingerprint = myNodeName;
		IPv6Fingerprint.append("-");
		IPv6Fingerprint.append(apgAddr.clusterIp1_IPv6);
		IPv4Fingerprint = "";
	}
	else if(apgAddr.stackType == ACS_APGCC_DUAL_STACK)
	{
		// In case of dual stack, create 2 fingerprints --->  MEID-IPv4,MEID-IPv6
		IPv4Fingerprint = myNodeName;
		IPv4Fingerprint.append("-");
		IPv4Fingerprint.append(apgAddr.clusterIp1);
		IPv6Fingerprint = myNodeName;
		IPv6Fingerprint.append("-");
		IPv6Fingerprint.append(apgAddr.clusterIp1_IPv6);
	}
	INFO("ACS_LM_Common_Util::generateFingerPrintForLicenseM() -  IPv4Fingerprint[%s]    &  IPv6Fingerprint[%s]",IPv4Fingerprint.c_str(),IPv6Fingerprint.c_str());
	DEBUG("ACS_LM_Common_Util::generateFingerPrintForLicenseM()  %s","Leaving");
	return true;

}//end of generateFingerPrint

