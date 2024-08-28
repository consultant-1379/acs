/*
 * ACS_CS_ImInterface.h
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMINTERFACE_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMINTERFACE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"
#include "ACS_CS_NetworkDefinitions.h"

using namespace std;

class ACS_CS_ImInterface : public ACS_CS_ImBase
{
public:

	ACS_CS_ImInterface();
   ACS_CS_ImInterface(const ACS_CS_ImInterface &other);
   virtual ~ACS_CS_ImInterface();
   ACS_CS_ImInterface &operator=(const ACS_CS_ImInterface &rhs);

   string interfaceId;
   string mac;
   int32_t domain;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   void copyObj(const ACS_CS_ImInterface &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImInterface & host, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImInterface, "ACS_CS_ImInterface");

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMINTERFACE_H_ */
