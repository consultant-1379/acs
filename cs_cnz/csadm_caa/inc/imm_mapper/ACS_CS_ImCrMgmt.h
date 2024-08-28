/*
 * ACS_CS_ImCrMgmt.h
 *
 *  Created on: Mar 27, 2015
 *      Author: eanform
 */

#ifndef ACS_CS_ImCrMgmt_H_
#define ACS_CS_ImCrMgmt_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"


using namespace std;

class ACS_CS_ImCrMgmt : public ACS_CS_ImBase
{
public:

	ACS_CS_ImCrMgmt();
   ACS_CS_ImCrMgmt(const ACS_CS_ImCrMgmt &other);
   virtual ~ACS_CS_ImCrMgmt();
   ACS_CS_ImCrMgmt &operator=(const ACS_CS_ImCrMgmt &rhs);

   string crMgmtId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   void copyObj(const ACS_CS_ImCrMgmt &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrMgmt & hdwm, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCrMgmt, "ACS_CS_ImCrMgmt");

#endif /* ACS_CS_ImCrMgmt_H_ */



