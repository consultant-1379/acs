//*****************************************************************************
//
// .NAME
//     ACS_APGCC_InitFile - Internal Template to simplify handling
//                    of configuration files.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE filnamn.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1997.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson Utvecklings AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//
// The  ACS_APGCC_InitFile class support the concept of INI-files.
// Each data item in the init file consists of a key and a data
// value on the form:
//    KEY=DATA
//
// Example:
//
// int main() {
//   MCS_InitFile f;
//   if (f.open("mydata.ini") < 0)
//      return -1;
//   cout << "ipaddr = " << f.data("ipaddr") << endl;
//   cout << "path   = " << f.data("path") << endl;
//   cout << "null 1 = " << f.data("null","my default string") << endl;
//
//   f.set("null","new value");
//
//   cout << "null 2 = " << f.data("null","my default string") << endl;
//   f.close();
//   return 0;
// }
//
// > cat mydata.ini
// ipaddr=123.456.789.0
// path=/home/user
//
// > a.out
// ipaddr = 123.456.789.0
// path   = /home/user
// null 1 = my default string
// null 2 = new value
//
// > cat mydata.init
// ipaddr=123.456.789.0
// path=/home/user
// null=new value
//
//
//  It does not make use of any UNIX signals; nor will it throw any
//  exceptions.
//
//  Over- and underflow will not be notified.

// .ERROR HANDLING
//
//  .fill_off
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.
//  .fill_on

// DOCUMENT NO
//      190 89-CAA 109 0171

// AUTHOR
//      1998-03-18 by UAB/I/LN  UABSJEN

// .REVISION
//      A       98-03-18

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               98-03-18        SJEN            First release

// .LINKAGE
//      -

//*****************************************************************************
#ifndef  ACS_APGCC_INITFILE_H
#define  ACS_APGCC_INITFILE_H

#ifndef INCLUDED_STRING
#  include <string>
#  define INCLUDED_STRING
#endif

#ifndef INCLUDED_SET
#  include <set>
#  define INCLUDED_SET
#endif

#ifndef INCLUDED_MAP
#  include <map>
#  define INCLUDED_MAP
#endif
#  include "ACS_APGCC_OS.H"
#  include "ACS_APGCC_Util.H"


class   ACS_APGCC_InitFile
{
public:

   typedef std::pair<std::string, std::string>  KeyValue;
   typedef std::map<std::string,KeyValue>       KeyValueMap;
   typedef std::map<std::string,KeyValueMap*>   SectionMap;


   ACS_APGCC_InitFile();
	virtual ~ ACS_APGCC_InitFile();

	bool open(const std::string& fname);
   bool save(const std::string& newFname="");
   void close();

   void section(const std::string& Section);

   std::string getStr(const std::string& Key, const std::string& DefaultStr);
   std::string getStr(const std::string& Section, const std::string& Key, const std::string& DefaultStr);
   int getVal(const std::string& Key, int DefaultVal);
   int getVal(const std::string& Section, const std::string& Key, int DefaultVal);

   bool setStr(const std::string& Key, const std::string& Value);
   bool setStr(const std::string& Section, const std::string& Key, const std::string& Value);
   bool setVal(const std::string& Key, int Value);
   bool setVal(const std::string& Section, const std::string& Key, int Value);

   void eraseSection(const std::string& Section);
   void eraseKey(const std::string& Section, const std::string& Key);

   void changed(bool TrueFalse);
   void dump();

protected:

   SectionMap sect_;
   std::string fname_;
   bool changed_;
   std::string currSection_;
};



#endif // ACS_APGCC_INITFILE_H
