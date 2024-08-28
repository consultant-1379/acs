//	*********************************************************
//	********************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************
//	********************

#ifndef ACS_CS_API_Util_Internal_h
#define ACS_CS_API_Util_Internal_h 1

#include <vector>
#include <iostream>

#include "ACS_CS_API.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_API_NeHandling.h"
#include "ACS_CS_ImModel.h"

typedef uint16_t APID;
typedef uint16_t CPID;


class ACS_CS_API_Util_Internal
{

  public:

    static bool getDefaultCPName (uint16_t cpId, std::string& defName);

    static bool getDefaultAPName (uint16_t apId, std::string& defName);

    static bool getAPIdentifier (const std::string& apName, uint16_t& apIdentifier);

    static bool getDomainName (const std::string& strToExtractFrom, std::string& domainName);

    static bool getServiceName (const std::string& strToExtractFrom, std::string& serviceName);

    static bool isDefaultCPName (const std::string& defName);

    static bool isAllowedName (const std::string& name);

    static bool isAllowedFunctionName (const std::string& name);

    static bool isAllowedNEID (const std::string& neid);

    static bool isReservedName (const std::string& name);

    static bool isReservedCPName (const std::string& cpName);

    static bool isReservedCPGroupName (const std::string& cpGroupName);

    static bool isPrintableString (const std::string& stringToCheck);

    static bool isCPIdentifier (const std::string& cpIdentifier);

    static bool isCSRunning ();

    static bool isCPDefined (CPID cpId);

    static bool compareWithoutCase (const std::string& string1, const std::string& string2);

    static bool getClusterOpModeValue (const std::string& requestedModeString, ACS_CS_API_ClusterOpMode::Value& requestedMode);

    static bool isValidClusterOpMode (const ACS_CS_API_ClusterOpMode::Value& clusterOpMode);

    static bool getClusterOpModeString (std::string& clusterOpModeStringOut, ACS_CS_API_ClusterOpMode::Value clusterOpMode);

    static bool setProfile (ACS_CS_API_OmProfileChange::Profile omProfile, ACS_CS_API_OmProfileChange::Profile aptProfile,
              ACS_CS_API_OmProfileChange::Profile apzProfile, ACS_CS_API_OmProfileChange::Profile changeReason);

    static bool readOmProfile (ACS_CS_TableEntry& entry, ACS_CS_API_OmProfileChange& omProfileChange);

    // TODO: fix/verify this function when implementing MCP, look at the original method above
    static bool readOmProfile (ACS_CS_ImModel* model, ACS_CS_API_OmProfileChange& omProfileChange);

    static bool writeOmProfile (const ACS_CS_API_OmProfileChange& omProfileChange);

    static bool writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode);

    static bool writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode,
    		ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType);

    static bool checkForMMLCommandRulesFile (const ACS_CS_API_OmProfileChange::Profile profile);

    static bool writeEmptyOG_ReasonCode (ACS_CS_API_OgChange_R1::ACS_CS_API_EmptyOG_ReasonCode reasonCode);

    static bool writeClusterOpMode (ACS_CS_API_ClusterOpMode::Value clusterOpMode,
    		ACS_CS_API_NE_NS::ACS_CS_ClusterOpModeType opType, AsyncActionStateType state, AsyncActionType idAction,
    		ActionResultType result = NOT_AVAILABLE, std::string reason = "");

    static bool readClusterOpMode (ACS_CS_API_ClusterOpMode::Value & clusterOpMode);

    static bool writeAsyncActionStruct (AsyncActionStateType state, AsyncActionType idAction, ActionResultType result = NOT_AVAILABLE, std::string reason = "", std::string candidateFilename = "");

    static bool isClusterAlignedAfterRestore();

  protected:

    ACS_CS_API_Util_Internal();

  private:

     static bool stringToUpper (std::string& stringToConvert);
     static void getCurrentTime(std::string& dateStr, std::string& timeStr);


     //static bool writeClusterModeAttributes(std::vector<ACS_CS_Attribute> &attributes);

};

#endif
