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



#include <vector>
#include "ACS_CS_Trace.h"
#include "ACS_CS_API.h"

#include "ACS_CS_SM_SubscribeProfilePhaseChange.h"
#include "ACS_CS_SM_SubscribeTable.h"

#include "ACS_CS_SM_CPTableNotify.h"
#include "ACS_CS_SM_CPGroupTableNotify.h"
#include "ACS_CS_SM_NETableNotify.h"
#include "ACS_CS_SM_ProfilePhaseNotify.h"
#include "ACS_CS_SM_HWCTableNotify.h"
#include "ACS_CS_CPId_GetBlockingInfo.h"
#include "ACS_CS_CPId_GetCPCapacity.h"
#include "ACS_CS_CPId_GetBlockingInfoResponse.h"
#include "ACS_CS_CPId_GetCPCapacityResponse.h"



// ACS_CS_HostNetworkConverter
#include "ACS_CS_HostNetworkConverter.h"

ACS_CS_Trace_TDEF(ACS_CS_HostNetworkConverter_TRACE);

// === static cast ========================================================================
template <bool B>
inline void STATIC_ASSERT_IMPL()
{  //if you get a compile error in the next line, then read comment block below carefully!
   char STATIC_ASSERT_FAILURE[B] = {0};
   static_cast<void>(STATIC_ASSERT_FAILURE); //<--only to avoid compilation warning _unused variable_
}

#define STATIC_ASSERT(B) STATIC_ASSERT_IMPL<B>()

// ========================================================================================
// STATIC_ASSERT does a compile time check; if the check fails, then modifications were
// done in a way that _might_ break the code.
// It's better to fix problem at compile time than during debugging/testing or in field
//
// How to fix:
//
// 1. click on output tab to identify the line of the static_assert that failed.
//    Now you know the structure that changed.
//
// 2. determine which of the following options apply (2A, 2B or 2C) and fix accordingly:
//    (be aware that there may be more than one change at a time !)
//
// 2A: the size of an existing attribute changed. This is the harmless case. Goto step 3.
//
// 2B: a new attribute (or more than one) was added. In this case find out which attribute it is and
//     add the serialization call AND the deserialization call for this new attribute.
//
//     Note: failure to do so will result in large amounts of debugging time or TR
//           handling time ;-)
//
// 2C: an existing attribute was deleted: obviously the attribute in serialize and deserialize
//     routine needs to be removed and the size adjusted according to step 3.
//
// 3:  adjust the static_assert, do this with the following steps:
//
// 3a: comment out the static assert and uncomment the two lines before
//
// 3b: try to compile, the compile error will tell you the actual size of the structure
//
// 3c: undo step 3a and insert the new value into static_assert
//
//
// Caveat: If a new attribute was added, but the overall size of the structure did not
//         change, the static_assert will not catch that. However, this should be unlikely.
// ========================================================================================

// ========================================================================================
// === static cast end ====================================================================
// ========================================================================================
// === ntoh / hton section starts here ====================================================
// ========================================================================================

// === helper templates for generic hton ntoh conversion  =================================
// Note: These templates allow the compiler to choose the right conversion routines
// (htons htonl) dependent on various types during COMPILE TIME !
// Note 2: note that enums have no defined size; the compiler is free to choose each time

namespace
{
   template<int STRUCT_LENGTH>
   struct conv_helper{};

   template<> struct conv_helper<sizeof(char)>  { typedef char Dummy; };
   template<> struct conv_helper<sizeof(short)> { typedef short Dummy; };
   template<> struct conv_helper<sizeof(long)>  { typedef long Dummy; };
}

namespace
{
   template<typename T>
   void hton(const T & inObject, T & outObject, conv_helper<sizeof(char)> /*unused*/)
   {
      outObject = inObject; // no conversion in case of char necessary
   }

   template<typename T>
   void hton(const T & inObject, T & outObject, conv_helper<sizeof(short)> /*unused*/)
   {
      outObject = static_cast<T>(htons(inObject));  // static_cast needed in case of enums
   }

   template<typename T>
  void hton(const T & inObject, T & outObject, conv_helper<sizeof(int)> /*unused*/)
  {
	 outObject = static_cast<T>(htonl(inObject));  // static_cast needed in case of enums
  }

   template<typename T>
   void hton(const T & inObject, T & outObject, conv_helper<sizeof(long)> /*unused*/)
   {
      outObject = static_cast<T>(htonl(inObject)); // static_cast needed in case of enums
   }
}

namespace
{
   template<typename T>
   void ntoh(const T & inObject, T & outObject, conv_helper<sizeof(char)> /*unused*/)
   {
      outObject = inObject; // no conversion in case of char necessary
   }

   template<typename T>
   void ntoh(const T & inObject, T & outObject, conv_helper<sizeof(short)> /*unused*/)
   {
      outObject = static_cast<T>(ntohs(static_cast<short>(inObject))); // 1st static_cast needed in case of enums, 2nd for size_t
   }

   template<typename T>
  void ntoh(const T & inObject, T & outObject, conv_helper<sizeof(int)> /*unused*/)
  {
	 outObject = static_cast<T>(ntohl(static_cast<int>(inObject))); // 1st static_cast needed in case of enums, 2nd for size_t
  }

   template<typename T>
   void ntoh(const T & inObject, T & outObject, conv_helper<sizeof(long)> /*unused*/)
   {
      outObject = static_cast<T>(ntohl(static_cast<long>(inObject))); // 1st static_cast needed in case of enums, 2nd for size_t
   }
}

namespace
{
   template<typename T>
   void hton(const T & inObject, T & outObject)
   {
      conv_helper<sizeof(T)> tmp;
      hton(inObject, outObject, tmp);
   }

   template<typename T>
   void ntoh(const T & inObject, T & outObject)
   {
      conv_helper<sizeof(T)> tmp;
      ntoh(inObject, outObject, tmp);
   }
}

// ========================================================================================
// === serialize section starts here ======================================================
// ========================================================================================
namespace
{
   template<typename T>
   int serialize(char * & poi, const T & object, const char * const buffer, const size_t maxSize)
   {
      if( buffer == NULL)
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<>\n"
                       "Error: NULL pointer, line: %d\n",
                       __LINE__));
         return -1;
      }

      // make sure there is enough space in the buffer
      if( poi - buffer + sizeof(T) > maxSize )
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<>\n"
                       "Error: buffer overflow, line: %d. sizeof(T): %u. %u > %u\n",
                       __LINE__, sizeof(T), (poi - buffer + sizeof(T)), maxSize));
         return -1;
      }

      T tmpObj;
      T * const destPoi = reinterpret_cast<T *>(poi);

      ::hton(object, tmpObj);

      *destPoi = tmpObj;

      poi += sizeof(T);
      return 0;
   }
}

namespace
{
   template<>
      int serialize(char * & poi, const std::string & object, const char * const buffer, const size_t maxSize)
   {
      if( buffer == NULL)
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<string>\n"
                       "Error: NULL pointer\n"));
         return -1;
      }

      unsigned int objLen = static_cast<unsigned int>(object.length());

      // make sure there is enough space in the buffer
      if( poi - buffer + objLen + sizeof(objLen) > maxSize )
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<string>\n"
                       "Error: buffer overflow. sizeof(objLen): %u. %u > %u\n",
                       sizeof(objLen), (poi - buffer + objLen + sizeof(objLen)), maxSize));
         return -1;
      }

      // write length of string first
      ::serialize(poi, objLen, buffer, maxSize);

      // it's a string of char, so no hton/ntoh conversion

      for( unsigned int i = 0; i < objLen; i++ )
      {
         *poi++ = object[i];
      }

      // *poi++ = (char)0; no, because we already know how many chars there are

      return 0;
   }
}

// function template specialization for int lists, which need hton but can't be just be reinterpret_cast
namespace
{
   template<>
      int serialize(char * & poi, const std::vector<CPID> & object, const char * const buffer, const size_t maxSize)
   {
      if( buffer == NULL)
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<vector<CPID>>\n"
                       "Error: NULL pointer, line: %d\n",
                       __LINE__));

         return -1;
      }

      size_t objNum = object.size();

      // make sure there is enough space in the buffer
      if( poi - buffer + sizeof(size_t) + objNum > maxSize )
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) serialize<vector<CPID>>\n"
                       "Error: buffer overflow, line: %d\n",
                       __LINE__));
         return -1;
      }

      // write number of list elements out
      int rc = serialize(poi, objNum, buffer, maxSize);

      for( unsigned int i = 0; i < objNum; i++ )
      {
         rc += ::serialize(poi, object[i], buffer, maxSize);
      }

      // no (char)0 at end necessary, because we know how long the string is.

      return rc;
   }
}

// ==========================================================================
// deserialize section starts here
// ==========================================================================
namespace
{
   template<typename T>
   int deserialize(const char * & poi, T & object)
   {
      if( poi == NULL)
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) deserialize<>\n"
                       "Error: NULL pointer, line: %d\n",
                       __LINE__));
         return -1;
      }

      const T * const objPoi = reinterpret_cast<const T *>(poi);
      const T tmpObj = *objPoi;

      ::ntoh(tmpObj, object);
      poi += sizeof(T);

      return 0;
   }
}

// function template specialization for strings, which need to ntoh and can't be just reinterpret_cast
namespace
{
   template<>
      int deserialize(const char * & poi, std::string & object)
   {
      if( poi == NULL )
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
            "(%t) deserialize<>\n"
            "Error: NULL pointer, line: %d\n",
            __LINE__));
         return -1;
      }

      // get length of string
      unsigned int strLen;
      int rc = ::deserialize(poi, strLen);

      // handle special case where string is empty
      if( strLen == 0)
      {
         object = "";
         return 0;
      }

      object.clear();

      unsigned int i;
      for( i = 0; i<strLen; i++)
      {
         object += poi[i];
      }

      poi += i;

      if(rc<0) rc=-1;
      return rc;
   }
}

// function template specialization for int lists, which need to ntoh but can't be just reinterpret_cast
namespace
{
   template<>
   int deserialize(const char * & poi, std::vector<CPID> & object)
   {
      if( poi == NULL )
      {
         ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                       "(%t) deserialize<>\n"
                       "Error: NULL pointer, line: %d\n",
                       __LINE__));
         return -1;
      }

      // get number of elements

      size_t objNum;
      int rc = ::deserialize(poi, objNum);

      unsigned short id;

      object.clear();
      for( size_t i = 0; i<objNum; i++ )
      {
         rc += ::deserialize(poi, id);
         object.push_back(id);
      }

      if(rc<0) rc=-1;
      return rc;
   }
}


// Class ACS_CS_HostNetworkConverter 

ACS_CS_HostNetworkConverter::ACS_CS_HostNetworkConverter()
{
}



 int ACS_CS_HostNetworkConverter::checkSize (size_t actualSize, size_t maxSize)
{
   if( actualSize > maxSize)
   {
      ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
                    "(%t) ACS_CS_Deserializer::deserialize()\n"
                    "Error: size mismatch; actSize=%d, maxSize=%d\n",
                    actualSize, maxSize));
      return -1;

   }
   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_API_OmProfileChange &omProfileChange)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi,omProfileChange.omProfileCurrent   ,buffer,size);
   rc += ::serialize(poi,omProfileChange.omProfileRequested ,buffer,size);
   rc += ::serialize(poi,omProfileChange.aptCurrent         ,buffer,size);
   rc += ::serialize(poi,omProfileChange.aptRequested       ,buffer,size);
   rc += ::serialize(poi,omProfileChange.apzCurrent         ,buffer,size);
   rc += ::serialize(poi,omProfileChange.apzRequested       ,buffer,size);
   rc += ::serialize(poi,omProfileChange.phase              ,buffer,size);
   rc += ::serialize(poi,omProfileChange.changeReason       ,buffer,size);
   rc += ::serialize(poi,omProfileChange.aptQueued          ,buffer,size);
   rc += ::serialize(poi,omProfileChange.apzQueued          ,buffer,size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_API_OmProfileChange)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_API_OmProfileChange)== 40);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);
   if(rc < 0) return -1;

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_ProfilePhaseNotify &notify)
{
   char * poi = buffer;

   ACS_CS_API_OmProfileChange prfChange;
   notify.getOmProfileChange(prfChange);
   int ompSize = static_cast<int>(sizeof(prfChange));

   int rc = serialize(poi, ompSize, prfChange );

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_API_OmProfileChange)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_API_OmProfileChange)== 40);

   int actualSize = ompSize;
   rc += checkSize(actualSize, size);
   if(rc < 0) return -1;

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_CPGroupTableNotify &cpGroupTableNotify)
{
   char * poi = buffer;
   int rc = 0;

   ACS_CS_CpGroupChange data;
   cpGroupTableNotify.getCpGroupChange(data);

   size_t objSize = data.cpGroupData.size();
   rc += ::serialize(poi,objSize,buffer,size);

   // now iterate through all elements
   for( std::vector<ACS_CS_CpGroupData>::const_iterator i = data.cpGroupData.begin(); i != data.cpGroupData.end(); ++i)
   {
      rc += ::serialize(poi, i->cpGroupName    ,buffer,size);
      rc += ::serialize(poi, i->cpIdList       ,buffer,size);
      rc += ::serialize(poi, i->operationType  ,buffer,size);
      rc += ::serialize(poi, i->emptyOgReason  ,buffer,size);
   }

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_CpGroupData)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CpGroupData)== 48);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);
   if(rc < 0) return -1;

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_CPTableNotify &cpTableNotify)
{
   char * poi = buffer;
   int rc = 0;

   ACS_CS_CpTableChange data;
   cpTableNotify.getCpTableChange(data);

   // put number of elements in first
   size_t objSize = data.cpData.size();
   rc += ::serialize(poi,objSize,buffer,size);

   // now iterate through all elements

   for( std::vector<ACS_CS_CpTableData>::const_iterator i = data.cpData.begin(); i != data.cpData.end(); ++i)
   {
      rc += ::serialize(poi,i->cpId            ,buffer,size);
      rc += ::serialize(poi,i->operationType   ,buffer,size);
      rc += ::serialize(poi,i->cpName          ,buffer,size);
      rc += ::serialize(poi,i->cpAliasName     ,buffer,size);
      rc += ::serialize(poi,i->apzSystem       ,buffer,size);
      rc += ::serialize(poi,i->cpState         ,buffer,size);
      rc += ::serialize(poi,i->applicationId   ,buffer,size);
      rc += ::serialize(poi,i->apzSubstate     ,buffer,size);
      rc += ::serialize(poi,i->aptSubstate     ,buffer,size);
      rc += ::serialize(poi,i->stateTransition ,buffer,size);
      rc += ::serialize(poi,i->cpType          ,buffer,size);
      rc += ::serialize(poi,i->blockingInfo    ,buffer,size);
      rc += ::serialize(poi,i->cpCapacity      ,buffer,size);
      rc += ::serialize(poi,i->mauType         ,buffer,size);
   }

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_CpTableData)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CpTableData)== 64);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);
   if(rc < 0) return -1;

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_NETableNotify &neTableNotify)
{
   char * poi = buffer;
   int rc = 0;

   ACS_CS_NetworkElementChange neChange;
   neTableNotify.getNeChange(neChange);
   int sizeOmP = sizeof(neChange.omProfile);

   rc += ::serialize(poi, neChange.neId                ,buffer, size);
   rc +=   serialize(poi, sizeOmP, neChange.omProfile);
   rc += ::serialize(poi, neChange.clusterMode         ,buffer, size);
   rc += ::serialize(poi, neChange.trafficIsolatedCpId ,buffer, size);
   rc += ::serialize(poi, neChange.trafficLeaderCpId   ,buffer, size);
   rc += ::serialize(poi, neChange.alarmMasterCpId     ,buffer, size);
   rc += ::serialize(poi, neChange.clockMasterCpId     ,buffer, size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_NetworkElementChange)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_NetworkElementChange)== 64);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);
   if(rc < 0) return -1;

   buffer = poi;
   size = actualSize;

   return 0;
}


int ACS_CS_HostNetworkConverter::serialize(char * &buffer, int &size, const ACS_CS_SM_HWCTableNotify &hwcTableNotify)
{
	char * poi = buffer;
	int rc = 0;

	ACS_CS_HWCChange hwcChange;
	hwcTableNotify.getHWCChange(hwcChange);

	rc += ::serialize(poi, hwcChange.hwcData.size(), buffer, size);

	std::vector<ACS_CS_HWCData>::const_iterator it;
	for(it = hwcChange.hwcData.begin(); it != hwcChange.hwcData.end(); it++)
	{
		rc += ::serialize(poi, it->operationType, buffer, size);
		rc += ::serialize(poi, it->magazine, buffer, size);
		rc += ::serialize(poi, it->slot, buffer, size);
		rc += ::serialize(poi, it->sysType, buffer, size);
		rc += ::serialize(poi, it->sysNo, buffer, size);
		rc += ::serialize(poi, it->fbn, buffer, size);
		rc += ::serialize(poi, it->side, buffer, size);
		rc += ::serialize(poi, it->seqNo, buffer, size);
		rc += ::serialize(poi, it->ipEthA, buffer, size);
		rc += ::serialize(poi, it->ipEthB, buffer, size);
		rc += ::serialize(poi, it->aliasEthA, buffer, size);
		rc += ::serialize(poi, it->aliasEthB, buffer, size);
		rc += ::serialize(poi, it->netmaskAliasEthA, buffer, size);
		rc += ::serialize(poi, it->netmaskAliasEthB, buffer, size);
		rc += ::serialize(poi, it->dhcpMethod, buffer, size);
		rc += ::serialize(poi, it->sysId, buffer, size);
		rc += ::serialize(poi, it->softwarePackage, buffer, size);
		rc += ::serialize(poi, it->productId, buffer, size);
		rc += ::serialize(poi, it->productRevision, buffer, size);
	}

	int actualSize = static_cast<int>(poi - buffer);
	rc += checkSize(actualSize, size);
	if (rc < 0)
	{
		ACS_CS_TRACE((ACS_CS_HostNetworkConverter_TRACE,
		                    "(%t) [%s@%d] Error: size mismatch; actual size = %d\n",
		                    __FUNCTION__, __LINE__, actualSize));
		return -1;
	}

	buffer = poi;
	size = actualSize;

	return 0;
}

int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_HWCTableNotify &hwcTableNotify)
{
	const char *poi = buffer;
	int rc = 0;

	// get number of objects
	size_t objNum = 0;
	rc += ::deserialize(poi, objNum);

	ACS_CS_HWCChange hwcChange;
	ACS_CS_HWCData hwcData;

	for(size_t i = 0; i < objNum; i++)
	{
		rc += ::deserialize(poi, hwcData.operationType);
		rc += ::deserialize(poi, hwcData.magazine);
		rc += ::deserialize(poi, hwcData.slot);
		rc += ::deserialize(poi, hwcData.sysType);
		rc += ::deserialize(poi, hwcData.sysNo);
		rc += ::deserialize(poi, hwcData.fbn);
		rc += ::deserialize(poi, hwcData.side);
		rc += ::deserialize(poi, hwcData.seqNo);
		rc += ::deserialize(poi, hwcData.ipEthA);
		rc += ::deserialize(poi, hwcData.ipEthB);
		rc += ::deserialize(poi, hwcData.aliasEthA);
		rc += ::deserialize(poi, hwcData.aliasEthB);
		rc += ::deserialize(poi, hwcData.netmaskAliasEthA);
		rc += ::deserialize(poi, hwcData.netmaskAliasEthB);
		rc += ::deserialize(poi, hwcData.dhcpMethod);
		rc += ::deserialize(poi, hwcData.sysId);
		rc += ::deserialize(poi, hwcData.softwarePackage);
		rc += ::deserialize(poi, hwcData.productId);
		rc += ::deserialize(poi, hwcData.productRevision);

		hwcChange.hwcData.push_back(hwcData);
	}

	if(rc < 0)
	{
		rc = -1;
	}

	hwcTableNotify.setHWCChange(hwcChange);

	buffer = poi;
	return rc;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_API_OmProfileChange &omProfileChange)
{

   const char * poi = buffer;
   int rc=0;

   rc += ::deserialize(poi, omProfileChange.omProfileCurrent);
   rc += ::deserialize(poi, omProfileChange.omProfileRequested);
   rc += ::deserialize(poi, omProfileChange.aptCurrent);
   rc += ::deserialize(poi, omProfileChange.aptRequested);
   rc += ::deserialize(poi, omProfileChange.apzCurrent);
   rc += ::deserialize(poi, omProfileChange.apzRequested);
   rc += ::deserialize(poi, omProfileChange.phase);
   rc += ::deserialize(poi, omProfileChange.changeReason);
   rc += ::deserialize(poi, omProfileChange.aptQueued);
   rc += ::deserialize(poi, omProfileChange.apzQueued);

   if(rc < 0)
   {
	   return -1;
   }

   buffer = poi;
   return 0;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_ProfilePhaseNotify &notify)
{
   const char * poi = buffer;
   int rc=0;

   ACS_CS_API_OmProfileChange omChange;
   rc  += deserialize(poi, omChange);

   if( rc < 0 )
   {
	   return -1;
   }

   notify.setOmProfileChange(omChange);

   buffer = poi;
   return 0;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_CPGroupTableNotify &cpGroupTableNotify)
{
   const char * poi = buffer;
   int rc=0;


   size_t objNum = 0;
   rc += ::deserialize(poi, objNum);

   ACS_CS_CpGroupChange data;
   ACS_CS_CpGroupData entry;

   for (size_t i = 0; i<objNum; i++)
   {
      rc += ::deserialize(poi, entry.cpGroupName);
      rc += ::deserialize(poi, entry.cpIdList);
      rc += ::deserialize(poi, entry.operationType);
      rc += ::deserialize(poi, entry.emptyOgReason);

      data.cpGroupData.push_back(entry);
   }

   if(rc < 0)
   {
	   return -1;
   }

   cpGroupTableNotify.setCpGroupChange(data);

   buffer = poi;
   return rc;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_CPTableNotify &cpTableNotify)
{
   const char * poi = buffer;
   int rc = 0;

   // get number of objects
   size_t objNum = 0;
   rc += ::deserialize(poi, objNum);

   ACS_CS_CpTableChange data;
   ACS_CS_CpTableData entry;

   for( size_t i = 0; i<objNum; i++ )
   {
      rc += ::deserialize(poi, entry.cpId);
      rc += ::deserialize(poi, entry.operationType);
      rc += ::deserialize(poi, entry.cpName);
      rc += ::deserialize(poi, entry.cpAliasName);
      rc += ::deserialize(poi, entry.apzSystem);
      rc += ::deserialize(poi, entry.cpState);
      rc += ::deserialize(poi, entry.applicationId);
      rc += ::deserialize(poi, entry.apzSubstate);
      rc += ::deserialize(poi, entry.aptSubstate);
      rc += ::deserialize(poi, entry.stateTransition);
      rc += ::deserialize(poi, entry.cpType);
      rc += ::deserialize(poi, entry.blockingInfo);
      rc += ::deserialize(poi, entry.cpCapacity);
      rc += ::deserialize(poi, entry.mauType);

      data.cpData.push_back(entry);
   }

   if(rc<0)
   {
	   rc=-1;
   }

   cpTableNotify.setCpTableChange(data);

   buffer = poi;
   return rc;

}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_NETableNotify &neTableNotify)
{
   const char * poi = buffer;
   int rc = 0;

   ACS_CS_NetworkElementChange neChange;

   rc += ::deserialize(poi, neChange.neId);
   rc +=   deserialize(poi, neChange.omProfile);
   rc += ::deserialize(poi, neChange.clusterMode);
   rc += ::deserialize(poi, neChange.trafficIsolatedCpId);
   rc += ::deserialize(poi, neChange.trafficLeaderCpId);
   rc += ::deserialize(poi, neChange.alarmMasterCpId);
   rc += ::deserialize(poi, neChange.clockMasterCpId);

   if (rc<0)
   {
	   return -1;
   }

   neTableNotify.setNeChange(neChange);

   buffer = poi;
   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_SubscribeProfilePhaseChange &ppc)
 {
    char * poi = buffer;
    int rc = 0;

    rc += ::serialize(poi, ppc.getPhase()     ,buffer, size);
    rc += ::serialize(poi, ppc.isSubscription ,buffer, size);

    int actualSize = static_cast<int>(poi-buffer);
    rc += checkSize(actualSize, size);

    // uncomment the following lines to get sizeof value
    //conv_helper<sizeof(ACS_CS_SM_SubscribeProfilePhaseChange)> xxx;
    //xxx.Dummy = 0;
    STATIC_ASSERT(sizeof(ACS_CS_SM_SubscribeProfilePhaseChange)== 16);

    if(rc < 0)
    {
       return -1;
    }

    buffer = poi;
    size = actualSize;

    return 0;
 }


 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_SM_SubscribeTable &subscriptionTable)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi, subscriptionTable.getTableScope(), buffer, size);
   rc += ::serialize(poi, subscriptionTable.isSubscription , buffer, size);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_SM_SubscribeTable)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_SM_SubscribeTable)== 16);

   if(rc < 0)
   {
	   return -1;
   }

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_SubscribeTable &subscriptionTable)
{
   const char * poi = buffer;
   int rc=0;

   ACS_CS_Protocol::CS_Scope_Identifier scope;

   rc += ::deserialize(poi, scope);
   rc += ::deserialize(poi, subscriptionTable.isSubscription);

   if(rc < 0)
   {
	   return -1;
   }

   subscriptionTable.setTableScope(scope);

   return rc;
}


 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_SM_SubscribeProfilePhaseChange &ppc)
{
   const char * poi = buffer;
   int rc=0;

   ACS_CS_API_OmProfilePhase::PhaseValue phase = ACS_CS_API_OmProfilePhase::Idle;

   rc += ::deserialize(poi, phase);
   rc += ::deserialize(poi, ppc.isSubscription);
   ppc.setPhase(phase);

   if(rc < 0)
   {
	   rc = -1;
   }

   return rc;
}

//--

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_CPId_GetCPCapacity &getCpCapacity)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi, getCpCapacity.getCpId(), buffer, size);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_CPId_GetCPCapacity)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CPId_GetCPCapacity)== 16);

   if(rc < 0)
   {
      return -1;
   }

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_CPId_GetCPCapacityResponse &getCpCapacityResponse)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi, getCpCapacityResponse.getResultCode()     ,buffer, size);
   rc += ::serialize(poi, getCpCapacityResponse.getCPCapacity()     ,buffer, size);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);

   // uncomment the following lines to get sizeof value
   // conv_helper<sizeof(ACS_CS_CPId_GetCPCapacityResponse)> xxx;
   // xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CPId_GetCPCapacityResponse)== 16);

   if(rc < 0)
   {
      return -1;
   }

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_CPId_GetBlockingInfo &getBlockingInfo)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi, getBlockingInfo.getCpId()     ,buffer, size);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_CPId_GetBlockingInfo)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CPId_GetBlockingInfo)== 16);

   if(rc < 0)
   {
      return -1;
   }

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::serialize (char * &buffer, int &size, const ACS_CS_CPId_GetBlockingInfoResponse &getBlockingInfoResponse)
{
   char * poi = buffer;
   int rc = 0;

   rc += ::serialize(poi, getBlockingInfoResponse.getResultCode()       ,buffer, size);
   rc += ::serialize(poi, getBlockingInfoResponse.getBlockingInfo()     ,buffer, size);

   int actualSize = static_cast<int>(poi-buffer);
   rc += checkSize(actualSize, size);

   // uncomment the following lines to get sizeof value
   //conv_helper<sizeof(ACS_CS_CPId_GetBlockingInfoResponse)> xxx;
   //xxx.Dummy = 0;
   STATIC_ASSERT(sizeof(ACS_CS_CPId_GetBlockingInfoResponse)== 16);

   if(rc < 0)
   {
      return -1;
   }

   buffer = poi;
   size = actualSize;

   return 0;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_CPId_GetCPCapacity &getCpCapacity)
{
   const char * poi = buffer;
   int rc=0;

   CPID cpId = 0;
   rc += ::deserialize(poi, cpId);
   getCpCapacity.setCpId(cpId);

   if(rc < 0)
   {
      rc = -1;
   }
   return rc;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_CPId_GetCPCapacityResponse &getCpCapacityResponse)
{
   const char * poi = buffer;
   int rc=0;

   ACS_CS_Protocol::CS_Result_Code resultCode = ACS_CS_Protocol::Result_NotSpecified;
   rc += ::deserialize(poi, resultCode);
   getCpCapacityResponse.setResultCode(resultCode);

   ACS_CS_API_NS::CpCapacity cpCapacity = 0;
   rc += ::deserialize(poi, cpCapacity);
   getCpCapacityResponse.setCPCapacity(cpCapacity);

   if(rc < 0)
   {
      rc = -1;
   }
   return rc;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_CPId_GetBlockingInfo &getBlockingInfo)
{
   const char * poi = buffer;
   int rc=0;

   CPID cpId = 0;
   rc += ::deserialize(poi, cpId);
   getBlockingInfo.setCpId(cpId);

   if(rc < 0)
   {
      rc = -1;
   }
   return rc;
}

 int ACS_CS_HostNetworkConverter::deserialize (const char * &buffer, ACS_CS_CPId_GetBlockingInfoResponse &getBlockingInfoResponse)
{
   const char * poi = buffer;
   int rc=0;

   ACS_CS_Protocol::CS_Result_Code resultCode = ACS_CS_Protocol::Result_NotSpecified;
   rc += ::deserialize(poi, resultCode);
   getBlockingInfoResponse.setResultCode(resultCode);

   ACS_CS_API_NS::BlockingInfo blockingInfo = 0;
   rc += ::deserialize(poi, blockingInfo);
   getBlockingInfoResponse.setBlockingInfo(blockingInfo);

   if(rc < 0)
   {
      rc = -1;
   }
   return rc;
}

//--

// Additional Declarations

