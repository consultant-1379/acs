/*
 * ACS_CS_ImCpProductInfo.h
 *
 *  Created on: Feb 4, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_ImCpProductInfo_H_
#define ACS_CS_ImCpProductInfo_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"


using namespace std;


class ACS_CS_ImCpProductInfo : public ACS_CS_ImBase
{

public:

	ACS_CS_ImCpProductInfo();
	ACS_CS_ImCpProductInfo(const ACS_CS_ImCpProductInfo &other);

   virtual ~ACS_CS_ImCpProductInfo();

   std::string cpProductInfoId;
   std::string productRevision;
   std::string productName;
   std::string manufacturingDate;
   std::string productVendor;
   std::string productNumber;
   std::string serialNumber;


   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImCpProductInfo &operator=(const ACS_CS_ImCpProductInfo &rhs);

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;


private:

   ACS_CS_ImCpProductInfo &copyObj(const ACS_CS_ImCpProductInfo &object);


};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCpProductInfo & prodInfo, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImCpProductInfo, "ACS_CS_ImCpProductInfo");


#endif /* ACS_CS_ImCpProductInfo_H_ */
