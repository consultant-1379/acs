//	*********************************************************
//	 COPYRIGHT Ericsson 2012.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************


#ifndef ACS_CS_API_CPGroup_Implementation_h
#define ACS_CS_API_CPGroup_Implementation_h 1

#include <set>
#include <map>
#include "ACS_CS_API_Internal.h"

// ACS_CS_API_CPGroup_R1
#include "ACS_CS_API_CPGroup_R1.h"

class ACS_CS_API_CPGroup_Implementation : public ACS_CS_API_CPGroup_R1
{

  public:
      ACS_CS_API_CPGroup_Implementation();

      virtual ~ACS_CS_API_CPGroup_Implementation();

      virtual ACS_CS_API_NS::CS_API_Result getGroupMembers (const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &cplist);

      virtual ACS_CS_API_NS::CS_API_Result getGroupNames (ACS_CS_API_NameList_R1 &nameList);

  private:

      ACS_CS_ImModel *model;
      ACS_CS_ImIMMReader * immReader;
      set<string> cpGroupNames;

      void getMatchingCpObjects(string groupName, set<const ACS_CS_ImClusterCp *> &cpSet);
};

#endif
