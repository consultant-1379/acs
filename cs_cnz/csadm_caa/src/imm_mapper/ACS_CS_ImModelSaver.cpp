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
 * @file ACS_CS_ImModelSaver.cpp
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
 * 2017-10-24  ZPRAPXX  Included re-try mechanisms 
 * 2011-08-29  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/

#include "ACS_CS_ImModelSaver.h"

#include "ACS_APGCC_OiHandler.h"
#include "ACS_CS_ImValuesDefinitionType.h"
#include "ACS_CS_ImImmObject.h"
#include "ACS_CS_ImUtils.h"
#include <set>
#include <iostream>
#include <iomanip>

#include "ACS_CS_Trace.h"

#define NUM_LEVELS 7

ACS_CS_Trace_TDEF(ACS_CS_ImModelSaver_TRACE);
ACS_CS_ImModelSaver::ACS_CS_ImModelSaver(ACS_CS_ImModel *model)
{
   this->model = model;
}

ACS_CS_ImModelSaver::~ACS_CS_ImModelSaver() {

}

ACS_CC_ReturnType ACS_CS_ImModelSaver::save(int implementerCheck) {

   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
			"ACS_CS_ImModelSaver::save()\nEntered..."));

   ACS_CC_ReturnType result;
   OmHandler omHandler;

   result = omHandler.Init(implementerCheck);
   if (result != ACS_CC_SUCCESS)
      return ACS_CC_FAILURE;

   // Make sure to call the objects in the correct order
   const int numLevels = NUM_LEVELS;
   std::set<const ACS_CS_ImBase *> objects[numLevels];

   fillObjects(numLevels, objects);

   for (int i = 0; i < numLevels; i++)
   {
      std::set<const ACS_CS_ImBase *>::const_iterator it;
      for (it = objects[i].begin(); it != objects[i].end(); it++)
      {
         ACS_CS_ImBase *object = const_cast<ACS_CS_ImBase *> (*it);

         if (object == 0)
            continue;

         if (object->action == ACS_CS_ImBase::UNDEFINED)
            continue;

         if (object->action == ACS_CS_ImBase::CREATE)
         {

            result = saveObject(object, &omHandler);

            if (result != ACS_CC_SUCCESS)
            {
            	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			"ACS_CS_ImModelSaver::save()\nFailed in saveObject()\nError code: %d (%s)",
            			omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

               // If the object already exists, try modifying it instead.
               if (imm_error_code::ERR_IMM_ALREADY_EXIST == omHandler.getInternalLastError())
               { 

                  result = modifyObject(object, &omHandler);
                  
                  if (result != ACS_CC_SUCCESS)
                  {
                     ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
                           "ACS_CS_ImModelSaver::save()\nFailed in modifyObject(1) \nError code:%d (%s)",
                           omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

                     omHandler.Finalize();
                     return ACS_CC_FAILURE;
                  }
                  else
                  {
                     // We managed to modify this object
                     object->action = ACS_CS_ImBase::UNDEFINED;
                     continue;
                  }
               }
               omHandler.Finalize();
               return ACS_CC_FAILURE;
            }
            object->action = ACS_CS_ImBase::UNDEFINED;
         }

         else if (object->action == ACS_CS_ImBase::DELETE)
         {  
            result = deleteObject(object, &omHandler);

            if (result == ACS_CC_SUCCESS || imm_error_code::ERR_IMM_NOT_EXIST == omHandler.getInternalLastError())
            {
               // Make sure we remove all the children aswell from the model
               //model->deleteSubTree(object);
            	removeChildren(object, i+1, objects);
            }
            else
            {
            	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			"ACS_CS_ImModelSaver::save()\nFailed in deleteObject()\nError code: %d (%s)",
            			omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

               omHandler.Finalize();
               return ACS_CC_FAILURE;
            }
         }

         else if (object->action == ACS_CS_ImBase::MODIFY) {
            result = modifyObject(object, &omHandler);
            if (result != ACS_CC_SUCCESS) {
            	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			"ACS_CS_ImModelSaver::save()\nFailed in modifyObject(2) \nError code: %d (%s)",
            			omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

               omHandler.Finalize();
               return ACS_CC_FAILURE;
            }

            object->action = ACS_CS_ImBase::UNDEFINED;
         }
      }
   }

   return omHandler.Finalize();

}

ACS_CC_ReturnType ACS_CS_ImModelSaver::save(string transactionName, int implementerCheck) {

   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,"ACS_CS_ImModelSaver::save(transactionName)\nStarting saving for transaction %s",transactionName.c_str()));
   ACS_CC_ReturnType result;
   OmHandler omHandler;

   result = omHandler.Init(implementerCheck);
   if (result != ACS_CC_SUCCESS)
      return ACS_CC_FAILURE;

   bool allObjectsSaved = true;
   bool allObjectsDeleted = true;
   bool attributesModified = true;

   // Make sure to call the objects in the correct order
   const int numLevels = NUM_LEVELS;
   std::set<const ACS_CS_ImBase *> objects[numLevels];

   fillObjects(numLevels, objects);

   for (int i = 0; i < numLevels; i++) {

      std::set<const ACS_CS_ImBase *>::const_iterator it;
      for (it = objects[i].begin(); it != objects[i].end(); it++) {

         ACS_CS_ImBase *object = const_cast<ACS_CS_ImBase *> (*it);

         if (object == 0)
            continue;

         if (object->action == ACS_CS_ImBase::UNDEFINED)
            continue;

         if (object->action == ACS_CS_ImBase::CREATE) {

//        	 std::cout << "Creating Object " << object->rdn << std::endl;

            result = saveObject(object, &omHandler, transactionName);

            if (result != ACS_CC_SUCCESS) {
            	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			"ACS_CS_ImModelSaver::save(transactionName)\nFailed in saveObject()\nError code: %d (%s)",
            			omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

               allObjectsSaved = false;

               // If the object already exists, try modifying it instead.
               if (imm_error_code::ERR_IMM_ALREADY_EXIST == omHandler.getInternalLastError()) {
                  result = modifyObject(object, &omHandler, transactionName);

                  if (result != ACS_CC_SUCCESS) {
                	  ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
                			  "ACS_CS_ImModelSaver::save(transactionName)\nFailed in modifyObject() \nError code: %d (%s)",
                			  omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

                     attributesModified = false;
                     break;
                  }
                  else
                     allObjectsSaved = true; // This object was not saved, but flag needs resetting anyways.
               }
            }
         }

         else if (object->action == ACS_CS_ImBase::DELETE) {

//        	 std::cout << "Deleting Object " << object->rdn << std::endl;

            if (model->exists(object))
            {
               result = deleteObject(object, &omHandler, transactionName);

               if (result == ACS_CC_SUCCESS || imm_error_code::ERR_IMM_NOT_EXIST == omHandler.getInternalLastError()) {
                  //model->deleteSubTree(object);
            	  removeChildren(object, i+1, objects);
               }
               else
               {
            	   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			   "ACS_CS_ImModelSaver::save(transactionName)\nFailed in deleteObject()\nError code: %d (%s)",
            			   omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

                  allObjectsDeleted = false;
                  break;
               }
            }
         }
         // TODO Figure out an better way of determining how to modify attributes
         else if (object->action == ACS_CS_ImBase::MODIFY) {

//        	 std::cout << "Modifying Object " << object->rdn << std::endl;
            result = modifyObject(object, &omHandler, transactionName);
            if (result != ACS_CC_SUCCESS) {
            	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
            			"ACS_CS_ImModelSaver::save(transactionName)\nFailed in modifyObject(2)\nError code: %d (%s)",
            			omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));

               attributesModified = false;
               //return ACS_CC_FAILURE;
               break;
            }
         }
      }
   }

   ACS_CC_ReturnType saved = ACS_CC_FAILURE;

   if (allObjectsSaved && allObjectsDeleted && attributesModified) {
	   result = omHandler.applyRequest(transactionName);

	   if (result == ACS_CC_SUCCESS) {
		   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
				   "ACS_CS_ImModelSaver::save(transactionName)\nSuccessfully called applyRequest for transaction %s",
				   transactionName.c_str()));
		   saved = ACS_CC_SUCCESS;
	   } else {
		   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
				   "ACS_CS_ImModelSaver::save(transactionName)\nFailed in applyRequest for transaction %s\nError code: %d (%s)",
				   transactionName.c_str(), omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));
	   }

   } else {
	   result = omHandler.resetRequest(transactionName);

	   if (result == ACS_CC_SUCCESS) {
		   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
				   "ACS_CS_ImModelSaver::save(transactionName)\nSuccessfully called resetRequest for transaction %s",
				   transactionName.c_str()));
	   } else {

		   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
				   "ACS_CS_ImModelSaver::save(transactionName)\nFailed in applyRequest for transaction %s\nError code: %d (%s)\nWARNING! FAILED TO RESET TRANSACTION, DATABASE MIGHT BE INCONSISTENT!!!!",
				   transactionName.c_str(), omHandler.getInternalLastError(), omHandler.getInternalLastErrorText()));
	   }
   }

   result = omHandler.Finalize();

   if (result != ACS_CC_SUCCESS) {
	   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
			   "ACS_CS_ImModelSaver::save(transactionName)\nFailed to finalize OmHandler for transaction %s",
			   transactionName.c_str()));
   }

   return saved;
}

ACS_CC_ReturnType ACS_CS_ImModelSaver::saveObject(ACS_CS_ImBase *object, OmHandler *omHandler) {

   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
      "ACS_CS_ImModelSaver::saveObject(ACS_CS_ImBase*, OmHandler*)\nrdn=%s",(object ? object->rdn.c_str() : "***object==NULL***")));

   ACS_CS_ImImmObject *immObject = object->toImmObject();
   std::vector<ACS_CS_ImValuesDefinitionType> values = immObject->getAttributes();
   std::vector<ACS_CC_ValuesDefinitionType> valuesAsBaseObject;

   std::vector<ACS_CS_ImValuesDefinitionType>::iterator it2;
   for (it2 = values.begin(); it2 != values.end(); it2++) {
      try {
         ACS_CC_ValuesDefinitionType baseType = dynamic_cast<ACS_CC_ValuesDefinitionType &> (*it2);
         valuesAsBaseObject.push_back(baseType);
         ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE, "ACS_CS_ImModelSaver::saveObject(2)"));
//         ACS_CS_ImUtils::printValuesDefinitionType(baseType);
      }
      catch (std::bad_cast e) {
    	  ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
    			  "ACS_CS_ImModelSaver::saveObject(2)\nException caught - bad cast"));
      }
   }

   const char *className = immObject->getClassName();
   const char *parentName = immObject->getParentName();
   if (*parentName == '\0')
      parentName = 0;

   ACS_CC_ReturnType result;


   for (int i=0; i<9; i++) 
   {
       result = omHandler->createObject(className, parentName, valuesAsBaseObject);

      if(result != ACS_CC_SUCCESS)
      {   
	  int error = omHandler->getInternalLastError();
	  if ( imm_error_code::ERR_IMM_TRY_AGAIN == error || imm_error_code::ERR_IMM_NOT_EXIST == error )
	  {
            ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_ERROR,"ACS_CS_ImModelSaver::FAILED in creating \"%s\" object with error %s, Re-trying... ",object->rdn.c_str(),omHandler->getInternalLastErrorText()));
            ACE_OS::sleep(1);
	  }
          else
          {
            ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_ERROR,"ACS_CS_ImModelSaver::FAILED in creating \"%s\" object with error %s, ",object->rdn.c_str(),omHandler->getInternalLastErrorText()));
            break;
          }
      }
      else
      { 
            ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_INFO,"ACS_CS_ImModelSaver::\"%s\" created successfully ",object->rdn.c_str()));
            break;
      }

   }
  
  return result;

}

ACS_CC_ReturnType ACS_CS_ImModelSaver::deleteObject(ACS_CS_ImBase *object, OmHandler *omHandler) {
   
   return omHandler->deleteObject(object->rdn.c_str(), ACS_APGCC_SUBTREE);
}

ACS_CC_ReturnType ACS_CS_ImModelSaver::modifyObject(ACS_CS_ImBase *object, OmHandler *omHandler)
{
	bool changedAttributes = false;

	ACS_CS_ImImmObject *immObject = object->toImmObject(true);
	std::vector<ACS_CS_ImValuesDefinitionType> attributes = immObject->getAttributes();

	std::vector<ACS_CS_ImValuesDefinitionType>::iterator it;

	//No attributes to modify, just check if the object exists.
	if (attributes.size() == 0)
	{
		changedAttributes = checkExistingObject(object, omHandler);

		ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
				"ACS_CS_ImModelSaver::modifyObject():\n"
				"Object DN: %s, No attribute to modify. Checking if object exists, result = %d,Re-trying..",
				object->rdn.c_str(), changedAttributes));
	}

	for (it = attributes.begin(); it != attributes.end(); it++) {
		ACS_CS_ImValuesDefinitionType vdt = *it;

		ACS_CC_ImmParameter parameter;
		parameter.attrName = vdt.getAttrName();
		parameter.attrType = vdt.getAttrType();
		parameter.attrValues = vdt.getAttrValues();
		parameter.attrValuesNum = vdt.getAttrValuesNum();

		
		for (int i=0; i<5; i++)
		{
			ACS_CC_ReturnType result = omHandler->modifyAttribute(object->rdn.c_str(), &parameter);
			if (result != ACS_CC_SUCCESS)
			{
                      		  ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
                                        "ACS_CS_ImModelSaver::modifyObject():\n"
                                        "Object DN: %s, Attribute Name: %s"
                                        " \nFailed in applyRequest. \nError code: %d (%s)",
                                        object->rdn.c_str(), parameter.attrName,
                                        omHandler->getInternalLastError(), omHandler->getInternalLastErrorText()));
	
				  ACE_OS::sleep(1);
		  
			}		
			else
			{
                   		ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_INFO,"ACS_CS_ImModelSaver::\"%s\" modified successfully ",object->rdn.c_str()));
				changedAttributes = true;
				break;
			}
		}
//		if (result != ACS_CC_SUCCESS) { 
//			ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
//					"ACS_CS_ImModelSaver::modifyObject():\n"
//					"Object DN: %s, Attribute Name: %s"
//					" \nFailed in applyRequest. \nError code: %d (%s)",
//					object->rdn.c_str(), parameter.attrName,
//					omHandler->getInternalLastError(), omHandler->getInternalLastErrorText()));
//		} else
//			changedAttributes = true;
//
	}

	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,"ACS_CS_ImModelSaver::modifyObject(ACS_CS_ImBase*, OmHandler*)\n"));

	delete immObject;

	if (!changedAttributes)
		return ACS_CC_FAILURE;

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_CS_ImModelSaver::saveObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName) {

   ACS_CS_ImImmObject *immObject = object->toImmObject();
   std::vector<ACS_CS_ImValuesDefinitionType> values = immObject->getAttributes();
   std::vector<ACS_CC_ValuesDefinitionType> valuesAsBaseObject;

   std::vector<ACS_CS_ImValuesDefinitionType>::iterator it2;
   for (it2 = values.begin(); it2 != values.end(); it2++) {
      try {
         ACS_CC_ValuesDefinitionType baseType = dynamic_cast<ACS_CC_ValuesDefinitionType &> (*it2);
         valuesAsBaseObject.push_back(baseType);
      }
      catch (std::bad_cast e) {
    	  ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
    			  "ACS_CS_ImModelSaver::saveObject(3)\nException caught - bad cast"));
      }
   }

   const char *className = immObject->getClassName();
   const char *parentName = immObject->getParentName();
   if (*parentName == '\0')
      parentName = 0;
   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
                          "ACS_CS_ImModelSaver::started %s object creation",transactionName.c_str()));


   ACS_CC_ReturnType result;
   
   for (int i=0; i<9; i++)
   {     
     result = omHandler->createObject(className, parentName, valuesAsBaseObject, transactionName); 

     if(result != ACS_CC_SUCCESS)
     {
	int error = omHandler->getInternalLastError();
        if ( imm_error_code::ERR_IMM_TRY_AGAIN == error || imm_error_code::ERR_IMM_NOT_EXIST == error )
        {
           ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_INFO,"ACS_CS_ImModelSaver::Transaction: %s,FAILED in creating \"%s\" object with error %s, Re-trying..  ",transactionName.c_str(),object->rdn.c_str(),omHandler->getInternalLastErrorText()));
	   ACE_OS::sleep(1);
	}
        else
        {  
           ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_INFO,"ACS_CS_ImModelSaver::Transaction: %s,FAILED in creating \"%s\" object with error %s ",transactionName.c_str(),object->rdn.c_str(),omHandler->getInternalLastErrorText()));
           break;

        }
     }
     else
     {
           ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_ERROR,"ACS_CS_ImModelSaver::Transaction: %s, \"%s\" created successfully ",transactionName.c_str(),object->rdn.c_str()));
           break;
     }
   }

   return result;
}

ACS_CC_ReturnType ACS_CS_ImModelSaver::deleteObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName) {

	return omHandler->deleteObject(object->rdn.c_str(), transactionName, ACS_APGCC_SUBTREE);
}


ACS_CC_ReturnType ACS_CS_ImModelSaver::modifyObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName) {

   bool changedAttributes = false;

   ACS_CS_ImImmObject *immObject = object->toImmObject(true);
   std::vector<ACS_CS_ImValuesDefinitionType> attributes = immObject->getAttributes();

   //No attributes to modify, just check if the object exists.
   if (attributes.size() == 0)
   {
	   changedAttributes = checkExistingObject(object, omHandler);

	   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
			   "ACS_CS_ImModelSaver::modifyObject(transactionName):\n"
			   "Object DN: %s, No attribute to modify. Checking if object exists, result = %d",
			   object->rdn.c_str(), changedAttributes));
   }

   std::vector<ACS_CS_ImValuesDefinitionType>::iterator it;
   for (it = attributes.begin(); it != attributes.end(); it++) {
      ACS_CS_ImValuesDefinitionType vdt = *it;

      ACS_CC_ImmParameter parameter;
      parameter.attrName = vdt.getAttrName();
      parameter.attrType = vdt.getAttrType();
      parameter.attrValues = vdt.getAttrValues();
      parameter.attrValuesNum = vdt.getAttrValuesNum();

   for(int i=0; i<5; i++)
   {
      ACS_CC_ReturnType result = omHandler->modifyAttribute(object->rdn.c_str(), &parameter, transactionName);
      if (result != ACS_CC_SUCCESS)
      {
	ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
                          "ACS_CS_ImModelSaver::modifyObject(transactionName)\n"
                          "Object DN: %s, Attribute Name: %s"
                          "Failed in modifyAttribute for transaction %s\nError code: %d (%s),Re-trying..",
                                  object->rdn.c_str(), parameter.attrName,
                          transactionName.c_str(), omHandler->getInternalLastError(), omHandler->getInternalLastErrorText()));
 
        ACE_OS::sleep(1);

      }
      else
      {	
	ACS_CS_FTRACE((ACS_CS_ImModelSaver_TRACE,LOG_LEVEL_INFO,"ACS_CS_ImModelSaver::Transaction %s, \"%s\" modified successfully ",transactionName.c_str(),object->rdn.c_str()));
	changedAttributes = true;
	break;
      }
   }
//      if (result != ACS_CC_SUCCESS) {
//   	  ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,
//    			  "ACS_CS_ImModelSaver::modifyObject(transactionName)\n"
//    			  "Object DN: %s, Attribute Name: %s"
//    			  "Failed in modifyAttribute for transaction %s\nError code: %d (%s)",
//				  object->rdn.c_str(), parameter.attrName,
//    			  transactionName.c_str(), omHandler->getInternalLastError(), omHandler->getInternalLastErrorText()));
//      } else
//         changedAttributes = true;
     }

   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,"ACS_CS_ImModelSaver::modifyObject(ACS_CS_ImBase*, OmHandler*)\n"));
   ACS_CS_TRACE((ACS_CS_ImModelSaver_TRACE,"ACS_CS_ImModelSaver::modified object %s",transactionName.c_str() ));
   delete immObject;

   if (!changedAttributes)
      return ACS_CC_FAILURE;

   return ACS_CC_SUCCESS;
}


bool ACS_CS_ImModelSaver::fillObjects(int numLevels, std::set<const ACS_CS_ImBase *> *objects)
{
   if (numLevels != NUM_LEVELS)
      return false;

   model->getObjects(objects[0], EQUIPMENT_T);
   model->getObjects(objects[0], FUNCTIONDISTRIBUTION_T);
   model->getObjects(objects[0], CRM_EQUIPMENT_T);
   model->getObjects(objects[0], TRM_TRANSPORT_T);
   model->getObjects(objects[1], HARDWAREMGMT_T);
   model->getObjects(objects[1], ADVANCEDCONFIGURATION_T);
   model->getObjects(objects[1], LOGICALMGMT_T);
   model->getObjects(objects[1], APSERVICECATEGORY_T);
   model->getObjects(objects[1], VLANCATEGORY_T);
   model->getObjects(objects[1], APGNODE_T);
   model->getObjects(objects[1], CRMGMT_T);
   model->getObjects(objects[1], CRM_COMPUTERESOURCE_T);
   model->getObjects(objects[1], TRM_HOST_T);
   model->getObjects(objects[1], TRM_EXTERNALNETWORK_T);
   model->getObjects(objects[1], TRM_INTERNALNETWORK_T);
   model->getObjects(objects[2], CPCLUSTER_T);
   model->getObjects(objects[2], SHELF_T);
   model->getObjects(objects[2], AP_T);
   model->getObjects(objects[2], DUALSIDEDCP_T);
   model->getObjects(objects[2], APSERVICE_T);
   model->getObjects(objects[2], VLAN_T);
   model->getObjects(objects[2], APFUNCTION_T);
   model->getObjects(objects[2], COMPUTERESOURCE_T);
   model->getObjects(objects[2], TRM_INTERFACE_T);
//   model->getObjects(objects[2], IPLBCLUSTER_T); //TODO: not in the scope for AXEX17A
   model->getObjects(objects[3], CLUSTERCP_T);
   model->getObjects(objects[3], CPBLADE_T);
   model->getObjects(objects[3], OTHERBLADE_T);
   model->getObjects(objects[3], APBLADE_T);
   model->getObjects(objects[3], CPCLUSTERSTRUCT_T);
   model->getObjects(objects[3], OMPROFILEMANAGER_T);
   model->getObjects(objects[4], CCFILEMANAGER_T);
   model->getObjects(objects[4], OMPROFILE_T);
   model->getObjects(objects[4], OMPROFILEMANAGERSTRUCT_T);
   model->getObjects(objects[4], CPPRODUCTINFO_T);
   model->getObjects(objects[5], CANDIDATECCFILE_T);
   model->getObjects(objects[5], CCFILE_T);
   model->getObjects(objects[6], CANDIDATECCFILESTRUCT_T);


   return true;

}

bool ACS_CS_ImModelSaver::removeChildren(const ACS_CS_ImBase * parent, int level, std::set<const ACS_CS_ImBase *> *objects)
{
	std::set<const ACS_CS_ImBase *>::const_iterator it;
	std::set<const ACS_CS_ImBase *> children;

	if (level >= NUM_LEVELS)
		return true;

	for (it = objects[level].begin(); it != objects[level].end(); it++)
	{
		const ACS_CS_ImBase *object = dynamic_cast<const ACS_CS_ImBase *> (*it);

		if (object)
		{
			if (parent->rdn.compare(ACS_CS_ImUtils::getParentName(object->rdn)) == 0)
			{
//				std::cout << "Found Children to remove " << object->rdn << std::endl;
				removeChildren(object, level + 1, objects);
				children.insert(object);
			}
		}
	}

	for (it = children.begin(); it != children.end(); it++)
	{
		const ACS_CS_ImBase *obj = dynamic_cast<const ACS_CS_ImBase *>(*it);
		objects[level].erase(obj);
	}

	return true;
}

bool ACS_CS_ImModelSaver::checkExistingObject(ACS_CS_ImBase *object, OmHandler *omHandler)
{
	ACS_CC_ReturnType result;
	ACS_APGCC_ImmObject emptyObject;

	emptyObject.objName = object->rdn;
	result = omHandler->getObject(&emptyObject);

	return (result == ACS_CC_SUCCESS);
}
