/*
 * ACS_CS_ImOmProfileStruct.h
 *
 *  Created on: Apr 16, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMOMPROFILESTRUCT_H_
#define ACS_CS_IMOMPROFILESTRUCT_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImOmProfileStruct : public ACS_CS_ImBase
{

public:

	ACS_CS_ImOmProfileStruct();
	ACS_CS_ImOmProfileStruct(const ACS_CS_ImOmProfileStruct &other);

   virtual ~ACS_CS_ImOmProfileStruct();

   string axeCpClusterStructId;
   AsyncActionType actionId;
   AsyncActionStateType state;
   ActionResultType result;
   string reason;
   string timeOfLastAction;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImOmProfileStruct &operator=(const ACS_CS_ImOmProfileStruct &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImOmProfileStruct &copyObj(const ACS_CS_ImOmProfileStruct &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOmProfileStruct & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImOmProfileStruct, "ACS_CS_ImOmProfileStruct");


#endif /* ACS_CS_IMOMPROFILESTRUCT_H_ */
