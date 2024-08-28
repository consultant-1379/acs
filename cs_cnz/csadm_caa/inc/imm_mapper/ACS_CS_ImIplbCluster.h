/*
 * ACS_CS_ImIplbCluster.h
 *
 *  Created on: Mar 30, 2015
 *      Author: eanform
 */

#ifndef CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMIPLBCLUSTER_H_
#define CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMIPLBCLUSTER_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"


using namespace std;

class ACS_CS_ImIplbCluster : public ACS_CS_ImBase
{
public:

	ACS_CS_ImIplbCluster();
   ACS_CS_ImIplbCluster(const ACS_CS_ImIplbCluster &other);
   virtual ~ACS_CS_ImIplbCluster();
   ACS_CS_ImIplbCluster &operator=(const ACS_CS_ImIplbCluster &rhs);

   string iplbClusterId;

   virtual bool isValid(int &errNo, const ACS_CS_ImModel &model);
   virtual bool modify(ACS_APGCC_AttrModification **attrMods);
   virtual ACS_CS_ImBase * clone() const;

   ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
   virtual bool equals(const ACS_CS_ImBase &obj) const;

private:
   void copyObj(const ACS_CS_ImIplbCluster &object);

};


namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImIplbCluster & hdwm, const unsigned int /*version*/);
   }
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImIplbCluster, "ACS_CS_ImIplbCluster");


#endif /* CSADM_CAA_INC_IMM_MAPPER_ACS_CS_IMIPLBCLUSTER_H_ */
