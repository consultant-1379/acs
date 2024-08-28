

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_NE_GetAlarmMasterResponse_h
#define ACS_CS_NE_GetAlarmMasterResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetAlarmMasterResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%45CC3AB0000C
{

  public:
      ACS_CS_NE_GetAlarmMasterResponse();

      ACS_CS_NE_GetAlarmMasterResponse(const ACS_CS_NE_GetAlarmMasterResponse &right);

      virtual ~ACS_CS_NE_GetAlarmMasterResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      unsigned short getAlarmMaster () const;

      void setAlarmMaster (unsigned short identifier);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetAlarmMasterResponse & operator=(const ACS_CS_NE_GetAlarmMasterResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       short unsigned alarmMaster;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetAlarmMasterResponse 



#endif
