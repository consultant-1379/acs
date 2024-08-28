/*
 * ACS_CS_ImTransport.h
 *
 *  Created on: Jan 19, 2017
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMTRANSPORT_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMTRANSPORT_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"


using namespace std;

class ACS_CS_ImTransport : public ACS_CS_ImBase
{
public:

	ACS_CS_ImTransport();
	ACS_CS_ImTransport(const ACS_CS_ImTransport &other);
   virtual ~ACS_CS_ImTransport();
   ACS_CS_ImTransport &operator=(const ACS_CS_ImTransport &rhs);

   string transportMId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   void copyObj(const ACS_CS_ImTransport &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImTransport & imtra, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImTransport, "ACS_CS_ImTransport");


#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMTRANSPORT_H_ */
