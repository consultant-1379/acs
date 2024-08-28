//	© Copyright Ericsson AB 2007. All rights reserved.

#ifndef ACS_CS_TableSearch_h
#define ACS_CS_TableSearch_h 1

#include <vector>

// ACS_CS_Attribute
#include "ACS_CS_Attribute.h"

class ACS_CS_Attribute;

class ACS_CS_TableSearch
{
    public:
        ACS_CS_TableSearch();

        virtual ~ACS_CS_TableSearch();

        int getAttributeCount() const;

        int getEntryCount() const;

        const ACS_CS_Attribute * getAttribute(int index) const;

        int getEntryList(unsigned short *array, int size) const;

        int setAttributeArray(const ACS_CS_Attribute **array, int size);

        int setAttribute(const ACS_CS_Attribute &attribute);

        int setEntryList(const unsigned short *array, int size);

    private:

        ACS_CS_TableSearch(const ACS_CS_TableSearch &right);

        ACS_CS_TableSearch & operator=(const ACS_CS_TableSearch &right);

    private:
        std::vector<ACS_CS_Attribute *> *attributeVector;

        short unsigned *entryList;

        int entryCount;

        int attributeCount;
};

#endif
