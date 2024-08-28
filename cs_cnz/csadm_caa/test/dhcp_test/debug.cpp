/*
 * @file debug.cpp
 * @author xminaon
 * @date Sep 17, 2010
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

#include <iostream>
#include <string>


#include "ACS_CS_Protocol.h"
#include "ACS_CS_Attribute.h"
#include "debug.h"

using std::cout;
using std::endl;
using std::string;
using ACS_CS_Protocol::CS_ProtocolChecker;
/*
// Debug function
void printErrorEx(int error, const char* file, int line) {

	if (error != 1) {
		printf("Error: %d: %s:%d\n", error, file, line);
	} else {
		//printf("Success: %s:%d\n", file, line);
	}
}

void printBoolResultEx(bool result, const char* file, int line) {

	if (!result) {
		printf("False result at %s:%d\n", file, line);
	}
}
*/
void printAttributeEx(const ACS_CS_Attribute *attribute) {

	if (attribute && attribute->getValueLength()) {
		cout << "Attribute: " << attribute->getIdentifier() << endl;
		char * buffer = new char[attribute->getValueLength() + 1];
		attribute->getValue(buffer, attribute->getValueLength());
		buffer[attribute->getValueLength()] = 0;
		string data = CS_ProtocolChecker::binToString(buffer, attribute->getValueLength());
		cout << "Value: " << data << endl;
		delete [] buffer;

	}
}
