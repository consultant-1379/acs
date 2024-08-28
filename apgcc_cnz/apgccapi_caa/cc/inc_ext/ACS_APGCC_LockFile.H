//*****************************************************************************
//
// .NAME
//     MCS_LockFile - Class for locking files.
// .LIBRARY 3C++
// .PAGENAME MCS_MCC
// .HEADER  MCS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MCS_LockFile.H

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
//  This class encapsulates the lock file interface.
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
//
// .LINKAGE
//      -lMCC

//*****************************************************************************
#ifndef ACS_APGCC_LOCKFILE_H
#define ACS_APGCC_LOCKFILE_H


#ifndef ACEF_SYNCH_H
#  include <ace/Synch.h>
#endif
#include <ace/File_Lock.h>
#  include "ACS_APGCC_OS.H"
#  include "ACS_APGCC_Path.H"



class ACS_APGCC_LockFile
{
public:
	ACS_APGCC_LockFile();
	ACS_APGCC_LockFile(const ACS_APGCC_Path& filename);
  ~ACS_APGCC_LockFile();

  bool  lock();
  // Description:
  //    lock file.
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //    Tries to lock the file. Returns if it failed.

  bool  lock(ACS_APGCC_Path& filename);
  // Description:
  //    Lock file.
  // Parameters:
  //    filename	File to lock.
  // Return value:
  //    -
  // Additional information:
  //    Tries to lock the file. Returns if it failed.

  bool  waitlock();
  // Description:
  //    wait until successfully locked
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

  bool  unlock();
  // Description:
  //    unlocks file
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

  pid_t is_locked();
  // Description:
  //    returns pid of locker if locked
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

  pid_t getpid();
  // Description:
  //    returns pid of locker if locked
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

  void  setpath(ACS_APGCC_Path& filename);
  // Description:
  //    set name of lock file
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

  ACE_HANDLE   fd();
  // Description:
  //    returns locked file fd
  // Parameters:
  //    -                        -
  // Return value:
  //    -
  // Additional information:
  //    -

protected:
  ACS_APGCC_Path  path;		// path to lock
  bool	    used;		// if lock is used
  ACE_HANDLE fd_;
  ACE_File_Lock lock_;
};


#endif //MCS_LOCKFILE_H

