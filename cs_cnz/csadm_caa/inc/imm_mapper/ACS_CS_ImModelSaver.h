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
 * @file ACS_CS_ImModelSaver.h
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

#ifndef ACS_CS_IMMODELSAVER_H_
#define ACS_CS_IMMODELSAVER_H_

#include "ACS_CC_Types.h"
#include "ACS_CS_ImModel.h"
#include "acs_apgcc_omhandler.h"


class ACS_CS_ImModelSaver {
public:
   ACS_CS_ImModelSaver(ACS_CS_ImModel *model);
   virtual ~ACS_CS_ImModelSaver();

   ACS_CC_ReturnType save(int implementerCheck = NO_REGISTERED_OI);
   ACS_CC_ReturnType save(string transactionName, int implementerCheck = NO_REGISTERED_OI);

private:


   ACS_CC_ReturnType saveObject(ACS_CS_ImBase *object, OmHandler *omHandler);
   ACS_CC_ReturnType saveObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName);
   ACS_CC_ReturnType deleteObject(ACS_CS_ImBase *object, OmHandler *omHandler);
   ACS_CC_ReturnType deleteObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName);
   ACS_CC_ReturnType modifyObject(ACS_CS_ImBase *object, OmHandler *omHandler);
   ACS_CC_ReturnType modifyObject(ACS_CS_ImBase *object, OmHandler *omHandler, string transactionName);

   bool fillObjects(int numLevels, std::set<const ACS_CS_ImBase *> *objects);
   bool removeChildren(const ACS_CS_ImBase * parent, int level, std::set<const ACS_CS_ImBase *> *objects);

   bool checkExistingObject(ACS_CS_ImBase *object, OmHandler *omHandler);

   ACS_CS_ImModel *model;


};

#endif /* ACS_CS_IMMODELSAVER_H_ */
