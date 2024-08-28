//	*********************************************************
//	 COPYRIGHT Ericsson 2011.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2011.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2011 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************

#include "ACS_CS_Trace.h"
#include "ACS_CS_HostNetworkConverter.h"

#include "ACS_CS_SM_HWCTableNotify.h"

ACS_CS_Trace_TDEF(ACS_CS_SM_HWCTableNotify);

ACS_CS_SM_HWCTableNotify::ACS_CS_SM_HWCTableNotify()
{
}

ACS_CS_SM_HWCTableNotify::ACS_CS_SM_HWCTableNotify(const ACS_CS_SM_HWCTableNotify &right) :
		ACS_CS_SM_NotifyObject(right), m_hwcChange(right.m_hwcChange)
{
}

ACS_CS_SM_HWCTableNotify::~ACS_CS_SM_HWCTableNotify()
{
}

ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_SM_HWCTableNotify::getPrimitiveType() const
{
	return ACS_CS_Protocol::Primitive_NotifyHWCTableChange;
}

int ACS_CS_SM_HWCTableNotify::setBuffer(const char *buffer, int size)
{
	if (size <= 0)
	{
		return -1;
	}
	return ACS_CS_HostNetworkConverter::deserialize(buffer, *this);
}

int ACS_CS_SM_HWCTableNotify::getBuffer(char *buffer, int size) const
{
	int rc = ACS_CS_HostNetworkConverter::serialize(buffer, size, *this);
	if (rc == -1)
	{
		m_length = 0;
		return -1;
	} else
	{
		m_length = size;
	}

	return 0;
}

ACS_CS_Primitive * ACS_CS_SM_HWCTableNotify::clone() const
{
	return new ACS_CS_SM_HWCTableNotify(*this);
}

void ACS_CS_SM_HWCTableNotify::getHWCChange(ACS_CS_HWCChange &hwcChange) const
{
	hwcChange = m_hwcChange;
}

void ACS_CS_SM_HWCTableNotify::setHWCChange(const ACS_CS_HWCChange &hwcChange)
{
	m_hwcChange = hwcChange;
	m_length = 0;
	getLength(); // Call getLength to initialize buffer.
}

unsigned short ACS_CS_SM_HWCTableNotify::getLength () const
{
	if( m_length == 0 )
	{
		//bufferSize
		size_t bufferSize = sizeof(std::vector<ACS_CS_HWCData>::size_type); //The buffer contains at least the size of the vector which is a size_type value

		{//figure out size
			for (size_t i = 0; i < m_hwcChange.hwcData.size(); i++)
			{
				ACS_CS_HWCData hwcNotificationObj = m_hwcChange.hwcData[i];
				size_t notificationObjSize = sizeof(hwcNotificationObj.operationType) +
								sizeof(hwcNotificationObj.magazine) +
								sizeof(hwcNotificationObj.slot) +
								sizeof(hwcNotificationObj.sysType) +
								sizeof(hwcNotificationObj.sysNo) +
								sizeof(hwcNotificationObj.fbn) +
								sizeof(hwcNotificationObj.side) +
								sizeof(hwcNotificationObj.seqNo) +
								sizeof(hwcNotificationObj.ipEthA) +
								sizeof(hwcNotificationObj.ipEthB) +
								sizeof(hwcNotificationObj.aliasEthA) +
								sizeof(hwcNotificationObj.aliasEthB) +
								sizeof(hwcNotificationObj.netmaskAliasEthA) +
								sizeof(hwcNotificationObj.netmaskAliasEthB) +
								sizeof(hwcNotificationObj.dhcpMethod) +
								sizeof(hwcNotificationObj.sysId);

				//bytes needed for softwarePackage: size + string
				notificationObjSize += sizeof(unsigned int);
				if (!hwcNotificationObj.softwarePackage.empty())
					notificationObjSize += hwcNotificationObj.softwarePackage.length();

				//bytes needed for productId: size + string
				notificationObjSize += sizeof(unsigned int);
				if (!hwcNotificationObj.productId.empty())
					notificationObjSize += hwcNotificationObj.productId.length();

				//bytes needed for productRevision: size + string
				notificationObjSize += sizeof(unsigned int);
				if (!hwcNotificationObj.productRevision.empty())
					notificationObjSize += hwcNotificationObj.productRevision.length();

				bufferSize += notificationObjSize;
				//std::cout << "DBG: [" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << "] (seqNo == " << hwcNotificationObj.seqNo << ") notificationObjSize == " << notificationObjSize << "<--------" << std::endl;
			}

		}

		//std::cout << "DBG: [" << __FILE__ << "::" << __FUNCTION__ << "@" << __LINE__ << "] bufferSize == " << bufferSize << "<--------" << std::endl;

		std::vector<char> buffer(bufferSize);
		getBuffer(&buffer[0],bufferSize);
	}

	return m_length;
}
