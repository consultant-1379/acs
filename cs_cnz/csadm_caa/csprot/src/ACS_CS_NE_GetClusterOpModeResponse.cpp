




#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"

// ACS_CS_NE_GetClusterOpModeResponse
#include "ACS_CS_NE_GetClusterOpModeResponse.h"

ACS_CS_Trace_TDEF(ACS_CS_NE_GetClusterOpModeResponse_TRACE);

using namespace ACS_CS_Protocol;


// Class ACS_CS_NE_GetClusterOpModeResponse 

ACS_CS_NE_GetClusterOpModeResponse::ACS_CS_NE_GetClusterOpModeResponse()
{
    this->clusterOpMode = 0;
}

ACS_CS_NE_GetClusterOpModeResponse::ACS_CS_NE_GetClusterOpModeResponse(const ACS_CS_NE_GetClusterOpModeResponse &right)
: ACS_CS_BasicResponse(right)
{
    this->clusterOpMode = right.clusterOpMode;
}


ACS_CS_NE_GetClusterOpModeResponse::~ACS_CS_NE_GetClusterOpModeResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetClusterOpModeResponse::getPrimitiveType () const
{
     return ACS_CS_Protocol::Primitive_GetClusterOpModeResponse;
}

 ACS_CS_Primitive * ACS_CS_NE_GetClusterOpModeResponse::clone () const
{
     return new ACS_CS_NE_GetClusterOpModeResponse(*this);
}

 int ACS_CS_NE_GetClusterOpModeResponse::setBuffer (const char *buffer, int size)
{
    if(buffer == 0) // Check for valid memory
    {
        ACS_CS_TRACE((ACS_CS_NE_GetClusterOpModeResponse_TRACE,
            "(%t) ACS_CS_NE_GetClusterOpModeResponse::setBuffer()\n"
            "Error: NULL pointer\n"));
        return -1;
    }
    else if ( (size != 2) && (size != 6) )  // Check for valid size
    {
      // Every PDU consists of 12 bytes of Header and X bytes of 
      // body. The body of getClusterOpModeResponse primitive consists of 
      // the result code (2 bytes) and in case of success, an additional
      // 4 bytes for the value of Cluster Operation Mode
        ACS_CS_TRACE((ACS_CS_NE_GetClusterOpModeResponse_TRACE,
            "(%t) ACS_CS_NE_GetClusterOpModeResponse::setBuffer()\n"
            "Error: Invalid size, size = %d\n",
            size ));
        return -1;
    }
    
    // Get and convert result code
    unsigned short tempShort = 0;
    unsigned int tempUint = 0;
    tempShort = *( reinterpret_cast<const unsigned short *>(buffer) );
    tempShort = ntohs(tempShort);
    
    if (CS_ProtocolChecker::checkResultCode(tempShort))
        this->resultCode = static_cast<CS_Result_Code>(tempShort);
    else
    {
        // Some error code
        ACS_CS_AttributeException ex;
        ex.setDescription("Error casting result code");
        throw ex;
    }

    // Get and convert cluster op mode
    if(size == 6) { // Check for valid size, result code (2 bytes) + cluster op mode (4 bytes)
        tempUint = 0;
        tempUint = *( reinterpret_cast<const unsigned int *>(buffer + 2) );
        this->clusterOpMode = ntohl(tempUint);
    }
    return 0;
}

 int ACS_CS_NE_GetClusterOpModeResponse::getBuffer (char *buffer, int size) const
{
     if(buffer == 0)  // Check for valid memory
     {
        ACS_CS_TRACE((ACS_CS_NE_GetClusterOpModeResponse_TRACE,
           "(%t) %s, %i\n"
           "Error: NULL pointer\n", __FUNCTION__, __LINE__));
        return -1;
     }

     if(size < this->getLength()) // Check for valid size
     {
        ACS_CS_TRACE((ACS_CS_NE_GetClusterOpModeResponse_TRACE,
           "(%t) %s, %i\n"
           "Error: Invalid size, size = %d\n", __FUNCTION__, __LINE__,size));
        return -1;
     }

     unsigned short rcode = static_cast<unsigned short>(this->resultCode);

     unsigned short tempShort = htons(rcode);
     memcpy(buffer, &tempShort, sizeof(tempShort));

     // buffer will be large enough to hold Cluster Operation Mode 
     // only when the result code is CS_Result_Code::Result_Success.
     if(rcode == ACS_CS_Protocol::Result_Success)
     {
        unsigned tempMode = htonl(this->clusterOpMode);
        memcpy(buffer + sizeof(tempShort), &(tempMode), sizeof(tempMode));
     }

     return 0;
}

 short unsigned ACS_CS_NE_GetClusterOpModeResponse::getLength () const
{
     if(resultCode == ACS_CS_Protocol::Result_Success)
     {
        return( sizeof(unsigned short) + sizeof(unsigned));   // Result code + 2 bytes + length of unsigned
     }
     else
     {
        return 2;   // Only result code
     }
}

 unsigned ACS_CS_NE_GetClusterOpModeResponse::getClusterOpMode () const
{
     return this->clusterOpMode;
}

 void ACS_CS_NE_GetClusterOpModeResponse::setClusterOpMode (unsigned newClusterOpMode)
{
    this->clusterOpMode = newClusterOpMode;
}

// Additional Declarations

