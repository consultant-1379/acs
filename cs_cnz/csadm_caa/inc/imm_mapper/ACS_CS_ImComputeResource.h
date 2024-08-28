/*
 * ACS_CS_ImComputeResource.h
 *
 *  Created on: Mar 30, 2015
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCE_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImComputeResource : public ACS_CS_ImBase
{
	//------------------------------
	// IMM OPERATIONS
	//------------------------------

public:

   ACS_CS_ImComputeResource();
   ACS_CS_ImComputeResource(const ACS_CS_ImComputeResource *other);
   virtual ~ACS_CS_ImComputeResource();

   string		computeResourceId;
   string		uuid;
   int			crType;
   string 		macAddressEthA;
   string 		macAddressEthB;
   string 		crRoleLabel;
   int			crRoleId;
   string 		ipAddressEthA;
   string 		ipAddressEthB;
   set<std::string>	networks;

   bool isValid(int &errNo, const ACS_CS_ImModel &model);
   bool modify(ACS_APGCC_AttrModification **attrMods);
   ACS_CS_ImBase * clone() const;
   void copyObj(const ACS_CS_ImComputeResource *other);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImComputeResource & cp, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImComputeResource, "ACS_CS_ImComputeResource");

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCE_H_ */
