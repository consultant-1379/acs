/*
 * ACS_CS_ImCandidateCcFile.h
 *
 *  Created on: Apr 12, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMCANDIDATECCFILE_H_
#define ACS_CS_IMCANDIDATECCFILE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCandidateCcFile : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCandidateCcFile();
	ACS_CS_ImCandidateCcFile(const ACS_CS_ImCandidateCcFile &other);

   virtual ~ACS_CS_ImCandidateCcFile();

   string candidateCcFileId;
   string reportProgress;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCandidateCcFile &operator=(const ACS_CS_ImCandidateCcFile &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCandidateCcFile &copyObj(const ACS_CS_ImCandidateCcFile &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCandidateCcFile & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCandidateCcFile, "ACS_CS_ImCandidateCcFile");

#endif /* ACS_CS_IMCANDIDATECCFILE_H_ */
