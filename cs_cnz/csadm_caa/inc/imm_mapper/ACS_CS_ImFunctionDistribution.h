/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2013
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
  *
  *
  *
  * ACS_CS_ImFunctionDistribution.h
  *
  *  Created on: May 3, 2013
  *      Author: egiacri
  *
  *
 -------------------------------------------------------------------------*//*
  *
  * REVISION HISTORY
  *
  * DATE        USER     DESCRIPTION/TR
  * --------------------------------
  * 2013-05-02  EGIACRI  Starting from scratch
  *
  ****************************************************************************/

#ifndef ACS_CS_IMFUNCTIONDISTRIBUTION_H_
#define ACS_CS_IMFUNCTIONDISTRIBUTION_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"

class ACS_CS_ImFunctionDistribution : public ACS_CS_ImBase
{
 public:
	ACS_CS_ImFunctionDistribution();
	ACS_CS_ImFunctionDistribution(const ACS_CS_ImFunctionDistribution &other);

	virtual ~ACS_CS_ImFunctionDistribution();
	ACS_CS_ImFunctionDistribution& operator=(const ACS_CS_ImFunctionDistribution &rhs);

	std::string m_FunctionDistributionMId;

	virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
	virtual bool modify(ACS_APGCC_AttrModification** attrMods);
	virtual ACS_CS_ImBase* clone() const;

	ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

 protected:

	virtual bool equals(const ACS_CS_ImBase& obj) const;

 private:

	void copyObj(const ACS_CS_ImFunctionDistribution& object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImFunctionDistribution & imfd, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImFunctionDistribution, "ACS_CS_ImFunctionDistribution");


#endif /* ACS_CS_IMFUNCTIONDISTRIBUTION_H_ */
