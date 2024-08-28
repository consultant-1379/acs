//  Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_Table_h
#define ACS_CS_Table_h 1

#include <map>
#include <vector>
#include <fstream>

#include "ACS_CS_Protocol.h"
//#include "ACS_CS_API_Util_Internal.h"

// ACS_CS_TableSearch
#include "ACS_CS_TableSearch.h"
#include "ACS_CS_Internal_Table.h"


//class ACS_CS_TableSearch;
class ACS_CS_TableEntry;
class ACS_CS_Attribute;
class ACS_CS_ReaderWriterLock;

class ACS_CS_Table
{
    typedef std::map<unsigned short, ACS_CS_TableEntry *> entryMap;
    static const unsigned long CRC_POLYNOMIAL = 0xEDB88320;

    public:
        ACS_CS_Table();

        ACS_CS_Table(const ACS_CS_Table &right);

        virtual ~ACS_CS_Table();

        ACS_CS_Table & operator=(const ACS_CS_Table &right);

        int operator==(const ACS_CS_Table &right) const;

        void addEntry(const ACS_CS_TableEntry &entry);

        ACS_CS_TableEntry getEntry(unsigned short entryId) const;

        int removeEntry(unsigned short entryId);

        bool containsEntry(unsigned short entryId) const;

        ACS_CS_Attribute getValue(unsigned short entryId,
                ACS_CS_Protocol::CS_Attribute_Identifier attributeId) const;

        int setValue(unsigned short entryId, const ACS_CS_Attribute &attribute);

        void removeValue(unsigned short entryId,
                ACS_CS_Protocol::CS_Attribute_Identifier attributeId);

        unsigned short getLogicalClock() const;

        void setLogicalClock(unsigned short clock);

        int getSize() const;

        int search(ACS_CS_TableSearch &searchObject) const;

        void clearTable();

//        bool save (ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType tableType) const;
//
//        bool load(std::string fileName);

        void setTable(std::vector<ACS_CS_TableEntry *> tableVector);

        entryMap* getTable() {return tableMap;}

        friend ostream & operator<<(ostream &stream,const ACS_CS_Table &right);

		friend istream & operator>>(istream &stream, ACS_CS_Table &object);

		bool isGood() const;

		void print();

		ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType getTableType() const {return tableType;}

		void setTableType(ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType tableType) {this->tableType = tableType;}

    private:

        void calcCrc(unsigned char byteValue, unsigned long &crc) const;

    private:

        entryMap *tableMap;

        short unsigned logicalClock;

        long unsigned *crcTable;

        bool good;

        ACS_CS_INTERNAL_API::CS_INTERNAL_API_TableType tableType;

        ACS_CS_ReaderWriterLock *lock;
};

#endif
