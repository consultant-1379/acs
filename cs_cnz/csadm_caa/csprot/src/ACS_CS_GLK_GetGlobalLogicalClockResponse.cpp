

//	Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Util.h"


// ACS_CS_GLK_GetGlobalLogicalClockResponse
#include "ACS_CS_GLK_GetGlobalLogicalClockResponse.h"


ACS_CS_Trace_TDEF(ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE);

using namespace ACS_CS_Protocol;
using std::vector;



// Class ACS_CS_GLK_GetGlobalLogicalClockResponse 

ACS_CS_GLK_GetGlobalLogicalClockResponse::ACS_CS_GLK_GetGlobalLogicalClockResponse()
: length(0)
{
}

ACS_CS_GLK_GetGlobalLogicalClockResponse::ACS_CS_GLK_GetGlobalLogicalClockResponse(const ACS_CS_GLK_GetGlobalLogicalClockResponse &right)
: ACS_CS_BasicResponse(right)
{

	this->length = right.length;
	//printf(" ACS_CS_VLAN_GetGlobalLogicalClockResponse::ACS_CS_GLK_GetGlobalLogicalClockResponse() length = %d size = %u \n",right.length,right.glcData.size());
	//printGLCVector(const_cast<std::vector<ACS_CS_GLC_DATA> &>(right.glcData));
	copyGLCVector(const_cast<std::vector<ACS_CS_GLC_DATA> &>(right.glcData), this->glcData);
	this->glcData = right.glcData;

}


ACS_CS_GLK_GetGlobalLogicalClockResponse::~ACS_CS_GLK_GetGlobalLogicalClockResponse()
{
}



ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_GLK_GetGlobalLogicalClockResponse::getPrimitiveType () const
{

	return ACS_CS_Protocol::Primitive_GetGlobalLogicalClockResponse;

}

int ACS_CS_GLK_GetGlobalLogicalClockResponse::setBuffer (const char *buffer, int size)
{

	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::setBuffer()\n"
				"Error: NULL pointer\n" ));

		return -1;
	}
	else if ( size <= 4 )	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::setBuffer()\n"
				"Error: Invalid size, size = %d\n",
				size ));

		return -1;
	}

	// Clear current vector
	this->glcData.clear();
	this->length = 2;

	// Get and convert result code
	unsigned short tempShort = 0;
	tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
	tempShort = ntohs(tempShort);

	if (CS_ProtocolChecker::checkResultCode(tempShort))
		this->resultCode = static_cast<CS_Result_Code>(tempShort);
	else
	{
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::setBuffer()\n"
				"Error: Illegal result code , result= %d\n",
				tempShort ));

		// Some error code
		ACS_CS_AttributeException ex;
		ex.setDescription("Error casting result code");
		throw ex;
	}

	if ( ((size > 4) && (this->resultCode == ACS_CS_Protocol::Result_Success)) )
	{
		unsigned short responseCount = 0;
		unsigned short offset = 4;


		// Get and convert response count
		tempShort = 0;
		tempShort = *( reinterpret_cast<const unsigned short *>(buffer + 2) );
		responseCount = ntohs(tempShort);
		this->length += 2;

		for (int i = 0; i < responseCount; i++)
		{

			//check that there is enough space for scope and clock
			if(size < 12)
			{
				// Clear current vector
				this->glcData.clear();
				this->length = 2;

				ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
						"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::setBuffer()\n"
						"Error: Illegal buffer size , size= %d\n",
						size ));

				ACS_CS_ParsingException ex;
				ex.setDescription("Illegal buffer size");
				throw ex;
			}

			ACS_CS_GLC_DATA tempData;
			memset(&tempData, 0, sizeof(tempData));


			memcpy(&tempShort, buffer + offset, sizeof(tempShort));
			tempData.attrScopeIdentifier = ntohs(tempShort);
			offset += sizeof(tempShort);

			memcpy(&tempShort, buffer + offset, sizeof(tempShort));
			tempData.attrLogicalClock = ntohs(tempShort);
			offset += sizeof(tempShort);



			// Store GLC data
			this->glcData.push_back(tempData);
			this->length += 4;
		}
	}
	ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::setBuffer()\n"
			"setting  length = %d size = %d\n",this->length,size));
	//  printf(" ACS_CS_VLAN_GetGlobalLogicalClockResponse::setBuffer() length = %d size = %d vector size=%d\n",this->length,size,glcData.size());

	//printGLCVector(glcData);

	return 0;

}

int ACS_CS_GLK_GetGlobalLogicalClockResponse::getBuffer (char *buffer, int size) const
{
	ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::getBuffer() \n"
			"size = %d length = %d\n",size, this->length));
	if (buffer == 0)	// Check for valid memory
	{
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::getBuffer()\n"
				"Error: NULL pointer\n" ));

		return -1;
	}
	else if (size != this->getLength())	// Check for valid size
	{
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_GLK_GetGlobalLogicalClockResponse::getBuffer()\n"
				"Error: Invalid size, size = %d\n",
				size ));

		return -1;
	}

	unsigned short tempShort = 0;
	unsigned short offset = 0;

	// Convert and copy result code
	tempShort = htons(this->resultCode);
	memcpy(buffer + offset, &tempShort, sizeof(tempShort));
	offset += sizeof(tempShort);

	if (this->getLength() > 2)
	{
		// Convert and copy response count
		tempShort = htons(static_cast<unsigned short>(this->glcData.size()));
		memcpy(buffer + offset, &tempShort, sizeof(tempShort));
		offset += sizeof(tempShort);

		vector<ACS_CS_GLC_DATA> tempVector = this->glcData;
		vector<ACS_CS_GLC_DATA>::iterator it;

		for (it = tempVector.begin(); it != tempVector.end(); ++it)
		{
			ACS_CS_GLC_DATA tempData = *it;

			if (CS_ProtocolChecker::checkScope(tempData.attrScopeIdentifier))
			{
				tempShort = htons(tempData.attrScopeIdentifier);
				memcpy(buffer + offset, &tempShort, sizeof(tempShort));
				offset += sizeof(tempShort);
			}
			else
			{
				ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
						"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::getBuffer()\n"
						"Error: Wrong scope, scope = %d\n",
						tempData.attrScopeIdentifier));

				ACS_CS_ParsingException ex;
				ex.setDescription("Not a valid scope");
				throw ex;
			}

			// Convert and copy gclk
			tempShort = htons(tempData.attrLogicalClock);
			memcpy(buffer + offset, &tempShort, sizeof(tempShort));
			offset += sizeof(tempShort);
		}
	}

	ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
			"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::getBuffer()\n"
			"setting  length = %d size = %d\n",this->length,size));
	//printf(" ACS_CS_VLAN_GetGlobalLogicalClockResponse::getBuffer() setting length = %d size = %d\n",this->length,size);
	return 0;

}

short unsigned ACS_CS_GLK_GetGlobalLogicalClockResponse::getLength () const
{

	if (resultCode == ACS_CS_Protocol::Result_Success)
		return this->length;    // Result code + content
	else
		return 2;               // Only result code

}

ACS_CS_Primitive * ACS_CS_GLK_GetGlobalLogicalClockResponse::clone () const
{

	return new ACS_CS_GLK_GetGlobalLogicalClockResponse(*this);

}

void ACS_CS_GLK_GetGlobalLogicalClockResponse::getGLCVector ( std::vector<ACS_CS_GLC_DATA> &dest_glcVector)
{

	copyGLCVector(this->glcData,dest_glcVector);

}

void ACS_CS_GLK_GetGlobalLogicalClockResponse::setGLCVector (std::vector<ACS_CS_GLC_DATA> glcVector)
{

	this->glcData.clear();
	this->length = 4;

	vector<ACS_CS_GLC_DATA>::iterator it;

	for (it = glcVector.begin(); it != glcVector.end(); ++it)
	{
		ACS_CS_GLC_DATA tempData = *it;
		length += 4;

		this->glcData.push_back(*it);
		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::setGLCVector()\n"
				"length = %d\n",length));
	}

}



void ACS_CS_GLK_GetGlobalLogicalClockResponse::printGLCVector (std::vector<ACS_CS_GLC_DATA> &glcVector)
{

	vector<ACS_CS_GLC_DATA>::iterator it;

	for (it = glcVector.begin(); it != glcVector.end(); ++it)
	{
		ACS_CS_GLC_DATA tempData = *it;

		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::printGLCVector()\n"
				"attrScopeIdentifier = %d   attrLogicalClock = %d  \n",tempData.attrScopeIdentifier,tempData.attrLogicalClock));

		//printf("ACS_CS_VLAN_GetGlobalLogicalClockResponse::printGLCVector() attrScopeIdentifier = %d   attrLogicalClock = %d \n",tempData.attrScopeIdentifier,tempData.attrLogicalClock);
	}

}


void ACS_CS_GLK_GetGlobalLogicalClockResponse::copyGLCVector ( std::vector<ACS_CS_GLC_DATA> &src_glcVector,  std::vector<ACS_CS_GLC_DATA> &dest_glcVector) 
{

	dest_glcVector.clear();

	vector<ACS_CS_GLC_DATA>::iterator it;

	for (it = src_glcVector.begin(); it != src_glcVector.end();++it)
	{
		ACS_CS_GLC_DATA tempData = *it;

		ACS_CS_TRACE((ACS_CS_GLK_GetGlobalLogicalClockResponse_TRACE,
				"(%t) ACS_CS_VLAN_GetGlobalLogicalClockResponse::copyGLCVector()\n"
				"attrScopeIdentifier = %d   attrLogicalClock = %d  \n",tempData.attrScopeIdentifier,tempData.attrLogicalClock));
		dest_glcVector.push_back(*it);

		// printf("ACS_CS_VLAN_GetGlobalLogicalClockResponse::copyGLCVector() attrScopeIdentifier = %d   attrLogicalClock = %d \n",tempData.attrScopeIdentifier,tempData.attrLogicalClock);
	}

}

// Additional Declarations

