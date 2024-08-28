//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4C729A1D014D.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4C729A1D014D.cm

//## begin module%4C729A1D014D.cp preserve=no
//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************
//## end module%4C729A1D014D.cp

//## Module: ACS_CS_API_Set_R3%4C729A1D014D; Package specification
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: T:\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_Set_R3.h

#ifndef ACS_CS_API_Set_R2_h
#define ACS_CS_API_Set_R2_h 1

//## begin module%4C729A1D014D.additionalIncludes preserve=no
//## end module%4C729A1D014D.additionalIncludes

//## begin module%4C729A1D014D.includes preserve=yes
//## end module%4C729A1D014D.includes

// ACS_CS_API_QuorumData_R2
//#include "ACS_CS_API_QuorumData_R2.h"
// ACS_CS_API_Set_R2
#include "ACS_CS_API_Set_R1.h"


class ACS_CS_API_Set_Implementation;


class ACS_CS_API_Set_R2 : public ACS_CS_API_Set_R1  //## Inherits: <unnamed>%4C729A6A01D5
{

public:

	//static ACS_CS_API_SET_NS::CS_API_Set_Result setQuorumData (const ACS_CS_API_QuorumData_R2& quorumData);

	//## Other Operations (specified)
	//## Operation: setQuorumData%4C729B20026A
	//	Set overall and per-CP quorum data
	//static ACS_CS_API_SET_NS::CS_API_Set_Result setDefaultPackage (int typeBoard, std::string defPackage);

	//## Operation: setSoftwareVersion%4C0CCDB201FA
	//	CCH block sets the container package for a specific SCX
	//	board when the system is running on a Common Based
	//	Architecture.

	static ACS_CS_API_SET_NS::CS_API_Set_Result setSoftwareVersion (std::string version, std::string mag,unsigned short boardId);


	//## Operation: setIptDefaultPackage%4CEB97FF027F
	//	CCH block sets the default software package used by IPT
	//	boards .
	//static ACS_CS_API_SET_NS::CS_API_Set_Result setIptDefaultPackage (std::string &iptDefPackage);
	//## Operation: setEpb1DefaultPackage%4CF9305E033C
	//	CCH block sets the default software package used by EPB1
	//	boards .
	//static ACS_CS_API_SET_NS::CS_API_Set_Result setEpb1DefaultPackage (std::string &epb1DefPackage);

	//## Operation: setEpb2DefaultPackage%4D0239330318
	//	CCH block sets the default software package used by EPB2
	//	boards .
	//static ACS_CS_API_SET_NS::CS_API_Set_Result setEpb2DefaultPackage (std::string &epb2DefPackage);
	//## Operation: setSoftwareVersionList%4D5133D501E7
	//	CCH block sets the container package for a specific SCX
	//	board when the system is running on a Common Based
	//	Architecture.
	//static ACS_CS_API_SET_NS::CS_API_Set_Result setSoftwareVersionList (std::string &version, std::vector <unsigned short> &boardIdList);

	// Additional Public Declarations
	//## begin ACS_CS_API_Set_R3%4C729A570155.public preserve=yes
	//## end ACS_CS_API_Set_R3%4C729A570155.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_API_Set_R3%4C729A570155.protected preserve=yes
      //## end ACS_CS_API_Set_R3%4C729A570155.protected

  private:
    //## Constructors (generated)
      ACS_CS_API_Set_R2();

      ACS_CS_API_Set_R2(const ACS_CS_API_Set_R2 &right);

    //## Assignment Operation (generated)
      ACS_CS_API_Set_R2 & operator=(const ACS_CS_API_Set_R2 &right);

    // Additional Private Declarations
      //## begin ACS_CS_API_Set_R3%4C729A570155.private preserve=yes
      //## end ACS_CS_API_Set_R3%4C729A570155.private

  private: //## implementation
    // Data Members for Class Attributes
      //## Attribute: implementation%4C729AA003B1
      //## begin ACS_CS_API_Set_R3::implementation%4C729AA003B1.attr preserve=no  private: static ACS_CS_API_Set_Implementation* {U} 0
      static ACS_CS_API_Set_Implementation* implementation;
      //## end ACS_CS_API_Set_R3::implementation%4C729AA003B1.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_API_Set_R3%4C729A570155.implementation preserve=yes
      //## end ACS_CS_API_Set_R3%4C729A570155.implementation

};

//## begin ACS_CS_API_Set_R3%4C729A570155.postscript preserve=yes
//## end ACS_CS_API_Set_R3%4C729A570155.postscript

// Class ACS_CS_API_Set_R3

//## begin module%4C729A1D014D.epilog preserve=yes
//## end module%4C729A1D014D.epilog


#endif
