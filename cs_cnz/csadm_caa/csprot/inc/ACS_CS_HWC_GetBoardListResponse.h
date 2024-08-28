

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_HWC_GetBoardListResponse_h
#define ACS_CS_HWC_GetBoardListResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_HWC_GetBoardListResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45C707F402FE
{

  public:
      ACS_CS_HWC_GetBoardListResponse();

      ACS_CS_HWC_GetBoardListResponse(const ACS_CS_HWC_GetBoardListResponse &right);

      virtual ~ACS_CS_HWC_GetBoardListResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int getBuffer (char *buffer, int size) const;

      int setBuffer (const char *buffer, int size);

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      int getResponseCount () const;

      int getResponseArray (unsigned short *array, int size) const;

      void setResultCode (ACS_CS_Protocol::CS_Result_Code result);

      unsigned short getLogicalClock () const;

      void setLogicalClock (unsigned short clockValue);

      int setResponseArray (const unsigned short *array, int size);

      unsigned short getResponse (int index) const;

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_HWC_GetBoardListResponse & operator=(const ACS_CS_HWC_GetBoardListResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned logicalClock;

       short unsigned responseCount;

       short unsigned *boardList;

       short unsigned length;

    // Additional Implementation Declarations

};


// Class ACS_CS_HWC_GetBoardListResponse 



#endif
