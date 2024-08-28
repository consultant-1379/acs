/*
 * ACS_CS_ImOmProfileManager.h
 *
 *  Created on: Apr 9, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_IMOMPROFILEMANAGER_H_
#define ACS_CS_IMOMPROFILEMANAGER_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImOmProfileManager : public ACS_CS_ImBase
{

public:

	ACS_CS_ImOmProfileManager();
	ACS_CS_ImOmProfileManager(const ACS_CS_ImOmProfileManager &other);

   virtual ~ACS_CS_ImOmProfileManager();

   string omProfileManagerId;
   string omProfile;
   string reportProgress;
   string activeCcFile;
   ProfileScopeType scope;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImOmProfileManager &operator=(const ACS_CS_ImOmProfileManager &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImOmProfileManager &copyObj(const ACS_CS_ImOmProfileManager &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImOmProfileManager & bladeClusterInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImOmProfileManager, "ACS_CS_ImOmProfileManager");


#endif /* ACS_CS_IMOMPROFILEMANAGER_H_ */
