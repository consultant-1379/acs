

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_CPId_GetCPListResponse_h
#define ACS_CS_CPId_GetCPListResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_CPId_GetCPListResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C86C4E03AA
{

   public:
      ACS_CS_CPId_GetCPListResponse();

      ACS_CS_CPId_GetCPListResponse(const ACS_CS_CPId_GetCPListResponse &right);

      virtual ~ACS_CS_CPId_GetCPListResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clock);

      unsigned short getResponseCount () const;

      int getCPList (unsigned short *buffer, unsigned short size) const;

      void setCPList (const unsigned short *buffer, unsigned short size);

      // Additional Public Declarations

   protected:
      // Additional Protected Declarations

   private:
      ACS_CS_CPId_GetCPListResponse & operator=(const ACS_CS_CPId_GetCPListResponse &right);

      // Additional Private Declarations

   private: //## implementation
      // Data Members for Class Attributes

      short unsigned logicalClock;

      short unsigned responseCount;

      short unsigned *cpList;

      // Additional Implementation Declarations

};


// Class ACS_CS_CPId_GetCPListResponse 



#endif
