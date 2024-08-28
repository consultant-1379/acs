

//	© Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_EntryCounter_h
#define ACS_CS_EntryCounter_h 1



#include <map>
#include <set>
#include <vector>
#include <string>




class ACS_CS_TableEntry;
class ACS_CS_API_Util_Implementation;







class ACS_CS_EntryCounter 
{

   typedef std::set<unsigned short> entrySet;
   typedef std::map<unsigned short, entrySet *> entryMap;


  public:
      ACS_CS_EntryCounter();

      virtual ~ACS_CS_EntryCounter();


      void addEntry (unsigned short sysId, unsigned short entryId);

      void removeEntry (unsigned short sysId, unsigned short entryId);

      bool containsEntry (unsigned short sysId) const;

      static unsigned short getSysId (const ACS_CS_TableEntry *tableEntry);

      static bool addToNameSet (std::string name);

      static void removeFromNameSet (std::string name);

      void getEntryVector (std::vector<unsigned short> &entryVector);

      static void clear ();

    // Additional Public Declarations

  protected:
    // Additional Protected Declarations

  private:
      ACS_CS_EntryCounter(const ACS_CS_EntryCounter &right);

      ACS_CS_EntryCounter & operator=(const ACS_CS_EntryCounter &right);

    // Additional Private Declarations

    // Data Members for Class Attributes

       entryMap *sysIdMap;

//RoGa        HANDLE entryMutexHandle;

      static  std::set<std::string> nameSet;

      static  ACS_CS_API_Util_Implementation *utilImpl;

    // Additional Implementation Declarations

};


// Class ACS_CS_EntryCounter 



#endif
