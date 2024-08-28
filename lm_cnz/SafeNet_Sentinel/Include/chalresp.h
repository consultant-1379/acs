/*******************************************************************/
/*                                                                 */
/*               Copyright (C) 2014 SafeNet, Inc.                  */
/*                      All Rights Reserved                        */
/*                                                                 */
/*     This Module contains Proprietary Information of SafeNet     */
/*          Inc., and should be treated as Confidential.           */
/*******************************************************************/

/*H****************************************************************
* FILENAME    : chalresp.h
*
* DESCRIPTION :
*           Simple demonstration of Sentinel RMS Development Kit
*           security features using the challenge response mechanism.
*           This file contains declarations used by other files
*           implementing Challenge-Response algorithm as described
*           in the LSAPI, a common Licensing API specified by major
*           software vendors.
*
* USAGE       :
*           This file is included by functions which calls standard
*           function which compute response to the challenge.
* NOTES       :
*H*/


#ifndef _CHALRESP_H_
#define _CHALRESP_H_
#define MAX_STR_LEN    256
#define MD4_DIGEST_LEN 16

void ComputeChallengeResponse(
#ifdef __V_STDC__
 unsigned char *secret,
 unsigned char *challenge,
 unsigned char *response,
 unsigned int seclen,
 unsigned int chalen,
 unsigned int *respSize
#endif
);
#endif
