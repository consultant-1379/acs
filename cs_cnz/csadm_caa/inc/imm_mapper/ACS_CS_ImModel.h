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
 * @file ACS_CS_ImModel.h
 *
 * @brief A container for a complete IMM structure of IMM class instances
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
#ifndef ACS_CS_IMMODEL_H_
#define ACS_CS_IMMODEL_H_

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CS_ImHost.h"

#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_ImUtils.h"

//IMPORTANT NOTE: DO NOT INCLUDE ACS_CS_ImSerialization.h!!!

class ACS_CS_ImModel
{
 public:
   ACS_CS_ImModel();
   ACS_CS_ImModel(const ACS_CS_ImModel &other);
   virtual ~ACS_CS_ImModel();


   /** Adds an object to the model
    *
    * @param[in] a reference to an object
    * @return True if successfully added.
    */
   bool addObject(ACS_CS_ImBase *object);


   void deleteSubTree(const ACS_CS_ImBase *object);

   void deleteObject(const ACS_CS_ImBase *object);

   /** Applies a subset to the model
    * The objects in the the subset are used to update the current model.
    *
    * @param[in] a reference to a model
    * @return True if successfully updated.
    */
   bool applySubset(const ACS_CS_ImModel *subset);


   /** Returns a ref to an object given the object name (rdn)
    *
    * @param[in] rdn to the object
    * @param[out]
    * @param[in,out]
    * @return The object, if found
    */
   ACS_CS_ImBase * getObject(const char *objName) const;

   bool exists(ACS_CS_ImBase *object);

   size_t size();

   /** Fills the set with objects, depending on the class type
    * The set will be cleared before filling with new objects
    *
    * @param[in] ref to a set of objects
    * @param[in] class type to use for filtering
    */
   void getObjects(std::set<const ACS_CS_ImBase *> &objects, ClassType type = UNDEFINED_T) const;

   ACS_CS_ImBase * getParentOf(const std::string &rdn) const;

   void getChildren(const string parentRdn, std::set<const ACS_CS_ImBase *> &children, ClassType type = UNDEFINED_T) const;
   void getChildren(const ACS_CS_ImBase *parent, std::set<const ACS_CS_ImBase *> &children, ClassType type = UNDEFINED_T) const;
   void getChildrenRecursive(const ACS_CS_ImBase *parent, std::set<const ACS_CS_ImBase *> &children);

   void createAssociations();

   ACS_CS_ImModel &operator=(const ACS_CS_ImModel &rhs);
   virtual bool isValid(int &errNo);

   //int getApzType() const;
   const ACS_CS_ImBlade* getBladeObject(string shelf, uint16_t slot) const;
   void getApzMagazineScx(const ACS_CS_ImBlade* & scx0Blade, const ACS_CS_ImBlade* & scx25Blade) const;

   bool operator==(const ACS_CS_ImModel &other) const;
   bool operator!=(const ACS_CS_ImModel &other) const;

   void print();

   const ACS_CS_ImBlade * getBlade(uint16_t boardId) const;

   ACS_CS_ImModel* findDifferences(ACS_CS_ImModel* newModel) const;

   const ACS_CS_ImHost * getNetworkHost(const std::string uuid) const;

   //IPMORTANT NOTE: This method shall be called explicitly only by ImSerialization class!!!
//   template<class Archive>
//   void serialize(Archive & ar)
//   {
//	   ar & m_objects;
//   }


protected:

   std::set<ACS_CS_ImBase *> m_objects;


private:

   ACS_CS_ImModel &copyObj(const ACS_CS_ImModel &object);
   void release();

//   void createConfigurationInfoCpCategoryAggregation(ACS_CS_ImCpCategory *obj);
//   void createConfigurationInfoConfigurationCategoryAggregation(ACS_CS_ImHardwareConfigurationCategory *obj);
//   void createConfigurationInfoApCategoryAggregation(ACS_CS_ImApCategory *obj);
//   void createConfigurationInfoBladeClusterInfoAggregation(ACS_CS_ImBladeClusterInfo *obj);
//   void createConfigurationInfoAdvancedConfigurationAggregation(ACS_CS_ImAdvancedConfiguration *obj);
//   void createCpCategoryCpAggregation(ACS_CS_ImCp *obj);
//   void createShelfCpBladeAggregation(ACS_CS_ImCpBlade *obj);
//   void createCpCpBladeAssociation(ACS_CS_ImCpBlade *obj);
//   void createApCategoryApAggregation(ACS_CS_ImAp *obj);
//   void createShelfOtherBladeAggregation(ACS_CS_ImOtherBlade *obj);
//   void createHardwareConfigurationCategoryShelfAggregation(ACS_CS_ImShelf *obj);
//   void createShelfApBladeAggregation(ACS_CS_ImApBlade *obj);
//   void createApApBladeAssociation(ACS_CS_ImApBlade *obj);
//   void createBladeClusterInfoCpGroupCategoryAggregation(ACS_CS_ImCpGroupCategory *obj);
//   void createCpGroupCategoryCpGroupAggregation(ACS_CS_ImCpGroup *obj);
//   void createCpCpGroupAssociation(ACS_CS_ImCpGroup *obj);
//   void createConfigurationInfoApServiceCategoryAggregation(ACS_CS_ImApServiceCategory *obj);
//   void createApServiceCategoryApServiceAggregation(ACS_CS_ImApService *obj);
//   void createApApServiceAssociation(ACS_CS_ImApService *obj);
//   void createConfigurationInfoVlanCategoryAggregation(ACS_CS_ImVlanCategory *obj);
//   void createVlanCategoryVlanAggregation(ACS_CS_ImVlan *obj);




   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive & ar, const unsigned int /*version*/)
   {
	   ar & m_objects;
   }

   ACS_CS_ReaderWriterLock *lock;


   friend class ACS_CS_ImRepository;


};



#endif /* ACS_CS_IMMODEL_H_ */
