

//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	*********************************************************


#ifndef ACS_CS_SM_CPTableNotify_h
#define ACS_CS_SM_CPTableNotify_h 1


#include <vector>
#include "ACS_CS_API.h"

// ACS_CS_SM_NotifyObject
#include "ACS_CS_SM_NotifyObject.h"

struct ACS_CS_CpTableData 
{
    // Data Members for Class Attributes
       CPID cpId;
       ACS_CS_API_TableChangeOperation::OpType operationType;
       std::string cpName;
       std::string cpAliasName;
       short unsigned apzSystem;
       short unsigned cpType;
       ACS_CS_API_NS::CpState cpState;
       ACS_CS_API_NS::ApplicationId applicationId;
       ACS_CS_API_NS::ApzSubstate apzSubstate;
       ACS_CS_API_NS::AptSubstate aptSubstate;
       ACS_CS_API_NS::StateTransition stateTransition;
       ACS_CS_API_NS::BlockingInfo blockingInfo;
       ACS_CS_API_NS::CpCapacity cpCapacity;
       ACS_CS_API_NS::MauType mauType;

};

struct ACS_CS_CpTableChange 
{
      std::vector<ACS_CS_CpTableData> cpData;
};







class ACS_CS_SM_CPTableNotify : public ACS_CS_SM_NotifyObject  //## Inherits: <unnamed>%4AAE78A50195
{

  public:
      ACS_CS_SM_CPTableNotify();

      virtual ~ACS_CS_SM_CPTableNotify();


      virtual ACS_CS_Protocol::CS_Primitive_Identifier getPrimitiveType () const;

      virtual int setBuffer (const char *buffer, int size);

      virtual int getBuffer (char *buffer, int size) const;

      virtual ACS_CS_Primitive * clone () const;

      void getCpTableChange (ACS_CS_CpTableChange &cpTableChange) const;

      void setCpTableChange (const ACS_CS_CpTableChange &cpTableChange);

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      explicit ACS_CS_SM_CPTableNotify(const ACS_CS_SM_CPTableNotify &right);

      ACS_CS_SM_CPTableNotify & operator=(const ACS_CS_SM_CPTableNotify &right);

    // Additional Private Declarations

  private: //## implementation
    // Data Members for Class Attributes

       ACS_CS_CpTableChange m_cpTableChange;

    // Additional Implementation Declarations

};


// Class ACS_CS_SM_CPTableNotify 



#endif
