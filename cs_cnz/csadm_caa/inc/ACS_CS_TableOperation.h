/*
 * @file ACS_CS_TableOperation.h
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

#ifndef ACS_CS_TableOperation_h
#define ACS_CS_TableOperation_h 1

#include <map>
#include <vector>

#include <stdint.h>

#include "ACS_CC_Types.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_Internal_Table.h"

// Forward declarations
class ACS_CS_Attribute;

// Type definitions
typedef uint64_t TOid;

namespace ACS_CS_TableOperation_NS
{

typedef enum {
    Unspecified = 0,
    Create      = 1,
    Modify      = 2,
    Delete      = 3
} ACS_CS_TableOperationType_t;

typedef std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*> AttributeTable;
typedef std::map<ACS_CS_TableOperationType_t, ACS_CS_Table*> EntryTable;
};


using ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType;
using ACS_CS_TableOperation_NS::ACS_CS_TableOperationType_t;
using ACS_CS_TableOperation_NS::AttributeTable;
using ACS_CS_TableOperation_NS::EntryTable;

typedef struct {
    AttributeTable attributeTable;	// Map with ACS_CS_Attribute instances that should be affected
    EntryTable entryTable;			// Map with ACS_CS_TableEntry instances that should be affected
} TableOperationData;


//
// Macro used for initializing a stl::vector with an ordinary array
// Should be passed into the constructor of the vector, e.g. std::vector<int> s(arr_vlist(int_array))
//
#define arr_vlist(x)        x, x + (sizeof(x) / sizeof(*x))

class ACS_CS_TableOperationEntry
{
    public:
        // Map with table type and then the TableEntry
        std::map<uint64_t, TableOperationData*> tableEntries;

        bool good;

        ACS_CS_TableOperationEntry();

        ACS_CS_TableOperationEntry(const ACS_CS_TableOperationEntry &right);

        ~ACS_CS_TableOperationEntry();

        int operator==(const ACS_CS_TableOperationEntry &right) const;

        bool hasOperationAttributes(CS_INTERNAL_API_TableType tableType,
        		ACS_CS_TableOperationType_t operationType,
        		std::vector<ACS_CS_Protocol::CS_Attribute_Identifier> &attr);

        bool isGood() const {return good;}

        friend ostream& operator<<(std::ostream &stream, const ACS_CS_TableOperationEntry &right);

		friend istream & operator>>(std::istream &stream, ACS_CS_TableOperationEntry &right);

    private:
		// To prevent use
		ACS_CS_TableOperationEntry & operator=(const ACS_CS_TableOperationEntry &right);
};


class ACS_CS_TableOperation
{
    // Data types
    public:

        typedef std::map<uint64_t, ACS_CS_TableOperationEntry*> TableOperation;


    // Public methods
    public:

        void addCreateOperationAttribute(TOid operationId, const char *parentname, CS_INTERNAL_API_TableType tableType, ACS_APGCC_AttrValues **attr);

        void addModifyOperationAttribute(TOid operationId, const char *objectRdn, CS_INTERNAL_API_TableType tableType,
                ACS_APGCC_AttrModification **attr);

        void addDeleteOperationAttribute(TOid operationId, const char *objectRdn, CS_INTERNAL_API_TableType tableType);

        void addOperationAttribute(TOid operationId, ACS_CS_TableOperationType_t operationType,
        		const char *objectRdn, CS_INTERNAL_API_TableType tableType, ACS_APGCC_AttrValues **attr);

        ACS_CS_TableOperationEntry* getOperation(TOid operationId);

        void disposeOperation(TOid operationId);

    // Private members
    private:

        void parseRdnData(const char *objectRdn, int &attributeid, int &entryId);

        TableOperation operations;

        TableOperationData* getTableOperation(TOid operationId, CS_INTERNAL_API_TableType tableType);

};

#endif // ACS_CS_TableOperation_h
