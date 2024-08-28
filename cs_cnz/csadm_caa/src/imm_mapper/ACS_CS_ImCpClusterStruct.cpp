/*
 * ACS_CS_ImCpClusterStruct.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: eanform
 */


#include "ACS_CS_ImCpClusterStruct.h"
#include "ACS_CS_ImUtils.h"
#include <typeinfo>
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCpClusterStruct);

ACS_CS_ImCpClusterStruct::ACS_CS_ImCpClusterStruct():
axeCpClusterStructId(""),actionId(UNDEFINED_TYPE),state(UNDEFINED_STATE),
result(NOT_AVAILABLE),reason(""), timeOfLastAction("")
{
   type = CPCLUSTERSTRUCT_T;
}

ACS_CS_ImCpClusterStruct::ACS_CS_ImCpClusterStruct(const ACS_CS_ImCpClusterStruct &other)
{
   *this = other;
}

ACS_CS_ImCpClusterStruct::~ACS_CS_ImCpClusterStruct()
{}


bool ACS_CS_ImCpClusterStruct::isValid(int & errNo, const ACS_CS_ImModel &/*model*/)
{
   errNo = 0;
   return true;
}

bool ACS_CS_ImCpClusterStruct::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++ )
   {
      ACS_APGCC_AttrModification *atrMod = attrMods[i];

      if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
         return false;

      ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

      if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID.c_str()) == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  axeCpClusterStructId = (char *) modAttr->attrValues[0];
    	  else
    		  axeCpClusterStructId = '\0';
      }
      else if (strcmp(modAttr->attrName, "state") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  state = (*(AsyncActionStateType *) modAttr->attrValues[0]);
    	  else
    		  state = UNDEFINED_STATE;
      }
      else if (strcmp(modAttr->attrName, "actionId") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  actionId = (*(AsyncActionType *) modAttr->attrValues[0]);
    	  else
    		  actionId = UNDEFINED_TYPE;
      }
      else if (strcmp(modAttr->attrName, "result") == 0)
      {
    	  //result = (*(ActionResultType *) modAttr->attrValues[0]);
    	  if (modAttr->attrValuesNum > 0)
    		  result = (*(ActionResultType *) modAttr->attrValues[0]);
    	  else
    		  result = NOT_AVAILABLE;
      }
      else if (strcmp(modAttr->attrName, "reason") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  reason = ((char *) modAttr->attrValues[0]);
    	  else
    		  reason = '\0';
      }
      else if (strcmp(modAttr->attrName, "timeOfLastAction") == 0)
      {
    	  if (modAttr->attrValuesNum > 0)
    		  timeOfLastAction = ((char *) modAttr->attrValues[0]);
    	  else
    		  timeOfLastAction = '\0';
      }
      else              // Bad attributeName
         return false;
   }

   return true;
}


ACS_CS_ImBase * ACS_CS_ImCpClusterStruct::clone() const
{
   ACS_CS_ImBase *bladeClusterInfo = new ACS_CS_ImCpClusterStruct(*this);
   return bladeClusterInfo;
}


ACS_CS_ImCpClusterStruct & ACS_CS_ImCpClusterStruct::operator=(const ACS_CS_ImCpClusterStruct &rhs)
{
   copyObj(rhs);
   return *this;
}


ACS_CS_ImCpClusterStruct & ACS_CS_ImCpClusterStruct::copyObj(const ACS_CS_ImCpClusterStruct &object)
{
	action = 				object.action;
	type = 					object.type;
	rdn = 					object.rdn;
	axeCpClusterStructId = 	object.axeCpClusterStructId;
	actionId = 				object.actionId;
	state = 				object.state;
	reason = 				object.reason;
	result = 				object.result;
	timeOfLastAction = 		object.timeOfLastAction;

   return *this;
}


bool ACS_CS_ImCpClusterStruct::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
      return false;

   try {
      const ACS_CS_ImCpClusterStruct & other = dynamic_cast<const ACS_CS_ImCpClusterStruct &>(obj);

      if (axeCpClusterStructId != other.axeCpClusterStructId)
    	  return false;
      if (actionId != other.actionId)
    	  return false;
      if (state != other.state)
    	  return false;
      if (reason != other.reason)
    	  return false;
      if (result != other.result)
    	  return false;
      if (timeOfLastAction != other.timeOfLastAction)
    	  return false;
   }
   catch (const std::bad_cast &e)
   {
      return false;
   }


   return true;

}


ACS_CS_ImImmObject * ACS_CS_ImCpClusterStruct::toImmObject(bool onlyModifiedAttrs)
{

   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID.c_str(), axeCpClusterStructId));

   if (strlen(reason.c_str()) != 0)
	   attrList.push_back(ACS_CS_ImUtils::createStringType("reason", reason));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("reason"));

   if (strlen(timeOfLastAction.c_str()) != 0)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType("timeOfLastAction", timeOfLastAction));
   else
   	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("timeOfLastAction"));

   if (state != UNDEF_ACTIONSTATETYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("state", state));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("state"));

   if (actionId != UNDEF_ACTIONTYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("actionId", actionId));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("actionId"));

   if (result != UNDEF_ACTIONRESULTTYPE)
	   attrList.push_back(ACS_CS_ImUtils::createIntType("result", result));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyIntType("result"));

   immObject->setAttributeList(attrList);


   return immObject;

}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpClusterStruct & bladeClusterInfo, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(bladeClusterInfo);
         ar & bladeClusterInfo.axeCpClusterStructId;
         ar & bladeClusterInfo.actionId;
         ar & bladeClusterInfo.state;
         ar & bladeClusterInfo.result;
         ar & bladeClusterInfo.reason;
         ar & bladeClusterInfo.timeOfLastAction;
      }
   }

}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCpClusterStruct & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCpClusterStruct & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCpClusterStruct & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCpClusterStruct & base, const unsigned int /*version*/);
   }
}
