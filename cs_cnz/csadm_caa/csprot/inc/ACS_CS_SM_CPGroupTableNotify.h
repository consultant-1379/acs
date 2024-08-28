

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


#ifndef ACS_CS_SM_CPGroupTableNotify_h
#define ACS_CS_SM_CPGroupTableNotify_h 1

class ACS_CS_HostNetworkConverter;

#include "ACS_CS_API.h"

// ACS_CS_SM_NotifyObject
#include "ACS_CS_SM_NotifyObject.h"

struct ACS_CS_CpGroupData 
{
    // Data Members for Class Attributes
       std::string cpGroupName;
       ACS_CS_API_TableChangeOperation::OpType operationType;
       std::vector<CPID> cpIdList;
       ACS_CS_API_OgChange::ACS_CS_API_EmptyOG_ReasonCode emptyOgReason;

};

struct ACS_CS_CpGroupChange
{
      std::vector<ACS_CS_CpGroupData> cpGroupData;

};






class ACS_CS_SM_CPGroupTableNotify : public ACS_CS_SM_NotifyObject  //## Inherits: <unnamed>%4AAE77CC001F
{

  public:
      explicit ACS_CS_SM_CPGroupTableNotify();

      virtual ~ACS_CS_SM_CPGroupTableNotify();


      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual ACS_CS_Primitive * clone () const;

      void getCpGroupChange (ACS_CS_CpGroupChange &cpGroupChange) const;

      void setCpGroupChange (const ACS_CS_CpGroupChange &cpGroupChange);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      explicit ACS_CS_SM_CPGroupTableNotify(const ACS_CS_SM_CPGroupTableNotify &right);

      ACS_CS_SM_CPGroupTableNotify & operator=(const ACS_CS_SM_CPGroupTableNotify &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_CpGroupChange m_cpGroupChange;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_CPGroupTableNotify 



#endif
