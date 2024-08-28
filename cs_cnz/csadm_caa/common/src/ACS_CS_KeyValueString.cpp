/*
 * @file ACS_CS_KeyValueString.cpp
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

#include "ACS_CS_KeyValueString.h"


//
// Split a string into a key value format
// e.g.
//
// str = "Hello=1,Active=true,Running=false"
// delim = ","
//
// Result is a vector with the items in KeyValue
//   Hello   1
//   Active  true
//   Running false
//
std::vector<KeyValuePair> ACS_CS_KeyValueString::splitKeyValueStrByDelim(const char *str, char delim)
{
    KeyValuePair kvp;
    std::vector<KeyValuePair> values;

    if (!str || !delim) {
        return values;
    }

    const std::string tmpStr = std::string(str);
    std::vector<std::string> names = split(tmpStr, delim);

    for (size_t i = 0; i < names.size(); i++) {
        // split string in the format attribute=value and put them into a vector
        std::vector<std::string> tmp = split(names[i], '=');

        kvp.first = tmp[0];  // attribute
        kvp.second = tmp[1]; // value

        values.push_back(kvp);
    }

    return values;
}


std::vector<std::string> & ACS_CS_KeyValueString::split(const std::string &s, char delim,
        std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> ACS_CS_KeyValueString::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

