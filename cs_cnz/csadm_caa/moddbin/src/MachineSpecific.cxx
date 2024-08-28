//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
//  **************************************************************************
//
//  File Name ........@(#)fn
//  Document Number ..@(#)dn
//  Revision Date ....@(#)rd
//  Current Version ..@(#)cv
//  Responsible ............ 
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "MachineSpecific.hxx"

#ifdef _WIN32
#include <iostream>
#include <stdio.h>

//----------------------------------------------------------------------------
//< \fn ostream& operator<<(ostream& o, const __int64& value)
//
//  The operator<< must be overloaded for __int64 on a WIN32 platform only
//  since the operator<< cannot print 64 bit variables.
//
//  \pin  o         Reference to a stream to use for output.
//  \pin  value     Reference to a 64 bit integer to print.
//
//  \return  Returns a reference to the stream that was supplied in the first
//           parameter in the input argument list.
//>
//----------------------------------------------------------------------------
ostream& operator<<(ostream& o, const __int64& value)
{
   char output[32];

   if ((o.flags() & ios::hex) == ios::hex)
   {
      sprintf(output, "%I64x", value);
   }
   else
   {
      sprintf(output, "%I64d", value);
   }
   o << output;
   return o;
}

//----------------------------------------------------------------------------
//< \fn ostream& operator<<(ostream& o, const unsigned __int64& value)
//
//  The operator<< must be overloaded for __int64 on a WIN32 platform only
//  since the operator<< cannot print 64 bit variables.
//
//  \pin  o         Reference to a stream to use for output.
//  \pin  value     Reference to a 64 bit integer to print.
//
//  \return  Returns a reference to the stream that was supplied in the first
//           parameter in the input argument list.
//>
//----------------------------------------------------------------------------
ostream& operator<<(ostream& o, const unsigned __int64& value)
{
   char output[32];

   if ((o.flags() & ios::hex) == ios::hex)
   {
      sprintf(output, "%I64x", value);
   }
   else
   {
      sprintf(output, "%I64d", value);
   }
   o << output;
   return o;
}

#endif // _WIN32
