/*
 * @file ACS_CS_KeyValueString.h
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

#ifndef ACS_CS_KeyValueString_h
#define ACS_CS_KeyValueString_h 1

#include <vector>
#include <string>
#include <sstream>

// Type definitions
typedef std::pair<std::string, std::string> KeyValuePair;


class ACS_CS_KeyValueString
{
    public:

        static std::vector<KeyValuePair> splitKeyValueStrByDelim(const char *str, char delim);

    private:

        static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

        static std::vector<std::string> split(const std::string &s, char delim);

};

#endif // ACS_CS_KeyValueString_h
