/*
 * ACS_CS_ImCcFile.h
 *
 *  Created on: Feb 4, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMCCFILE_H_
#define ACS_CS_IMCCFILE_H_


#include <boost/serialization/export.hpp>


#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCcFile : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCcFile();
	ACS_CS_ImCcFile(const ACS_CS_ImCcFile &other);

   virtual ~ACS_CS_ImCcFile();

   string ccFileId;
   CcFileStateType state;
   string rulesVersion;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCcFile &operator=(const ACS_CS_ImCcFile &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCcFile &copyObj(const ACS_CS_ImCcFile &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCcFile & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCcFile, "ACS_CS_ImCcFile");

#endif /* ACS_CS_IMCCFILE_H_ */
