/*=================================================================== */
/**
   @file  acs_aca_msdls_api_struct.h 

   @brief Header file for aca module.

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
   N/A       28/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef __ACS_ACA_MSDLS_API_STRUCT_H__
#define __ACS_ACA_MSDLS_API_STRUCT_H__
/*=====================================================================
                          DEFINE DECLARATION SECTION
==================================================================== */

/****************************************************************************
** BIT MASK :
**      option mask
**
****************************************************************************/
#define ACA_MSDLS_AP	0x0001
#define ACA_MSDLS_CP	0x0002
#define ACA_MSDLS_MS	0x0004
#define ACA_MSDLS_ALL	0x0008
#define ACA_MSDLS_DATA	0x0010
#define ACA_MSDLS_TRUNC	0x0020
#define ACA_MSDLS_NUM	0x0040
#define ACA_MSDLS_STAT	0x0080
#define ACA_MSDLS_SRC	0x0100

#define CHECK_OPT_AP(opt)		((ACA_MSDLS_AP & opt) == ACA_MSDLS_AP)
#define CHECK_OPT_CP(opt)		((ACA_MSDLS_CP & opt) == ACA_MSDLS_CP)
#define CHECK_OPT_MS(opt)		((ACA_MSDLS_MS & opt) == ACA_MSDLS_MS)
#define CHECK_OPT_ALL(opt)		((ACA_MSDLS_ALL & opt) == ACA_MSDLS_ALL)
#define CHECK_OPT_DATA(opt)		((ACA_MSDLS_DATA & opt) == ACA_MSDLS_DATA)
#define CHECK_OPT_TRUNC(opt)	((ACA_MSDLS_TRUNC & opt) == ACA_MSDLS_TRUNC)
#define CHECK_OPT_NUM(opt)		((ACA_MSDLS_NUM & opt) == ACA_MSDLS_NUM)
#define CHECK_OPT_STAT(opt)		((ACA_MSDLS_STAT & opt) == ACA_MSDLS_STAT)
#define CHECK_OPT_SRC(opt)		((ACA_MSDLS_SRC & opt) == ACA_MSDLS_SRC)

#define SET_OPT_AP(mask)		(mask = mask | ACA_MSDLS_AP)
#define SET_OPT_CP(mask)		(mask = mask | ACA_MSDLS_CP)
#define SET_OPT_MS(mask)		(mask = mask | ACA_MSDLS_MS)
#define SET_OPT_ALL(mask)		(mask = mask | ACA_MSDLS_ALL)
#define SET_OPT_DATA(mask)		(mask = mask | ACA_MSDLS_DATA)
#define SET_OPT_TRUNC(mask)		(mask = mask | ACA_MSDLS_TRUNC)
#define SET_OPT_NUM(mask)		(mask = mask | ACA_MSDLS_NUM)
#define SET_OPT_STAT(mask)		(mask = mask | ACA_MSDLS_STAT)
#define SET_OPT_SRC(mask)		(mask = mask | ACA_MSDLS_SRC)

/****************************************************************************
** SIZE
****************************************************************************/
#define MSGSTORE_SIZE	16
#define CPSOURCE_SIZE	16
#define DATA_SIZE		28

/****************************************************************************
** DATA STRUCTURE :
**      Data type for MSDLS PIPE API
**
****************************************************************************/
/*=====================================================================
                          TYPEDEF DECLARATION SECTION
==================================================================== */
typedef struct
{
	unsigned short	cpID;						//	  2 cp id
	unsigned short	optmask;					//	  2
	char			cpsource[CPSOURCE_SIZE];	//	 16 cp site
	char			msname[MSGSTORE_SIZE];		//	 16
	char			data[DATA_SIZE];			//	 28
} aca_msdls_req_t;								//	---- 64 bytes
/*=====================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
enum ErrorType
{
	OK,
	INVALID,
	CS_ERROR55,
    CS_ERROR56,
	CS_ERROR118,
	COMMIT_ERROR,
	DATA_FILE_ERROR,
	GENERAL_FAULT,
    EOF_FILE,
	STATISTICS_ERROR,
	END
};
/*=====================================================================
                          TYPEDEF DECLARATION SECTION
==================================================================== */
typedef struct
{
	unsigned int	data_lenght;	//  4
	unsigned int	seqNo;			//	4
	unsigned short	error;			//  2
	unsigned short	cpID;			//  2
	char			pad[3];			//  3
	char			data[1];		//  1
} aca_msdls_resp_t;					// --- 16 bytes


#endif
