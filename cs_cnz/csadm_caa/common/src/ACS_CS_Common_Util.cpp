/*
 * @file ACS_CS_Common_Util.cpp
 * @author xminaon
 * @date Nov 8, 2010
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */
#include <stdlib.h>

#include "ACS_CS_Common_Util.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_Util.h"
#include <ACS_APGCC_Util.H>

unsigned short ACS_CS_Common_Util::getSysId (const ACS_CS_TableEntry *tableEntry)
{
	unsigned short sysId = ACS_CS_NS::ILLEGAL_SYSID;

	if (tableEntry == 0)
	  return sysId;

	ACS_CS_Attribute sysTypeAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SysType);
	ACS_CS_Attribute sysNoAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SysNo);
	ACS_CS_Attribute seqNoAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SeqNo);

	unsigned short sysType = 0;
	unsigned short sysNo = 0;
	unsigned short seqNo = 0;

	if (sysTypeAtt.getValueLength() == sizeof(sysType))
	{
	  if (sysTypeAtt.getValue(reinterpret_cast<char *>(&sysType), sizeof(sysType)) > 0)
	  {
		 if (sysType == ACS_CS_Protocol::SysType_BC)
		 {
			if (seqNoAtt.getValue(reinterpret_cast<char *>(&seqNo), sizeof(seqNo)) > 0)
			{
			   sysId = sysType + seqNo;
			}
		 }
		 else if (sysType == ACS_CS_Protocol::SysType_CP)
		 {
			if (sysNoAtt.getValue(reinterpret_cast<char *>(&sysNo), sizeof(sysNo)) > 0)
			{
			   sysId = sysType + sysNo;
			}
		 }
		 else if (sysType == ACS_CS_Protocol::SysType_AP)
		 {
			if (sysNoAtt.getValue(reinterpret_cast<char *>(&sysNo), sizeof(sysNo)) > 0)
			{
			   sysId = sysType + sysNo;
			}
		 }
	  }
	}

	return sysId;
}

//uint16_t ACS_CS_Common_Util::getSysId (const ACS_CS_ImCp *cp)
//{
//	uint16_t sysId = ACS_CS_NS::ILLEGAL_SYSID;
//
//	if (cp == 0)
//	  return ACS_CS_NS::ILLEGAL_SYSID;
//
//	if(cp->defaultName.size() < 3){
//		return ACS_CS_NS::ILLEGAL_SYSID;
//	}
//
//	string cpType = cp->defaultName.substr(0,2);
//	int id = atoi(cp->defaultName.substr(2).c_str());
//
//	if(id < 1 || id > 64){
//		return ACS_CS_NS::ILLEGAL_SYSID;
//	}
//
//	if(strcasecmp("BC", cpType.c_str())== 0){
//		sysId = ACS_CS_Protocol::SysType_BC + id;
//	}
//	else if(strcasecmp("CP", cpType.c_str())== 0){
//		sysId = ACS_CS_Protocol::SysType_CP + id;
//	}
//
//	return sysId;
//}

bool ACS_CS_Common_Util::getAddress(const ACS_CS_Attribute *attribute, uint32_t &address)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(address))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		address =  *(reinterpret_cast<uint32_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}

	return true;
}

bool ACS_CS_Common_Util::getMacAddress(const ACS_CS_Attribute *attribute, char ** mac, int length)
{
	if (!attribute || !mac)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	if (length == attribute->getValueLength())
	{
		attribute->getValue(*mac, length);
	}
	else
	{
		return false;
	}

	return true;
}



bool ACS_CS_Common_Util::getDhcpMethod(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_DHCP_Method_Identifier &method)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	uint16_t tempvalue = 0;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(tempvalue))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		tempvalue =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}


	if (ACS_CS_Protocol::CS_ProtocolChecker::checkDHCP(tempvalue))
	{
		method = static_cast<ACS_CS_Protocol::CS_DHCP_Method_Identifier>(tempvalue);
		return true;
	}
	else
	{
		return false;
	}
}

bool ACS_CS_Common_Util::getSlot(const ACS_CS_Attribute *attribute, uint16_t &slot)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(slot))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		slot =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}

	return true;
}

bool ACS_CS_Common_Util::getSide(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_Side_Identifier &side)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	uint16_t tempvalue = 0;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(tempvalue))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		tempvalue =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}


	if (ACS_CS_Protocol::CS_ProtocolChecker::checkSide(tempvalue))
	{
		side = static_cast<ACS_CS_Protocol::CS_Side_Identifier>(tempvalue);
		return true;
	}
	else
	{
		return false;
	}
}

bool ACS_CS_Common_Util::getNetwork(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_Network_Identifier &network)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	uint16_t tempvalue = 0;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(tempvalue))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		tempvalue =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}


	if ( (tempvalue == ACS_CS_Protocol::Eth_A)
		|| (tempvalue == ACS_CS_Protocol::Eth_B) )
	{
		network = static_cast<ACS_CS_Protocol::CS_Network_Identifier>(tempvalue);
		return true;
	}
	else
	{
		return false;
	}
}


bool ACS_CS_Common_Util::getFBN(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_FBN_Identifier &fbn)
{

	if (!attribute)
	{
		return false;
	}
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
	{
		return false;
	}

	uint16_t tempvalue = 0;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(tempvalue))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		tempvalue =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{

		return false;
	}

//	ACS_CS_Protocol::CS_ProtocolChecker::checkFBN(tempvalue);

	printf("ACS_CS_Protocol::CS_ProtocolChecker::checkFBN %s %d tempvalue=%d\n",__FUNCTION__,__LINE__,tempvalue);

	if (ACS_CS_Protocol::CS_ProtocolChecker::checkFBN(tempvalue))
	{
		fbn = static_cast<ACS_CS_Protocol::CS_FBN_Identifier>(tempvalue);

		return true;
	}
	else
	{
		return false;
	}

	return true;
}

bool ACS_CS_Common_Util::getSysType(const ACS_CS_Attribute *attribute, ACS_CS_Protocol::CS_SystemType_Identifier &sysType)
{
	if (!attribute)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	uint16_t tempvalue = 0;

	int bufferLength = attribute->getValueLength();

	if (bufferLength == sizeof(tempvalue))
	{
		char * buffer = new char[bufferLength];

		attribute->getValue(buffer, bufferLength);
		tempvalue =  *(reinterpret_cast<uint16_t*>(buffer));

		delete [] buffer;
	}
	else
	{
		return false;
	}


	if (ACS_CS_Protocol::CS_ProtocolChecker::checkSysType(tempvalue))
	{
		sysType = static_cast<ACS_CS_Protocol::CS_SystemType_Identifier>(tempvalue);
		return true;
	}
	else
	{
		return false;
	}
}

bool ACS_CS_Common_Util::getBladeProductNumber(const ACS_CS_Attribute *attribute, char ** bladePrNu)
{
	if (!attribute || !bladePrNu)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	attribute->getValue(*bladePrNu, attribute->getValueLength());

	return true;
}


bool ACS_CS_Common_Util::getCurrentLoadModuleVersion(const ACS_CS_Attribute *attribute, char ** loadmod)
{
	if (!attribute || !loadmod)
		return false;
	else if (attribute->getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified)
		return false;

	attribute->getValue(*loadmod, attribute->getValueLength());

	return true;
}

bool ACS_CS_Common_Util::isIpAddress(const std::string ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}
int ACS_CS_Common_Util::GetHWType()
{
	ACS_APGCC_CommonLib apgccLibObj;
	ACS_APGCC_HWINFO hwInfo;
	ACS_APGCC_HWINFO_RESULT hwInfoRes;
	int hwVariant = ACS_CS_HWTYPE_NOVALUE;

	apgccLibObj.GetHwInfo( &hwInfo, &hwInfoRes, ACS_APGCC_GET_HWTYPE );

		if(hwInfoRes.hwTypeResult == ACS_APGCC_HWINFO_SUCCESS){
			switch(hwInfo.hwType){
				case ACS_APGCC_HWTYPE_VM:
					hwVariant = ACS_CS_HWTYPE_VM;
					break;
				case ACS_APGCC_HWTYPE_GEP1:
					hwVariant = ACS_CS_HWTYPE_GEP1;
					break;
				case ACS_APGCC_HWTYPE_GEP2:
					hwVariant = ACS_CS_HWTYPE_GEP2;
					break;
				case ACS_APGCC_HWTYPE_GEP5:
					hwVariant = ACS_CS_HWTYPE_GEP5;
					break;
				case ACS_APGCC_HWTYPE_GEP5_400:
					hwVariant = ACS_CS_HWTYPE_GEP5_400;
					break;
				case ACS_APGCC_HWTYPE_GEP5_1200:
					hwVariant = ACS_CS_HWTYPE_GEP5_1200;
					break;
				case ACS_APGCC_HWTYPE_GEP5_64_1200:
					hwVariant = ACS_CS_HWTYPE_GEP5_64_1200;
					break;
				case ACS_APGCC_HWTYPE_GEP7L_400:
					hwVariant = ACS_CS_HWTYPE_GEP7L_400;
					break;
				case ACS_APGCC_HWTYPE_GEP7L_1600:
					hwVariant = ACS_CS_HWTYPE_GEP7L_1600;
					break;
				case ACS_APGCC_HWTYPE_GEP7_128_1600:
					hwVariant = ACS_CS_HWTYPE_GEP7_128_1600;
					break;
				default:
					break;
			}
		}
		else
		{
			hwVariant = ACS_CS_HWTYPE_NOVALUE;
		}
		return hwVariant;
}
