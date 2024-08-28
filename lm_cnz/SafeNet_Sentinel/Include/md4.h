/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2020 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H******************************************************************
* FILENAME     : md4.h
* DESCRIPTION  : used in challenge-response
*                functionality; to map from a challenge to a
*                response. Functions in here have to be used by
*                both the client and the LS server.
*                As per LS-API 1.0
*
*                Taken verbatim from public domain. See
*                copyright notices below.
*
*H*/

/*
 **********************************************************************
 ** md4.h -- Header file for implementation of MD4                   **
 ** RSA Data Security, Inc. MD4 Message Digest Algorithm             **
 ** Created: 2/17/90 RLR                                             **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version              **
 **********************************************************************
 */

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD4 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD4 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */
#if defined (_V_LP64_)
typedef unsigned int UINT4;
#else
/* typedef a 32 bit type */
typedef unsigned long int UINT4;
#endif /* _V_LP64_ */




/* Data structure for MD4 (Message Digest) computation */
typedef struct
{
   UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
   UINT4 buf[4];                                    /* scratch buffer */
   unsigned char in[64];                              /* input buffer */
   unsigned char digest[16];     /* actual digest after MD4Final call */
}
MD4_CTX;

void MD4Init (MD4_CTX *mdContext);
void MD4Update (MD4_CTX *mdContext,
                unsigned char *inBuf,
                unsigned int inLen
);
void MD4Final (MD4_CTX *mdContext);

/*
 **********************************************************************
 ** End of md4.h                                                     **
 ******************************* (cut) ********************************
 */

