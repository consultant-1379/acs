



#ifndef ACS_CS_NE_GetClusterOpModeResponse_h
#define ACS_CS_NE_GetClusterOpModeResponse_h 1



// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"







class ACS_CS_NE_GetClusterOpModeResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%4A806CA50283
{

  public:
      ACS_CS_NE_GetClusterOpModeResponse();

      virtual ~ACS_CS_NE_GetClusterOpModeResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      ACS_CS_Primitive * clone () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      unsigned getClusterOpMode () const;

      void setClusterOpMode (unsigned newClusterOpMode);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_NE_GetClusterOpModeResponse(const ACS_CS_NE_GetClusterOpModeResponse &right);

      ACS_CS_NE_GetClusterOpModeResponse & operator=(const ACS_CS_NE_GetClusterOpModeResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       unsigned clusterOpMode;

    // Additional Implementation Declarations

};


// Class ACS_CS_NE_GetClusterOpModeResponse 



#endif
