/*
 * ACS_CS_ImCrmComputeResource.h
 *
 *  Created on: Mar 30, 2015
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_ImCrmComputeResource_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_ImCrmComputeResource_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImCrmComputeResource : public ACS_CS_ImBase
{
	//------------------------------
	// IMM OPERATIONS
	//------------------------------

public:

   ACS_CS_ImCrmComputeResource();
   ACS_CS_ImCrmComputeResource(const ACS_CS_ImCrmComputeResource *other);
   virtual ~ACS_CS_ImCrmComputeResource();

   string		computeResourceId;
   string		uuid;

   std::set<string>		macAddress;

   bool isValid(int &errNo, const ACS_CS_ImModel &model);
   bool modify(ACS_APGCC_AttrModification **attrMods);
   ACS_CS_ImBase * clone() const;
   void copyObj(const ACS_CS_ImCrmComputeResource *other);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrmComputeResource & cp, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCrmComputeResource, "ACS_CS_ImCrmComputeResource");

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_ImCrmComputeResource_H_ */
