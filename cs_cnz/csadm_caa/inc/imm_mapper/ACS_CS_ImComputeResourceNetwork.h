/*
 * ACS_CS_ImComputeResourceNetwork.h
 *
 *  Created on:Jan 11, 2017
 *      Author: xmalrao
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCENETWORKRE_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCENETWORKRE_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImImmObject.h"


using namespace std;

class ACS_CS_ImComputeResourceNetwork : public ACS_CS_ImBase
{
	//------------------------------
	// IMM OPERATIONS
	//------------------------------

public:

   ACS_CS_ImComputeResourceNetwork();
   ACS_CS_ImComputeResourceNetwork(const ACS_CS_ImComputeResourceNetwork *other);
   virtual ~ACS_CS_ImComputeResourceNetwork();

   string   networkId;
   string	nicName;
   string	netName;
   string 	macAddress;

	bool isValid(int &errNo, const ACS_CS_ImModel &model);
	bool modify(ACS_APGCC_AttrModification **attrMods);
	ACS_CS_ImBase * clone() const;
	void copyObj(const ACS_CS_ImComputeResourceNetwork *other);
	int getNetworkRefToNumberOfElements();               
   void addNetworkRefToElement(std::string networksRefTo);                      
    std::set<string> getNetworkRefToElements();

	ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
	virtual bool equals(const ACS_CS_ImBase &obj) const;
	
};


namespace boost {
   namespace serialization {
   template<class Archive>
   void serialize(Archive & ar, ACS_CS_ImComputeResourceNetwork & cp, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImComputeResourceNetwork, "ACS_CS_ImComputeResourceNetwork");

#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMCOMPUTERESOURCENETWORKR.H */
