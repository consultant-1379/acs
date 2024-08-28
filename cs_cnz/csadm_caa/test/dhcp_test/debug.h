/*
 * @file debug.h
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

#ifndef DEBUG_H_
#define DEBUG_H_

class ACS_CS_Attribute;
// Debug functions
//void printErrorEx(int, const char*, int);
//void printBoolResultEx(bool, const char*, int);
void printAttributeEx(const ACS_CS_Attribute *);


// Debug function
//#define printError(X) printErrorEx(X, __FILE__, __LINE__)
//#define printBoolResult(X) printBoolResultEx(X, __FILE__, __LINE__)

#endif /* DEBUG_H_ */
