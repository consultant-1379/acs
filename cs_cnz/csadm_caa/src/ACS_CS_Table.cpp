//  Copyright Ericsson AB 2007. All rights reserved.


#include <sstream>

#include "ACS_CS_Util.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_EventReporter.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_ExceptionHeaders.h"
#include "ACS_CS_ReaderWriterLock.h"


// ACS_CS_Table
#include "ACS_CS_Table.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_Table_TRACE);
ACS_CS_Trace_TDEF(ACS_CS_TableSave_TRACE);
ACS_CS_Trace_TDEF(ACS_CS_TableSaveLoop_TRACE);

using std::map;
using std::string;
using std::vector;
using ACS_CS_NS::ILLEGAL_TABLE_ENTRY;
using namespace ACS_CS_NS;
using namespace ACS_CS_INTERNAL_API;


// Class ACS_CS_Table 

ACS_CS_Table::ACS_CS_Table()
      : tableMap(0),
        logicalClock(0),
        crcTable(0),
        good(true),
        tableType(ACS_CS_INTERNAL_API::Table_NotSpecified),
        lock(0)
{
   lock = new ACS_CS_ReaderWriterLock();

    tableMap = new entryMap();

    // Create CRC table
    unsigned long polynomial = CRC_POLYNOMIAL;
    unsigned long crc;
    crcTable = new unsigned long[256];

    for (int i = 0; i < 256; i++)
    {
        crc = i;

        for (int j = 8; j > 0; j--)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }

        crcTable[i] = crc;
    }
}

ACS_CS_Table::ACS_CS_Table(const ACS_CS_Table &right)
      : tableMap(0),
        logicalClock(0),
        crcTable(0),
        good(true),
        tableType(ACS_CS_INTERNAL_API::Table_NotSpecified),
        lock(0)
{

   /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
      "(%t) ACS_CS_Table::ACS_CS_Table(ACS_CS_Table &)()\n"
        "Entering function\n"));*/

    logicalClock = right.logicalClock;
    good = right.good;
    tableType = right.tableType;

    tableMap = new entryMap();

   lock = new ACS_CS_ReaderWriterLock();

    // Create CRC table
    unsigned long polynomial = CRC_POLYNOMIAL;
    unsigned long crc;
    this->crcTable = new unsigned long[256];

   if (this->crcTable)
   {
       for (int i = 0; i < 256; i++)
       {
           crc = i;

           for (int j = 8; j > 0; j--)
           {
               if (crc & 1)
                   crc = (crc >> 1) ^ polynomial;
               else
                   crc >>= 1;
           }

           this->crcTable[i] = crc;
       }
   }

    // Copy MAP

    if (right.tableMap)
    {
        if (this->tableMap)
        {
            entryMap::iterator it;

            // Loop through map
            for (it = right.tableMap->begin(); it != right.tableMap->end(); ++it)
            {
                // Suppress warning for not freeing newEntry,
                // which we shouldn't since it is stored in an STL map.
                // Map and content freed in destructor
                //lint --e{429}

            ACS_CS_TableEntry * oldEntry = it->second;
                ACS_CS_TableEntry * newEntry = 0;

            if (oldEntry)
               newEntry = new ACS_CS_TableEntry( *oldEntry );

                // Add id to the map
            if (newEntry)
            {
                   this->tableMap->insert( entryMap::value_type(newEntry->getId(), newEntry) );
            }
            }
        }
    }
}


ACS_CS_Table::~ACS_CS_Table()
{
    if (tableMap)
    {
        entryMap::iterator it;

        // Loop through map
        for (it = tableMap->begin(); it != tableMap->end(); ++it)
        {
         ACS_CS_TableEntry * entry = it->second;

         if (entry)
            delete entry;       // Delete all entries
        }

        delete tableMap;            // Delete the map
    }

    if (crcTable)
        delete [] crcTable;

    if (lock)
        delete lock;
}


ACS_CS_Table & ACS_CS_Table::operator=(const ACS_CS_Table &right)
{
   // Suppress warning for not assigning mutexes and crctable
   // These objects can be re-used and need not be assigned
    //lint --e{1539}

   /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
        "(%t) ACS_CS_Table::operator=()\n"
        "Entering function\n"));*/

   if (this != &right)
   {
      ACS_CS_TRACE((ACS_CS_Table_TRACE,
                "(%t) ACS_CS_Table::operator=()\n"
                "Calling lock->start_writing()\n"));

      lock->start_writing();

      if (crcTable)
      {
           delete [] crcTable;
         crcTable = 0;
      }

       // Create CRC table
       unsigned long polynomial = CRC_POLYNOMIAL;
       unsigned long crc;
       this->crcTable = new unsigned long[256];

      if (this->crcTable)
      {
          for (int i = 0; i < 256; i++)
          {
              crc = i;

              for (int j = 8; j > 0; j--)
              {
                  if (crc & 1)
                      crc = (crc >> 1) ^ polynomial;
                  else
                      crc >>= 1;
              }

              this->crcTable[i] = crc;
          }
      }

      if (this->tableMap)
      {
         entryMap::iterator it;

        // Loop through map
           for (it = tableMap->begin(); it != tableMap->end(); ++it)
           {
            ACS_CS_TableEntry * entry = it->second;

            if (entry)
               delete entry;     // Delete all entries
           }

         this->tableMap->clear();
      }
      else
         this->tableMap = new entryMap();

      this->logicalClock = right.logicalClock;
      this->good = right.good;
      this->tableType = right.tableType;

       // Copy MAP

       if (right.tableMap)
       {
           if (this->tableMap)
           {
               entryMap::iterator it;

               // Loop through map
               for (it = right.tableMap->begin(); it != right.tableMap->end(); ++it)
               {
                   // Suppress warning for not freeing newEntry,
                   // which we shouldn't since it is stored in an STL map.
                   // Map and content freed in destructor
                   //lint --e{429}

				   ACS_CS_TableEntry * oldEntry = it->second;
					   ACS_CS_TableEntry * newEntry = 0;

				   if (oldEntry)
					  newEntry = new ACS_CS_TableEntry( *oldEntry );

					   // Add id to the map
				   if (newEntry)
				   {
						  this->tableMap->insert( entryMap::value_type(newEntry->getId(), newEntry) );
				   }
               }
           }
       }

          ACS_CS_TRACE((ACS_CS_Table_TRACE,
                "(%t) ACS_CS_Table::operator=()\n"
                "Calling lock->stop_writing()\n"));

      lock->stop_writing();
   }

    return *this;
}


int ACS_CS_Table::operator==(const ACS_CS_Table &right) const
{
	int result = 0;

	if (this->tableMap->size() == right.tableMap->size())
	{
		entryMap::const_iterator leftIt;

		for (leftIt = this->tableMap->begin(); leftIt != this->tableMap->end(); ++leftIt)
		{
			uint16_t leftKey = leftIt->first;
			ACS_CS_TableEntry * leftEntry = leftIt->second;

			if (leftEntry)
			{
				entryMap::const_iterator rightIt = right.tableMap->find(leftKey);

				if (rightIt != right.tableMap->end())
				{
					ACS_CS_TableEntry * rightEntry = rightIt->second;

					if (rightEntry)
					{
						if (leftEntry->operator ==(*rightEntry))
						{
							result = 1;
						}
						else
						{
							result = 0;
							break;
						}
					}
				}
			}
		}
	}

	return result;
}


void ACS_CS_Table::addEntry (const ACS_CS_TableEntry &entry)
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::addEntry()\n"
            "Entering function\n"));*/

    // Suppress warning for not freeing newEntry,
    // which we shouldn't since it is stored in an STL map.
    // Map and content freed in destructor
    //lint --e{429}

    if (tableMap == 0)
        return;
    else if (entry.getId() == ILLEGAL_TABLE_ENTRY)
        return;

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::addEntry()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    ACS_CS_TableEntry * newEntry = new ACS_CS_TableEntry(entry); // Create new entry

    entryMap::iterator it = tableMap->find(entry.getId()); // Search for old entry

    if (it != tableMap->end()) // if entry exists
    {
        ACS_CS_TRACE((ACS_CS_Table_TRACE,
                        "(%t) ACS_CS_Table::addEntry()\n"
                        "entry exists!!\n"));

        ACS_CS_TableEntry * oldEntry = it->second; // get pointer to old entry
        (void) tableMap->erase(it); // remove from map

        if (oldEntry)
            delete oldEntry; // delete entry

    }

    tableMap->insert(entryMap::value_type(newEntry->getId(), newEntry)); // Add id to the map

    logicalClock++; // Increase change counter

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::addEntry()\n"
                    "Calling lock->stop_writing(), logicalClock: %u, tableSize:%d\n", logicalClock, tableMap->size()));

    lock->stop_writing();
}

ACS_CS_TableEntry ACS_CS_Table::getEntry (unsigned short entryId) const
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::getEntry()\n"
            "Entering function\n"));*/

    if (!tableMap)
        return 0;

    lock->start_reading();

    ACS_CS_TableEntry * ptr = 0;
    entryMap::iterator it = tableMap->find(entryId); // Search for entry in table

    if (it != tableMap->end()) // if entry exists
    {
        ptr = it->second; // get pointer
    } else {
        ACS_CS_TRACE((ACS_CS_Table_TRACE,
                        "(%t) ACS_CS_Table::getEntry()\n"
                        "Error: cannot find entryId: %u\n", entryId));
    }

    lock->stop_reading();
    if (!ptr)
    	return ACS_CS_TableEntry(ILLEGAL_TABLE_ENTRY);
    else
    	return *ptr; // and return it

}

int ACS_CS_Table::removeEntry (unsigned short entryId)
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::removeEntry(unsigned short)\n"
            "Entering function\n"));*/

    if (tableMap == 0)
        return -1;

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::removeEntry()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    int success = -1;

    ACS_CS_TableEntry * ptr = 0;
    entryMap::iterator it = tableMap->find(entryId); // Search for entry

    if (it != tableMap->end()) // if entry exists
    {
        ptr = it->second; // get pointer
        (void) tableMap->erase(it); // remove from map

        if (ptr)
            delete ptr; // delete entry

        logicalClock++; // increase change counter
        success = 0;
    }

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::removeEntry()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();

    return success;
}

bool ACS_CS_Table::containsEntry (unsigned short entryId) const
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::containsEntry()\n"
            "Entering function\n"));*/

   if (tableMap == 0)
        return false;

    lock->start_reading();

    bool entryFound = false;

    entryMap::iterator it = tableMap->find(entryId); // Search for entry

    if (it != tableMap->end()) {
        entryFound = true;
    }

    lock->stop_reading();

    return entryFound;
}

ACS_CS_Attribute ACS_CS_Table::getValue (unsigned short entryId, ACS_CS_Protocol::CS_Attribute_Identifier attributeId) const
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::getValue()\n"
            "Entering function\n"));*/

    ACS_CS_Attribute attribute;

    if (tableMap == 0)
        return attribute;

    lock->start_reading();

    entryMap::iterator it = tableMap->find(entryId); // Search for entry

    if (it != tableMap->end()) // if entry exists
    {
        ACS_CS_TableEntry * entry = it->second;

        if (entry)
            attribute = entry->getValue(attributeId); // get attribute
    }

    lock->stop_reading();

    return attribute; // and return it
}

int ACS_CS_Table::setValue (unsigned short entryId, const ACS_CS_Attribute &attribute)
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::setValue()\n"
            "Entering function\n"));*/

    if (tableMap == 0)
        return -1;

    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::setValue()\n"
                    "Calling lock->start_writing()\n"));*/

    lock->start_writing();

    int success = -1;

    entryMap::iterator it = tableMap->find(entryId); // Search for entry

    if (it != tableMap->end()) // If entry exists
    {
        ACS_CS_TableEntry * entry = it->second;

        if (entry)
            entry->setValue(attribute); // set value

        logicalClock++; // increase change counter
        success = 0;
    }

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::setValue()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();

    return success;
}

void ACS_CS_Table::removeValue (unsigned short entryId, ACS_CS_Protocol::CS_Attribute_Identifier attributeId)
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::removeValue()\n"
            "Entering function\n"));*/

    if (tableMap == 0)
        return;

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::removeValue()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    entryMap::iterator it = tableMap->find(entryId); // Search for entry

    if (it != tableMap->end()) // If entry exists
    {
        ACS_CS_TableEntry * entry = it->second;

        if (entry)
            entry->removeValue(attributeId); // set value

        logicalClock++; // increase change counter
    }

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::removeValue()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();
}

short unsigned ACS_CS_Table::getLogicalClock () const
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::getLogicalClock()\n"
            "Entering function\n"));*/

    return logicalClock;
}

void ACS_CS_Table::setLogicalClock (unsigned short clock)
{
   /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::setLogicalClock()\n"
            "Entering function\n"));*/

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::setLogicalClock()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    this->logicalClock = clock;

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::setLogicalClock()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();
}

int ACS_CS_Table::getSize () const
{
    return tableMap != 0 ? tableMap->size() : 0;
}

int ACS_CS_Table::search (ACS_CS_TableSearch &searchObject) const
{

    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::search()\n"
            "Entering function\n"));*/

    if (tableMap == 0)
        return -1;

    lock->start_reading();

    int success = 0;

    vector<unsigned short> entryList; // List of entries that match the search

    entryMap::iterator it;

    // Loop through map
    for (it = tableMap->begin(); it != tableMap->end(); ++it) {
        ACS_CS_TableEntry * entry = it->second; // Get each entry in map
        bool match = true;

        // Loop through all attributes in the search object
        for (int i = 0; i < searchObject.getAttributeCount(); i++) {
            // Get attribute from search object
            const ACS_CS_Attribute * attribute = searchObject.getAttribute(i);

            if (attribute == 0) {
                match = false;
                break;
            }

            // Get attribute id
            ACS_CS_Protocol::CS_Attribute_Identifier id =
                    attribute->getIdentifier();

            // Use id and get atribute from entry
            ACS_CS_Attribute entryAttribute = entry->getValue(id);

            if (entryAttribute.getIdentifier()
                    == ACS_CS_Protocol::Attribute_NotSpecified) // If there is no such attribute for the entry
            {
                match = false;
                break;
            } else if (entryAttribute != *attribute) // Compare attributes
            {
                match = false;
                break;
            }
        }

        if (match) // Add id of matching entries to the vector
            entryList.push_back(entry->getId());
    }

    if (entryList.size() > 0) // If there were any matching entries
    {
        unsigned short * arr = new unsigned short[entryList.size()]; // Create array for the list

        if (arr) // Check for successful new
        {
            for (unsigned int i = 0; i < entryList.size(); i++) // Loop through vector
            {
                arr[i] = entryList[i]; // Copy each entry id to the list
            }

            int size = static_cast<int> (entryList.size());

            (void) searchObject.setEntryList(arr, size); // Pass list to search object

            delete[] arr;
        }
    }

    lock->stop_reading();

    return success;
}

void ACS_CS_Table::clearTable ()
{
    /*ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::clearTable()\n"
            "Entering function\n"));*/

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::clearTable()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    if (tableMap) {
        entryMap::iterator it;

        // Loop through map
        for (it = tableMap->begin(); it != tableMap->end(); ++it) {
            ACS_CS_TableEntry * entry = it->second;

            if (entry)
                delete entry; // Delete all entries
        }

        tableMap->clear();

        logicalClock++;
    }

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::clearTable()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();
}

/*
bool ACS_CS_Table::save (ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType tableType) const
{
    ACS_CS_TRACE((ACS_CS_TableSave_TRACE,
            "(%t) ACS_CS_Table::save()\n"
            "Entering function with table type: %d\n", tableType));

    bool success = false;
    ACS_CS_Internal_Table *internalTable = ACS_CS_Internal_Table::createTableInstance();

    // walk thru the complete map
    entryMap::iterator it = tableMap->begin();

    ACS_CS_TableEntry **entries = new ACS_CS_TableEntry*[tableMap->size()], **ptr = entries;

    for (;it != tableMap->end(); ++it) {
        *ptr = it->second;
        ptr++;
    }

    // update internal table (IMM)
    success = internalTable->updateEntries(tableType, (const ACS_CS_TableEntry**) entries, tableMap->size());

    delete internalTable;
    delete [] entries;

    if (!success) {
        ACS_CS_TRACE((ACS_CS_Table_TRACE,
                "(%t) ACS_CS_Table::save()\n"
                "updateEntries() failed on internal table"));

        return false;
    }

    ACS_CS_TRACE((ACS_CS_TableSave_TRACE,
            "(%t) ACS_CS_Table::save()\n"
            "updateEntries() completed successfully"));

    return true;
}
*/
/*
bool ACS_CS_Table::load (std::string fileName)
{
//MIHA    //## begin ACS_CS_Table::load%45D404620181.body preserve=yes
//    ACS_CS_TRACE((ACS_CS_Table_TRACE,
//        "(%t) ACS_CS_Table::load()\n"
//        "Entering function\n"));
//
//    // Combine filenames and paths
//    // Start each filename with the path
//    string fileNameSystemTmp = systemDiskPath;
//    string fileNameSystemReal = systemDiskPath;
//    string fileNameSystemBackup= systemDiskPath;
//    string fileNameDatadiskTmp = dataDiskPath;
//    string fileNameDatadiskReal = dataDiskPath;
//
//    // Append backslash
//    fileNameSystemTmp.append("\\");
//    fileNameSystemReal.append("\\");
//    fileNameSystemBackup.append("\\");
//    fileNameDatadiskTmp.append("\\");
//    fileNameDatadiskReal.append("\\");
//
//    // Append the actual filename
//    fileNameSystemTmp.append(fileName);
//    fileNameSystemReal.append(fileName);
//    fileNameSystemBackup.append(fileName);
//    fileNameDatadiskTmp.append(fileName);
//    fileNameDatadiskReal.append(fileName);
//
//    // Append the file type
//    fileNameSystemTmp.append(ACS_CS_NS::FILE_TYPE_TMP);
//    fileNameSystemReal.append(ACS_CS_NS::FILE_TYPE_STD);
//    fileNameSystemBackup.append(ACS_CS_NS::FILE_TYPE_BAK);
//    fileNameDatadiskTmp.append(ACS_CS_NS::FILE_TYPE_TMP);
//    fileNameDatadiskReal.append(ACS_CS_NS::FILE_TYPE_STD);
//
//    ACS_CS_TableSignature * signatureInstance = ACS_CS_TableSignature::getInstance();
//
//    unsigned long versionData = this->checkFileVersion(fileNameDatadiskReal);
//    unsigned long versionSignature = 0;
//
//    // If master, load from data disk if that file is newer
//    if (serviceType == ACS_CS_Protocol::ServiceType_MASTER) {
//        if (signatureInstance) versionSignature = signatureInstance->getSignature(fileName);
//
//        ACS_CS_TRACE((ACS_CS_Table_TRACE,
//            "(%t) ACS_CS_Table::load()\n"
//            "fileName: %s, versionSignature: %u, versionData: %u\n", fileName.c_str(), versionSignature, versionData));
//
//        if (versionData != versionSignature) {
//            // Try to load from data disk
//            // loadFile("DATADISK\PATH\hwctable.dat")
//            if (readFile(fileNameDatadiskReal) == true) {
//                // Save table on local system disk
//                // saveTable("SYSTEMDISK\PATH\hwctable.dat")
//                if (writeFile(fileNameSystemReal) && signatureInstance) signatureInstance->sign(versionData, fileName, serviceType);
//                return true;
//            }
//            else {
//                ACS_CS_TRACE((ACS_CS_Table_TRACE,
//                    "(%t) ACS_CS_Table::load()\n"
//                    "Error reading file from data disk (%s)\n", fileName.c_str()));
//
//                if (readFile(fileNameSystemReal) == true) return true;
//                else {
//                    ACS_CS_TRACE((ACS_CS_Table_TRACE,
//                        "(%t) ACS_CS_Table::load()\n"
//                        "Error reading file from system disk (%s)\n", fileName.c_str()));
//                    return false;
//                }
//            }
//        }
//        // TR HJ94181
//        // If data disk file and system disk file are same, load from data disk
//        // loadFile("DATADISK\PATH\hwctable.dat")
//        else {
//            // Try to load from data disk
//            // loadFile("DATADISK\PATH\hwctable.dat")
//            if (readFile(fileNameDatadiskReal) == true) {
//                // Save table on local system disk
//                // saveTable("SYSTEMDISK\PATH\hwctable.dat")
//                if (writeFile(fileNameSystemReal) && signatureInstance) signatureInstance->sign(versionData, fileName, serviceType);
//            } else {
//                ACS_CS_TRACE((ACS_CS_Table_TRACE,
//                    "(%t) ACS_CS_Table::load()\n"
//                    "Error reading file from data disk (%s)\n", fileName.c_str()));
//            }
//            // TR HJ94181
//            // If there is no data disk file, load from local system disk
//            // loadFile("SYSTEMDISK\PATH\hwctable.dat")
//            if (readFile(fileNameSystemReal) == true) {
//                return true;
//            }
//            else {
//                ACS_CS_TRACE((ACS_CS_Table_TRACE,
//                    "(%t) ACS_CS_Table::load()\n"
//                    "Error reading file from system disk (%s)\n", fileName.c_str()));
//                return false;
//            }
//        }
//    }
//    // If slave, only load from system disk
//    else {
//        // loadFile("SYSTEMDISK\PATH\hwctable.dat")
//       if (readFile(fileNameSystemReal) == true)
//       {
//           return true;
//       }
//       else
//           return false;
//   }
//
//  //## end ACS_CS_Table::load%45D404620181.bodytrue
    return true; // MIHA ADDED TO REMOVE COMPILATION WARNING
}
*/
void ACS_CS_Table::setTable (std::vector<ACS_CS_TableEntry *> tableVector)
{
   ACS_CS_TRACE((ACS_CS_Table_TRACE,
            "(%t) ACS_CS_Table::setTable()\n"
            "Entering function\n"));

    // Suppress warning for not freeing newEntry,
    // which we shouldn't since it is stored in an STL map.
    // Map and content freed in destructor
    //lint --e{429}


    // Clear the table
    clearTable();

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::set_table()\n"
                    "Calling lock->start_writing()\n"));

    lock->start_writing();

    vector<ACS_CS_TableEntry *>::iterator it;

    // Loop through vector with new entries
    for (it = tableVector.begin(); it != tableVector.end(); ++it) {
        ACS_CS_TableEntry * currentEntry = *it;

        // If valid entry
        if (currentEntry) {
            if (currentEntry->getId() != ILLEGAL_TABLE_ENTRY) {
                // Create new entry
                ACS_CS_TableEntry * newEntry = new ACS_CS_TableEntry(
                        *currentEntry);

                // Add id to the map
                tableMap->insert(entryMap::value_type(newEntry->getId(),
                        newEntry));

                logicalClock++; // Increase change counter
            }
        }
    }

    ACS_CS_TRACE((ACS_CS_Table_TRACE,
                    "(%t) ACS_CS_Table::setTable()\n"
                    "Calling lock->stop_writing()\n"));

    lock->stop_writing();
}

void ACS_CS_Table::calcCrc (unsigned char byteValue, unsigned long &crc) const
{
   // No trace in this function since that would slow it down.

   // Calculate CRC32 checksum
   if (crcTable)
       crc = ((crc) >> 8) ^ crcTable[(byteValue) ^ ((crc) & 0x000000FF)];
}

ostream & operator<<(ostream &stream, const ACS_CS_Table &right)
{

	(void) stream.write( reinterpret_cast<const char *> (&right.logicalClock), sizeof(right.logicalClock) );

	int tableType = right.tableType;
	(void) stream.write( reinterpret_cast<const char *> (&tableType), sizeof(tableType) );

	size_t tableSize = right.tableMap != 0 ? right.tableMap->size(): 0;

	(void) stream.write( reinterpret_cast<const char *> (&tableSize), sizeof(tableSize) );

	if (tableSize)
	{
		std::map<unsigned short, ACS_CS_TableEntry *>::iterator it;

		for (it = right.tableMap->begin(); it != right.tableMap->end(); ++it)
		{
			ACS_CS_TableEntry * ptr = it->second;

			if (ptr)
				stream << (*ptr);
		}
	}

	return stream;

}

istream & operator>>(istream &stream, ACS_CS_Table &object)
{

	object.clearTable();

	unsigned short logicalClock = 0;
	(void) stream.read( reinterpret_cast<char *> (&logicalClock), sizeof(logicalClock) );
	object.logicalClock = logicalClock;

	int tableType = 0;
	(void) stream.read( reinterpret_cast<char *> (&tableType), sizeof(tableType) );
	object.tableType = static_cast<ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType>(tableType);

	size_t tableSize = 0;
	(void) stream.read( reinterpret_cast<char *> (&tableSize), sizeof(tableSize) );

	object.good = true;

	for (size_t i = 0; i < tableSize; i++)
	{
		ACS_CS_TableEntry entry(ILLEGAL_TABLE_ENTRY);
		stream >> entry;

		if (entry.getId() != ILLEGAL_TABLE_ENTRY && entry.isGood())
		{
			ACS_CS_TableEntry * newEntry = new ACS_CS_TableEntry(entry);
			object.tableMap->insert(std::map<unsigned short, ACS_CS_TableEntry *>::value_type(newEntry->getId(),
					newEntry)); // Add id to the map
		}
		else
		{
			object.good = false;
			break;
		}
	}

	return stream;

}

bool ACS_CS_Table::isGood() const
{
	return good;
}

void ACS_CS_Table::print()
{
	if (tableMap)
	{
		entryMap::iterator mapIt;

		std::stringstream stream;

		stream << "[table=" << this->tableType;

		for (mapIt = tableMap->begin(); mapIt != tableMap->end(); ++mapIt)
		{
			ACS_CS_TableEntry * entry = mapIt->second;

			stream << "[entryId=" << entry->getId();

			std::vector<ACS_CS_Attribute> valueVector;

			entry->getValueVector(valueVector);

			std::vector<ACS_CS_Attribute>::iterator vecIt;

			for (vecIt = valueVector.begin(); vecIt != valueVector.end(); ++vecIt)
			{
				ACS_CS_Attribute attribute = *vecIt;
				int bufferSize = attribute.getValueLength();

				stream << "[attId=" << attribute.getIdentifier();

				if (bufferSize > 0)
				{
					char * buffer = new char[bufferSize];

					attribute.getValue(buffer, bufferSize);

					stream << ", value=" << ACS_CS_Protocol::CS_ProtocolChecker::binToString(buffer, bufferSize);
					stream << "]";

					delete [] buffer;
				}

			}

			stream << "]";
		}

		stream << "]\n";

		std::ofstream os;
		os.open("/tmp/hwctable.txt");
		os << stream.str();
		os.close();

		ACS_CS_TRACE((ACS_CS_Table_TRACE,
			"(%t) ACS_CS_Table::print()\n"
			"%s", stream.str().c_str()));
	}

}
