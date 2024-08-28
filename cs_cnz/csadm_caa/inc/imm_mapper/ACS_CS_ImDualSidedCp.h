/*
 * ACS_CS_ImDualSidedCp.h
 *
 *  Created on: Apr 20, 2012
 *      Author: estevol
 */

#ifndef ACS_CS_IMDUALSIDEDCP_H_
#define ACS_CS_IMDUALSIDEDCP_H_

#include <boost/serialization/export.hpp>

#include "ACS_CS_ImBase.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImImmObject.h"
//#include "ACS_CS_ImCpBlade.h"



using namespace std;

class ACS_CS_ImDualSidedCp : public ACS_CS_ImBase
{

public:

	ACS_CS_ImDualSidedCp();
	ACS_CS_ImDualSidedCp(const ACS_CS_ImDualSidedCp *other);
	virtual ~ACS_CS_ImDualSidedCp();

	string					axeDualSidedCpId;
	uint16_t				systemIdentifier;
	string					alias;
	string					apzSystem;
	uint16_t 				cpType;
	int32_t                                mauType;
	set<string> 			axeCpBladeRefTo;
	//set<ACS_CS_ImCpBlade *> cpBlades;

	bool isValid(int &errNo, const ACS_CS_ImModel &model);
	bool modify(ACS_APGCC_AttrModification **attrMods);
	ACS_CS_ImBase * clone() const;
	void copyObj(const ACS_CS_ImDualSidedCp *other);

	ACS_CS_ImImmObject * toImmObject(bool onlyModifiedAttrs = false);

protected:
	virtual bool equals(const ACS_CS_ImBase &obj) const;

};


namespace boost {
	namespace serialization {
		template<class Archive>
			void serialize(Archive & ar, ACS_CS_ImDualSidedCp & cp, const unsigned int /*version*/);
	}
}

BOOST_CLASS_EXPORT_KEY2(ACS_CS_ImDualSidedCp, "ACS_CS_ImDualSidedCp");

#endif /* ACS_CS_IMDUALSIDEDCP_H_ */
