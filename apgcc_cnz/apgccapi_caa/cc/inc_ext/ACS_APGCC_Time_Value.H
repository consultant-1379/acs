
//******************************************************************************
//
// .NAME
//  	MCS_Time_Value - Time handler
// .LIBRARY 3C++
// .PAGENAME MCS_Time_Value
// .HEADER  MCS MCC Library
// .LEFT_FOOTER Telefonaktiebolaget LM Ericsson
// .INCLUDE MCS_Time_Value.H
//
// .COPYRIGHT
//  COPYRIGHT Telefonaktiebolaget LM Ericsson 1998, Sweden.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Telefonaktiebolaget LM Ericsson, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Telefonaktiebolaget LM Ericsson or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// .DESCRIPTION
//     This class centralizes all the time-related processing in
//     MCS.  These timers are typically used in conjunction with
//     lower-level OS mechanisms like <select>, <poll>, or
//     <cond_timedwait>.  MCS_Time_Value help make the use of these
//     mechanisms portable across OS platforms.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      190 89-CAA 109 0171 A

// AUTHOR
// 	1997-10-03 by UAB/I/LD DAPA

// .REVISION
//	A		1998-03-12

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      PA1             1997-10-03      DAPA            First revision.
//	A		1998-03-12	DAPA		Minor updates.

// .LINKAGE
//	libMCS_MCC.so libACS_ALOG.so libACS_AEH.so librwtool.so.3
//	libACS_DSD.so libACS_TRA.so libsocket.so libnsl.so libdl.so
//	libgen.so libCsup.so libC.so libc.so.1

// .SEE ALSO
// 	-

//******************************************************************************

#ifndef ACS_APGCC_TIME_VALUE_H
#define ACS_APGCC_TIME_VALUE_H

#include "ACS_APGCC_OS.H"
#include <ace/Time_Value.h>
class ACS_APGCC_Time_Value
{
 public:

  static const ACS_APGCC_Time_Value zero;

  ACS_APGCC_Time_Value(void);
  // Description:
  //	Constructor
  // Parameters:
  //	-
  // Return value:
  //	-
  // Additional information:
  //	-

  ACS_APGCC_Time_Value (long sec, long usec = 0);
  // Description:
  //	Constructor
  // Parameters:
  //	sec:IN		Seconds.
  //	usec:IN		Milli-seconds.
  // Return value:
  //	-
  // Additional information:
  //	-

  ACS_APGCC_Time_Value (const struct timeval &t);
  // Description:
  //	Constructor.
  // Parameters:
  //	t:IN		Reference to a timeval structure.
  // Return value:
  //	-
  // Additional information:
  //	-

  ACS_APGCC_Time_Value (const timespec_t& t);
  // Description:
  //	Constructor.
  // Parameters:
  //	t:IN		Reference to a timespec_t structure.
  // Return value:
  //	-
  // Additional information:
  //	-

  ACS_APGCC_Time_Value (const ACS_APGCC_Time_Value &tv);
  // Description:
  //
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //
  // Copy constructor.

  ~ACS_APGCC_Time_Value();
  // Description:
  //    Destructor
  // Parameters:
  //    -
  // Return value:
  //    -
  // Additional information:
  //    -

  void set(long sec, long usec);
  // Description:
  //	Construct a <Time_Value> from two <long>s.
  // Parameters:
  //	sec:IN		Seconds.
  //	usec:IN		Milli-seconds.
  // Return value:
  //	-
  // Additional information:
  //	-

  void set(double d);
  // Description:
  //	Construct a <Time_Value> from a <double>, which is assumed
  //	to be in second format, with any remainder treated as
  //	microseconds.
  // Parameters:
  //	d:IN
  // Return value:
  //	-
  // Additional information:
  //	-

  void set(const timeval &t);
  // Description:
  //	Construct a <Time_Value> from a <timeval>.
  // Parameters:
  //	t:IN		Reference to a timeval.
  // Return value:
  //	-
  // Additional information:
  //	-

  void set(const timespec_t &t);
  // Description:
  //	Initializes the <Time_Value> object from a <timespec_t>.
  // Parameters:
  //	t:IN		Reference to a timespec_t.
  // Return value:
  //	-
  // Additional information:
  //	-



  long msec(void) const;
  // Description:
  //	Converts from <Time_Value> format into milli-seconds format.
  // Parameters:
  //	-
  // Return value:
  //	Milli-second value.
  // Additional information:
  //	-


  void msec(long);
  // Description:
  //	Converts from milli-seconds format into <Time_Value> format.
  // Parameters:
  //	long:IN		Value to convert.
  // Return value:
  //	-
  // Additional information:
  //	-

  operator timespec_t() const;
  // Description:
  //	Returns the value of the object as a <timespec_t>.
  // Parameters:
  //	-
  // Return value:
  //	-
  // Additional information:
  //	-

  operator timeval() const;
  // Description:
  //	Returns the value of the object as a <timeval>.
  // Parameters:
  //	-
  // Return value:
  //	-
  // Additional information:
  //	-


  operator timeval* () const;
  // Description:
  //	Returns a pointer to the object as a <timeval>.
  // Parameters:
  //	-
  // Return value:
  //	-
  // Additional information:
  //	-



  // = The following are accessor/mutator methods.

  long sec(void) const;
  // Description:
  //	Get seconds.
  // Parameters:
  //	-
  // Return value:
  //	Seconds.
  // Additional information:
  //	-


  void sec(long sec);
  // Description:
  //	Set seconds.
  // Parameters:
  //	sec:IN		Number of seconds.
  // Return value:
  //	-
  // Additional information:
  //	-


  long usec(void) const;
  // Description:
  //	Get microseconds.
  // Parameters:
  //	-
  // Return value:
  //	Micro seconds
  // Additional information:
  //	-


  void usec(long usec);
  // Description:
  //	Set microseconds.
  // Parameters:
  //	usec:IN		Number of microseconds.
  // Return value:
  //	-
  // Additional information:
  //	-


  // The following are arithmetic methods for operating on
  // Time_Values.

  void operator += (const ACS_APGCC_Time_Value &tv);
  // Description:
  //	Add <tv> to this.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  void operator -= (const ACS_APGCC_Time_Value &tv);
  // Description:
  //	Subtract <tv> to this.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //


  friend ACS_APGCC_Time_Value operator + (const ACS_APGCC_Time_Value &tv1,
				    const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	Adds two MCS_Time_Value objects together, returns the sum.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //


  friend ACS_APGCC_Time_Value operator - (const ACS_APGCC_Time_Value &tv1,
				    const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	Subtracts two MCS_Time_Value objects, returns the difference.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //


  friend int operator < (const ACS_APGCC_Time_Value &tv1,
			 const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 < tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //


  friend int operator > (const ACS_APGCC_Time_Value &tv1,
			 const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 > tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  friend int operator <= (const ACS_APGCC_Time_Value &tv1,
			  const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 <= tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  friend int operator >= (const ACS_APGCC_Time_Value &tv1,
			  const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 >= tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  friend int operator == (const ACS_APGCC_Time_Value &tv1,
			  const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 == tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  friend int operator != (const ACS_APGCC_Time_Value &tv1,
				     const ACS_APGCC_Time_Value &tv2);
  // Description:
  //	True if tv1 != tv2.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //


  void dump (void) const;
  // Description:
  //	Dump the state of an object.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //
private:

  void normalize (void);
  // Description:
  //	Put the timevalue into a canonical form.
  // Parameters:
  //
  // Return value:
  //
  // Additional information:
  //

  timeval tv_;
  // Store the values as a <timeval>.
};

#endif

// EOF MCS_Time_Value.H
