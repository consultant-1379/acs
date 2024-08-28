/*
 * @file ACS_CS_TableOperation.cpp
 * @author xmikhal
 * @date dec 13, 2010
 *
 *      COPYRIGHT Ericsson AB, 2010
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 */

#include <vector>

#include "ACS_CS_Util.h"
#include "ACS_CS_Attribute.h"
#include "ACS_CS_KeyValueString.h"
#include "ACS_CS_TableOperation.h"

#include "ACS_CS_Trace.h"
ACS_CS_Trace_TDEF(ACS_CS_TableOperation_TRACE);

// Constants
const char *ENTRY_ID_NAME = "entryId";
const char *VALUE_DATA_NAME = "value";
const char *ATTRIBUTE_ID_NAME = "attributeId";

ACS_CS_TableOperationEntry::ACS_CS_TableOperationEntry():
		good(true)
{}


ACS_CS_TableOperationEntry::ACS_CS_TableOperationEntry(const ACS_CS_TableOperationEntry &right)
{
	this->good = right.good;

	std::map<uint64_t, TableOperationData*>::const_iterator it;

	for (it = right.tableEntries.begin(); it != right.tableEntries.end(); ++it)
	{
		uint64_t rightTableType = it->first;
		TableOperationData * rightData = it->second;

		if (rightData)
		{
			TableOperationData * newData = new TableOperationData();

			std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator tableIt;

			for (tableIt = rightData->attributeTable.begin(); tableIt != rightData->attributeTable.end(); ++tableIt)
			{
				ACS_CS_TableOperationType_t rightOperationType = tableIt->first;
				ACS_CS_Table * rightTable = tableIt->second;

				if (rightTable)
				{
					newData->attributeTable[rightOperationType] = new ACS_CS_Table(*rightTable);
				}
			}

			for (tableIt = rightData->entryTable.begin(); tableIt != rightData->entryTable.end(); ++tableIt)
			{
				ACS_CS_TableOperationType_t rightOperationType = tableIt->first;
				ACS_CS_Table * rightTable = tableIt->second;

				if (rightTable)
				{
					newData->entryTable[rightOperationType] = new ACS_CS_Table(*rightTable);
				}
			}


			this->tableEntries[rightTableType] = newData;
		}
	}
}

ACS_CS_TableOperationEntry::~ACS_CS_TableOperationEntry()
{
	std::map<uint64_t, TableOperationData*>::iterator it;

	for (it = tableEntries.begin(); it != tableEntries.end(); ++it)
	{
		TableOperationData * operationData = it->second;

		if (operationData)
		{
			std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator it;

			for (it = operationData->attributeTable.begin(); it != operationData->attributeTable.end(); ++it)
			{
				if (it->second)
					delete it->second;
			}

			for (it = operationData->entryTable.begin(); it != operationData->entryTable.end(); ++it)
			{
				if (it->second)
					delete it->second;
			}

			delete operationData;
		}
	}
}


bool ACS_CS_TableOperationEntry::hasOperationAttributes(CS_INTERNAL_API_TableType tableType,
		ACS_CS_TableOperationType_t operationType,
        std::vector<ACS_CS_Protocol::CS_Attribute_Identifier> &attr)
{
    bool result = true;

    TableOperationData * data = tableEntries[tableType];

    if (!data)
		return false;   // Invalid table type

    ACS_CS_Table * table = data->attributeTable[operationType];

    // Check that there is an entry with all the specified attributes
    if (!table)
        return false;   // Invalid operation type

    // Get data for this scope
    ACS_CS_TableSearch tableSearch;
    // Search for all entries for this scope
    (void) table->search(tableSearch);
    int entryCount = tableSearch.getEntryCount();

    if (entryCount == 0)
    	return false;

    unsigned short * entryList = new unsigned short[entryCount];

    // Get list with all entry ids
    tableSearch.getEntryList(entryList, entryCount);

    // Loop through list of entry ids
    for (int entryNo = 0; entryNo < entryCount; entryNo++)
    {
    	// Get table entry with this id
    	unsigned short entryId = entryList[entryNo];
    	ACS_CS_TableEntry entry = table->getEntry(entryId);

    	result = true;

    	// Loop through attribute list and check if they are all present
		for (size_t i = 0;i < attr.size(); i++)
		{
			ACS_CS_Protocol::CS_Attribute_Identifier tmp = attr[i];

			if (entry.getValue(tmp).getIdentifier() == ACS_CS_Protocol::Attribute_NotSpecified) {
				// Attribute could not be found, exit
				result = false;
				break;
			}
		}
    }

    if (entryList)
    	delete [] entryList;

    return result;
}


void ACS_CS_TableOperation::addModifyOperationAttribute(TOid operationId, const char *objectRdn,
		CS_INTERNAL_API_TableType tableType, ACS_APGCC_AttrModification **attr)
{
    ACS_APGCC_AttrValues **attrArray = new ACS_APGCC_AttrValues*[2];

    for (size_t i = 0;attr[i] != NULL; i++) {
        attrArray[0] = &attr[i]->modAttr;
        attrArray[1] = NULL;

        //this->addCreateOperationAttribute(operationId, objectRdn, tableType, attrArray);

        addOperationAttribute(operationId, ACS_CS_TableOperation_NS::Modify, objectRdn,
			tableType, attrArray);
    }

    delete[] attrArray;
}


ACS_CS_TableOperationEntry* ACS_CS_TableOperation::getOperation(TOid operationId)
{
    TableOperation::iterator it = operations.find(operationId);

    if (it == operations.end()) {

    	ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
				"ACS_CS_TableOperation::getOperation()\n"
   				"Could not find an operation with operationId = %d, map size = %d",
   				operationId, operations.size()));

   		return NULL;    // Could not find an operation with that id
   	}

    return it->second;
}


void ACS_CS_TableOperation::disposeOperation(TOid operationId)
{
    TableOperation::iterator it = operations.find(operationId);

    if (it != operations.end())
    {
        ACS_CS_TableOperationEntry *tableOpEntry = it->second;

        std::map<uint64_t, TableOperationData*>::iterator dataIt = tableOpEntry->tableEntries.begin();

        for (;dataIt != tableOpEntry->tableEntries.end(); ) {

			TableOperationData * operationData = dataIt->second;

			if (operationData)
			{
				std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator mapIt;

				for (mapIt = operationData->attributeTable.begin(); mapIt != operationData->attributeTable.end(); ++mapIt)
				{
					if (mapIt->second)
						delete mapIt->second;
				}

				for (mapIt = operationData->entryTable.begin(); mapIt != operationData->entryTable.end(); ++mapIt)
				{
					if (mapIt->second)
						delete mapIt->second;
				}

				delete operationData;
				dataIt->second = 0;
				tableOpEntry->tableEntries.erase(dataIt++);

			}
			else
			{
				++dataIt;
			}
        }

        operations.erase(it);
    }
}


TableOperationData* ACS_CS_TableOperation::getTableOperation(TOid operationId, CS_INTERNAL_API_TableType tableType)
{
    TableOperation::iterator it;

    it = operations.find(operationId);

    if (it == operations.end()) {
        // Operation does not exist, create it!
        ACS_CS_TableOperationEntry *tableOpEntry = new ACS_CS_TableOperationEntry();

        tableOpEntry->tableEntries[tableType] = new TableOperationData();

        operations.insert(std::pair<uint64_t,ACS_CS_TableOperationEntry*>(operationId, tableOpEntry));

        return tableOpEntry->tableEntries[tableType];
    }

    // Found the table operation id, check if the scope can be found
    ACS_CS_TableOperationEntry *tableOpEntry = it->second;

    TableOperationData *tableOpData = tableOpEntry->tableEntries[tableType];

    if (tableOpData == NULL) {
        // Not allocated for this scope yet
        tableOpData = new TableOperationData;
        tableOpEntry->tableEntries[tableType] = tableOpData;
    }

    return tableOpData;
}

void ACS_CS_TableOperation::addDeleteOperationAttribute(TOid operationId, const char *objectRdn, CS_INTERNAL_API_TableType tableType)
{
	ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
		"ACS_CS_TableOperation::addDeleteOperationAttribute()\n"
		"operationId = %d, objectRdn = %s, tableType = %d",
		operationId, objectRdn ? objectRdn : "0", tableType));

//	int attributeId = -1, entryId = -1;
//
//	 // Parse the rdn object data if any
//	parseRdnData(objectRdn, attributeId, entryId);
//
//	// Fetch the operation entry
//	TableOperationData *tableOpData = this->getTableOperation(operationId, tableType);
//
//	if (entryId != -1)	// We should always have an entryId
//	{
//		// If it is the entry itself that should be deleted, attributeId will not be present (i.e. == -1)
//		// If attributeId is present (i.e. != -1), then it is the attribute not the entry that should be
//		// deleted
//		if (attributeId == -1)
//		{
//			// Entry should be deleted
//
//			ACS_CS_Table * entriesToDeleteTable = tableOpData->entryTable[ACS_CS_TableOperation_NS::Delete];
//
//			if (!entriesToDeleteTable)
//			{
//				entriesToDeleteTable = new ACS_CS_Table();
//				tableOpData->entryTable[ACS_CS_TableOperation_NS::Delete] = entriesToDeleteTable;
//			}
//
//			if ( ! entriesToDeleteTable->containsEntry(entryId))
//			{
//				ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
//					"ACS_CS_TableOperation::addDeleteOperationAttribute()\n"
//					"Adding entry %d", entryId));
//
//				ACS_CS_TableEntry entry(entryId);
//
//				// When deleting an object, we have only the object name.
//				// The attributes need to be fetched from IMM
//				ACS_CS_INTERNAL_API::ACS_CS_Internal_Table * internalTable =
//						ACS_CS_INTERNAL_API::ACS_CS_Internal_Table::createTableInstance();
//
//				std::vector<ACS_CS_Attribute> attributes;
//
//				bool result = internalTable->getAttributes(tableType, entryId, attributes);
//
//				delete internalTable;
//
//				if (result)
//				{
//					std::vector<ACS_CS_Attribute>::iterator it;
//
//					for (it = attributes.begin(); it != attributes.end(); ++it)
//					{
//						entry.setValue(*it);
//					}
//				}
//				else
//				{
//					ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
//						"ACS_CS_TableOperation::addDeleteOperationAttribute()\n"
//						"Error: Failed to get attributes for entry %s", objectRdn));
//				}
//
//				//tableOpEntry->table.addEntry(entry);
//				entriesToDeleteTable->addEntry(entry);
//			}
//
//			// If there are attributes that have been requested to be removed,
//			// we can ignore those requests since the whole TableEntry will be removed
//			ACS_CS_Table * attributesToDeleteTable = tableOpData->attributeTable[ACS_CS_TableOperation_NS::Delete];
//
//			if (attributesToDeleteTable && attributesToDeleteTable->containsEntry(entryId))
//			{
//				attributesToDeleteTable->removeEntry(entryId);
//			}
//		}
//		else
//		{
//			// Attribute should be deleted
//
//			// Check for valid attribute
//			if (ACS_CS_Protocol::CS_ProtocolChecker::checkAttribute(attributeId))
//			{
//				ACS_CS_Protocol::CS_Attribute_Identifier id =
//						static_cast<ACS_CS_Protocol::CS_Attribute_Identifier> (attributeId);
//
//				// First see if there is an entry being deleted in this session
//				ACS_CS_Table * entriesTable = tableOpData->entryTable[ACS_CS_TableOperation_NS::Delete];
//
//				if (entriesTable && entriesTable->containsEntry(entryId))
//				{
//					// Do nothing, we will remove the whole entry so need to do anything
//					// with the attributes individually
//				}
//				else
//				{
//					// No entry i.e. only attributes deleted
//
//					ACS_CS_Table * attributesToDeleteTable = tableOpData->attributeTable[ACS_CS_TableOperation_NS::Delete];
//
//					if (!attributesToDeleteTable)
//					{
//						attributesToDeleteTable = new ACS_CS_Table();
//						tableOpData->attributeTable[ACS_CS_TableOperation_NS::Delete] = attributesToDeleteTable;
//					}
//
//					if ( ! attributesToDeleteTable->containsEntry(entryId))
//					{
//						// Entry doesn't exist, create it and then add the attribute to delete
//
//						ACS_CS_TableEntry entry(entryId);
//						ACS_CS_Attribute attribute(id);
//						char buffer[1] = {0};			// Dummy value
//						attribute.setValue(buffer, 1);
//						entry.setValue(attribute);
//						attributesToDeleteTable->addEntry(entry);
//					}
//					else
//					{
//						// Entry already exist, just add the attribute to delete
//
//						ACS_CS_Attribute attribute(id);
//						char buffer[1] = {0};			// Dummy value
//						attribute.setValue(buffer, 1);
//						attributesToDeleteTable->setValue(entryId, attribute);
//					}
//				}
//			}
//		}
//	}
}

void ACS_CS_TableOperation::addCreateOperationAttribute(TOid operationId, const char *parentname, CS_INTERNAL_API_TableType tableType,
		ACS_APGCC_AttrValues **attr)
{
	addOperationAttribute(operationId, ACS_CS_TableOperation_NS::Create, parentname,
			tableType, attr);
}

void ACS_CS_TableOperation::addOperationAttribute(TOid operationId, ACS_CS_TableOperationType_t operationType,
        		const char *objectRdn, CS_INTERNAL_API_TableType tableType, ACS_APGCC_AttrValues **attr)
{

	ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
			"ACS_CS_TableOperation::addOperationAttribute()\n"
			"operationId = %d, operationType = %d, objectRdn = %s, tableType = %d",
			operationId, operationType, objectRdn ? objectRdn : "0", tableType));

    char *attributeData = NULL;
    size_t attributeDataLength = 0;
    int attributeId = -1, entryId = -1;

    // Default to creating/modifying ACS_CS_TableEntry
    bool createEntry = true;

    // Parse the rdn object data if any
    parseRdnData(objectRdn, attributeId, entryId);

    // If parent name contains entryId, it is an ACS_CS_Attribute we should create/modify,
	// hence createEntry = false
	if (entryId != -1)
		createEntry = false;

    // Fetch the operation entry
    TableOperationData *tableOpData = this->getTableOperation(operationId, tableType);

    // Go through all the attributes
    for (size_t i = 0;attr[i] != NULL; i++)
    {
        string type = attr[i]->attrName;

        if (type == ATTRIBUTE_ID_NAME || type == ENTRY_ID_NAME)
        {
            string tmp;
            SaNameT *name = (SaNameT*) attr[i]->attrValues[0];

            // Extract the Id (could be entryId or the attributeId)
            char *ptr = strchr(reinterpret_cast<char*>(name->value), '=');

            if (ptr != NULL) {
                ptr++;  // Skip the '=' character

                uint16_t tmpId = atoi(ptr);

                if (type == ATTRIBUTE_ID_NAME)
                {
                    attributeId = tmpId;
                }
                else
                {
                    entryId = tmpId;
                }
            }
        }
        // ACS_CS_Attribute data
        else if (type == VALUE_DATA_NAME)
        {
            SaAnyT *ptr = (SaAnyT*) attr[i]->attrValues[0];

            attributeDataLength = ptr->bufferSize;

            if(attributeData) {
            	delete attributeData;
            	attributeData = NULL;
            }

            attributeData = new char[attributeDataLength];
            memcpy(attributeData, ptr->bufferAddr, attributeDataLength);
        }
    }

    if (createEntry)
    {
    	// Entry should be created/modified

    	ACS_CS_Table * entriesTable = tableOpData->entryTable[operationType];

		if (!entriesTable)
		{
			entriesTable = new ACS_CS_Table();
			tableOpData->entryTable[operationType] = entriesTable;
		}

		if (! entriesTable->containsEntry(entryId))
		{
			ACS_CS_TableEntry entry(entryId);
			entriesTable->addEntry(entry);
		}
    }
    else
    {
    	// Attribute should be created/modified

    	// Make sure that the attribute is real!
		if (ACS_CS_Protocol::CS_ProtocolChecker::checkAttribute(attributeId))
		{
			ACS_CS_Attribute attribute((ACS_CS_Protocol::CS_Attribute_Identifier) attributeId);
			attribute.setValue(attributeData, attributeDataLength);

			// First see if there is an entry being created/modified in this session
			ACS_CS_Table * entriesTable = tableOpData->entryTable[operationType];

			if (entriesTable && entriesTable->containsEntry(entryId))
			{
				entriesTable->setValue(entryId, attribute);
			}
			else
			{
			// No entry i.e. only attributes created/modified

				ACS_CS_Table * attributesTable = tableOpData->attributeTable[operationType];

				if (!attributesTable)
				{
					attributesTable = new ACS_CS_Table();
					tableOpData->attributeTable[operationType] = attributesTable;
				}

				if ( ! attributesTable->containsEntry(entryId))
				{
					// Entry doesn't exist, create it and then add the attribute to create

					ACS_CS_TableEntry entry(entryId);
					entry.setValue(attribute);
					attributesTable->addEntry(entry);
				}
				else
				{
					// Entry already exist, just add the attribute to create
					attributesTable->setValue(entryId, attribute);
				}
			}
		}
		else
		{
			ACS_CS_TRACE((ACS_CS_TableOperation_TRACE,
				"ACS_CS_TableOperation::addOperationAttribute()\n"
				"Error: unknown attribute (%d)", attributeId));
		}
    }

    if (attributeData)
        delete [] attributeData;
}


void ACS_CS_TableOperation::parseRdnData(const char *objectRdn, int &attributeid, int &entryId)
{
    std::vector<KeyValuePair> values = ACS_CS_KeyValueString::splitKeyValueStrByDelim(objectRdn, ',');

    for (size_t i = 0;i < values.size(); i++)
    {
        string key = values[i].first;
        string value = values[i].second;

        if (key == ATTRIBUTE_ID_NAME)
        {
            attributeid = atoi(value.c_str());
        }
        else if (key == ENTRY_ID_NAME)
        {
            entryId = atoi(value.c_str());
        }
    }
}




int ACS_CS_TableOperationEntry::operator==(const ACS_CS_TableOperationEntry &right) const
{
	int result = 1;

	int leftSize = 0, rightSize = 0;

	// count non-null entries (the tableEntries map can include null-entries)
	std::map<uint64_t, TableOperationData*>::const_iterator it;
	for (it = this->tableEntries.begin(); it != this->tableEntries.end(); ++it)
	{
		if (it->second)
			leftSize++;
	}

	for (it = right.tableEntries.begin(); it != right.tableEntries.end(); ++it)
	{
		if (it->second)
			rightSize++;
	}

	if (leftSize == rightSize)
	{
		std::map<uint64_t, TableOperationData*>::const_iterator leftIt;

		for (leftIt = this->tableEntries.begin(); leftIt != this->tableEntries.end(); ++leftIt)
		{
			uint64_t leftKey = leftIt->first;
			TableOperationData * leftData = leftIt->second;

			if (leftData)
			{
				std::map<uint64_t, TableOperationData*>::const_iterator rightIt = right.tableEntries.find(leftKey);

				if (rightIt != right.tableEntries.end())
				{
					TableOperationData * rightData = rightIt->second;

					if (rightData)
					{
						if ( (leftData->attributeTable.size() == rightData->attributeTable.size())
						&& (leftData->entryTable.size() == rightData->entryTable.size()))
						{
							std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator leftTableIt;

							for (leftTableIt = leftData->attributeTable.begin();
									leftTableIt != leftData->attributeTable.end(); ++leftTableIt)
							{
								ACS_CS_TableOperationType_t operationType = leftTableIt->first;
								ACS_CS_Table * leftTable = leftTableIt->second;

								std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator rightTableIt;

								rightTableIt = rightData->attributeTable.find(operationType);

								ACS_CS_Table * rightTable = rightTableIt->second;

								if (leftTable && rightTable)
								{
									if ( !  (*leftTable == *rightTable) )
									{
										result = 0;
										break;
									}
								}
							}

							if (!result)
								break;
						}
					}
				}
				else
				{
					result = 0;
					break;
				}
			}
		}
	}
	else
	{
		result = 0;
	}

	return result;
}


ostream & operator<<(ostream &stream, const ACS_CS_TableOperationEntry &right)
{
	size_t size = right.tableEntries.size();

	// Write number of table entries
	(void) stream.write( reinterpret_cast<const char *> (&size), sizeof(size) );

	std::map<uint64_t, TableOperationData*>::const_iterator it;

	// Loop through table entries and write all of them to stream
	for (it = right.tableEntries.begin(); it != right.tableEntries.end(); ++it)
	{
		uint64_t tableType = it->first;
		TableOperationData * data = it->second;

		if (!data)
			continue;

		// Write type of table
		(void) stream.write( reinterpret_cast<const char *> (&tableType), sizeof(tableType) );

		size = data->attributeTable.size();

		// Write number of attributes
		(void) stream.write( reinterpret_cast<const char *> (&size), sizeof(size) );

		std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*>::iterator dataIt;

		for (dataIt = data->attributeTable.begin(); dataIt != data->attributeTable.end(); ++dataIt)
		{
			int operationType = static_cast<int>(dataIt->first);
			ACS_CS_Table * table = dataIt->second;

			if (!table)
				continue;

			// Write type of operation
			(void) stream.write( reinterpret_cast<const char *> (&operationType), sizeof(operationType) );

			// Write table
			stream << *table;
		}

		size = data->entryTable.size();

		// Write number of entries
		(void) stream.write( reinterpret_cast<const char *> (&size), sizeof(size) );

		for (dataIt = data->entryTable.begin(); dataIt != data->entryTable.end(); ++dataIt)
		{
			int operationType = static_cast<int>(dataIt->first);
			ACS_CS_Table * table = dataIt->second;

			if (!table)
				continue;

			// Write type of operation
			(void) stream.write( reinterpret_cast<const char *> (&operationType), sizeof(operationType) );

			// Write table
			stream << *table;
		}
	}

	return stream;

}

istream & operator>>(istream &stream, ACS_CS_TableOperationEntry &right)
{
	right.good = true;

	size_t size = 0;

	// Read number of table entries
	(void) stream.read( reinterpret_cast<char *> (&size), sizeof(size) );

	for (size_t i = 0; i < size; i++)
	{

		TableOperationData * data = new TableOperationData;
		uint64_t tableType = 0;

		(void) stream.read( reinterpret_cast<char *> (&tableType), sizeof(tableType) );

		size_t attributeSize = 0;
		// Read number of attributes
		(void) stream.read( reinterpret_cast<char *> (&attributeSize), sizeof(attributeSize) );

		for (size_t j = 0; j < attributeSize; j++)
		{
			int operationType = 0;
			(void) stream.read( reinterpret_cast<char *> (&operationType), sizeof(operationType) );

			ACS_CS_Table table;
			stream >> table;

			if (table.isGood())
			{
				data->attributeTable[(ACS_CS_TableOperationType_t) operationType] = new ACS_CS_Table(table);
			}
		}

		size_t entrySize = 0;
		// Read number of entries
		(void) stream.read( reinterpret_cast<char *> (&entrySize), sizeof(entrySize) );

		for (size_t k = 0; k < entrySize; k++)
		{
			int operationType = 0;
			(void) stream.read( reinterpret_cast<char *> (&operationType), sizeof(operationType) );

			ACS_CS_Table table;
			stream >> table;

			if (table.isGood())
			{
				data->entryTable[(ACS_CS_TableOperationType_t) operationType] = new ACS_CS_Table(table);
			}

		}

		right.tableEntries[tableType] = data;
	}

	return stream;

}
