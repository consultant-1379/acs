

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_GLK_GetGlobalLogicalClockResponse_h
#define ACS_CS_GLK_GetGlobalLogicalClockResponse_h 1



#include "ACS_CS_Util.h"
#include <vector>


// ACS_CS_BasicResponse
#include "ACS_CS_BasicResponse.h"


// Struct to keep track of a GLC
struct ACS_CS_GLC_DATA
{
   uint16_t attrScopeIdentifier;
   uint16_t attrLogicalClock;
};







class ACS_CS_GLK_GetGlobalLogicalClockResponse : public ACS_CS_BasicResponse  //## Inherits: <unnamed>%461B3FF10332
{

  public:
      ACS_CS_GLK_GetGlobalLogicalClockResponse();

      ACS_CS_GLK_GetGlobalLogicalClockResponse(const ACS_CS_GLK_GetGlobalLogicalClockResponse &right);

      virtual ~ACS_CS_GLK_GetGlobalLogicalClockResponse();


      ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      int setBuffer (const char *buffer, int size);

      int getBuffer (char *buffer, int size) const;

      unsigned short getLength () const;

      ACS_CS_Primitive * clone () const;

      void getGLCVector ( std::vector<ACS_CS_GLC_DATA> &glcVector) ;

      void setGLCVector (std::vector<ACS_CS_GLC_DATA> glcVector);


    // Additional Public Declarations
 void printGLCVector (std::vector<ACS_CS_GLC_DATA> &glcVector);
 
 void copyGLCVector ( std::vector<ACS_CS_GLC_DATA> &src_glcVector,  std::vector<ACS_CS_GLC_DATA> &dest_glcVector) ;

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_GLK_GetGlobalLogicalClockResponse & operator=(const ACS_CS_GLK_GetGlobalLogicalClockResponse &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       std::vector<ACS_CS_GLC_DATA> glcData;

       short unsigned length;

       
    // Additional Implementation Declarations

};





#endif
