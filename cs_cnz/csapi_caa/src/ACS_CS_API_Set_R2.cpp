//	Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_API_Set_Implementation.h"


// ACS_CS_API_Set_R2
#include "ACS_CS_API_Set_R2.h"


// Class ACS_CS_API_Set_R2
class ACS_CS_API_Set_Implementation;

ACS_CS_API_Set_Implementation* ACS_CS_API_Set_R2::implementation = 0;

/*
static ACS_CS_API_SET_NS::CS_API_Set_Result setQuorumData (const ACS_CS_API_QuorumData_R2& quorumData)
{
   if (implementation == 0)
		implementation = new ACS_CS_API_Set_Implementation();

	if (implementation)
      return implementation->setQuorumData(quorumData);
	else
      return ACS_CS_API_SET_NS::Result_Failure;

}
*/

/*
ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R2::setDefaultPackage (int typeBoard,std::string defPackage)
{
   if (implementation == 0)
   {
		implementation = new ACS_CS_API_Set_Implementation();
   }
   if (implementation)
   {
      return implementation->setDefaultPackage(typeBoard,defPackage);
   }
   else
   {
		return ACS_CS_API_SET_NS::Result_Failure;
   }
}
*/

ACS_CS_API_SET_NS::CS_API_Set_Result ACS_CS_API_Set_R2::setSoftwareVersion (std::string version, std::string mag,unsigned short boardId)
 {
    if (implementation == 0)
    {
       implementation = new ACS_CS_API_Set_Implementation;
    }
    if (implementation)
    {
    	return implementation->setSoftwareVersion(version,mag,boardId);
    }
    else
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

 }
/*
static ACS_CS_API_SET_NS::CS_API_Set_Result setIptDefaultPackage (std::string &iptDefPackage)
 {
    if (implementation == 0)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation)
    {
       return implementation->setIptDefaultPackage(iptDefPackage);
    }
    else
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

 }

static ACS_CS_API_SET_NS::CS_API_Set_Result setEpb1DefaultPackage (std::string &epb1DefPackage)
 {
    if (implementation == 0)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation)
    {
       return implementation->setEpb1DefaultPackage(epb1DefPackage);
    }
    else
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

 }

static ACS_CS_API_SET_NS::CS_API_Set_Result setEpb2DefaultPackage (std::string &epb2DefPackage)
 {
    if (implementation == 0)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation)
    {
       return implementation->setEpb2DefaultPackage(epb2DefPackage);
    }
    else
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

 }

static ACS_CS_API_SET_NS::CS_API_Set_Result setSoftwareVersionList (std::string &version, std::vector <unsigned short> &boardIdList)
 {
    if (implementation == 0)
    {
       implementation = new ACS_CS_API_Set_Implementation();
    }

    if (implementation)
    {
       return implementation->setSoftwareVersionList(version,boardIdList);
    }
    else
    {
       return ACS_CS_API_SET_NS::Result_Failure;
    }

 }*/


