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
//  Responsible ............ UAB/B/SF Martin Wahlstrom
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
#include "Media.hxx"

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The default constructor must exist in order to be able to instantiate
//  the derived classes. It is defined explicitly just to make it protected
//  instead of public wich is the case with the default constructor that the
//  compiler constructs in the absence of one.
//>
//----------------------------------------------------------------------------
Media::Media()
{
}

//----------------------------------------------------------------------------
//< \ingroup commAPI
//
//  The destructor for the base class
//>
//----------------------------------------------------------------------------
Media::~Media()
{
}
