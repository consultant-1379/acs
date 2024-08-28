

//	© Copyright Ericsson AB 2007. All rights reserved.




#include "ACS_CS_Util.h"
//RoGa#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_TableEntry.h"
//RoGa #include "ACS_CS_API_Util_Implementation.h"


// ACS_CS_EntryCounter
#include "ACS_CS_EntryCounter.h"


//RoGa #include "ACS_CS_Trace.h"
//RoGa ACS_CS_Trace_TDEF(ACS_CS_EntryCounter_TRACE);

using ACS_CS_NS::ILLEGAL_SYSID;
using std::map;
using std::set;
using std::vector;
using std::string;



// Class ACS_CS_EntryCounter 

 std::set<std::string> ACS_CS_EntryCounter::nameSet;

 ACS_CS_API_Util_Implementation *ACS_CS_EntryCounter::utilImpl = 0;

ACS_CS_EntryCounter::ACS_CS_EntryCounter()
      : sysIdMap(0)/*RoGa,
        entryMutexHandle(0)RoGa*/
{

   // Create utility object
/*RoGa   if (ACS_CS_EntryCounter::utilImpl == 0)
      ACS_CS_EntryCounter::utilImpl  = new ACS_CS_API_Util_Implementation();RoGa*/

   sysIdMap = new entryMap();

   // Mutex to protect threads from each other
	//RoGa entryMutexHandle = CreateMutex(NULL, FALSE, NULL);
/*RoGa
	if (entryMutexHandle == 0)
	{
		ACS_CS_TRACE((ACS_CS_EntryCounter_TRACE,
			"(%t) ACS_CS_EntryCounter::ACS_CS_EntryCounter()\n"
			"Error: Cannot create mutex, GetLastError() = %d\n",
			GetLastError()));

		ACS_CS_EVENT(Event_SubSystemError,
						"Create Windows Mutex",
						"Cannot create Windows mutex",
						"");
	}
RoGa*/
}


ACS_CS_EntryCounter::~ACS_CS_EntryCounter()
{

   if (sysIdMap)
   {
      entryMap::iterator it;

      // Loop through map
      for (it = sysIdMap->begin(); it != sysIdMap->end(); ++it)
      {
         if( it->second )
            delete it->second;	// Delete all entries
      }

      delete sysIdMap;  // Delete map
   }

/*RoGa   if (entryMutexHandle)
      (void) CloseHandle(entryMutexHandle);RoGa*/

}



 void ACS_CS_EntryCounter::addEntry (unsigned short sysId, unsigned short entryId)
{

   // Suppress warning for not freeing set ptr,
	// which we shouldn't since it is stored in an STL map.
	// Map and content freed in destructor
	//lint --e{429}

   // --- Critical section ---
/*   (void) WaitForSingleObject(entryMutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   if (sysIdMap)
   {
      entryMap::iterator it = sysIdMap->find(sysId); // Search for entry in table

      entrySet * setPtr = 0;

	   if ( it != sysIdMap->end() )				// if entry exists
	   {
		   setPtr = it->second;						// get pointer

         if (setPtr)
            setPtr->insert(entryId);
      }
      else
      {
         setPtr = new entrySet();

         if (setPtr)
         {
            setPtr->insert(entryId);

            sysIdMap->insert( entryMap::value_type(sysId, setPtr) ); // Add set to the map
         }
      }
   }

/*RoGa   (void) ReleaseMutex(entryMutexHandle);*/
	// --- End of critical section ---

}

 void ACS_CS_EntryCounter::removeEntry (unsigned short sysId, unsigned short entryId)
{

   // --- Critical section ---
/*   (void) WaitForSingleObject(entryMutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   if (sysIdMap)
   {
      entryMap::iterator it = sysIdMap->find(sysId); // Search for entry in table
      entrySet * setPtr = 0;

      if ( it != sysIdMap->end() )				// if entry exists
	   {
		   setPtr = it->second;						// get pointer

         if (setPtr)
         {
            (void) setPtr->erase(entryId);

            if (setPtr->size() == 0)
            {
               (void) sysIdMap->erase(it);
               delete setPtr;
            }
         }
      }
   }

/*RoGa   (void) ReleaseMutex(entryMutexHandle);RoGa*/
	// --- End of critical section ---

}

 bool ACS_CS_EntryCounter::containsEntry (unsigned short sysId) const
{

   bool sysIdFound = false;

   // --- Critical section ---
   /*RoGa (void) WaitForSingleObject(entryMutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa */

   if (sysIdMap)
   {
      entryMap::iterator it = sysIdMap->find(sysId); // Search for entry in table

      if ( it != sysIdMap->end() )				// if entry exists
	      sysIdFound = true;
   }

/*RoGa   (void) ReleaseMutex(entryMutexHandle);RoGa*/
	// --- End of critical section ---

   return sysIdFound;

}

 short unsigned ACS_CS_EntryCounter::getSysId (const ACS_CS_TableEntry *tableEntry)
{

   unsigned short sysId = ILLEGAL_SYSID;

   if (tableEntry == 0)
      return sysId;

   ACS_CS_Attribute sysTypeAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SysType);
   ACS_CS_Attribute sysNoAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SysNo);
   ACS_CS_Attribute seqNoAtt = tableEntry->getValue(ACS_CS_Protocol::Attribute_SeqNo);

   unsigned short sysType = 0;
   unsigned short sysNo = 0;
   unsigned short seqNo = 0;

   if (sysTypeAtt.getValueLength() == sizeof(sysType))
   {
      if (sysTypeAtt.getValue(reinterpret_cast<char *>(&sysType), sizeof(sysType)) > 0)
      {
         if (sysType == ACS_CS_Protocol::SysType_BC)
         {
            if (seqNoAtt.getValue(reinterpret_cast<char *>(&seqNo), sizeof(seqNo)) > 0)
            {
               sysId = sysType + seqNo;
            }
         }
         else if (sysType == ACS_CS_Protocol::SysType_CP)
         {
            if (sysNoAtt.getValue(reinterpret_cast<char *>(&sysNo), sizeof(sysNo)) > 0)
            {
               sysId = sysType + sysNo;
            }
         }
         else if (sysType == ACS_CS_Protocol::SysType_AP)
         {
            if (sysNoAtt.getValue(reinterpret_cast<char *>(&sysNo), sizeof(sysNo)) > 0)
            {
               sysId = sysType + sysNo;
            }
         }
      }
   }

   return sysId;

}

 bool ACS_CS_EntryCounter::addToNameSet (std::string name)
{

/*RoGa   if (ACS_CS_EntryCounter::utilImpl == 0)
      ACS_CS_EntryCounter::utilImpl = new ACS_CS_API_Util_Implementation();RoGa*/

   // Open mutex to make function thread safe
/*RoGa   HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ACS_CS_NS::MUTEX_NAME_ENTRYCOUNTER_PROTECTION);RoGa*/

   bool successful = true;

   // --- Critical section ---
/*RoGa   (void) WaitForSingleObject(mutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   set<string>::iterator it;

   // Loop through name set
   for (it = nameSet.begin(); it != nameSet.end(); ++it)
   {
      // Compare excluding case
/*RoGa      if ( utilImpl->compareWithoutCase(*it, name) )
      {
         // Name already exist
         successful = false;
         break;
      } RoGa*/
   }

   // Add if name didn't exist
   if (successful)
      nameSet.insert(name);

/*RoGa   (void) ReleaseMutex(mutexHandle);RoGa*/
	// --- End of critical section ---

/*RoGa
   if (mutexHandle)
      (void) CloseHandle(mutexHandle);
RoGa*/
   return successful;

}

 void ACS_CS_EntryCounter::removeFromNameSet (std::string name)
{

   // Open mutex to make function thread safe
 /*  HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ACS_CS_NS::MUTEX_NAME_ENTRYCOUNTER_PROTECTION);*/

   // --- Critical section ---
/*RoGa   (void) WaitForSingleObject(mutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   (void) nameSet.erase(name);

/*RoGa   (void) ReleaseMutex(mutexHandle);RoGa*/
	// --- End of critical section ---

/*RoGa   if (mutexHandle)
      (void) CloseHandle(mutexHandle);RoGa*/

}

 void ACS_CS_EntryCounter::getEntryVector (std::vector<unsigned short> &entryVector)
{

   // --- Critical section ---
/*   (void) WaitForSingleObject(entryMutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   if (sysIdMap)
   {
      entryMap::iterator it;

      for (it = sysIdMap->begin(); it != sysIdMap->end(); ++it)
      {
         entryVector.push_back(it->first);
      }
   }

/*RoGa   (void) ReleaseMutex(entryMutexHandle);RoGa*/
	// --- End of critical section ---

}

 void ACS_CS_EntryCounter::clear ()
{

   // Open mutex to make function thread safe
/*RoGa   HANDLE mutexHandle = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ACS_CS_NS::MUTEX_NAME_ENTRYCOUNTER_PROTECTION);RoGa*/

   // --- Critical section ---
/*RoGa   (void) WaitForSingleObject(mutexHandle, ACS_CS_NS::Mutex_Timeout);RoGa*/

   nameSet.clear();

/*RoGa   (void) ReleaseMutex(mutexHandle);
	// --- End of critical section ---

   if (mutexHandle)
      (void) CloseHandle(mutexHandle);
RoGa*/
}

// Additional Declarations

