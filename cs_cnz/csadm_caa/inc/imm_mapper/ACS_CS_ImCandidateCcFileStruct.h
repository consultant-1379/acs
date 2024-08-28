/*
 * ACS_CS_ImCandidateCcFileStruct.h
 *
 *  Created on: Apr 16, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMCANDIDATECCFILESTRUCT_H_
#define ACS_CS_IMCANDIDATECCFILESTRUCT_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCandidateCcFileStruct : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCandidateCcFileStruct();
	ACS_CS_ImCandidateCcFileStruct(const ACS_CS_ImCandidateCcFileStruct &other);

   virtual ~ACS_CS_ImCandidateCcFileStruct();

   string axeCpClusterStructId;
   AsyncActionType actionId;
   AsyncActionStateType state;
   ActionResultType result;
   string reason;
   string timeOfLastAction;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCandidateCcFileStruct &operator=(const ACS_CS_ImCandidateCcFileStruct &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCandidateCcFileStruct &copyObj(const ACS_CS_ImCandidateCcFileStruct &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCandidateCcFileStruct & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCandidateCcFileStruct, "ACS_CS_ImCandidateCcFileStruct");

#endif /* ACS_CS_IMCANDIDATECCFILESTRUCT_H_ */
