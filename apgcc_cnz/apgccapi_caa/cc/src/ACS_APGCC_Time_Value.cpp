//*******************************************************************
//
// NAME
//      ACS_APGCC_Time_Value

//  COPYRIGHT Telefonaktiebolaget LM Ericsson, Sweden 1998.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Telefonaktiebolaget LM Ericsson, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Telefonaktiebolaget LM Ericsson or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// DESCRIPTION
//      Implementation of the MCS_Time_Value class.

// DOCUMENT MCSNO
//      190 89-CAA 109 0171 A

// AUTHOR
// 	1997-10-03	UAB/I/LD DAPA

// REVISION
//	A		1998-01-22

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//	A		1998-03-12	DAPA		Minor updates
//							added ACS_trace.

// SEE ALSO
// 	MCS_Reactor

//*******************************************************************

#include "ACS_APGCC_Time_Value.H"

#ifdef DEBUG_OBJ
#include <iostream.h>
#endif


const ACS_APGCC_Time_Value ACS_APGCC_Time_Value::zero;

static const long ONE_SECOND = 1000000L;

//*******************************************************************
//	Constructor()
//*******************************************************************
ACS_APGCC_Time_Value::ACS_APGCC_Time_Value(const timeval &tv)
{
  this->set(tv);

#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value constructor 1 "<<this<<endl;
#endif
}

//*******************************************************************
//	Constructor()
//*******************************************************************
ACS_APGCC_Time_Value::ACS_APGCC_Time_Value(void)
{
  this->set(0, 0);

#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value constructor 2 "<<this<<endl;
#endif
}

//*******************************************************************
//	Constructor()
//*******************************************************************
ACS_APGCC_Time_Value::ACS_APGCC_Time_Value(long sec, long usec)
{
  this->set (sec, usec);
  this->normalize ();

#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value constructor 3 "<<this<<endl;
#endif
}

//*******************************************************************
//	Constructor()
//*******************************************************************
ACS_APGCC_Time_Value::ACS_APGCC_Time_Value(const timespec_t &tv)
{
  this->set(tv);

#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value constructor 4 "<<this<<endl;
#endif
}

//*******************************************************************
//	Constructor()
//*******************************************************************
ACS_APGCC_Time_Value::ACS_APGCC_Time_Value(const ACS_APGCC_Time_Value &tv)
  :tv_(tv.tv_)
{
#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value constructor 5 "<<this<<endl;
#endif
}

//*******************************************************************
//       Destructor()
//*******************************************************************
ACS_APGCC_Time_Value::~ACS_APGCC_Time_Value()
{
#ifdef DEBUG_OBJ
  cout<<"ACS_APGCC_Time_Value destructor"<<this<<endl;
#endif
}

//*******************************************************************
//	normalize()
//*******************************************************************
void
ACS_APGCC_Time_Value::normalize(void)
{
  if(this->tv_.tv_usec >= ONE_SECOND)
    {
      do
	{
	  this->tv_.tv_sec++;
	  this->tv_.tv_usec -= ONE_SECOND;
	}
      while(this->tv_.tv_usec >= ONE_SECOND);
    }
  else if(this->tv_.tv_usec <= -ONE_SECOND)
    {
      do
	{
	  this->tv_.tv_sec--;
	  this->tv_.tv_usec += ONE_SECOND;
	}
      while(this->tv_.tv_usec <= -ONE_SECOND);
    }

  if(this->tv_.tv_sec >= 1 && this->tv_.tv_usec < 0)
    {
      this->tv_.tv_sec--;
      this->tv_.tv_usec += ONE_SECOND;
    }
  else if(this->tv_.tv_sec < 0 && this->tv_.tv_usec > 0)
    {
      this->tv_.tv_sec++;
      this->tv_.tv_usec -= ONE_SECOND;
    }
}

//*******************************************************************
//	timeval()
//*******************************************************************
ACS_APGCC_Time_Value::operator timeval() const
{
  return this->tv_;
}

ACS_APGCC_Time_Value::operator timeval * () const
{
  return (timeval *) &this->tv_;
}
void
ACS_APGCC_Time_Value::set (long sec, long usec)
{
  this->tv_.tv_sec = sec;
  this->tv_.tv_usec = usec;
}

//*******************************************************************
//	set()
//*******************************************************************
void
ACS_APGCC_Time_Value::set (double d)
{
  long l = (long) d;
  this->tv_.tv_sec = l;
  this->tv_.tv_usec = (long) ((d - (double) l) * 1000000);
  this->normalize ();
}

void
ACS_APGCC_Time_Value::set (const timespec_t &tv)
{
#if ! defined(MCS_HAS_BROKEN_TIMESPEC_MEMBERS)
  this->tv_.tv_sec = tv.tv_sec;
  this->tv_.tv_usec = tv.tv_nsec / 1000;
#else
  this->tv_.tv_sec = tv.ts_sec;
  this->tv_.tv_usec = tv.ts_nsec / 1000;
#endif /* MCS_HAS_BROKEN_TIMESPEC_MEMBERS */

  this->normalize ();
}


//*******************************************************************
//	set()
//*******************************************************************
void
ACS_APGCC_Time_Value::set (const timeval &tv)
{
  this->tv_.tv_sec = tv.tv_sec;
  this->tv_.tv_usec = tv.tv_usec;

  this->normalize ();
}

//*******************************************************************
//	operator >
//*******************************************************************
int
operator > (const ACS_APGCC_Time_Value &tv1,
	    const ACS_APGCC_Time_Value &tv2)
{
  if (tv1.tv_.tv_sec > tv2.tv_.tv_sec)
    return 1;
  else if (tv1.tv_.tv_sec == tv2.tv_.tv_sec
	   && tv1.tv_.tv_usec > tv2.tv_.tv_usec)
    return 1;
  else
    return 0;
}

//*******************************************************************
//	operator >=
//*******************************************************************
int
operator >= (const ACS_APGCC_Time_Value &tv1,
	     const ACS_APGCC_Time_Value &tv2)
{
  if (tv1.tv_.tv_sec > tv2.tv_.tv_sec)
    return 1;
  else if (tv1.tv_.tv_sec == tv2.tv_.tv_sec
	   && tv1.tv_.tv_usec >= tv2.tv_.tv_usec)
    return 1;
  else
    return 0;
}

//*******************************************************************
//	operator timespec_t
//*******************************************************************
ACS_APGCC_Time_Value::operator timespec_t () const
{
  timespec_t tv;
#if ! defined(MCS_HAS_BROKEN_TIMESPEC_MEMBERS)
  tv.tv_sec = this->tv_.tv_sec;
  tv.tv_nsec = this->tv_.tv_usec * 1000;
#else
  tv.ts_sec = this->tv_.tv_sec;
  tv.ts_nsec = this->tv_.tv_usec * 1000;
#endif /* MCS_HAS_BROKEN_TIMESPEC_MEMBERS */
  return tv;
}


//*******************************************************************
//	set()
//*******************************************************************
long
ACS_APGCC_Time_Value::sec (void) const
{
  return this->tv_.tv_sec;
}

//*******************************************************************
//	sec()
//*******************************************************************
void
ACS_APGCC_Time_Value::sec (long sec)
{
  this->tv_.tv_sec = sec;
}

//*******************************************************************
//	msec()
//*******************************************************************
long
ACS_APGCC_Time_Value::msec (void) const
{
  return this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
}

//*******************************************************************
//	msec()
//*******************************************************************
void
ACS_APGCC_Time_Value::msec (long milliseconds)
{
  this->tv_.tv_sec = milliseconds / 1000;
  this->tv_.tv_usec = (milliseconds - (this->tv_.tv_sec * 1000)) * 1000;
}

//*******************************************************************
//	usec()
//*******************************************************************
long
ACS_APGCC_Time_Value::usec (void) const
{
  return this->tv_.tv_usec;
}

//*******************************************************************
//	usec()
//*******************************************************************
void
ACS_APGCC_Time_Value::usec (long usec)
{
  this->tv_.tv_usec = usec;
}

//*******************************************************************
//	operator <
//*******************************************************************
int
operator < (const ACS_APGCC_Time_Value &tv1,
	    const ACS_APGCC_Time_Value &tv2)
{
  return tv2 > tv1;
}

//*******************************************************************
//	operator <=
//*******************************************************************
int
operator <= (const ACS_APGCC_Time_Value &tv1,
	     const ACS_APGCC_Time_Value &tv2)
{
  return tv2 >= tv1;
}

//*******************************************************************
//	operator ==
//*******************************************************************
int
operator == (const ACS_APGCC_Time_Value &tv1,
	     const ACS_APGCC_Time_Value &tv2)
{
  return tv1.tv_.tv_sec == tv2.tv_.tv_sec
    && tv1.tv_.tv_usec == tv2.tv_.tv_usec;
}

//*******************************************************************
//	operator !=
//*******************************************************************
int
operator != (const ACS_APGCC_Time_Value &tv1,
	     const ACS_APGCC_Time_Value &tv2)
{
  return !(tv1 == tv2);
}

//*******************************************************************
//	operator +=
//*******************************************************************
void
ACS_APGCC_Time_Value::operator+= (const ACS_APGCC_Time_Value &tv)
{
  this->tv_.tv_sec += tv.tv_.tv_sec;
  this->tv_.tv_usec += tv.tv_.tv_usec;
  this->normalize ();
}

//*******************************************************************
//	operator -=
//*******************************************************************
void
ACS_APGCC_Time_Value::operator-= (const ACS_APGCC_Time_Value &tv)
{
  this->tv_.tv_sec -= tv.tv_.tv_sec;
  this->tv_.tv_usec -= tv.tv_.tv_usec;
  this->normalize ();
}

//*******************************************************************
//	operator +
//*******************************************************************
ACS_APGCC_Time_Value
operator + (const ACS_APGCC_Time_Value &tv1,
	    const ACS_APGCC_Time_Value &tv2)
{
	ACS_APGCC_Time_Value sum (tv1.tv_.tv_sec + tv2.tv_.tv_sec,
		      tv1.tv_.tv_usec + tv2.tv_.tv_usec);

  sum.normalize ();
  return sum;
}

//*******************************************************************
//	operator -
//*******************************************************************
ACS_APGCC_Time_Value
operator - (const ACS_APGCC_Time_Value &tv1,
	    const ACS_APGCC_Time_Value &tv2)
{
	ACS_APGCC_Time_Value delta (tv1.tv_.tv_sec - tv2.tv_.tv_sec,
			tv1.tv_.tv_usec - tv2.tv_.tv_usec);
  delta.normalize ();
  return delta;
}

// EOF ACS_APGCC_Time_Value.cpp
