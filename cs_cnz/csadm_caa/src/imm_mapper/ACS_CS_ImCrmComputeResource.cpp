
#include "ACS_CS_ImCrmComputeResource.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_Registry.h"
#include <cstring>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>



BOOST_CLASS_EXPORT_IMPLEMENT(ACS_CS_ImCrmComputeResource);

ACS_CS_ImCrmComputeResource::ACS_CS_ImCrmComputeResource()
{
   type = CRM_COMPUTERESOURCE_T;
}

ACS_CS_ImCrmComputeResource::ACS_CS_ImCrmComputeResource(const ACS_CS_ImCrmComputeResource *other)
{
	copyObj(other);
}

ACS_CS_ImCrmComputeResource::~ACS_CS_ImCrmComputeResource()
{}


bool ACS_CS_ImCrmComputeResource::isValid(int & errNo, const ACS_CS_ImModel & /*model*/)
{
	errNo = 0;

	return true;
}

bool ACS_CS_ImCrmComputeResource::modify(ACS_APGCC_AttrModification **attrMods)
{
   for (int i = 0; attrMods[i] != NULL; i++)
	{
		ACS_APGCC_AttrModification *atrMod = attrMods[i];

		if (atrMod->modType != ACS_APGCC_ATTR_VALUES_REPLACE)
			return false;

		ACS_APGCC_AttrValues *modAttr = &attrMods[i]->modAttr;

		if (strcmp(modAttr->attrName, ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID.c_str()) == 0)
		{
			computeResourceId = (char *) modAttr->attrValues[0];
		}
		else if (strcmp(modAttr->attrName, "uuid") == 0)
		{
			if (modAttr->attrValuesNum > 0)
				uuid = (char *) modAttr->attrValues[0];
			else
				uuid = "";
		}
		else if (strcmp(modAttr->attrName, "macAddress") == 0)
		{
			macAddress.clear();
			for (unsigned int j = 0; j < modAttr->attrValuesNum; ++j)
			{
				macAddress.insert((char *) modAttr->attrValues[j]);
			}
		}
		else
			// Bad attributeName
			return false;
	}

	return true;
}

ACS_CS_ImBase * ACS_CS_ImCrmComputeResource::clone() const
{
   ACS_CS_ImBase *cp = new ACS_CS_ImCrmComputeResource(*this);
   return cp;
}


bool ACS_CS_ImCrmComputeResource::equals(const ACS_CS_ImBase &obj) const
{
   if (!ACS_CS_ImBase::equals(obj))
		return false;

	try
	{
		const ACS_CS_ImCrmComputeResource & other = dynamic_cast<const ACS_CS_ImCrmComputeResource &>(obj);

		if (computeResourceId != other.computeResourceId)
			return false;
		if (uuid != other.uuid)
			return false;
		if (macAddress != other.macAddress)
			return false;
	}
	catch (const std::bad_cast &e)
	{
		return false;
	}

	return true;
}

void ACS_CS_ImCrmComputeResource::copyObj(const ACS_CS_ImCrmComputeResource *other)
{
	rdn = other->rdn;
	type = other->type;
	action = other->action;
	computeResourceId = other->computeResourceId;
	uuid = other->uuid;
	macAddress = other->macAddress;
}
ACS_CS_ImImmObject * ACS_CS_ImCrmComputeResource::toImmObject(bool onlyModifiedAttrs)
{
   ACS_CS_ImImmObject *immObject = new ACS_CS_ImImmObject();

   immObject->setClassName(ACS_CS_ImmMapper::CLASS_CRM_COMPUTE_RESOURCE.c_str());
   immObject->setParentName(ACS_CS_ImUtils::getParentName(rdn).c_str());

   std::vector<ACS_CS_ImValuesDefinitionType> attrList;

   if(!onlyModifiedAttrs)
   	   attrList.push_back(ACS_CS_ImUtils::createStringType(ACS_CS_ImmMapper::ATTR_CRM_COMPUTE_RESOURCE_ID.c_str(), computeResourceId));

   if (!uuid.empty())
	   attrList.push_back(ACS_CS_ImUtils::createStringType("uuid", uuid));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("uuid"));

   if (!macAddress.empty())
	   attrList.push_back(ACS_CS_ImUtils::createMultipleStringType("macAddress", macAddress));
   else
	   attrList.push_back(ACS_CS_ImUtils::createEmptyStringType("macAddress"));

   immObject->setAttributeList(attrList);

   return immObject;
}

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive & ar, ACS_CS_ImCrmComputeResource & cp, const unsigned int /*version*/)
      {
         ar & boost::serialization::base_object<ACS_CS_ImBase>(cp);
         ar & cp.computeResourceId;
         ar & cp.uuid;
         ar & cp.macAddress;
      }
   }
}

// without the explicit instantiations below, the program will
// fail to link for lack of instantiation of the above function

namespace boost {
   namespace serialization {
      template void serialize( boost::archive::text_iarchive & ar, ACS_CS_ImCrmComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::text_oarchive & ar, ACS_CS_ImCrmComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_iarchive & ar, ACS_CS_ImCrmComputeResource & base, const unsigned int /*version*/);
      template void serialize( boost::archive::binary_oarchive & ar, ACS_CS_ImCrmComputeResource & base, const unsigned int /*version*/);
   }
}
