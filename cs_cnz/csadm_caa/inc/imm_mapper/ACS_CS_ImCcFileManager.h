/*
 * ACS_CS_ImCcFileManager.h
 *
 *  Created on: Apr 12, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMCCFILEMANAGER_H_
#define ACS_CS_IMCCFILEMANAGER_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCcFileManager : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCcFileManager();
	ACS_CS_ImCcFileManager(const ACS_CS_ImCcFileManager &other);

   virtual ~ACS_CS_ImCcFileManager();

   string ccFileManagerId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCcFileManager &operator=(const ACS_CS_ImCcFileManager &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCcFileManager &copyObj(const ACS_CS_ImCcFileManager &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCcFileManager & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCcFileManager, "ACS_CS_ImCcFileManager");

#endif /* ACS_CS_IMCCFILEMANAGER_H_ */
