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
 * @file ACS_CS_ImEntryIdHelper.cpp
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
 * 2011-11-23  XBJOAXE  Starting from scratch
 *
 ****************************************************************************/
#include <iostream>
using namespace std;

#include "ACS_CS_ImRepository.h"
#include "ACS_CS_ImEntryIdHelper.h"

ACS_CS_ImEntryIdHelper::ACS_CS_ImEntryIdHelper() {
   // TODO Auto-generated constructor stub

}

ACS_CS_ImEntryIdHelper::~ACS_CS_ImEntryIdHelper() {
   // TODO Auto-generated destructor stub
}

uint16_t ACS_CS_ImEntryIdHelper::getNewEntryId(){

	uint16_t entryId = 0;
	uint16_t entries = 0;
	std::set<uint16_t> entryIds;

	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModel();
	const set<ACS_CS_ImModelSubset *> subsets = ACS_CS_ImRepository::instance()->getSubsets();

	std::set<const ACS_CS_ImBase *> objects;

	model->getObjects(objects, AP_T);
	model->getObjects(objects, APBLADE_T);
	model->getObjects(objects, CPBLADE_T);
	model->getObjects(objects, OTHERBLADE_T);

	std::set<ACS_CS_ImModelSubset *>::iterator subset;

	for(subset=subsets.begin(); subset != subsets.end(); subset++){
		ACS_CS_ImModelSubset * s  = *subset;
		s->getObjects(objects, AP_T);
		s->getObjects(objects, APBLADE_T);
		s->getObjects(objects, CPBLADE_T);
		s->getObjects(objects, OTHERBLADE_T);
	}

	std::set<const ACS_CS_ImBase *>::iterator it;

	for (it = objects.begin(); it != objects.end(); it++)
	{
		const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);
		const ACS_CS_ImAp *ap = dynamic_cast<const ACS_CS_ImAp *>(*it);

		if (blade != NULL){
			entryIds.insert(blade->entryId);
			entries++;
		}
		if (ap != NULL){
			entryIds.insert(ap->entryId);
			entries++;
		}
	}

	// Find a free entryId
	for(int i = entries+1; i>0; i--){
		std::set<uint16_t>::iterator it;
		it = entryIds.find(i);
		if(it == entryIds.end()){
			entryId	= i;
			break;
		}
	}

	return entryId;
}


bool ACS_CS_ImEntryIdHelper::calculateEntryIdNumber(const ACS_CS_ImModel *model, ClassType type, uint16_t &entryId)
{
   std::set<const ACS_CS_ImBase *>::const_iterator it;

   if (type != APBLADE_T && type != CPBLADE_T && type != OTHERBLADE_T && type != AP_T)
      return false;

   std::set<const ACS_CS_ImBase *> objects;
   model->getObjects(objects, type);

   if (objects.empty())
      return false;

   uint16_t maxEntryId = 0;

   for (it = objects.begin(); it != objects.end(); it++)
   {
      const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);
      const ACS_CS_ImAp *ap = dynamic_cast<const ACS_CS_ImAp *>(*it);

      if (blade != NULL && blade->entryId > maxEntryId)
            maxEntryId = blade->entryId;

      if (ap != NULL && ap->entryId > maxEntryId)
            maxEntryId = ap->entryId;
   }

   bool *isOccupied = new bool[maxEntryId + 1];
   for (uint16_t index = 0; index < maxEntryId; index++)
      isOccupied[index] = false;


   for (it = objects.begin(); it != objects.end(); it++)
   {
      const ACS_CS_ImBlade *blade = dynamic_cast<const ACS_CS_ImBlade *>(*it);
      const ACS_CS_ImAp *ap = dynamic_cast<const ACS_CS_ImAp *>(*it);

      if (blade != NULL)
         isOccupied[blade->entryId] = true;
      if (ap != NULL)
         isOccupied[ap->entryId] = true;
   }

   bool foundNewEntryId = false;

   for (uint16_t i = 0; i < maxEntryId; i++)
      if (!isOccupied[i])
      {
         entryId = i;
         foundNewEntryId = true;
      }

   delete [] isOccupied;

   return foundNewEntryId ? ACS_CC_SUCCESS : ACS_CC_FAILURE;

}
