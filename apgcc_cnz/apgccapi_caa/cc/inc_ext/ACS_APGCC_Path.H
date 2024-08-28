//*****************************************************************************
//
// .NAME
//     ACS_APGCC_Path - Internal Class for expanding path names
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_Path.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998.
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
// Class for expanding path names into valid path names.
// Given $HOME = /home/user, the path $HOME/tmp is
// expanded to /home/user/tmp
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
//      -lMCC

//*****************************************************************************
#ifndef ACS_APGCC_PATH_H
#define ACS_APGCC_PATH_H


#ifndef ACE_OS_H
#  include <ace/OS.h>
#endif
#  include "ACS_APGCC_OS.H"
#  include <string>

//#define DEFAULT_REG_HIVE	HKEY_LOCAL_MACHINE
//#define DEFAULT_REG_PATH	"Software\\Ericsson\\Adjunct Processor\\APGCC"
//#define DEFAULT_REG_KEY		"Common"



class ACS_APGCC_Path
{
public:

	ACS_APGCC_Path();
	ACS_APGCC_Path(const char* path);
	ACS_APGCC_Path(const std::string& path);
	~ACS_APGCC_Path();

   void setpath(const std::string& path);
   void setpath(const char* path);

	void path(const std::string& path);
   void path(const char* path);
	const char* path();
   const char* path() const;

  const char* operator*() const;
  // Description:
  //    Return expanded path.
  // Parameters:
  //    -                        -
  // Return value:
  //    Expanded path.
  // Additional information:
  //    -

  operator const char*() const;
  // Description:
  //    Return expanded path.
  // Parameters:
  //    -                        -
  // Return value:
  //    Expanded path.
  // Additional information:
  //    -

  operator char*() const;
  // Description:
  //    Return expanded path.
  // Parameters:
  //    -                        -
  // Return value:
  //    Expanded path.
  // Additional information:
  //    -

  bool found() const;
  // Description:
  //    Returns true if some part of the path was expanded.
  // Parameters:
  //    -                        -
  // Return value:
  //    true         All, or some part, of the given path
  //                 was expanded.
  //    false        No part was expanded.
  // Additional information:
  //    -


protected:

   // Internal methods

	void expand(const std::string& path);
   void expand_path(std::string& path);

private:
   bool found_;
	std::string path_;
};




#endif // ACS_APGCC_PATH_H

