#ifndef ACS_CS_ImCrmEquipment_H_
#define ACS_CS_ImCrmEquipment_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImBlade.h"
#include "ACS_CC_Types.h"
//#include "ACS_CS_ImCp.h"




class ACS_CS_ImCrmEquipment : public ACS_CS_ImBase
{

public:
   ACS_CS_ImCrmEquipment();
   ACS_CS_ImCrmEquipment(const ACS_CS_ImCrmEquipment &other);

   virtual ~ACS_CS_ImCrmEquipment ();

   string equipmentId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);

   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCrmEquipment &operator=(const ACS_CS_ImCrmEquipment &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCrmEquipment &copyObj(const ACS_CS_ImCrmEquipment &object);
};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrmEquipment & logical, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCrmEquipment, "ACS_CS_ImCrmEquipment");


#endif /* ACS_CS_ImCrmEquipment_H_ */
