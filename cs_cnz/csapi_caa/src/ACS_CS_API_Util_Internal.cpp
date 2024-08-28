//  *********************************************************
//  ********************
//   COPYRIGHT Ericsson 2009.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2009.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2009 or in
//  accordance with the terms and conditions stipulated in
//  the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  *********************************************************
//  ********************


#include <sstream>
#include <set>
#include <string.h>
#include <stdlib.h>

#include "ACS_CC_Types.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_API_Util_Internal.h"

#if defined(CS_API_EXPORT)
#include "ACS_CS_API_UsageLogger.h"
#endif

#include "ACS_CS_API_Util_Internal.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_API.h"

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImTypes.h"

#include "ACS_CS_API_OmProfileChange_R1.h"

using std::string;
using std::stringstream;
using std::ostringstream;
using std::set;
using namespace ACS_CS_NS;
using namespace ACS_CS_Protocol;
using namespace ACS_CS_INTERNAL_API;

#include "ACS_CS_API_Tracer.h"
ACS_CS_API_TRACER_DEFINE(ACS_CS_API_Util_TRACE);


// Class ACS_CS_API_Util_Internal

ACS_CS_API_Util_Internal::ACS_CS_API_Util_Internal()
{
}

bool ACS_CS_API_Util_Internal::getDefaultCPName (uint16_t cpId, std::string& defName)
{
    if (cpId < (ACS_CS_Protocol::SysType_BC + 1000))  // Single sided CP (blade)
	{
      stringstream defNameStringStream;
		defNameStringStream << "BC" << cpId;            // For example "BC0"
		defName = defNameStringStream.str().c_str();
	
		return true;
	}
   else if ((cpId >= ACS_CS_Protocol::SysType_CP) && (cpId < (ACS_CS_Protocol::SysType_CP + 1000)))   // Double sided CP (SPX)
	{
      stringstream defNameStringStream;
		defNameStringStream << "CP" << (cpId - 1000);   // For example "CP0"
		defName = defNameStringStream.str().c_str();

		return true;
	}
	else
	{
		// Failure if this is reached RoGa */
		return false;
 	}
}

bool ACS_CS_API_Util_Internal::getDefaultAPName (uint16_t apId, std::string& defName)
{
   if ((apId >= ACS_CS_Protocol::SysType_AP + 1) && (apId <= (ACS_CS_Protocol::SysType_AP + 16)))  //Valid AP ids are 2001 - 2016
	{
      stringstream apNameStringStream;
		apNameStringStream<<"AP"<<(apId - 2000);			//For example "AP1"
		defName = apNameStringStream.str().c_str();

		return true;
	}

   return false;  //Invalid AP id given
}

bool ACS_CS_API_Util_Internal::getAPIdentifier (const std::string& apName, uint16_t& apIdentifier)
{
   string apNameUpperCase = apName;
      
   if (!stringToUpper(apNameUpperCase))
      return false;

   //Check if given name equals any of "AP1" to "AP16"
   for (int i = 1; i <= 16; i++)
   {
      stringstream tmpAPName;

      tmpAPName<<"AP"<<i;
      
      if (tmpAPName.str() == apNameUpperCase)
      {
         apIdentifier = ACS_CS_Protocol::SysType_AP + i; //store AP indentifier, e.g. 2000 + 1 => 2001
         return true;
      }
   }

   return false;
}

bool ACS_CS_API_Util_Internal::getDomainName (const std::string& strToExtractFrom, std::string& domainName)
{
	size_t pos = (size_t)strToExtractFrom.find(':', 0);

   if (pos == string::npos)
      return false;

   domainName = strToExtractFrom.substr(0, pos);
   return true;
}

bool ACS_CS_API_Util_Internal::getServiceName (const std::string& strToExtractFrom, std::string& serviceName)
{
   size_t pos = (size_t)strToExtractFrom.find(':', 0);

   if (pos == string::npos)
      return false;

   if ((++pos) >= strToExtractFrom.length())
      return false;

   serviceName = strToExtractFrom.substr(pos);
   return true;
}

bool ACS_CS_API_Util_Internal::isDefaultCPName (const std::string& defName)
{
   stringstream addStringStream;
   set<string> invalidNameSet;
   string stringToCheckUpperCase = defName;
   
   if (!stringToUpper(stringToCheckUpperCase))
      return false;
   
   //Add default CP names to the SET (BC0-BC999 and CP0-CP999)
   for (int i = 0; i < 1000; i++)
   {
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"BC"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "BCn" to SET (n = 0 - 999)
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"CP"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "CPn" to SET (n = 0 - 999)
   }

   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name is a default CP name

   if (stringCount == 1)      //check if given name was found
      return true;
   else
      return false;
}

bool ACS_CS_API_Util_Internal::isAllowedName (const std::string& name)
{

   //This function verifies that the string given as argument starts with
   //a letter and contains nothing but letters, digits and hyphens

   string validChars = "";    //string to hold valid characters

   for (int i = 'A'; i <= 'Z'; i++)      //add chars "A-Z"
   {
      validChars = validChars + char(i);
   }

   for (int i = 'a'; i <= 'z'; i++)         //add chars "a-z"
   {
      validChars = validChars + char(i);
   }

   size_t num = strspn((name.substr(0,1)).c_str(), validChars.c_str());   //check if first char in str 1 match chars from str 2
   
   if (num != 1)     //check if string 1 doesn't start with a letter
      return false;

   validChars = validChars + char(45); //add char "-"

   for (int i = '0'; i <= '9'; i++)          //add chars "0-9"
   {
      validChars = validChars + char(i);
   }

   num = 0;
   num = strspn(name.c_str(), validChars.c_str());   //check how many chars in str 1 that match chars from str 2
   
   if (num == name.length())     //check if all characters in string 1 was found in string 2
      return true;
   else                          //otherwise invalid characters have been used
      return false;
}

bool ACS_CS_API_Util_Internal::isAllowedFunctionName (const std::string& name)
{

   //This function verifies that the string given as argument starts with a
   //letter and contains nothing but letters, digits, hyphens and underscores

   string validChars = "";    //string to hold valid characters

   for (int i = 'A'; i <= 'Z'; i++)       //add chars "A-Z"
   {
      validChars = validChars + char(i);
   }

   for (int i = 'a'; i <= 'z'; i++)           //add chars "a-z"
   {
      validChars = validChars + char(i);
   }

   size_t num = strspn((name.substr(0,1)).c_str(), validChars.c_str());   //check if first char in str 1 match chars from str 2
   
   if (num != 1)     //check if string 1 doesn't start with a letter
      return false;

   validChars = validChars + char('-');   //add hyphen ("minus") char
   validChars = validChars + char('_');   //add underscore char

   for (int i = '0'; i <= '9'; i++)           //add chars "0-9"
   {
      validChars = validChars + char(i);
   }

   num = 0;
   num = strspn(name.c_str(), validChars.c_str());   //check how many chars in str 1 that match chars from str 2
   
   if (num == name.length())     //check if all characters in string 1 was found in string 2
      return true;
   else                          //otherwise invalid characters have been used
      return false;

}

bool ACS_CS_API_Util_Internal::isAllowedNEID (const std::string& neid)
{
   //This function verifies that the string given as argument is an empty string or
   //has at most 23 characters. If the string isn't an empty string it must start with
   //a letter and contain nothing but letters, digits, hyphens and spaces.
   //The string mustn't end with a space.

   if (neid == "")   //check if an empty string has been given
      return true;

   if (neid.length() > 23)    //check if invalid neid name length has been given
      return false;

   string validChars = "";    //string to hold valid characters

   for (int i = 'A'; i <= 'Z'; i++)       //add chars "A-Z"
   {
      validChars = validChars + char(i);
   }

   for (int i = 'a'; i <= 'z'; i++)           //add chars "a-z"
   {
      validChars = validChars + char(i);
   }

   size_t num = strspn((neid.substr(0,1)).c_str(), validChars.c_str());   //check if first char in str 1 match chars from str 2
   
   if (num != 1)     //check if string 1 doesn't start with a letter
      return false;

   if (neid.length() > 0)
   {
      if (neid.substr(neid.length() - 1, 1) == " ")   //check if string ends with a space
         return false;
   }

   validChars = validChars + char('-');   //add hyphen ("minus") char
   validChars = validChars + char(' ');   //add space char

   for (int i = '0'; i <= '9'; i++)           //add chars "0-9"
   {
      validChars = validChars + char(i);
   }

   num = 0;
   num = strspn(neid.c_str(), validChars.c_str());   //check how many chars in str 1 that match chars from str 2
   
   if (num == neid.length())     //check if all characters in string 1 was found in string 2
      return true;
   else                          //otherwise invalid characters have been used
      return false;
}

bool ACS_CS_API_Util_Internal::isReservedName (const std::string& name)
{
   stringstream addStringStream;
   set<string> invalidNameSet;
   string stringToCheckUpperCase = name;
   
   if (!stringToUpper(stringToCheckUpperCase))
      return false;
   
   //Add default CP names to the SET (BC0-BC999 and CP0-CP999)
   for (int i = 0; i < 1000; i++)
   {
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"BC"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "BCn" to SET (n = 0 - 999)
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"CP"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "CPn" to SET (n = 0 - 999)
   }

   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_ALL);
   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_ALLBC);
   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_OPGROUP);

   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name is a default CP name

   if (stringCount == 1)      //check if given name was found
      return true;
   else
      return false;
}

bool ACS_CS_API_Util_Internal::isReservedCPName (const std::string& cpName)
{
   stringstream addStringStream;
   set<string> invalidNameSet;
   string stringToCheckUpperCase = cpName;
   
   if (!stringToUpper(stringToCheckUpperCase))
      return false;
   
   //Add default CP names to the SET (BC0-BC999 and CP0-CP999)
   for (int i = 0; i < 1000; i++)
   {
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"BC"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "BCn" to SET (n = 0 - 999)
      addStringStream.str("");   //empty the stringstream
      addStringStream<<"CP"<<i;
      invalidNameSet.insert(addStringStream.str());   //add "CPn" to SET (n = 0 - 999)
   }

   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name is a default CP name

   if (stringCount == 1)      //check if given name was found
      return true;
   else
      return false;
}

bool ACS_CS_API_Util_Internal::isReservedCPGroupName (const std::string& cpGroupName)
{
   set<string> invalidNameSet;
   string stringToCheckUpperCase = cpGroupName;
   
   if (!stringToUpper(stringToCheckUpperCase))
      return false;

   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_ALL);
   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_ALLBC);
   invalidNameSet.insert(ACS_CS_NS::CP_GROUP_OPGROUP);

   size_t stringCount = invalidNameSet.count(stringToCheckUpperCase);   //check if given name is a default CP name

   if (stringCount == 1)      //check if given name was found
      return true;
   else
      return false;
}

bool ACS_CS_API_Util_Internal::isPrintableString (const std::string& stringToCheck)
{
   string validChars = "";    //string to hold valid characters

   for (int i = 'A'; i <= 'Z'; i++)      //add chars "A-Z"
   {
      validChars = validChars + char(i);
   }

   for (int j = 'a'; j <= 'z'; j++)      //add chars "a-z"
   {
      validChars = validChars + char(j);
   }

   for (int k = '0'; k <= '9'; k++)      //add chars "0-9"
   {
      validChars = validChars + char(k);
   }

   validChars = validChars + char(' ');   //add space char
   validChars = validChars + char('\'');  //add apostrofe char
   validChars = validChars + char('(');   //add left parentheses char
   validChars = validChars + char(')');   //add right parentheses char
   validChars = validChars + char('+');   //add plus sign char
   validChars = validChars + char(',');   //add comma char
   validChars = validChars + char('-');   //add minus char
   validChars = validChars + char('.');   //add full stop char
   validChars = validChars + char('/');   //add solidus char
   validChars = validChars + char(':');   //add colon char
   validChars = validChars + char('=');   //add equals sign char
   validChars = validChars + char('?');   //add question mark char

   size_t num = strspn(stringToCheck.c_str(), validChars.c_str());   //check how many chars in str 1 that match chars from str 2
   
   if (num == stringToCheck.length())   //check if all characters in string 1 was found in string 2
      return true;
   else                                //otherwise invalid characters have been used
      return false;
}

bool ACS_CS_API_Util_Internal::isCPIdentifier (const std::string& cpIdentifier)
{
   //This function verifies that the string given as argument contains 
   //nothing but digits and has a value between 0-999 or 1000-1999

   if (cpIdentifier.length() < 1 || cpIdentifier.length() > 4)
      return false;     //cpId should be a string of 1-4 digits

   string validChars = "";    //string to hold valid characters

   for (int i = '0'; i <= '9'; i++)      //add chars "0-9"
   {
      validChars = validChars + char(i);
   }

   size_t num = strspn(cpIdentifier.c_str(), validChars.c_str());   //check how many chars in str 1 that match chars from str 2
   
   if (num == cpIdentifier.length())  //check if all characters in string 1 was found in string 2
   {
      int tempCPId = atoi(cpIdentifier.c_str());  //convert string to integer

      if ((tempCPId >= ACS_CS_Protocol::SysType_BC && tempCPId < (ACS_CS_Protocol::SysType_BC + 1000)) ||   //check if 0-999
         (tempCPId >= ACS_CS_Protocol::SysType_CP && tempCPId < (ACS_CS_Protocol::SysType_CP + 1000)))      //or 1000-1999
         return true;
      else
         return false;
   }
   else                                //otherwise invalid characters have been used
      return false;
}


bool ACS_CS_API_Util_Internal::isCSRunning ()
{

#if defined(CS_API_EXPORT)
   ACS_CS_API_UsageLogger::instance().logUsage(__FUNCTION__);
#endif

   //bool returnValue = false;     //default return value
/* RoGa
   TCPClient* client = 0;        //TCP client

   ACS_CS_API_TCP_Common_Implementation impl;

   if (impl.startSession(&client))
	{
		if (client)
		{
         returnValue = true;

         (void)impl.closeSession(client); //close TCP connection
      }
   }
RoGa */
   //return returnValue;
   return true;

}

bool ACS_CS_API_Util_Internal::isCPDefined (CPID cpId)
{
    bool result = false;
    ACS_CS_ImModel *model = new ACS_CS_ImModel();
    ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

    if(immReader->loadCpObjects(model) > 0){

    	set<const ACS_CS_ImBase *>::iterator it;
    	std::set<const ACS_CS_ImBase *> cpObjects;
    	model->getObjects(cpObjects, CLUSTERCP_T);
    	std::set<const ACS_CS_ImBase *> dualSidedCpObjects;
    	model->getObjects(dualSidedCpObjects, DUALSIDEDCP_T);

    	for(it = cpObjects.begin(); it != cpObjects.end(); it++){
    		const ACS_CS_ImBase *base = *it;
    		const ACS_CS_ImClusterCp* cp = dynamic_cast<const ACS_CS_ImClusterCp*>(base);

    		if(cp && (cp->systemIdentifier == cpId))
    			result = true;

    		if(result)
    			break;
    	}

    	for(it = dualSidedCpObjects.begin(); it != dualSidedCpObjects.end() && result == false; it++)
    	{
    		const ACS_CS_ImBase *base = *it;
    		const ACS_CS_ImDualSidedCp* cp = dynamic_cast<const ACS_CS_ImDualSidedCp*>(base);

    		if(cp && (cp->systemIdentifier == cpId))
    			result = true;

    		if(result)
    			break;

    	}
    }

    delete model;
    delete immReader;

    return result;
}

bool ACS_CS_API_Util_Internal::compareWithoutCase (const std::string& string1, const std::string& string2)
{
   string str1Upper = string1;
   string str2Upper = string2;

   if (!stringToUpper(str1Upper))
      return false;

   if (!stringToUpper(str2Upper))
      return false;

   if (str1Upper == str2Upper)
      return true;
   else
      return false;
}

bool ACS_CS_API_Util_Internal::stringToUpper (std::string& stringToConvert)
{
   //Changes each element of the string to upper case
   string convertedString = stringToConvert;

   for(unsigned int i = 0; i < stringToConvert.length(); i++)
   {
      convertedString[i] = toupper(stringToConvert[i]);
   }

   stringToConvert = convertedString;    //return the converted string

   return true;
}

bool ACS_CS_API_Util_Internal::getClusterOpModeValue (const std::string& requestedModeString, ACS_CS_API_ClusterOpMode::Value& requestedMode)
{
  string req = requestedModeString;
  if(!stringToUpper(req))
  {
    return false;
  }

  if(req == "EXPERT")
  {
      requestedMode = ACS_CS_API_ClusterOpMode::Expert;
  }
  else if(req == "NORMAL")
  {
      requestedMode = ACS_CS_API_ClusterOpMode::Normal;
  }
  else
  {
    return false;
  }

  return true;
}

bool ACS_CS_API_Util_Internal::isValidClusterOpMode (const ACS_CS_API_ClusterOpMode::Value& clusterOpMode)
{
    return(clusterOpMode >= ACS_CS_API_ClusterOpMode::Normal &&
           clusterOpMode <= ACS_CS_API_ClusterOpMode::SwitchingToExpert);
	 return false;
}

bool ACS_CS_API_Util_Internal::getClusterOpModeString (std::string& clusterOpModeStringOut, ACS_CS_API_ClusterOpMode::Value clusterOpMode)
{
   switch(clusterOpMode)
   {
   case ACS_CS_API_ClusterOpMode::Normal:
      clusterOpModeStringOut = "NORMAL MODE";
      break;

   case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
      clusterOpModeStringOut = "SWITCHING TO NORMAL MODE";
      break;

   case ACS_CS_API_ClusterOpMode::Expert:
      clusterOpModeStringOut = "EXPERT MODE";
      break;

   case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
      clusterOpModeStringOut = "SWITCHING TO EXPERT MODE";
      break;
   default:
      clusterOpModeStringOut = "-";
      return false;
   }

   return true;
}

bool ACS_CS_API_Util_Internal::setProfile (ACS_CS_API_OmProfileChange::Profile omProfile,
		ACS_CS_API_OmProfileChange::Profile aptProfile, ACS_CS_API_OmProfileChange::Profile apzProfile,
		ACS_CS_API_OmProfileChange::Profile changeReason)
{
	bool result = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	if(!info) {
		ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::setProfile\n"
				"Failed to set profile values (1)"));
		delete base;
		delete immReader;
		return result;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	info->aptProfileRequested	= aptProfile;
	info->apzProfileRequested	= apzProfile;
	info->omProfileRequested 	= omProfile;

	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);

   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
   saved = saver->save("setProfile");
   delete saver;

   if(saved == ACS_CC_SUCCESS){
		result = true;
	}
	else{
		ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::setProfile\n"
				"Failed to set profile values (2)"));
	}

	delete model;
	delete immReader;

	return result;
}






bool ACS_CS_API_Util_Internal::readOmProfile (ACS_CS_ImModel* /*model*/, ACS_CS_API_OmProfileChange& omProfileChange)
{
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

   if (!advancedConfiguration)
   {
      omProfileChange.omProfileCurrent    = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.omProfileRequested  = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.aptCurrent          = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.aptRequested        = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.apzCurrent          = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.apzRequested        = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.phase               = ACS_CS_API_OmProfilePhase::Idle;
      omProfileChange.changeReason        = ACS_CS_API_OmProfileChange::NoChange;
      omProfileChange.aptQueued           = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.apzQueued           = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   }
   else
   {

	   omProfileChange.omProfileCurrent = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileCurrent);
	   omProfileChange.omProfileRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->omProfileRequested);
	   omProfileChange.aptCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileCurrent);
	   omProfileChange.aptRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileRequested);
	   omProfileChange.aptQueued = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->aptProfileQueued);
	   omProfileChange.apzCurrent   = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileCurrent);
	   omProfileChange.apzRequested = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileRequested);
	   omProfileChange.apzQueued    = static_cast<ACS_CS_API_OmProfileChange::Profile> (advancedConfiguration->apzProfileQueued);
	   omProfileChange.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue> (advancedConfiguration->omProfilePhase);
	   omProfileChange.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue> (advancedConfiguration->omProfileChangeTrigger);
   }

   if (immReader) delete immReader;
   if (advBase) delete advBase;

   return true;
}






bool ACS_CS_API_Util_Internal::readOmProfile (ACS_CS_TableEntry& entry, ACS_CS_API_OmProfileChange& omProfileChange)
{
   ACS_CS_Attribute attOmProfileCurrent(Attribute_NE_OmProfileCurrent);
   ACS_CS_Attribute attOmProfileRequested(Attribute_NE_OmProfileRequested);
   ACS_CS_Attribute attAptProfileCurrent(Attribute_NE_AptProfileCurrent);
   ACS_CS_Attribute attAptProfileRequested(Attribute_NE_AptProfileRequested);
   ACS_CS_Attribute attApzProfileCurrent(Attribute_NE_ApzProfileCurrent);
   ACS_CS_Attribute attApzProfileRequested(Attribute_NE_ApzProfileRequested);
   ACS_CS_Attribute attPhase(Attribute_NE_Phase);
   ACS_CS_Attribute attChangeReason(Attribute_NE_ProfileChangeReason);
   ACS_CS_Attribute attApzProfileQueued(Attribute_NE_ApzProfileQueued);
   ACS_CS_Attribute attAptProfileQueued(Attribute_NE_AptProfileQueued);

   if (entry.getId() == ILLEGAL_TABLE_ENTRY)  // If no entry was found, use defaults.
   {
      omProfileChange.omProfileCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.aptCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.apzCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;
      omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;
      omProfileChange.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      omProfileChange.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   }
   else
   {

      attOmProfileCurrent    = entry.getValue(Attribute_NE_OmProfileCurrent);
      attOmProfileRequested  = entry.getValue(Attribute_NE_OmProfileRequested);
      attAptProfileCurrent   = entry.getValue(Attribute_NE_AptProfileCurrent);
      attAptProfileRequested = entry.getValue(Attribute_NE_AptProfileRequested);
      attApzProfileCurrent   = entry.getValue(Attribute_NE_ApzProfileCurrent);
      attApzProfileRequested = entry.getValue(Attribute_NE_ApzProfileRequested);
      attPhase               = entry.getValue(Attribute_NE_Phase);
      attChangeReason        = entry.getValue(Attribute_NE_ProfileChangeReason);
      attApzProfileQueued    = entry.getValue(Attribute_NE_ApzProfileQueued);
      attAptProfileQueued    = entry.getValue(Attribute_NE_AptProfileQueued);

      char buffer[sizeof(int) + 1]; // buffer must be big enough to hold the biggest attribute
      // phase attribute is biggest (int)


      if(attOmProfileCurrent.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.omProfileCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      }
      else
      {
         attOmProfileCurrent.getValue(buffer,sizeof(omProfileChange.omProfileCurrent));
         omProfileChange.omProfileCurrent = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile  *>(buffer));
      }

      if(attOmProfileRequested.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      }
      else
      {
         attOmProfileRequested.getValue(buffer,sizeof(omProfileChange.omProfileRequested));
         omProfileChange.omProfileRequested = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attAptProfileCurrent.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.aptCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      }
      else
      {
         attAptProfileCurrent.getValue(buffer,sizeof(omProfileChange.aptCurrent));
         omProfileChange.aptCurrent = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attAptProfileRequested.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      }
      else
      {
         attAptProfileRequested.getValue(buffer,sizeof(omProfileChange.aptRequested));
         omProfileChange.aptRequested = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attApzProfileCurrent.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.apzCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
      }
      else
      {
         attApzProfileCurrent.getValue(buffer,sizeof(omProfileChange.apzCurrent));
         omProfileChange.apzCurrent = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attApzProfileRequested.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      }
      else
      {
         attApzProfileRequested.getValue(buffer,sizeof(omProfileChange.apzRequested));
         omProfileChange.apzRequested = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attPhase.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;
      }
      else
      {
         attPhase.getValue(buffer,sizeof(omProfileChange.phase));
         omProfileChange.phase = static_cast<ACS_CS_API_OmProfilePhase::PhaseValue>(*(reinterpret_cast<int *>(buffer)));
      }

      if(attChangeReason.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;
      }
      else
      {
         attChangeReason.getValue(buffer,sizeof(omProfileChange.changeReason));
         omProfileChange.changeReason = static_cast<ACS_CS_API_OmProfileChange::ChangeReasonValue>(*(reinterpret_cast<int *>(buffer)));
      }

      if(attApzProfileQueued.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      }
      else
      {
         attApzProfileQueued.getValue(buffer,sizeof(omProfileChange.apzQueued));
         omProfileChange.apzQueued = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

      if(attAptProfileQueued.getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
      {
         omProfileChange.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
      }
      else
      {
         attAptProfileQueued.getValue(buffer,sizeof(omProfileChange.aptQueued));
         omProfileChange.aptQueued = *(reinterpret_cast<ACS_CS_API_OmProfileChange::Profile *>(buffer));
      }

   }
   return true;
}

bool ACS_CS_API_Util_Internal::writeOmProfile (const ACS_CS_API_OmProfileChange& newProfile)
{
	bool result = false;


	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* info = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(base);

	if(!info)
	{
		ACS_CS_API_TRACER_MESSAGE(("Failed getting advancedConfiguration object"));
		if (base) delete base;
		delete immReader;
		return result;
	}

	ACS_CC_ReturnType saved = ACS_CC_FAILURE;
	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	info->aptProfileCurrent		= newProfile.aptCurrent;
	info->aptProfileQueued		= newProfile.aptQueued;
	info->aptProfileRequested	= newProfile.aptRequested;

	info->apzProfileCurrent		= newProfile.apzCurrent;
	info->apzProfileQueued		= newProfile.apzQueued;
	info->apzProfileRequested	= newProfile.apzRequested;

	info->omProfileCurrent = newProfile.omProfileCurrent;
	info->omProfileRequested = newProfile.omProfileRequested;

	switch(newProfile.changeReason)
	{
		case ACS_CS_API_OmProfileChange::NoChange:
			info->omProfileChangeTrigger = NO_CHANGE;
			break;
		case ACS_CS_API_OmProfileChange::NechCommand:
			info->omProfileChangeTrigger = MANUAL;
			break;
		case ACS_CS_API_OmProfileChange::NechCommandForApgOnly:
			info->omProfileChangeTrigger = MANUAL_FOR_APG_ONLY;
			break;
		case ACS_CS_API_OmProfileChange::AutomaticProfileAlignment:
			info->omProfileChangeTrigger = AUTOMATIC;
			break;
		default:
			info->omProfileChangeTrigger = UNDEF_PROFILECHANGETRIGGER;
			break;
	}

	switch(newProfile.phase)
	{
		case ACS_CS_API_OmProfilePhase::ApNotify:
			info->omProfilePhase = AP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::CpNotify:
			info->omProfilePhase = CP_NOTIFY;
			break;
		case ACS_CS_API_OmProfilePhase::Commit:
			info->omProfilePhase = COMMIT;
			break;
		case ACS_CS_API_OmProfilePhase::Validate:
			info->omProfilePhase = VALIDATE;
			break;
		case ACS_CS_API_OmProfilePhase::Idle:
			info->omProfilePhase = IDLE;
			break;
		default: info->omProfilePhase = UNDEF_OMPROFILEPHASE;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	model->addObject(info);
   ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
   saved = saver->save(__FUNCTION__);
   delete saver;

	if(saved == ACS_CC_SUCCESS){
		result = true;
	}
	else{
		ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_NeHandling::updatePhaseChange\n"
				"Failed to set property for updating phase change (2)"));
	}

	delete model;
	delete immReader;

	return result;
}

bool ACS_CS_API_Util_Internal::writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode)
{
	bool returnValue = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(!immReader)
		return false;

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);

	if(!base){
		delete immReader;
		return false;
	}

	ACS_CS_ImCpCluster* info = dynamic_cast<ACS_CS_ImCpCluster*>(base);

	if(!info) {
		delete immReader;
		delete base;

		return false;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	switch(clusterOpMode){
	case ACS_CS_API_ClusterOpMode::Expert:
		info->clusterOpMode = EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::Normal:
		info->clusterOpMode = NORMAL;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
		info->clusterOpMode = SWITCHING_TO_EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
		info->clusterOpMode = SWITCHING_TO_NORMAL;
		break;
	default: info->clusterOpMode = UNDEF_CLUSTEROPMODE;

	}

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	if (model) {
		model->addObject(info);

		ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
		ACS_CC_ReturnType saved = saver->save();
		delete saver;

		if(saved == ACS_CC_FAILURE){
			ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::writeClusterOpMode()\n"
					"Could not save to IMM"));
		}
		else{
			returnValue = true;
		}

		delete model;
	} else {
		delete info;
	}

	if(immReader)
		delete immReader;

	return returnValue;
}

bool ACS_CS_API_Util_Internal::writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode,
		ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType)
{
	bool returnValue = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(!immReader)
		return false;

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);

	if(!base){
		delete immReader;
		return false;
	}

	ACS_CS_ImCpCluster* info = dynamic_cast<ACS_CS_ImCpCluster*>(base);

	if(!info) {
		delete immReader;

		if (base)
			delete base;

		return false;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	switch(clusterOpMode){
	case ACS_CS_API_ClusterOpMode::Expert:
		info->clusterOpMode = EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::Normal:
		info->clusterOpMode = NORMAL;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
		info->clusterOpMode = SWITCHING_TO_EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
		info->clusterOpMode = SWITCHING_TO_NORMAL;
		break;
	default: info->clusterOpMode = UNDEF_CLUSTEROPMODE;
	}

	switch(opType){
	case CLUSTER_OP_MODE_CHANGED:
		info->clusterOpModeType = CLUSTER_OP_MODE_CHANGED;
		break;
	case CLUSTER_OP_MODE_REPLICATION:
		info->clusterOpModeType = CLUSTER_OP_MODE_REPLICATION;
		break;
	case CLUSTER_OP_MODE_REQUEST:
		info->clusterOpModeType = CLUSTER_OP_MODE_REQUEST;
		break;
	default: info->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
	}

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	if (model) {
		model->addObject(info);

		ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
		ACS_CC_ReturnType saved = saver->save();
		delete saver;

		if(saved == ACS_CC_FAILURE){
			ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::writeClusterOpMode()\n"
					"Could not save to IMM"));
		}
		else{
			returnValue = true;
		}

		delete model;
	}
	else
	{
		delete info;
	}

	if(immReader)
		delete immReader;

	return returnValue;
}

bool ACS_CS_API_Util_Internal::checkForMMLCommandRulesFile (const ACS_CS_API_OmProfileChange::Profile /*profile*/)
{
	/*
	ostringstream mmlFileStream;
	//mmlFileStream << getMCSDataDir() << CC_FILE_DIR << CC_FILENAME_BASE << profile << CC_FILENAME_EXT;
	mmlFileStream << CC_FILE_DIR << ACS_CS_Registry::getAPTType() << CC_FILENAME_BASE << profile << CC_FILENAME_EXT;
	string mmlFile(mmlFileStream.str());

	ifstream ins(mmlFile.c_str());

	if(ins.fail())
	{
		return false;
	}*/

	return true;
}

bool ACS_CS_API_Util_Internal::writeEmptyOG_ReasonCode (ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode reasonCode)
{
	bool returnValue = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(!immReader)
		return false;

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);

	if(!base){
		delete immReader;
		return false;
	}

	ACS_CS_ImCpCluster* info = dynamic_cast<ACS_CS_ImCpCluster*>(base);

	if(!info) {
		delete immReader;
		delete base;
		return false;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	switch(reasonCode){
	case ACS_CS_API_OgChange::NotEmpty:
		info->ogClearCode = NOT_EMPTY;
		break;
	case ACS_CS_API_OgChange::ClusterRecovery:
		info->ogClearCode = CLUSTER_RECOVERY;
		break;
	case ACS_CS_API_OgChange::OtherReason:
		info->ogClearCode = OTHER_REASON;
		break;
	default: info->ogClearCode = UNDEF_CLEARCODE;
	}

	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	if (model) {
		model->addObject(info);

		ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
		ACS_CC_ReturnType saved = saver->save();
		delete saver;

		if(saved == ACS_CC_FAILURE){
			ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::writeEmptyOG_ReasonCode()\n"
					"Could not save to IMM"));
		}
		else{
			returnValue = true;
		}

		delete model;
	}

	if(immReader)
		delete immReader;

	return returnValue;
}

bool ACS_CS_API_Util_Internal::writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode,
		ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType, AsyncActionStateType state, AsyncActionType idAction, ActionResultType result, std::string reason)
{
	bool returnValue = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	if(!immReader)
		return false;

	ACS_CS_ImBase *base= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	ACS_CS_ImBase *baseStruct= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER_STRUCT);

	if(!base || !baseStruct){
		delete immReader;
		return false;
	}

	ACS_CS_ImCpCluster* info = dynamic_cast<ACS_CS_ImCpCluster*>(base);
	ACS_CS_ImCpClusterStruct* infoStruct = dynamic_cast<ACS_CS_ImCpClusterStruct*>(baseStruct);

	if(!info || !infoStruct) {
		delete immReader;

		if (base)
			delete base;
		if (baseStruct)
			delete baseStruct;

		return false;
	}

	info->action = ACS_CS_ImBase::MODIFY;

	switch(clusterOpMode){
	case ACS_CS_API_ClusterOpMode::Expert:
		info->clusterOpMode = EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::Normal:
		info->clusterOpMode = NORMAL;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToExpert:
		info->clusterOpMode = SWITCHING_TO_EXPERT;
		break;
	case ACS_CS_API_ClusterOpMode::SwitchingToNormal:
		info->clusterOpMode = SWITCHING_TO_NORMAL;
		break;
	default: info->clusterOpMode = UNDEF_CLUSTEROPMODE;
	}

	switch(opType){
	case CLUSTER_OP_MODE_CHANGED:
		info->clusterOpModeType = CLUSTER_OP_MODE_CHANGED;
		break;
	case CLUSTER_OP_MODE_REPLICATION:
		info->clusterOpModeType = CLUSTER_OP_MODE_REPLICATION;
		break;
	case CLUSTER_OP_MODE_REQUEST:
		info->clusterOpModeType = CLUSTER_OP_MODE_REQUEST;
		break;
	default: info->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
	}

	string currentDate("");
	string currentTime("");
	getCurrentTime(currentDate,currentTime);

	infoStruct->action = ACS_CS_ImBase::MODIFY;
	infoStruct->actionId = idAction;
	infoStruct->state = state;
	infoStruct->result = result;
	infoStruct->timeOfLastAction = currentDate +" "+currentTime;
	if (reason.compare("") == 0) infoStruct->reason.clear();
	else infoStruct->reason = reason;


	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	if (model) {
		model->addObject(info);
		model->addObject(infoStruct);

		ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
		ACS_CC_ReturnType saved = saver->save("ModdImmAsyncActionProgress");
		delete saver;

		if(saved == ACS_CC_FAILURE){
			ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::writeClusterOpMode()\n"
					"Could not save to IMM"));
		}
		else{
			returnValue = true;
		}

		delete model;
	}
	else
	{
		delete info;
		delete infoStruct;
	}

	if(immReader)
		delete immReader;

	return returnValue;
}


bool ACS_CS_API_Util_Internal::readClusterOpMode (ACS_CS_API_ClusterOpMode::Value & clusterOpMode)
{

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *bcBase= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER);
	ACS_CS_ImCpCluster* bc = dynamic_cast<ACS_CS_ImCpCluster*>(bcBase);

   if (bc == NULL)
   {
	   ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::readClusterOpMode()\n"
			   "...using default Normal"));

	   // Use default value of Normal
	   clusterOpMode = ACS_CS_API_ClusterOpMode::Normal;
   }
   else
   {
	   ClusterOpModeEnum com = bc->clusterOpMode;
	   clusterOpMode = (ACS_CS_API_ClusterOpMode::Value) com;
   }

   delete immReader;
   delete bcBase;

   return true;
}


void ACS_CS_API_Util_Internal::getCurrentTime(std::string& dateStr, std::string& timeStr)
{
	time_t longTime;
	time(&longTime);  // Get the current time
	struct tm* today = localtime(&longTime); // Convert to local time

	// Retrieve time information
	char dateChar[256] = {0};
	strftime(dateChar, sizeof (dateChar) - 1, "%Y-%m-%d", today);

	char timeChar[256] = {0};
	strftime(timeChar, sizeof (timeChar) - 1, "%H:%M:%S", today);

	dateStr = dateChar;
	timeStr = timeChar;
}

bool ACS_CS_API_Util_Internal::writeAsyncActionStruct (AsyncActionStateType state, AsyncActionType idAction, ActionResultType result, std::string reason, std::string candidateFilename)
{
	bool returnValue = false;
	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();

	ACS_CS_ImBase *baseStruct = NULL;
	ACS_CS_ImCpClusterStruct* infoStruct = NULL;
	ACS_CS_ImCandidateCcFileStruct* infoStructCcFile = NULL;
	ACS_CS_ImOmProfileStruct* infoStructProfile = NULL;

	if(!immReader)
		return false;

	if (idAction == CHANGE_OP_MODE )
		baseStruct= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER_STRUCT);
	else if (idAction == IMPORT_CCF ){
		string candidateStructDn = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=" + candidateFilename + "," + ACS_CS_ImmMapper::ATTR_CANDIDATE_CCFILE_ID + "=" + candidateFilename + "," +  ACS_CS_ImmMapper::RDN_AXE_CCFILE_MANAGER;
		baseStruct= immReader->getObject(candidateStructDn);
	}
	else
	{
		//at the end of the activation or changing of the OmProfile
		//the idAction attribute is setted to UNDEFINED
		baseStruct= immReader->getObject(ACS_CS_ImmMapper::RDN_AXE_OM_PROFILE_MANAGER_STRUCT);
	}

	if(!baseStruct){
		delete immReader;
		return false;
	}

	if (idAction == CHANGE_OP_MODE )
		infoStruct = dynamic_cast<ACS_CS_ImCpClusterStruct*>(baseStruct);
	else if (idAction == IMPORT_CCF )
		infoStructCcFile = dynamic_cast<ACS_CS_ImCandidateCcFileStruct*>(baseStruct);
	else
		infoStructProfile = dynamic_cast<ACS_CS_ImOmProfileStruct*>(baseStruct);

	if ((!infoStruct) && (!infoStructCcFile) && (!infoStructProfile)) {
		delete immReader;

		if (baseStruct)
			delete baseStruct;

		return false;
	}

	string currentDate("");
	string currentTime("");
	getCurrentTime(currentDate,currentTime);

	if (infoStruct)	infoStruct->action = ACS_CS_ImBase::MODIFY;
	else if (infoStructCcFile)	infoStructCcFile->action = ACS_CS_ImBase::MODIFY;
	else if (infoStructProfile)	infoStructProfile->action = ACS_CS_ImBase::MODIFY;

	if (idAction != UNDEF_ACTIONTYPE){
		if (infoStruct)	infoStruct->actionId = idAction;
		else if (infoStructCcFile)	infoStructCcFile->actionId = idAction;
		else if (infoStructProfile)	infoStructProfile->actionId = idAction;

		if (infoStruct)	infoStruct->reason.clear();
		else if (infoStructCcFile)	infoStructCcFile->reason.clear();
		else if (infoStructProfile)	infoStructProfile->reason.clear();
	}

	if (result != UNDEF_ACTIONRESULTTYPE){
		if (infoStruct)	infoStruct->result = result;
		else if (infoStructCcFile)	infoStructCcFile->result = result;
		else if (infoStructProfile)	infoStructProfile->result = result;
	}

	if (infoStruct)	infoStruct->state = state;
	else if (infoStructCcFile) infoStructCcFile->state = state;
	else if (infoStructProfile) infoStructProfile->state = state;

	if (infoStruct)	infoStruct->timeOfLastAction = currentDate +" "+currentTime;
	else if (infoStructCcFile) infoStructCcFile->timeOfLastAction = currentDate +" "+currentTime;
	else if (infoStructProfile) infoStructProfile->timeOfLastAction = currentDate +" "+currentTime;

	if (reason.compare("") != 0){
		if (infoStruct)	infoStruct->reason = reason;
		else if (infoStructCcFile)	infoStructCcFile->reason = reason;
		else if (infoStructProfile)	infoStructProfile->reason = reason;
	}


	ACS_CS_ImModel *model = new ACS_CS_ImModel();

	if (model) {
		if (infoStruct)	model->addObject(infoStruct);
		else if (infoStructCcFile)	model->addObject(infoStructCcFile);
		else if (infoStructProfile)	model->addObject(infoStructProfile);

		ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(model);
		ACS_CC_ReturnType saved = saver->save("ModdImmAsyncActionProgress");
		delete saver;

		if(saved == ACS_CC_FAILURE){
			ACS_CS_API_TRACER_MESSAGE(("ACS_CS_API_Util_Internal::writeClusterOpMode()\n"
					"Could not save to IMM"));
		}
		else{
			returnValue = true;
		}

		delete model;
	}
	else
	{
		if (infoStruct)	delete infoStruct;
		if (infoStructCcFile) delete infoStructCcFile;
		if (infoStructProfile) delete infoStructProfile;
	}

	if(immReader)
		delete immReader;

	return returnValue;
}

bool ACS_CS_API_Util_Internal::isClusterAlignedAfterRestore()
{
	bool isClusterAlignedAfterRestore = true;

	ACS_CS_ImIMMReader * immReader = new ACS_CS_ImIMMReader();
	ACS_CS_ImBase *advBase= immReader->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION);
	ACS_CS_ImAdvancedConfiguration* advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

	if (advancedConfiguration)
		isClusterAlignedAfterRestore = advancedConfiguration->clusterAligned;

	if (immReader) delete immReader;
	if (advBase) delete advBase;

	return isClusterAlignedAfterRestore;
}
