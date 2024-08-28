/*
 * ACS_CS_ImApg.cpp

 *
 *  Created on: May 3, 2013
 *      Author: egiacri
 */

#include "ACS_CS_ImApg.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CC_Types.h"

#include "ACS_CS_ImUtils.h"

#include "ACS_CS_Trace.h"

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>

namespace parseSymbol {
		const char comma = ',';
		const char equal = '=';
};

BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImApg);

ACS_CS_Trace_TDEF(ACS_CS_ImApg_TRACE);

ACS_CS_ImApg::ACS_CS_ImApg() {
	// TODO Auto-generated constructor stub
	type=APGNODE_T;
}

ACS_CS_ImApg::ACS_CS_ImApg(const ACS_CS_ImApg &other) {

	*this = other;
}

ACS_CS_ImApg::~ACS_CS_ImApg() {

}

ACS_CS_ImApg& ACS_CS_ImApg::operator=(const ACS_CS_ImApg &rhs)
{
	copyObj(rhs);
	return *this;
}

bool ACS_CS_ImApg::isValid(int &errNo, const ACS_CS_ImModel &model)
{
	errNo = 0;
	return true;
}

bool ACS_CS_ImApg::modify(ACS_APGCC_AttrModification **attrMods)
{
	return false;
}

ACS_CS_ImBase* ACS_CS_ImApg::clone() const
{
	ACS_CS_ImBase* ImApg = new ACS_CS_ImApg(*this);
	return ImApg;
}

ACS_CS_ImImmObject* ACS_CS_ImApg::toImmObject(bool onlyModifiedAttrs)
{
	ACS_CS_TRACE((ACS_CS_ImApg_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

	immObject->setClassName(ACS_CS_ImmMapper::CLASS_APGNODE.c_str());
	immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

	ACS_CS_TRACE((ACS_CS_ImApg_TRACE, "[%s@%d] parent DN:<%s>", __FUNCTION__, __LINE__, immObject->getParentName() ));

	std::vector<ACS_CS_ImValuesDefinitionType> attrList;

	if(!onlyModifiedAttrs)
		attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_APGNODE_ID.c_str(), m_ApgId));

	immObject->setAttributeList(attrList);

	return immObject;
}

bool ACS_CS_ImApg::getAPGNodeName(std::string& apgNode) const
{
	bool result = false;
	// Split functionId to key and value
	size_t equalPos = m_ApgId.find_first_of(parseSymbol::equal);

	if( (std::string::npos != equalPos) )
	{
		apgNode = m_ApgId.substr(equalPos + 1);
		result = true;
	}
	return result;
}

bool ACS_CS_ImApg::equals(const ACS_CS_ImBase &other) const
{
	ACS_CS_TRACE((ACS_CS_ImApg_TRACE, "Entry in [%s@%d]", __FUNCTION__, __LINE__ ));

	bool result = false;

	if (ACS_CS_ImBase::equals(other))
	{
		try
		{
			const ACS_CS_ImApg& apgObject = dynamic_cast<const ACS_CS_ImApg& >(other);
			result = (m_ApgId.compare(apgObject.m_ApgId) == 0);
		}
		catch (const std::bad_cast &e)
		{
			// failed reference cast
		}
	}

	ACS_CS_TRACE((ACS_CS_ImApg_TRACE, "Leaving [%s@%d], result:<%s>", __FUNCTION__, __LINE__, (result? "TRUE":"FALSE") ) );
	return result;
}

void ACS_CS_ImApg::copyObj(const ACS_CS_ImApg &object)
{
	action = object.action;
	type = object.type;
	rdn = object.rdn;
	m_ApgId = object.m_ApgId;
}

namespace boost {
	namespace serialization {
	template<class Archive>
	void serialize(Archive & ar, ACS_CS_ImApg & imapg, const unsigned int /*version*/)
		{
			ar & boost::serialization::base_object<ACS_CS_ImBase>(imapg);
			ar & imapg.m_ApgId;

		}
	}
}
// without the explicit instantiations below, the program will
// fail to link for lack of instantiantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImApg & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImApg & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImApg & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImApg & base, const unsigned int /*version*/);
   }
}
