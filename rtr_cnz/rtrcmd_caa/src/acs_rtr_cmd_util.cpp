//******************************************************************************
//
//  NAME
//   acs_rtr_cmd_util.cpp
//
//  COPYRIGHT
//   Ericsson AB, Sweden 2013. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//  This file implements the acs_rtr_cmd_util functions
//
//  DOCUMENT NO
//
//  AUTHOR
//  2013-02-27 by XLANSRI
//
//  CHANGE HISTORY
//  VERSION     DATE    MODIFIED BY     DESCRIPTION
//
//******************************************************************************

#include <acs_rtr_cmd_util.h>
//*****************************************************************************************
//                                   strlwr
//*****************************************************************************************
ACE_TCHAR* strlwr(ACE_TCHAR* string)
{
    ACE_TCHAR* ret = string;
    ACE_INT32 i;
    for(i=0;;i++)
    {
        if ((string[i]>='A')&&(string[i]<='Z') )
            string[i]+=32;
        else
            if(string[i]=='\0')
                break;
    }
    return ret;
}


//*****************************************************************************************
//                                   strupr
//*****************************************************************************************
ACE_TCHAR* strupr(ACE_TCHAR* string)
{
    ACE_TCHAR* ret = string;
    ACE_INT32 i;
    for(i=0;;i++)
    {
        if ((string[i]>='a')&&(string[i]<='z') )
            string[i]-=32;
        else
            if(string[i]=='\0')
                break;
    }
    return ret;
}

//*****************************************************************************************
//                                   stricmp
//*****************************************************************************************

ACE_INT32 stricmp( const ACE_TCHAR *s1, const ACE_TCHAR *s2 )
{
    while (1)
    {
        ACE_INT32 c1 = tolower( (unsigned char) *s1++ );
        ACE_INT32 c2 = tolower( (unsigned char) *s2++ );
        if (c1 == 0 || c1 != c2) return c1 - c2;
    }
}

