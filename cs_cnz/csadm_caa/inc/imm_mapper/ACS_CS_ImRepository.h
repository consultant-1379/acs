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
 * @file ACS_CS_ImRepository.h
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

#ifndef ACS_CS_IMREPOSITORY_H_
#define ACS_CS_IMREPOSITORY_H_

#include <set>

#include <ace/Singleton.h>
#include "ace/Mutex.h"
#include <ace/Recursive_Thread_Mutex.h>

#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSubset.h"
#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImIMMReader.h"

class ACS_CS_ImRepository {
public:
   ACS_CS_ImRepository();
   virtual ~ACS_CS_ImRepository();

   ACS_CS_ImBase * createObject(ACS_APGCC_CcbId ccbId, const char *className, const char *parentName, ACS_APGCC_AttrValues **attr);
   bool modifyObject(ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
   bool deleteObject(ACS_APGCC_CcbId ccbId, const char *objName);

   bool completeTransaction(ACS_APGCC_CcbId ccbId, int &errNo);
   bool disposeTransaction(ACS_APGCC_CcbId ccbId);
   bool applyTransaction(ACS_APGCC_CcbId ccbId);

   bool applySubset(const ACS_CS_ImModel *subset);
   const set<ACS_CS_ImModelSubset *> getSubsets(){return subsets;};

   const ACS_CS_ImModel * getModel() const;

   const ACS_CS_ImModel * getSubset(ACS_APGCC_CcbId ccbId) const;
   ACS_CS_ImModel * getModel() { return model; }
   void setModel(ACS_CS_ImModel * model);

   const ACS_CS_ImBase * getParentOf(const std::string &rdn) const;
   bool isValid(int & errNo);

   ACS_CC_ReturnType save();
   ACS_CC_ReturnType save(string transactionName);


   //static ACS_CS_ImRepository * getInstance();
   static ACS_CS_ImRepository* instance();

   static void finalize();

   bool getIpAssignment(ACS_CS_ImBlade *blade) const;

   bool addObject(ACS_CS_ImBase *object);

   bool storedObject(const char * classname);

   void replaceModel(ACS_CS_ImModel *model);

   ACS_CS_ImModel * getModelCopy();

   bool replaceObject(ACS_CS_ImBase *object);

   void reloadModel();


private:

   //static ACS_CS_ImRepository * instance;
   typedef ACE_Singleton<ACS_CS_ImRepository, ACE_Recursive_Thread_Mutex> instance_;

   ACS_CS_ImModelSubset * findSubset(ACS_APGCC_CcbId ccbId) const;
   bool addObject(ACS_APGCC_CcbId ccbId, ACS_CS_ImBase *object);

   ACS_CS_ImModel *model;
   ACS_CS_ImIMMReader *immReader;
   set<ACS_CS_ImModelSubset *> subsets;

   mutable ACE_Recursive_Thread_Mutex mutex;
};

#endif /* ACS_CS_IMREPOSITORY_H_ */
