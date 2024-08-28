/*
 * ACS_CS_ImLogicalNetwork.h
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMEXTERNALNETWORK_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMEXTERNALNETWORK_H_


#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImExternalNetwork : public ACS_CS_ImBase
{
	//------------------------------
	// IMM OPERATIONS
	//------------------------------

public:

   ACS_CS_ImExternalNetwork();
   ACS_CS_ImExternalNetwork(const ACS_CS_ImExternalNetwork *other);
   virtual ~ACS_CS_ImExternalNetwork();

   std::string						networkId;
   std::string						name;
   std::string						restrictedName;
   std::string 						description;
   std::set<std::string>			attachedInterfaces;
   AdminState 						adminState;
   std::set<std::string>			nicName;


   bool isValid(int &errNo, const ACS_CS_ImModel &model);
   bool modify(ACS_APGCC_AttrModification **attrMods);
   ACS_CS_ImBase * clone() const;
   void copyObj(const ACS_CS_ImExternalNetwork *other);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImExternalNetwork & ln, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImExternalNetwork, "ACS_CS_ImLogicalNetwork");

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMEXTERNALNETWORK_H_ */
