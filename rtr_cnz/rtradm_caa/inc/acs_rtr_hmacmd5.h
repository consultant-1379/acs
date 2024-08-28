/*=================================================================== */
/**
   @file  acs_rtr_hmacmd5.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/01/2013   XHARBAV   Initial Release
==================================================================== */
//*****************************************************************************
// DESCRIPTION
// This file implements the HMAC-MD5 algorithm, i.e
//   MD5(secretKey XOR opad),MD5(secretKey XOR ipad,data))
// 
// This means:
// (1) append 0x00 to the secret key (16 bytes) to create a 
//     64 byte secret key.
// (2) XOR the secret key (1) with ipad.
// (3) append the data to the result in (2).
// (4) get the hash of the result in (3).
// (5) XOR the secret key (1) with opad.
// (6) append the result in (4) to the result in (5).
// (7) get the hash of the result in (6).
//******************************************************************************
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_hmacmd5_h
#define ACS_RTR_hmacmd5_h
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <openssl/md5.h>
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      ACS_RTR_HMACMD5

 */
/*=================================================================== */
class ACS_RTR_HMACMD5
{
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for ACS_RTR_HMACMD5 class.

   @param      ipad

   @param      opad

   @param      key 

 */
/*=================================================================== */
	ACS_RTR_HMACMD5(unsigned char* ipad, unsigned char* opad, unsigned char* key);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	~ACS_RTR_HMACMD5();
/*=================================================================== */
/**
   @brief      addData

   @param      ln

   @param      data

   @return     bool

 */
/*=================================================================== */
	bool addData(size_t ln, unsigned char* data);
/*=================================================================== */
/**
   @brief      endData

   @return      bool

 */
/*=================================================================== */
	bool endData(void);
/*=================================================================== */
/**
   @brief       getHash

   @param       hashVal

   @return      bool

 */
/*=================================================================== */
	bool getHash(unsigned char* hashVal);
/*=================================================================== */
/**
   @brief       getState

   @return      bool

 */
/*=================================================================== */
	bool getState(void);
/*=================================================================== */
/**
   @brief       getError

   @param       string

   @return      void

 */
/*=================================================================== */
	void getError(std::string&);
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief       md5
 */
/*=================================================================== */
	MD5_CTX md5;
/*=================================================================== */
/**
   @brief       k_ipad
 */
/*=================================================================== */
	unsigned char k_ipad[64];
/*=================================================================== */
/**
   @brief       k_opad
 */
/*=================================================================== */
	char k_opad[64];
/*=================================================================== */
/**
   @brief       hashInner
 */
/*=================================================================== */
	unsigned char hashInner[16];
/*=================================================================== */
/**
   @brief       state
 */
/*=================================================================== */
	bool state;
/*=================================================================== */
/**
   @brief       err
 */
/*=================================================================== */
	std::string err;
};

#endif
