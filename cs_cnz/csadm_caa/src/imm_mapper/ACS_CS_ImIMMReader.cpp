/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImIMMReader.cpp
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XBJOAXE
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImObjectCreator.h"
#include "ACS_CS_ImUtils.h"
#include "ACS_APGCC_Util.H"
#include "ACS_CS_API_Tracer.h"
#include "ACS_CS_Trace.h"

ACS_CS_Trace_TDEF(ACS_CS_ImIMMReader_TRACE);
ACS_CS_API_TRACER_DEFINE(ACS_CS_ImIMMReader_TRACE);

using namespace std;


ACS_CS_ImIMMReader::ACS_CS_ImIMMReader()
{
	classNames.push_back(ACS_CS_ImmMapper::CLASS_EQUIPMENT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_HARDWARE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_ADVANCED_CONF);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_LOGICAL);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_SHELF);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_APG);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CLUSTER_CP);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_CLUSTER);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_SERVICE_CATEGORY);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN_CATEGORY);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OTHER_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_BLADE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_AP_SERVICE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_VLAN);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CCFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE_MANAGER);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CCFILE_MANAGER);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CANDIDATE_CCFILE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_OM_PROFILE_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CANDIDATE_STRUCT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CP_PRODUCT_INFO);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRMGMT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_COMPUTE_RESOURCE_NETWORK);
//	classNames.push_back(ACS_CS_ImmMapper::CLASS_IPLB_CLUSTER); //TODO: not in the scope for AXEX17A

	// To load Function Distribution objects
	classNames.push_back(ACS_CS_ImmMapper::CLASS_FUNCTION_DIST);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_APGNODE);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_FUNCTION);

	// To load Crm Compute Resource objects
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRM_EQUIPMENT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_CRM_COMPUTE_RESOURCE);

	// To load TransportM objects
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_TRANSPORT);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_EXTERNALNETWORK);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_INTERNALNETWORK);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_HOST);
	classNames.push_back(ACS_CS_ImmMapper::CLASS_TRM_INTERFACE);
}

ACS_CS_ImIMMReader::~ACS_CS_ImIMMReader()
{
}

bool ACS_CS_ImIMMReader::loadModel(ACS_CS_ImModel *model)
{
   ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::loadModel(ACS_CS_ImModel *model)"));
//   std::vector<std::string> rdn_list = getChildren(rdn, scope);

   vector<std::string> rdn_list = getClassInstancesRdns(classNames);

   if (rdn_list.empty())
      return false;

   populateModel(model, rdn_list);

   return true;
}

int ACS_CS_ImIMMReader::loadBladeObjects(ACS_CS_ImModel *model)
{
	std::vector<std::string> rdnList;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	OmHandler immHandle;
	result = immHandle.Init();

	if (result != ACS_CC_SUCCESS)
	{
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE, LOG_LEVEL_ERROR,
				"ACS_CS_ImIMMReader::loadBladeObjects()\nERROR: Initialization of OmHandler FAILED!!!"));
		return -1;
	}

	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_AP_BLADE.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_CP_BLADE.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_OTHER_BLADE.c_str(), rdnList);

	result = immHandle.Finalize();

	if (result != ACS_CC_SUCCESS)
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE,
				LOG_LEVEL_INFO,
				"ACS_CS_ImIMMReader::loadBladeObjects()\nERROR: Finalization of OmHandler FAILED!!!\n%s",
				immHandle.getInternalLastErrorText()));

	if (rdnList.empty())
		return 0;

	if(!populateModel(model, rdnList))
		return -1;

	return rdnList.size();
}

int ACS_CS_ImIMMReader::loadCpObjects(ACS_CS_ImModel *model)
{
	int retCode = -1;
	ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_ImIMMReader::loadCpObjects()");

	std::vector<std::string> rdnList;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	OmHandler immHandle;
	result = immHandle.Init();

	if (result != ACS_CC_SUCCESS)
	{
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE, LOG_LEVEL_ERROR,
				"ACS_CS_ImIMMReader::loadCpObjects()\nERROR: Initialization of OmHandler FAILED!!!"));
		return retCode;
	}

	retCode = 0;

	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_CLUSTER_CP.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_DUAL_SIDED_CP.c_str(), rdnList);

	if (!rdnList.empty())
	{
		ACS_CS_API_TRACER_MESSAGE("ACS_CS_ImIMMReader::loadCpObjects() -> rdnList is NOT empty - size == %d!", rdnList.size());
		if(!populateModel(model, immHandle, rdnList))
		{
			retCode = -1;
		}
	}
	else
		ACS_CS_API_TRACER_MESSAGE("ACS_CS_ImIMMReader::loadCpObjects() -> rdnList is empty!");

	result = immHandle.Finalize();

	if (result != ACS_CC_SUCCESS)
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE,
				LOG_LEVEL_INFO,
				"ACS_CS_ImIMMReader::loadCpObjects()\nERROR: Finalization of OmHandler FAILED!!!\n%s",
				immHandle.getInternalLastErrorText()));

	if(retCode < 0)
		return retCode;

	ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_ImIMMReader::loadCpObjects() - return code(rdnSize) == %d", rdnList.size());
	return rdnList.size();
}

bool ACS_CS_ImIMMReader::loadModel(ACS_CS_ImModel *model, const std::string &rootRdn, ACS_APGCC_ScopeT scope) const
{
   ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::loadModel(ACS_CS_ImModel *model)"));

	std::vector<std::string> rdn_list = getChildren(rootRdn, scope);

	if (rdn_list.empty())
		return false;

	populateModel(model, rdn_list);

	return true;
}


std::vector<std::string> ACS_CS_ImIMMReader::getChildren(const string &rdn, ACS_APGCC_ScopeT scope)
{
   OmHandler immHandle;
   ACS_CC_ReturnType result = ACS_CC_SUCCESS;

   char *rootName = const_cast<char*> (rdn.c_str());
   std::vector<std::string> rdnList;
   rdnList.push_back(rdn);

   result = immHandle.Init();
   if (result != ACS_CC_SUCCESS) {
      ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getChildren(const string rdn, ACS_APGCC_ScopeT scope)"
            "ERROR: Initialization FAILURE!!!"));
   }

   result = immHandle.getChildren(rootName, scope, &rdnList);
   if (result == ACS_CC_SUCCESS) {
      ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getChildren(const string rdn, ACS_APGCC_ScopeT scope)"
            "INFO: getChildren Successful"));
   }

   result = immHandle.Finalize();

   if (result != ACS_CC_SUCCESS) {
      ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getChildren(const sATTR_TRM_INTERNALINTERFACE_IDtring rdn, ACS_APGCC_ScopeT scope)"
            "ERROR: Finalization FAILURE!!!"));
   }

   return rdnList;;
}


std::vector<std::string> ACS_CS_ImIMMReader::getClassInstancesRdns(const std::vector<std::string> &classNames)
{
	std::vector<std::string>::const_iterator it;
	std::vector<std::string> rdnList;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	OmHandler immHandle;
	result = immHandle.Init();
	if (result != ACS_CC_SUCCESS)
	{
		ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getInstances()\nERROR: Initialization of OmHandler FAILED!!!"));
	}


	for(it = classNames.begin(); it != classNames.end(); ++it)
	{
		if(immHandle.getClassInstances(it->c_str(), rdnList) != ACS_CC_SUCCESS)
		{
			ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getInstances()\nCould not fetch objects for %s class.", it->c_str()));
		}
	}


	result = immHandle.Finalize();
	if (result != ACS_CC_SUCCESS)
	{
		ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE, "ACS_CS_ImIMMReader::getInstances()\nERROR: Finalization of OmHandler FAILED!!!"));
	}

	return rdnList;
}


bool ACS_CS_ImIMMReader::populateModel(ACS_CS_ImModel *model, const std::vector<std::string> &rdn_list)
{
	 OmHandler immHandle;

	 ACS_CC_ReturnType result;

	 result = immHandle.Init();

	 if (result != ACS_CC_SUCCESS)
	 {
		ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
									   "ACS_CS_ImIMMReader::getObject()",
									   "Failed in OmHandler.init()"));
			 return false;
	 }

	    for (vector<string>::const_iterator it = rdn_list.begin(); it != rdn_list.end(); ++it) {
	       ACS_CS_ImBase *obj = getObject(*it, immHandle);

	       if (!obj)
	       {
	          // If we get NULL here it means we have read av object that we cannot convert to our internal structure
	          // We really should return false here and notify the user that there are unsupported information in the
	          // imm-model
	       }
	       else {
	          model->addObject(obj);
	          //ACS_CS_ImUtils::printImBaseObject(obj);
	       }
	    }
	    result = immHandle.Finalize();

	    if (result != ACS_CC_SUCCESS)
	    {
	                 ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
	                                 "ACS_CS_ImIMMReader::getObject()",
	                                 "Failed in OmHandler.Finalize() "));
	    }

	    return true;
}


ACS_CS_ImBase * ACS_CS_ImIMMReader::getObject(const std::string &rdn)
{

   OmHandler immHandle;
   ACS_CC_ReturnType result = ACS_CC_SUCCESS;

   if(rdn.length() < 1){
		ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
				"ACS_CS_ImIMMReader::getObject()",
				"Error: No RDN provided() "));
	    return NULL;
   }

   result = immHandle.Init();
   if (result != ACS_CC_SUCCESS) {
	   ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
	   				"ACS_CS_ImIMMReader::getObject()",
	   				"Failed in OmHandler.init() "));
     // immHandle.Finalize(); //HY96205
      return NULL;

   }

   char *nameObject = const_cast<char*> (rdn.c_str());
   ACS_APGCC_ImmObject object;
   object.objName = nameObject;

   result = immHandle.getObject(&object);
   if (result != ACS_CC_SUCCESS) {
	   ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
	   				"ACS_CS_ImIMMReader::getObject()",
	   				"Failed in OmHandler.getObject() "));
      immHandle.Finalize();
      return NULL;
   }

   ACS_CS_ImBase *imBaseObj = ACS_CS_ImObjectCreator::createImBaseObject(object);

   result = immHandle.Finalize();
   if (result != ACS_CC_SUCCESS) {
		ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
				"ACS_CS_ImIMMReader::getObject()",
				"Failed in OmHandler.Finalize() "));
   }

   return imBaseObj;

}

ACS_CS_ImBase * ACS_CS_ImIMMReader::getObject(const std::string &rdn, OmHandler &immHandle)
{
    ACS_CC_ReturnType result;

    if(rdn.length() < 1){
                 ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
                                 "ACS_CS_ImIMMReader::getObject()",
                                 "Error: No RDN provided() "));
             return NULL;
    }

    ACS_APGCC_ImmObject object;
    object.objName = rdn;

    result = immHandle.getObject(&object);

    if (result != ACS_CC_SUCCESS)
    {
            ACS_CS_TRACE((ACS_CS_ImIMMReader_TRACE,
                                           "ACS_CS_ImIMMReader::getObject()",
                                           "Failed in OmHandler.getObject() "));
       return NULL;
    }

    ACS_CS_ImBase *imBaseObj = ACS_CS_ImObjectCreator::createImBaseObject(object);

    return imBaseObj;
 }


int ACS_CS_ImIMMReader::loadBladeShelfObjects(ACS_CS_ImModel *model)
{
	std::vector<std::string> rdnList;
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;

	OmHandler immHandle;
	result = immHandle.Init();

	if (result != ACS_CC_SUCCESS)
	{
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE, LOG_LEVEL_ERROR,
				"ACS_CS_ImIMMReader::loadBladeObjects()\nERROR: Initialization of OmHandler FAILED!!!"));
		return -1;
	}
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_SHELF.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_AP_BLADE.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_CP_BLADE.c_str(), rdnList);
	immHandle.getClassInstances(ACS_CS_ImmMapper::CLASS_OTHER_BLADE.c_str(), rdnList);

	result = immHandle.Finalize();

	if (result != ACS_CC_SUCCESS)
		ACS_CS_FTRACE((ACS_CS_ImIMMReader_TRACE,
				LOG_LEVEL_INFO,
				"ACS_CS_ImIMMReader::loadBladeObjects()\nERROR: Finalization of OmHandler FAILED!!!\n%s",
				immHandle.getInternalLastErrorText()));

	if (rdnList.empty())
		return 0;

	if(!populateModel(model, rdnList))
		return -1;

	return rdnList.size();
}

bool ACS_CS_ImIMMReader::populateModel(ACS_CS_ImModel *model, OmHandler &immHandle, const std::vector<std::string> &rdn_list)
{
	ACS_CS_API_TRACER_MESSAGE("Entering ACS_CS_ImIMMReader::populateModel(immHandle)");
	ACS_CC_ReturnType result;

	for (vector<string>::const_iterator it = rdn_list.begin(); it != rdn_list.end(); ++it) {
		ACS_CS_ImBase *obj = getObject(*it, immHandle);

		if (!obj)
		{
			// If we get NULL here it means we have read av object that we cannot convert to our internal structure
			// We really should return false here and notify the user that there are unsupported information in the
			// imm-model
		}
		else {
			model->addObject(obj);
			//ACS_CS_ImUtils::printImBaseObject(obj);
		}
	}
	ACS_CS_API_TRACER_MESSAGE("Leaving ACS_CS_ImIMMReader::populateModel(immHandle)");
	return true;
}
