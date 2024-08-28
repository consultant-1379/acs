

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"


// ACS_CS_VLAN_GetNetworkTableResponse
#include "ACS_CS_VLAN_GetNetworkTableResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_VLAN_GetNetworkTableResponse_TRACE);

using namespace ACS_CS_Protocol;
using std::vector;



// Class ACS_CS_VLAN_GetNetworkTableResponse

ACS_CS_VLAN_GetNetworkTableResponse::ACS_CS_VLAN_GetNetworkTableResponse()
: length(0),
  logicalClock(0),
  tableVersion(0),
  cpVersion(0),
  boardId(0)
{

}

ACS_CS_VLAN_GetNetworkTableResponse::ACS_CS_VLAN_GetNetworkTableResponse(const ACS_CS_VLAN_GetNetworkTableResponse &right)
: ACS_CS_BasicResponse(right)
{

	length = right.length;
	logicalClock = right.logicalClock;
	tableVersion = right.tableVersion;
	boardId = right.boardId;
	vlanData = right.vlanData;
	//GEP2_GEP5 to GEP7 HWSWAP added cpVersion
    cpVersion = right.cpVersion;

}


ACS_CS_VLAN_GetNetworkTableResponse::~ACS_CS_VLAN_GetNetworkTableResponse()
{
}



ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_VLAN_GetNetworkTableResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetNetworkTableResponse;

}

int ACS_CS_VLAN_GetNetworkTableResponse::setBuffer (const char *buffer, int size)
{


	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
				"Error: NULL pointer\n" ));

		return -1;
	}
	else if(! ( (size == 2) || (size == 10) || (size >= 22)))	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
				"Error: Invalid size, size = %d\n",
				size ));

		return -1;
	}

	// Clear current vector
	this->vlanData.clear();
	this->length = 0;

	// Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
	tempShort = ntohs(tempShort);
	this->length += sizeof(tempShort);

	if (CS_ProtocolChecker::checkResultCode(tempShort))
		this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
				"Error: Illegal result code , result= %d\n",
				tempShort ));

		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

	if ( ((size == 10) && (this->resultCode == ACS_CS_Protocol::Result_Success)) ||
			((size >= 22) && (this->resultCode == ACS_CS_Protocol::Result_Success)) )
	{
		unsigned short responseCount = 0;
		unsigned short currentNameLength = 0, currentNICNameLength = 0;

		// Get and convert logical clock
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
		this->logicalClock = ntohs(tempShort);
		this->length += sizeof(tempShort);

		//Get and convert table version
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
		this->tableVersion = ntohs(tempShort);
		this->length += sizeof(tempShort);

		//Get and convert boardId
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
		this->boardId = ntohs(tempShort);
		this->length += sizeof(tempShort);

		// Get and convert response count
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
		responseCount = ntohs(tempShort);
		this->length += sizeof(tempShort);

		int entrySize;
		if(this->tableVersion == ACS_CS_Protocol::VLANVersion_0)
		{
			entrySize = 12;
		}
		else if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1)
		{
			entrySize =16;
		}
		else
		{   // VLAN Version 2
			entrySize = 20;
		}
		for (int i = 0; i < responseCount; i++)
		{
			//printf(" ACS_CS_VLAN_GetNetworkTableResponse::setBuffer() printing entrySize=%d , responseCount=%d,  \n",entrySize,responseCount);

			// Check that the remaining space is enough
			if ( size < (  this->length + entrySize) )
			{
				// Clear current vector
				this->vlanData.clear();
				this->length = 2;

				ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
						"Error: Illegal buffer size , size= %d\n",
						size ));

				ACS_CS_ParsingException ex;
				ex.setDescription("Illegal buffer size");
				throw ex;
			}

			tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
			currentNameLength = ntohs(tempShort);
			this->length += sizeof(tempShort);
			int entrySizeWithoutNameLength = entrySize - sizeof(tempShort);

			// Check that there is space for the name
			if ( (size < (this->length + entrySizeWithoutNameLength + currentNameLength) ) ||
					(currentNameLength > ACS_CS_NS::MAX_VLAN_NAME_LENGTH) )
			{
				// Clear current vector
				this->vlanData.clear();
				this->length = 2;

				ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
						"Error: Illegal string size , size = %d, string length = %d\n",
						size, currentNameLength ));

				ACS_CS_ParsingException ex;
				ex.setDescription("Illegal string size");
				throw ex;
			}

			ACS_CS_VLAN_DATA tempData;
			memset(&tempData, 0, sizeof(tempData));

			// Copy VLAN name # i
			memcpy(tempData.vlanName, buffer + this->length, currentNameLength);
			tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
			this->length += currentNameLength;

			// Copy Network address # i
			uint32_t tempLong = 0;
			memcpy(&tempLong, buffer + this->length, sizeof(tempLong));
			tempData.vlanAddress = ntohl(tempLong);
			this->length += sizeof(tempLong);

			// Copy Netmask # i
			memcpy(&tempLong, buffer + this->length, sizeof(tempLong));
			tempData.vlanNetmask = ntohl(tempLong);
			this->length += sizeof(tempLong);

			// Copy VLAN protocol stack # i
			memcpy(&tempShort, buffer + this->length, sizeof(tempShort));
			tempData.vlanStack = ntohs(tempShort);
			this->length += sizeof(tempShort);

			if((this->tableVersion == ACS_CS_Protocol::VLANVersion_1) ||
						(this->tableVersion == ACS_CS_Protocol::VLANVersion_2))
			{

				// Copy VLAN pcp value
				memcpy(&tempShort, buffer + this->length, sizeof(tempShort));
				tempData.pcp = ntohs(tempShort);
				this->length += sizeof(tempShort);

				// Copy VLAN type value
				memcpy(&tempShort, buffer + this->length, sizeof(tempShort));
				tempData.vlanType = ntohs(tempShort);
				this->length += sizeof(tempShort);

				ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
						"setting pcp=%d and type=%d length=%d size=%d\n",tempData.pcp,tempData.vlanType,length,size));
				//printf(" ACS_CS_VLAN_GetNetworkTableResponse::setBuffer() setting pcp=%d and type=%d length=%d size=%d****\n",tempData.pcp,tempData.vlanType,length,size);

			}
			if (tempData.vlanType == ACS_CS_Protocol::Type_APZ)
			{
				if ( (tempData.vlanStack != ACS_CS_Protocol::Stack_KIP) &&
						(tempData.vlanStack != ACS_CS_Protocol::Stack_TIP) )

				{
					// Clear current vector
					this->vlanData.clear();
					this->length = 2;

					ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
							"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
							"Error: Illegal VLAN protocol stack , type = %d\n",
							tempData.vlanStack ));

					// Some error code
					ACS_CS_AttributeException ex;
					ex.setDescription("Error casting VLAN protocol stack");
					throw ex;
				}
			}

			//GEP2_GEP5 to GEP7 HW SWap. Copy vlanTag info.
			if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1 && this->cpVersion == ACS_CS_Protocol::Version_7)
			{

				   ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
							"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
							"Copy vlanTag CP VERSION = %d\n",this->cpVersion ));

					// Copy VLAN Tag value
					memcpy(&tempShort, buffer + this->length, sizeof(tempShort));
					tempData.vlanTag = ntohs(tempShort);
					this->length += sizeof(tempShort);
			}
 
			if(this->tableVersion == ACS_CS_Protocol::VLANVersion_2)
			{

				// Copy VLAN Tag value
				memcpy(&tempShort, buffer + this->length, sizeof(tempShort));
				tempData.vlanTag = ntohs(tempShort);
				this->length += sizeof(tempShort);

				tempShort = *( reinterpret_cast<const unsigned short *>(buffer + this->length) );
				currentNICNameLength = ntohs(tempShort);
				this->length += sizeof(tempShort);


				if (currentNICNameLength > ACS_CS_NS::MAX_VNIC_NAME_LENGTH)
				{
					// Clear current vector
					this->vlanData.clear();
					this->length = 2;

					ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
							"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBuffer()\n"
							"Error: Illegal vNIC Name string size , size = %d, string length = %d\n",
							size, currentNICNameLength ));

					ACS_CS_ParsingException ex;
					ex.setDescription("Illegal string size");
					throw ex;
				}

				// Copy VLAN vNIC Name value
				memcpy(tempData.vNICName, buffer + this->length, currentNICNameLength);
				tempData.vNICName[ACS_CS_NS::MAX_VNIC_NAME_LENGTH] = 0;
				this->length += currentNICNameLength;

			}

			// Store VLAN data
			this->vlanData.push_back(tempData);
		}
	}

	return 0;

}

int ACS_CS_VLAN_GetNetworkTableResponse::getBuffer (char *buffer, int size) const
{

	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBuffer()\n"
				"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBuffer()\n"
				"Error: Invalid size, size = %d\n",
				size ));

		return -1;
	}

	unsigned short tempShort = 0;
	unsigned short offset = 0;
	unsigned short currentNameLength = 0, currentNICNameLength = 0;

	// Convert and copy result code
	tempShort = htons(this->resultCode);
	memcpy(buffer + offset, &tempShort, sizeof(tempShort));
	offset += sizeof(tempShort);

	if (this->getLength() > 2)
	{
		// Convert and copy logical clock
		tempShort = htons(this->logicalClock);
		memcpy(buffer + offset, &tempShort, sizeof(tempShort));
		offset += sizeof(tempShort);

		//Convert and copy table version
		tempShort = htons(this->tableVersion);
		memcpy(buffer + offset, &tempShort, sizeof(tempShort));
		offset += sizeof(tempShort);

		//Convert and copy boardId
		tempShort = htons(this->boardId);
		memcpy(buffer + offset, &tempShort, sizeof(tempShort));
		offset += sizeof(tempShort);


		// Convert and copy response count
		tempShort = htons(static_cast<unsigned short>(this->vlanData.size()));
		memcpy(buffer + offset, &tempShort, sizeof(tempShort));
		offset += sizeof(tempShort);

		vector<ACS_CS_VLAN_DATA> tempVector = this->vlanData;
		vector<ACS_CS_VLAN_DATA>::iterator it;
		ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBuffer()\n"
				"vlandata size = %d\n",
				tempVector.size()));
		for (it = tempVector.begin(); it != tempVector.end(); ++it)
		{
			ACS_CS_VLAN_DATA tempData = *it;
			tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
			currentNameLength = static_cast<unsigned short>(strlen(tempData.vlanName));
			tempShort = htons(currentNameLength);;

			//Copy name length
			memcpy(buffer + offset, &tempShort, sizeof(tempShort));
			offset += sizeof(tempShort);

			//Copy VLAN name
			memcpy(buffer + offset, tempData.vlanName, currentNameLength);
			offset += currentNameLength;

			// Convert and copy network address
			uint32_t tempLong = htonl(tempData.vlanAddress);
			memcpy(buffer + offset, &tempLong, sizeof(tempLong));
			offset += sizeof(tempLong);

			// Convert and copy netmask
			tempLong = htonl(tempData.vlanNetmask);
			memcpy(buffer + offset, &tempLong, sizeof(tempLong));
			offset += sizeof(tempLong);

			// Convert and copy VLAN protocol stack
			tempShort = htons(tempData.vlanStack);
			memcpy(buffer + offset, &tempShort, sizeof(tempShort));
			offset += sizeof(tempShort);
			if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1 ||
				this->tableVersion == ACS_CS_Protocol::VLANVersion_2)
			{
				// Convert and copy VLAN pcp
				tempShort = htons(tempData.pcp);
				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);

				// Convert and copy VLAN Type
				tempShort = htons(tempData.vlanType);
				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);

				ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBuffer()\n"
						"setting pcp=%d and type=%d length=%d size=%d \n",tempData.pcp,tempData.vlanType,length,size));
				//printf(" ACS_CS_VLAN_GetNetworkTableResponse::getBuffer() getting pcp=%d and type=%d*length=%d size=%d ***\n",tempData.pcp,tempData.vlanType,length,size);

			}

			//GEP2_GEP5 to GEP7 HW SWap. Copy vlanTag info.
			if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1 && this->cpVersion == ACS_CS_Protocol::Version_7)
			{
			   ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBuffer()\n"
						"Copy vlanTag CP VERSION= %d\n",this->cpVersion ));

				// Convert and copy VLAN Tag
				tempShort = htons(tempData.vlanTag);
				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);
			}

			if(this->tableVersion == ACS_CS_Protocol::VLANVersion_2)
			{
				// Convert and copy VLAN Tag
				tempShort = htons(tempData.vlanTag);
				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);

				// Convert and copy vNIC Name Length
				tempData.vNICName[ACS_CS_NS::MAX_VNIC_NAME_LENGTH] = 0;
				currentNICNameLength = static_cast<unsigned short>(strlen(tempData.vNICName));
				tempShort = htons(currentNICNameLength);;

				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);

				//Copy vNIC Name
				memcpy(buffer + offset, tempData.vNICName, currentNICNameLength);
				offset += currentNICNameLength;

			}
		}
	}

	return 0;

}

short unsigned ACS_CS_VLAN_GetNetworkTableResponse::getLength () const
{

	if (resultCode == ACS_CS_Protocol::Result_Success)
		return this->length;    // Result code + content
	else
		return 2;               // Only result code

}

ACS_CS_Primitive * ACS_CS_VLAN_GetNetworkTableResponse::clone () const
{

	return new ACS_CS_VLAN_GetNetworkTableResponse(*this);

}

void ACS_CS_VLAN_GetNetworkTableResponse::getVLANVector (std::vector<ACS_CS_VLAN_DATA> &vlanVector) const
{

	vlanVector = this->vlanData;

}

void ACS_CS_VLAN_GetNetworkTableResponse::setVLANVector (std::vector<ACS_CS_VLAN_DATA> vlanVector)
{

	this->vlanData.clear();
	this->length = 10;

	vector<ACS_CS_VLAN_DATA>::iterator it;
	for (it = vlanVector.begin(); it != vlanVector.end(); ++it)
	{

		ACS_CS_VLAN_DATA tempData = *it;
		tempData.vlanName[ACS_CS_NS::MAX_VLAN_NAME_LENGTH] = 0;
		length += static_cast<unsigned short>(strlen(tempData.vlanName));
		length += 12;

		//GEP2_GEP5 to GEP7 HWSWAP
		if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1 && this->cpVersion == ACS_CS_Protocol::Version_7)
		{
			length += 6;
		}
		else if(this->tableVersion == ACS_CS_Protocol::VLANVersion_1)
		{
			length += 4;
		}
		else if(this->tableVersion == ACS_CS_Protocol::VLANVersion_2)
		{
			length += 8;
			tempData.vNICName[ACS_CS_NS::MAX_VNIC_NAME_LENGTH] = 0;
			length += static_cast<unsigned short>(strlen(tempData.vNICName));
		}
		this->vlanData.push_back(*it);
	}

}

short unsigned ACS_CS_VLAN_GetNetworkTableResponse::getLogicalClock () const
{

	return this->logicalClock;

}

void ACS_CS_VLAN_GetNetworkTableResponse::setLogicalClock (unsigned short clock)
{

	this->logicalClock = clock;

}



void ACS_CS_VLAN_GetNetworkTableResponse::setTableVersion(unsigned short vlanTabVersion)
{

	this->tableVersion = vlanTabVersion;

	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setTableVersion()\n"
			"setting vlan version=%d \n",this->tableVersion));

}
short unsigned ACS_CS_VLAN_GetNetworkTableResponse::getTableVersion()
{
	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getTableVersion()\n"
			"getting vlan version=%d \n",this->tableVersion));

	return this->tableVersion;
}

void ACS_CS_VLAN_GetNetworkTableResponse::setBoardId(unsigned short id)
{

	boardId = id;

	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setBoardId()\n"
			"setting board ID=%d \n",boardId));

}
short unsigned ACS_CS_VLAN_GetNetworkTableResponse::getBoardId()
{
	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getBoardId()\n"
			"getting board ID=%d \n",boardId));

	return boardId;
}

//GEP2_GEP5 to GEP7 HWSWAP
void ACS_CS_VLAN_GetNetworkTableResponse::setCPVersion(unsigned short cpVersion)
{

	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::setCPVersion()\n"
			"setting cp version=%d \n",this->cpVersion));

	this->cpVersion = cpVersion;
}

//GEP2_GEP5 to GEP7 HWSWAP
short unsigned ACS_CS_VLAN_GetNetworkTableResponse::getCPVersion()
{
	ACS_CS_TRACE((ACS_CS_VLAN_GetNetworkTableResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetNetworkTableResponse::getCPVersion()\n"
			"getting cp version=%d \n",this->cpVersion));

	return this->cpVersion;
}

// Additional Declarations

