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
//  Responsible ............ UAB/Y/SG Ingvar Nilsson
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//  This file defines the enumeration type ObjectStatus.
//>


#ifndef OBJECTSTATUS_HXX
#define OBJECTSTATUS_HXX

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------

//< \ingroup genAPI
//
//  The enumeration type should be used as the return type of the methods
//  returning the status of an instance.
//>
enum ObjectStatus
{
   ObjectOK,
   ObjectNotOK
};

#endif // OBJECTSTATUS_HXX
