/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2010
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under whicFh the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file ACS_CS_ImModel.cpp
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
 * 2011-08-31  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/
#include "ACS_CS_ImModel.h"
#include <algorithm>
#include <map>

#include "acs_apgcc_paramhandling.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_ImModelSaver.h"

#include <iostream>

using std::set;
using std::map;

using namespace std;

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_ImModel_TRACE);



ACS_CS_ImModel::ACS_CS_ImModel()
   : lock(0)
{
   lock = new ACS_CS_ReaderWriterLock();
}

ACS_CS_ImModel::ACS_CS_ImModel(const ACS_CS_ImModel &other)
   : lock(0)
{
   lock = new ACS_CS_ReaderWriterLock();
	*this = other;

}

ACS_CS_ImModel::~ACS_CS_ImModel()
{
   if (lock)
      delete lock;

	release();
}

bool ACS_CS_ImModel::isValid(int &errNo)
{
   set<ACS_CS_ImBase*>::iterator it;

   bool result = true;

   lock->start_reading();

   for (it = m_objects.begin(); it != m_objects.end(); it++) {

      ACS_CS_ImBase *object = *it;

      if (object) {
    	  //cout << "Validating object " << object->rdn << "type " << object->type << endl;
    	  if (!object->isValid(errNo, *this)) {
    		  ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
    				  "ACS_CS_ImModel::isValid()\n"
    				  "Failed to validate %s", object->rdn.c_str()));
    		  result = false;
    		  break;
    	  }
      } else {
    	  cout << "Internal error during validation!" << endl;
    	  result = false;
    	  break;
      }
   }

   lock->stop_reading();


   return result;
}

/**
 * Takes the ownership over the given object.
 */
bool ACS_CS_ImModel::addObject(ACS_CS_ImBase *object)
{
   //ACS_CS_TRACE((ACS_CS_ImModel_TRACE, "ACS_CS_ImModel::addObject(ACS_CS_ImBase *object)\nrdn=%s", (object ? object->rdn.c_str() : "***object==NULL***")));

   pair<set<ACS_CS_ImBase*>::iterator, bool> result;
   //Start of TR IA77510
   if(object == NULL)
   	return false;
   //End of TR IA77510
   result = m_objects.insert(object);
   return result.second;
}




bool ACS_CS_ImModel::applySubset(const ACS_CS_ImModel *subset)
{
   pair<set<ACS_CS_ImBase*>::iterator, bool> result;
   bool ret = true;

   lock->start_writing();

   set<ACS_CS_ImBase *>::iterator it;
   for (it = subset->m_objects.begin(); it != subset->m_objects.end(); it++) {

      ACS_CS_ImBase *newObject = *it;
      ACS_CS_ImBase *retObj = 0;

      std::set<ACS_CS_ImBase *>::iterator readIt;
      for (readIt = m_objects.begin(); readIt != m_objects.end(); readIt++) {
         ACS_CS_ImBase *object = *readIt;

         if (object && object->rdn.compare(newObject->rdn) == 0) {
            retObj = object;
            break;
         }
      }

      if(retObj != 0)
      {
    	  //delete
         delete retObj;
         m_objects.erase(retObj);

         if(newObject->action != ACS_CS_ImBase::DELETE)
         {
        	 //delete -> add == modify
        	retObj = newObject->clone();
         	retObj->action = ACS_CS_ImBase::UNDEFINED;
            result = m_objects.insert(retObj);
            if(result.second == false)
            	ret = false;
         }

      } else {
    	  //add
    	 retObj = newObject->clone();
    	 retObj->action = ACS_CS_ImBase::UNDEFINED;
         result = m_objects.insert(retObj);
         if(result.second == false)
        	 ret = false;
      }
   }

   lock->stop_writing();

   return ret;
}



ACS_CS_ImBase * ACS_CS_ImModel::getObject(const char *objName) const
{
   if (m_objects.empty()) {
      return NULL;
   }

   ACS_CS_ImBase *retObj = 0;

   std::set<ACS_CS_ImBase *>::iterator it;

   lock->start_reading();

   for (it = m_objects.begin(); it != m_objects.end(); ++it) {

      ACS_CS_ImBase *object = *it;

      if (object == 0)
      {
         ACS_CS_TRACE((ACS_CS_ImModel_TRACE, "ACS_CS_ImModel::getObject(const char *objName) const\n"
               "object is null"));
         break;
      }

      if (strcmp(object->rdn.c_str(),objName) == 0) {

         if (object->rdn.empty())
            ACS_CS_TRACE((ACS_CS_ImModel_TRACE, "ACS_CS_ImModel::getObject(const char *objName) const\n"
               "object->rdn.empty() == true"));

         retObj = object;
         break;
      }
   }

   lock->stop_reading();

   return retObj;
}


void ACS_CS_ImModel::getObjects(std::set<const ACS_CS_ImBase *> &objects, ClassType type) const {

   std::set<ACS_CS_ImBase*>::const_iterator it;

   lock->start_reading();

   for (it = m_objects.begin(); it != m_objects.end(); it++) {
      ACS_CS_ImBase * object = *it;

      if (!object)
         continue;

      if (type == BLADE_T)
      {
         if (object->type == APBLADE_T || object->type == CPBLADE_T || object->type == OTHERBLADE_T)
            objects.insert(object);
      }
      else if (object->type == type || type == UNDEFINED_T)
         objects.insert(object);
   }

   lock->stop_reading();

}


ACS_CS_ImModel &ACS_CS_ImModel::operator=(const ACS_CS_ImModel &rhs) {
   copyObj(rhs);
   return *this;
}


ACS_CS_ImBase* ACS_CS_ImModel::getParentOf(const std::string& rdn) const
{
	ACS_CS_ImBase* parent = 0;

	size_t index = rdn.find_first_of(",");
	if(std::string::npos != index)
	{
		std::string parentRdn( rdn.substr(index + 1) );
		parent = getObject(parentRdn.c_str());
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_ImModel_TRACE, LOG_LEVEL_ERROR, "[%s:%d] Failed to get parent of:<%s>",
				__FUNCTION__, __LINE__, rdn.c_str()));
	}

	return parent;
}



void ACS_CS_ImModel::getChildren(const ACS_CS_ImBase *parent, std::set<const ACS_CS_ImBase *> &children, ClassType type) const
{
   std::set<ACS_CS_ImBase*>::const_iterator it;

   lock->start_reading();

   for (it = m_objects.begin(); it != m_objects.end(); it++)
   {
      ACS_CS_ImBase * object = *it;

      if ((object) && (ACS_CS_ImUtils::getParentName(object).compare(parent->rdn) == 0))
      {
    	  if (object->type == type || type == UNDEFINED_T)
    	  {
    		  children.insert(object);
    	  }
      }

   }

   lock->stop_reading();
}

void ACS_CS_ImModel::getChildren(const string parentRdn, std::set<const ACS_CS_ImBase *> &children, ClassType type) const
{
   std::set<ACS_CS_ImBase*>::const_iterator it;

   lock->start_reading();

   for (it = m_objects.begin(); it != m_objects.end(); it++)
   {
      ACS_CS_ImBase * object = *it;

      if ((object) && (ACS_CS_ImUtils::getParentName(object).compare(parentRdn) == 0))
      {
    	  if (object->type == type || type == UNDEFINED_T)
    	  {
    		  children.insert(object);
    	  }
      }
   }
   lock->stop_reading();
}

void ACS_CS_ImModel::getChildrenRecursive(const ACS_CS_ImBase *parent, std::set<const ACS_CS_ImBase *> &children)
{
   std::set<ACS_CS_ImBase*>::const_iterator it;

   for (it = m_objects.begin(); it != m_objects.end(); it++)
   {
      ACS_CS_ImBase * object = *it;

      if ((object) && (ACS_CS_ImUtils::getParentName(object).compare(parent->rdn) == 0))
      {
         children.insert(object);
         getChildrenRecursive(object, children);
      }
   }
}



ACS_CS_ImModel & ACS_CS_ImModel::copyObj(const ACS_CS_ImModel &model)
{
   // First empty our own object list, even though it is most likely empty
   this->release();

   lock->start_writing();

   // Copy the source objects to us
   std::set<ACS_CS_ImBase*>::iterator it;
   for (it = model.m_objects.begin(); it != model.m_objects.end(); it++) {
      ACS_CS_ImBase * object = *it;
      this->addObject(object->clone());
   }

   lock->stop_writing();

   return *this;
}


size_t ACS_CS_ImModel::size()
{
   return m_objects.size();
}


void ACS_CS_ImModel::release()
{
	std::set<ACS_CS_ImBase *>::iterator it;

	for (it = m_objects.begin(); it != m_objects.end(); it++)
	{
		ACS_CS_ImBase *base = *it;
		delete base;
	}
	m_objects.clear();
}

void ACS_CS_ImModel::createAssociations()
{
//	std::set<ACS_CS_ImBase *>::iterator it;
//
//	for (it = m_objects.begin(); it != m_objects.end(); it++)
//	{
//		if (!(*it))
//			continue;
//
//		if (dynamic_cast<ACS_CS_ImConfigurationInfo *>(*it))
//		{
//
//		}
//
//		else if (ACS_CS_ImCpCategory *obj = dynamic_cast<ACS_CS_ImCpCategory *>(*it))
//		{
//			createConfigurationInfoCpCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImHardwareConfigurationCategory *obj = dynamic_cast<ACS_CS_ImHardwareConfigurationCategory *>(*it))
//		{
//		   createConfigurationInfoConfigurationCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImApCategory *obj = dynamic_cast<ACS_CS_ImApCategory *>(*it))
//		{
//		   createConfigurationInfoApCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImBladeClusterInfo *obj = dynamic_cast<ACS_CS_ImBladeClusterInfo *>(*it))
//		{
//			createConfigurationInfoBladeClusterInfoAggregation(obj);
//		}
//
//		else if (ACS_CS_ImAdvancedConfiguration *obj = dynamic_cast<ACS_CS_ImAdvancedConfiguration *>(*it))
//		{
//			createConfigurationInfoAdvancedConfigurationAggregation(obj);
//		}
//
//		else if (ACS_CS_ImCp *obj = dynamic_cast<ACS_CS_ImCp *>(*it))
//		{
//			createCpCategoryCpAggregation(obj);
//		}
//
//		else if (ACS_CS_ImCpBlade *obj= dynamic_cast<ACS_CS_ImCpBlade *>(*it))
//		{
//			createShelfCpBladeAggregation(obj);
//			createCpCpBladeAssociation(obj);
//		}
//
//		/*else if (ACS_CS_ImBlade *obj = dynamic_cast<ACS_CS_ImBlade *>(*it))
//		{
//		}*/
//
//
//      else if (ACS_CS_ImAp *obj = dynamic_cast<ACS_CS_ImAp *>(*it))
//      {
//         createApCategoryApAggregation(obj);
//      }
//
//		else if (ACS_CS_ImOtherBlade *obj = dynamic_cast<ACS_CS_ImOtherBlade *>(*it))
//		{
//		   createShelfOtherBladeAggregation(obj);
//		}
//
//		else if (ACS_CS_ImShelf *obj = dynamic_cast<ACS_CS_ImShelf *>(*it))
//		{
//		   createHardwareConfigurationCategoryShelfAggregation(obj);
//		}
//
//		else if (ACS_CS_ImApBlade *obj = dynamic_cast<ACS_CS_ImApBlade *>(*it))
//		{
//		   createShelfApBladeAggregation(obj);
//		   createApApBladeAssociation(obj);
//		}
//
//
//		else if (ACS_CS_ImCpGroupCategory *obj = dynamic_cast<ACS_CS_ImCpGroupCategory *>(*it))
//		{
//		   createBladeClusterInfoCpGroupCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImCpGroup *obj = dynamic_cast<ACS_CS_ImCpGroup *>(*it))
//		{
//		   createCpGroupCategoryCpGroupAggregation(obj);
//		   createCpCpGroupAssociation(obj);
//		}
//
//		else if (ACS_CS_ImApServiceCategory *obj = dynamic_cast<ACS_CS_ImApServiceCategory *>(*it))
//		{
//		   createConfigurationInfoApServiceCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImApService *obj = dynamic_cast<ACS_CS_ImApService *>(*it))
//		{
//		   createApServiceCategoryApServiceAggregation(obj);
//		   createApApServiceAssociation(obj);
//		}
//
//		else if (ACS_CS_ImVlanCategory *obj = dynamic_cast<ACS_CS_ImVlanCategory *>(*it))
//		{
//		   createConfigurationInfoVlanCategoryAggregation(obj);
//		}
//
//		else if (ACS_CS_ImVlan *obj = dynamic_cast<ACS_CS_ImVlan *>(*it))
//		{
//		   createVlanCategoryVlanAggregation(obj);
//
//		}
//	}
}


//TODO: Removed Associatione Methods
/*
void ACS_CS_ImModel::createConfigurationInfoCpCategoryAggregation(ACS_CS_ImCpCategory *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->cpCategory = obj;

}


void ACS_CS_ImModel::createConfigurationInfoConfigurationCategoryAggregation(ACS_CS_ImHardwareConfigurationCategory *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->hardwareConfigurationCategory = obj;

}


void ACS_CS_ImModel::createConfigurationInfoApCategoryAggregation(ACS_CS_ImApCategory *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->apCategory = obj;

}

void ACS_CS_ImModel::createConfigurationInfoBladeClusterInfoAggregation(ACS_CS_ImBladeClusterInfo *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->bladeClusterInfo = obj;

}


void ACS_CS_ImModel::createConfigurationInfoAdvancedConfigurationAggregation(ACS_CS_ImAdvancedConfiguration *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->advancedConfiguration = obj;
}


void ACS_CS_ImModel::createCpCategoryCpAggregation(ACS_CS_ImCp *obj)
{
   ACS_CS_ImCpCategory *parent = dynamic_cast<ACS_CS_ImCpCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->cp.insert(obj);
}


void ACS_CS_ImModel::createShelfCpBladeAggregation(ACS_CS_ImCpBlade *obj)
{
   ACS_CS_ImShelf *parent = dynamic_cast<ACS_CS_ImShelf *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->cpBlades.insert(obj);
}


void ACS_CS_ImModel::createCpCpBladeAssociation(ACS_CS_ImCpBlade *obj)
{
   ACS_CS_ImCp *cp = dynamic_cast<ACS_CS_ImCp *>(getObject(obj->cpRdn.c_str()));

   if (cp)
      obj->cp = cp;
}



void ACS_CS_ImModel::createApCategoryApAggregation(ACS_CS_ImAp *obj)
{
   ACS_CS_ImApCategory *parent = dynamic_cast<ACS_CS_ImApCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->ap.insert(obj);
}


void ACS_CS_ImModel::createShelfOtherBladeAggregation(ACS_CS_ImOtherBlade *obj)
{
   ACS_CS_ImShelf *parent = dynamic_cast<ACS_CS_ImShelf *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->otherBlades.insert(obj);

}


void ACS_CS_ImModel::createHardwareConfigurationCategoryShelfAggregation(ACS_CS_ImShelf *obj)
{
   ACS_CS_ImHardwareConfigurationCategory *parent = dynamic_cast<ACS_CS_ImHardwareConfigurationCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->shelves.insert(obj);
}

void ACS_CS_ImModel::createShelfApBladeAggregation(ACS_CS_ImApBlade *obj)
{
   ACS_CS_ImShelf *parent = dynamic_cast<ACS_CS_ImShelf *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->apBlades.insert(obj);
}

void ACS_CS_ImModel::createApApBladeAssociation(ACS_CS_ImApBlade *obj)
{
   ACS_CS_ImAp *ap = dynamic_cast<ACS_CS_ImAp *>(getObject(obj->apRdn.c_str()));

   if (ap)
      obj->ap = ap;
}


void ACS_CS_ImModel::createBladeClusterInfoCpGroupCategoryAggregation(ACS_CS_ImCpGroupCategory *obj)
{
   ACS_CS_ImBladeClusterInfo *parent = dynamic_cast<ACS_CS_ImBladeClusterInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->cpGroupCategory = obj;

}

void ACS_CS_ImModel::createCpGroupCategoryCpGroupAggregation(ACS_CS_ImCpGroup *obj)
{
   ACS_CS_ImCpGroupCategory *parent = dynamic_cast<ACS_CS_ImCpGroupCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->cpGroups.insert(obj);

}


void ACS_CS_ImModel::createCpCpGroupAssociation(ACS_CS_ImCpGroup *obj)
{
   set<string>::const_iterator it;
   for (it = obj->cpRdns.begin(); it != obj->cpRdns.end(); it++)
   {
      ACS_CS_ImCp *cp = dynamic_cast<ACS_CS_ImCp *>(getObject((*it).c_str()));

      if (cp)
         obj->cps.insert(cp);
   }
}


void ACS_CS_ImModel::createConfigurationInfoApServiceCategoryAggregation(ACS_CS_ImApServiceCategory *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->apServiceCategory = obj;
}


void ACS_CS_ImModel::createApServiceCategoryApServiceAggregation(ACS_CS_ImApService *obj)
{
   ACS_CS_ImApServiceCategory *parent = dynamic_cast<ACS_CS_ImApServiceCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->apServices.insert(obj);
}

void ACS_CS_ImModel::createApApServiceAssociation(ACS_CS_ImApService *obj)
{
	set<string>::const_iterator it;
	for (it = obj->apRdns.begin(); it != obj->apRdns.end(); it++)
	{
		ACS_CS_ImAp *ap = dynamic_cast<ACS_CS_ImAp *>(getObject((*it).c_str()));

		if (ap)
			obj->aps.insert(ap);
	}
}

void ACS_CS_ImModel::createConfigurationInfoVlanCategoryAggregation(ACS_CS_ImVlanCategory *obj)
{
   ACS_CS_ImConfigurationInfo *parent = dynamic_cast<ACS_CS_ImConfigurationInfo *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->vlanCategory = obj;
}


void ACS_CS_ImModel::createVlanCategoryVlanAggregation(ACS_CS_ImVlan *obj)
{
   ACS_CS_ImVlanCategory *parent = dynamic_cast<ACS_CS_ImVlanCategory *>(getParentOf(obj->rdn));

   if(!parent)
      return;

   parent->vlans.insert(obj);

}
*/

bool sameObj(ACS_CS_ImBase *a, ACS_CS_ImBase *b) {
  return (*a == *b);
}



bool ACS_CS_ImModel::operator==(const ACS_CS_ImModel &other) const
{
   bool isEqual = true;

   // For each object, check if there is a corresponding object in the other model
   std::set<ACS_CS_ImBase *>::iterator it;
   for (it = m_objects.begin(); it != m_objects.end(); ++it) {
      ACS_CS_ImBase *object = *it;

      const ACS_CS_ImBase *otherObject = other.getObject(object->rdn.c_str());
      if (otherObject == 0) {
         isEqual = false;
         break;
      }

      if (*object != *otherObject ) {
         isEqual = false;
         break;
      }
   }
   return isEqual;
}


bool ACS_CS_ImModel::operator!=(const ACS_CS_ImModel &other) const {
   return !(*this == other);
}

//int ACS_CS_ImModel::getApzType() const
//{
//	int returnValue = 4;
//
//	acs_apgcc_paramhandling acsParamHandling;
//	acsParamHandling.getParameter("apConfigurationId=1,safApp=safImmService", "cpAndProtocolType", &returnValue);
//	return returnValue;
//}

const ACS_CS_ImBlade* ACS_CS_ImModel::getBladeObject(string shelf, uint16_t slot) const
{

	const ACS_CS_ImBlade* blade = 0;
	stringstream ss;
	ss << slot;
	string slot_str = ss.str();

	string shelf_dn = ACS_CS_ImmMapper::ATTR_SHELF_ID + "=" + shelf + "," + ACS_CS_ImmMapper::RDN_HARDWARECATEGORY;

	std::set<ACS_CS_ImBase *>::iterator it;

	lock->start_reading();

	for (it = m_objects.begin(); it != m_objects.end(); it++)
	{
		ACS_CS_ImBase * object = *it;

		if ((object) && (ACS_CS_ImUtils::getParentName(object).compare(shelf_dn) == 0)) {

			if (slot_str.compare(ACS_CS_ImUtils::getIdValueFromRdn(object->rdn)) == 0) {
				blade = dynamic_cast<const ACS_CS_ImBlade*> (object);

				break;
			}
		}
	}

	lock->stop_reading();

	return blade;


}

void ACS_CS_ImModel::print()
{
   std::set<ACS_CS_ImBase *>::iterator it;

   for (it = m_objects.begin(); it != m_objects.end(); it++)
      ACS_CS_ImUtils::printImBaseObject(*it);
}


void ACS_CS_ImModel::deleteSubTree(const ACS_CS_ImBase *object)
{
   std::set<const ACS_CS_ImBase *> children;
   getChildrenRecursive(object, children);

   std::set<const ACS_CS_ImBase *>::const_iterator it;

   lock->start_writing();

   for (it = children.begin(); it != children.end(); it++)
   {
      ACS_CS_ImBase *obj = const_cast<ACS_CS_ImBase *>(*it);
      delete obj;
      m_objects.erase(obj);
      obj = 0;
   }

   ACS_CS_ImBase *obj = const_cast<ACS_CS_ImBase *>(object);
   delete obj;
   m_objects.erase(obj);

   lock->stop_writing();

}

bool ACS_CS_ImModel::exists(ACS_CS_ImBase *object)
{
   std::set<ACS_CS_ImBase *>::iterator it;
   it = m_objects.find(object);
   return (it == m_objects.end() ? false : true);
}

const ACS_CS_ImBlade * ACS_CS_ImModel::getBlade(uint16_t entryId) const
{
   std::set<const ACS_CS_ImBase *> objects;

   const ACS_CS_ImBlade * result = 0;

   getObjects(objects, APBLADE_T);
   getObjects(objects, CPBLADE_T);
   getObjects(objects, OTHERBLADE_T);

   std::set<const ACS_CS_ImBase *>::const_iterator it;

   lock->start_reading();

   for (it = objects.begin(); it != objects.end() && !result; it++)
   {
      const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);

      if (blade && blade->entryId == entryId)
         result = blade;
   }

   lock->stop_reading();

   return result;
}

void ACS_CS_ImModel::getApzMagazineScx(const ACS_CS_ImBlade* & scx0Blade, const ACS_CS_ImBlade* & scx25Blade) const
{
	scx0Blade = scx25Blade = 0;

	//Get AP1 Magazine
	std::set<const ACS_CS_ImBase *> blades;
	getObjects(blades, APBLADE_T);
	bool found = false;
	std::string dnShelf;
	for (std::set<const ACS_CS_ImBase *>::iterator it = blades.begin(); ((it != blades.end()) && !found); it++)
	{
		const ACS_CS_ImApBlade * apBlade = dynamic_cast<const ACS_CS_ImApBlade *>(*it);
		if (apBlade && (apBlade->systemNumber == 1 && apBlade->functionalBoardName == APUB))
		{
			//Read DN of parent magazine
			dnShelf = ACS_CS_ImUtils::getParentName(apBlade->rdn);
			found = true;
		}
	}

	//Get SCX in APZ magazine
	if (found)
	{
		ACS_CS_ImShelf * shelf = dynamic_cast<ACS_CS_ImShelf *>(getObject(dnShelf.c_str()));
		if (shelf)
		{
			scx0Blade = getBladeObject(shelf->address, 0);
			scx25Blade = getBladeObject(shelf->address, 25);
		}
	}
}


ACS_CS_ImModel* ACS_CS_ImModel::findDifferences(ACS_CS_ImModel* newModel) const
{
	if (!newModel) {
		ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
				"ACS_CS_ImModel::findDifferences()\n"
				" Model passed as argument is empty"));
		return 0;
	}

	lock->start_reading();

	std::set<ACS_CS_ImBase *>::iterator it;

	ACS_CS_ImModel* diffModel = new ACS_CS_ImModel();

	//Look for objects that need to be deleted or modified
	for (it = m_objects.begin(); it != m_objects.end(); it++)
	{

		ACS_CS_ImBase *newObj = newModel->getObject((*it)->rdn.c_str());

		if (!newObj)
		{
			//The object doesn't exist in the new Model. Then it should be deleted.
			ACS_CS_ImBase* delObj = (*it)->clone();
			if (delObj)
			{
				delObj->action = ACS_CS_ImBase::DELETE;
				diffModel->addObject(delObj);
				ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
						"ACS_CS_ImModel::findDifferences()\n"
						"The object with RDN: %s has to be deleted", delObj->rdn.c_str()));
			}
		}
		else
		{
			if (*newObj != *(*it))
			{
				//The object is changed in the new Model. Then it should be modified.
				newObj->action = ACS_CS_ImBase::MODIFY;
				ACS_CS_ImBase* modObj = newObj->clone();
				if (modObj)
				{
					diffModel->addObject(modObj);
					ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
							"ACS_CS_ImModel::findDifferences()\n"
							"The object with RDN: %s has to be modified", modObj->rdn.c_str()));
				}
			}
			else{
				newObj->action = ACS_CS_ImBase::MODIFY;
			}

		}
	}

	//Look for objects that need to be created
	for (it = newModel->m_objects.begin(); it != newModel->m_objects.end(); it++)
	{
		//const ACS_CS_ImBase *oldObj = getObject(it->rdn.c_str());

		//if (!oldObj)
		if((*it)->action != ACS_CS_ImBase::MODIFY)
		{
			//The object doesn't exist in the old Model. Then it should be created.
			ACS_CS_ImBase* addObj = (*it)->clone();
			if (addObj)
			{
				addObj->action = ACS_CS_ImBase::CREATE;
				diffModel->addObject(addObj);
				ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
						"ACS_CS_ImModel::findDifferences()\n"
						"The object with RDN: %s has to be added", addObj->rdn.c_str()));


			}
		}
	}
	lock->stop_reading();

	if (diffModel->size() == 0) {
		ACS_CS_TRACE((ACS_CS_ImModel_TRACE,
				"ACS_CS_ImModel::findDifferences()\n"
				" No differences between the two models"));
		delete diffModel;
		return 0;
	}


	return diffModel;

}

void ACS_CS_ImModel::deleteObject(const ACS_CS_ImBase *object)
{

   lock->start_writing();

   ACS_CS_ImBase *obj = const_cast<ACS_CS_ImBase *>(object);
   delete obj;
   m_objects.erase(obj);

   lock->stop_writing();

}

const ACS_CS_ImHost * ACS_CS_ImModel::getNetworkHost(const std::string uuid) const
{
	std::set<const ACS_CS_ImBase *> computeResourceBase;
	std::set<const ACS_CS_ImBase *> hostBase;

	getObjects(computeResourceBase, CRM_COMPUTERESOURCE_T);
	getObjects(hostBase, TRM_HOST_T);

	std::string computeResourceDn;

	for (std::set<const ACS_CS_ImBase *>::iterator it = computeResourceBase.begin(); computeResourceBase.end() != it && computeResourceDn.empty(); ++it)
	{
		const ACS_CS_ImCrmComputeResource* computResource = dynamic_cast<const ACS_CS_ImCrmComputeResource*>(*it);

		if (computResource && computResource->uuid.compare(uuid) == 0)
		{
			computeResourceDn = computResource->rdn;
		}
	}

	const ACS_CS_ImHost * matching_host = 0;

	for (std::set<const ACS_CS_ImBase *>::iterator it = hostBase.begin(); hostBase.end() != it && !matching_host; ++it)
	{


		const ACS_CS_ImHost* host = dynamic_cast<const ACS_CS_ImHost*>(*it);

		if (host && host->computeResourceDn.compare(computeResourceDn) == 0)
		{
			matching_host = host;
		}
	}

	return matching_host;

}

