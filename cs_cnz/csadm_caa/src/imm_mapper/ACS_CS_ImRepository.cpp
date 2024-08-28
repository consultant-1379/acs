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
 * @file ACS_CS_ImRepository.cpp
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

#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImObjectCreator.h"
#include "ACS_CS_ImModelSaver.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ImRepository_TRACE);

#include <string>

ACS_CS_ImRepository::ACS_CS_ImRepository()
{
	model = new ACS_CS_ImModel();
	immReader = new ACS_CS_ImIMMReader();
	immReader->loadModel(model);
}

ACS_CS_ImRepository::~ACS_CS_ImRepository()
{
	if(model)
		delete model;

	set<ACS_CS_ImModelSubset *>::iterator it;
	for (it = subsets.begin(); it != subsets.end(); it++)
		if (*it)
			delete *it;

	if (immReader)
		delete immReader;
}



ACS_CS_ImRepository* ACS_CS_ImRepository::instance()
{
    return instance_::instance();
}

void ACS_CS_ImRepository::finalize()
{
    instance_::close();
}


ACS_CS_ImBase * ACS_CS_ImRepository::createObject(ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr)
{

  // printf("ACS_CS_ImRepository::createObject classname %s, parent %s\n",className,parentName);
   ACS_CS_ImBase * object = ACS_CS_ImObjectCreator::createImBaseObject(className, parentName, attr);
   if(NULL == object)
   {
 		ACS_CS_TRACE((ACS_CS_ImRepository_TRACE,
 				"ACS_CS_ImRepository::createObject()",
 				"Failed to createImBaseObject. Classname = %s Parent = %s", className, parentName));
	   return NULL;
   }

   object->action = ACS_CS_ImBase::CREATE;

   //printf("ACS_CS_ImRepository::createObject classname %s, parent %s ADD\n",className,parentName);

   addObject(ccbId, object);
   return object;
}

bool ACS_CS_ImRepository::addObject(ACS_APGCC_CcbId ccbId, ACS_CS_ImBase *object)
{
   ACS_CS_ImModelSubset *existingSubset = findSubset(ccbId);

   bool result = false;

   if(NULL == object)
   {
	   return false;
   }
   if (existingSubset)
   {
	   result = existingSubset->addObject(object);
   }
   else
   {
      ACS_CS_ImModelSubset *newSubset = new ACS_CS_ImModelSubset(ccbId);

      if (newSubset)
      {
    	  result = newSubset->addObject(object);
      }

      subsets.insert(newSubset);

   }

   return result;
}

bool ACS_CS_ImRepository::modifyObject(ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	bool cloned = false;
	ACS_CS_ImBase *clonedObject = NULL;

	//fetch the subset
	ACS_CS_ImModel *subset = findSubset(ccbId);
	if(NULL != subset)
	{//if  the subset already exists, fetch the object with the given RDN
		clonedObject = subset->getObject(objName);
	}

	if(NULL == clonedObject)
	{//if the subset does not exist or the object does not exist
		//clone the object that exists in current model
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

		ACS_CS_ImBase *object = model->getObject(objName);

		if(NULL == object)
			return false;

		clonedObject = object->clone();
		cloned = true;
		clonedObject->action = ACS_CS_ImBase::MODIFY;
	}

	//apply the changes to the object
	if(clonedObject->modify(attrMods))
	{
		addObject(ccbId, clonedObject);
		return true;
	}

	if(cloned)//if the object has been really cloned
		delete clonedObject;
	return false;
}

bool ACS_CS_ImRepository::completeTransaction(ACS_APGCC_CcbId ccbId, int &errNo)
{
	
	errNo = 0;

	ACS_CS_ImModelSubset *existingSubset = findSubset(ccbId);

	if (existingSubset) {
		existingSubset->isComplete = true;
	} else {
		return false;
	}
	
	if(!existingSubset->isValid(errNo)){
		return false;
	}

	mutex.acquire();
	ACS_CS_ImModel *newModel = new ACS_CS_ImModel(*model);// Copy model
	mutex.release();

	bool result;

	newModel->applySubset(existingSubset);// Apply subset, check if things should be deleted or just applied
	
	newModel->createAssociations();
	
	result = newModel->isValid(errNo);// Validate the copy
	
	if (result) {
		replaceModel(newModel);
	} else {
		delete newModel;
	}

//	subsets.erase(existingSubset);// Remove the subset
//	delete existingSubset;
	
	return result;
}


bool ACS_CS_ImRepository::disposeTransaction(ACS_APGCC_CcbId ccbId)
{
	ACS_CS_ImModelSubset *existingSubset = findSubset(ccbId);

	if (!existingSubset)
		return false;

	subsets.erase(existingSubset);
	delete existingSubset;
	return true;
}

bool ACS_CS_ImRepository::applyTransaction(ACS_APGCC_CcbId /*ccbId*/)
{
	return true;
}


bool ACS_CS_ImRepository::applySubset(const ACS_CS_ImModel *subset)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
	return this->model->applySubset(subset);
}

bool ACS_CS_ImRepository::deleteObject(ACS_APGCC_CcbId ccbId, const char *objName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	const ACS_CS_ImBase *object = model->getObject(objName);

	if(object != NULL)
	{
		ACS_CS_ImBase *clonedObject = object->clone();

		clonedObject->action = ACS_CS_ImBase::DELETE;
		return addObject(ccbId, clonedObject);
	}
	else
	{
		return false;
	}
}

const ACS_CS_ImModel * ACS_CS_ImRepository::getModel() const
{
	return model;
}

void ACS_CS_ImRepository::setModel(ACS_CS_ImModel * model)
{
	this->model = model;
}

const ACS_CS_ImModel * ACS_CS_ImRepository::getSubset(ACS_APGCC_CcbId ccbId) const
{
   return findSubset(ccbId);
}


ACS_CS_ImModelSubset * ACS_CS_ImRepository::findSubset(ACS_APGCC_CcbId ccbId) const
{
   ACS_CS_ImModelSubset *existingSubset = NULL;

   set<ACS_CS_ImModelSubset *>::iterator it;
   for (it = subsets.begin(); it != subsets.end(); it++)
   {
      ACS_APGCC_CcbId id = (*it)->getCcbId();

      if (id == ccbId) {
         existingSubset = *it;
         break;
      }
   }

   return existingSubset;

}

const ACS_CS_ImBase * ACS_CS_ImRepository::getParentOf(const std::string &rdn) const
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
	return model->getParentOf(rdn);
}

bool ACS_CS_ImRepository::isValid(int &errNo)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);
	return model->isValid(errNo);
}

ACS_CC_ReturnType ACS_CS_ImRepository::save()
{
	ACS_CS_ImModel *modelCopy = getModelCopy();

	ACS_CS_ImModelSaver saver(modelCopy);
	ACS_CC_ReturnType result = saver.save();
	delete modelCopy;
	return result;
}

ACS_CC_ReturnType ACS_CS_ImRepository::save(string transactionName)
{
	ACS_CS_ImModel *modelCopy = getModelCopy();

	ACS_CS_ImModelSaver *saver = new ACS_CS_ImModelSaver(modelCopy);
	ACS_CC_ReturnType result = saver->save(transactionName);
	delete saver;
	delete modelCopy;
	return result;
}

bool ACS_CS_ImRepository::addObject(ACS_CS_ImBase *object)
{
   return model->addObject(object);
}

bool ACS_CS_ImRepository::storedObject(const char * classname)
{

   if(strcmp(classname, "UpgradeLoadModule") == 0 || strcmp(classname, "ReportProgress") == 0 ||
		   strcmp(classname, "OtherBladeHardwareInfo") == 0 || strcmp(classname, "PowerFanModule") == 0 ||
		   strcmp(classname, "LoadModulesHandler") == 0 || strcmp(classname, "LoadModulesCategory") == 0 ||
		   strcmp(classname, "LoadModulesInfo") == 0){
	   return false;
   }

   return true;
}

void ACS_CS_ImRepository::replaceModel(ACS_CS_ImModel *newModel)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	ACS_CS_ImModel *tempModel = model;
	model = newModel; // Replace model with the new one
	delete tempModel;
}

ACS_CS_ImModel * ACS_CS_ImRepository::getModelCopy()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	return new ACS_CS_ImModel(*getModel());
}

bool ACS_CS_ImRepository::replaceObject(ACS_CS_ImBase *object)
{
	if(0 == object)
		return false;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex);

	ACS_CS_ImBase *oldObject = model->getObject(object->rdn.c_str());

	if (oldObject == NULL)
		return false;

	model->deleteObject(oldObject);

	return model->addObject(object);
}

void ACS_CS_ImRepository::reloadModel()
{
	ACS_CS_ImModel *tempModel = new ACS_CS_ImModel();

	immReader->loadModel(tempModel); //Reload model

	replaceModel(tempModel); // Replace with new one

}
