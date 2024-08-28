//	Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_TableEntry_h
#define ACS_CS_TableEntry_h 1

#include "ACS_CS_Protocol.h"

#include <vector>

class ACS_CS_Attribute;

using std::istream;
using std::ostream;

class ACS_CS_TableEntry 
{
    public:

        ACS_CS_TableEntry();

        ACS_CS_TableEntry(const ACS_CS_TableEntry &right);

        ACS_CS_TableEntry(unsigned short identifier);

        virtual ~ACS_CS_TableEntry();

        ACS_CS_TableEntry & operator=(const ACS_CS_TableEntry &right);

        int operator<(const ACS_CS_TableEntry &right) const;

        int operator>(const ACS_CS_TableEntry &right) const;

        int operator<=(const ACS_CS_TableEntry &right) const;

        int operator>=(const ACS_CS_TableEntry &right) const;

        friend ostream& operator<<(ostream &stream, const ACS_CS_TableEntry &right);

        friend istream & operator>>(istream &stream, ACS_CS_TableEntry &object);

        unsigned short getId() const;

        void setId(unsigned short identifier);

        ACS_CS_Attribute getValue(ACS_CS_Protocol::CS_Attribute_Identifier identifier) const;

        void setValue(const ACS_CS_Attribute &attribute);

        void removeValue(ACS_CS_Protocol::CS_Attribute_Identifier identifier);

        bool hasValue(ACS_CS_Protocol::CS_Attribute_Identifier identifier) const;

        bool isGood() const;

        void getValueVector(std::vector<ACS_CS_Attribute> &valueVector) const;

        size_t getValueLength();

        int operator==(const ACS_CS_TableEntry &right) const;

    private:

        int operator!=(const ACS_CS_TableEntry &right) const;

    private:

        short unsigned entryId;

        std::set<ACS_CS_Attribute *> attributeSet;

        bool good;

        std::string fileMutexName;
};


#endif
