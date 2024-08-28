/*
 * ACS_CS_ImCpClusterStruct.h
 *
 *  Created on: Jan 29, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMCPCLUSTERSTRUCT_H_
#define ACS_CS_IMCPCLUSTERSTRUCT_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"




using namespace std;


class ACS_CS_ImCpClusterStruct : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCpClusterStruct();
	ACS_CS_ImCpClusterStruct(const ACS_CS_ImCpClusterStruct &other);

   virtual ~ACS_CS_ImCpClusterStruct();

   string axeCpClusterStructId;
   AsyncActionType actionId;
   AsyncActionStateType state;
   ActionResultType result;
   string reason;
   string timeOfLastAction;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCpClusterStruct &operator=(const ACS_CS_ImCpClusterStruct &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCpClusterStruct &copyObj(const ACS_CS_ImCpClusterStruct &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpClusterStruct & bladeClusterInfo, const unsigned int /*version*/);
   }

}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCpClusterStruct, "ACS_CS_ImCpClusterStruct");


#endif /* ACS_CS_IMCPCLUSTERSTRUCT_H_ */
