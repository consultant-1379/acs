#ifndef ACS_CS_IMLOGICALMGMT_H_
#define ACS_CS_IMLOGICALMGMT_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CC_Types.h"
//#include "ACS_CS_ImCp.h"




class ACS_CS_ImLogicalMgmt : public ACS_CS_ImBase
{

public:
   ACS_CS_ImLogicalMgmt();
   ACS_CS_ImLogicalMgmt(const ACS_CS_ImLogicalMgmt &other);

   virtual ~ACS_CS_ImLogicalMgmt ();

   string axeLogicalMgmtId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);

   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImLogicalMgmt &operator=(const ACS_CS_ImLogicalMgmt &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImLogicalMgmt &copyObj(const ACS_CS_ImLogicalMgmt &object);
};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImLogicalMgmt & logical, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImLogicalMgmt, "ACS_CS_ImLogicalMgmt");


#endif /* ACS_CS_ImLogicalMgmt_H_ */
