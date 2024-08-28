

//	*********************************************************
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************



#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_Trace.h"
#include "ACS_CS_Protocol.h"
#include "ACS_CS_API.h"
#include "ACS_CS_HostNetworkConverter.h"

// ACS_CS_NE_GetOmProfileResponse
#include "ACS_CS_NE_GetOmProfileResponse.h"

ACS_CS_Trace_TDEF(ACS_CS_NE_GetOmProfileResponse_TRACE);


// Class ACS_CS_NE_GetOmProfileResponse 

ACS_CS_NE_GetOmProfileResponse::ACS_CS_NE_GetOmProfileResponse()
{
   m_OmProfileChange.omProfileCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_OmProfileChange.omProfileRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_OmProfileChange.aptCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_OmProfileChange.aptRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_OmProfileChange.apzCurrent = ACS_CS_API_OmProfileChange::BladeProfile;
   m_OmProfileChange.apzRequested = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_OmProfileChange.phase = ACS_CS_API_OmProfilePhase::Idle;
   m_OmProfileChange.changeReason = ACS_CS_API_OmProfileChange::NoChange;
   m_OmProfileChange.aptQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
   m_OmProfileChange.apzQueued = ACS_CS_API_OmProfileChange::UnspecifiedProfile;
}

ACS_CS_NE_GetOmProfileResponse::ACS_CS_NE_GetOmProfileResponse(const ACS_CS_NE_GetOmProfileResponse &right)
   : ACS_CS_BasicResponse(right),
   m_OmProfileChange(right.m_OmProfileChange)
{
}


ACS_CS_NE_GetOmProfileResponse::~ACS_CS_NE_GetOmProfileResponse()
{
}



 ACS_CS_Protocol::CS_Primitive_Identifier ACS_CS_NE_GetOmProfileResponse::getPrimitiveType () const
{
   return ACS_CS_Protocol::Primitive_GetOmProfileResponse;
}

 ACS_CS_Primitive * ACS_CS_NE_GetOmProfileResponse::clone () const
{
   return new ACS_CS_NE_GetOmProfileResponse(*this);
}

 int ACS_CS_NE_GetOmProfileResponse::setBuffer (const char *buffer, int /*size*/)
{
   if(buffer == NULL)
   {
      ACS_CS_TRACE((ACS_CS_NE_GetOmProfileResponse_TRACE,
                    "(%t) ACS_CS_NE_GetOmProfileResponse::setBuffer()\n"
                    "Error: NULL pointer\n"));
      return -1;
   }

   // Get and convert result
   const unsigned short rc = ntohs(*( reinterpret_cast<const unsigned short *>(buffer)));

   if( ACS_CS_Protocol::CS_ProtocolChecker::checkResultCode(rc))
   {
      this->resultCode = static_cast<ACS_CS_Protocol::CS_Result_Code>(rc);
   }
   else
   {
      // Some error code
      ACS_CS_AttributeException ex;
      ex.setDescription("Error casting result code");
      throw ex;
   }

   // Extract profile data from buffer only if resultCode is succesful
   if(this->resultCode == ACS_CS_Protocol::Result_Success)
   {
      //char * poi = const_cast<char *>(buffer+ sizeof(unsigned short)); // temp buffer pointer for extraction
	   const char * poi = (buffer+ sizeof(unsigned short)); // temp buffer pointer for extraction

      return ACS_CS_HostNetworkConverter::deserialize(poi, m_OmProfileChange);
   }

   return 0;

}

 int ACS_CS_NE_GetOmProfileResponse::getBuffer (char *buffer, int size) const
{

   if(buffer == NULL)
   {
      ACS_CS_TRACE((ACS_CS_NE_GetOmProfileResponse_TRACE,
                    "(%t) __FUNCTION__ \n",
                    "Error: NULL pointer\n"));
      return -1;
   }

   if(size < this->getLength())
   {
      ACS_CS_TRACE((ACS_CS_NE_GetOmProfileResponse_TRACE,
                    "(%t) __FUNCTION__ \n",
                    "Error: INVALID size, size = %d (should be: %d)\n",
                    size,
                    this->getLength()));

      return -1;
   }

   // Convert and copy result code
   unsigned short * const poi = reinterpret_cast<unsigned short *>(buffer);
   *poi = htons(this->resultCode);

   // copy profile into buffer only if result is success
   if(this->resultCode == ACS_CS_Protocol::Result_Success)
   {
      char * poi = const_cast<char *>(buffer+ sizeof(unsigned short)); // temp buffer pointer for extraction
  
      return ACS_CS_HostNetworkConverter::serialize(poi, size, m_OmProfileChange);
   }

   return 0;

}

 short unsigned ACS_CS_NE_GetOmProfileResponse::getLength () const
{
   if( resultCode == ACS_CS_Protocol::Result_Success )
   {
      return( sizeof(unsigned short) + sizeof(ACS_CS_API_OmProfileChange));
   }
   else
   {
      return sizeof(unsigned short); // Only result code
   }
}

 void ACS_CS_NE_GetOmProfileResponse::getOmProfile (ACS_CS_API_OmProfileChange& omProfileChange) const
{
   omProfileChange =  m_OmProfileChange;
}

 void ACS_CS_NE_GetOmProfileResponse::setOmProfile (const ACS_CS_API_OmProfileChange& omProfileChange)
{
   m_OmProfileChange = omProfileChange;
}

// Additional Declarations

