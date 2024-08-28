/*=================================================================== */
/**
   @file  acs_rtr_lib.h 

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
   N/A       22/01/2013   XHARBAV   Initial Release
==================================================================== */
/*==================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef acs_rtr_lib_h
#define acs_rtr_lib_h
/*==================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ace/ACE.h>
#include "acs_rtr_systemconfig.h"

/*==================================================================
                          CONSTANT DECLARATION SECTION
==================================================================== */
/*================================================================== */
/**
   @brief  RTRconfigFile
*/
/*================================================================== */
const char* const RTRconfigFile="RTR_Configuration";
/*================================================================== */
/**
   @brief  RTRpipePrefix
*/
/*================================================================== */
const char* const RTRpipePrefix="RTRintPipe";
/*=================================================================== */
/**
   @brief  serviceOffline
*/
/*================================================================== */
const int serviceOffline=0xABCD;
/*================================================================== */
/**
   @brief  twk
*/
/*=================================================================== */
const int twk[32]={0,1,4,6,3,5,2,3,1,8,5,3,6,2,3,0,0,3,4,1,3,6,0,1,1,2,3,5,4,3,0,0};
/*=================================================================== */
/**
   @brief  alarmSuppFile
*/
/*=================================================================== */
const char* const alarmSuppFile= "asf.info";
/*==================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  RTROutputType
*/
/*=================================================================== */
enum RTROutputType
{
	FileOutput,
	BlockOutput
};
/*=================================================================== */
/**
   @brief  parID
*/
/*=================================================================== */
enum parID
{ 
	a_blockLength,
	b_cdrFileFormat,
	c_key,
	d_blockLengthType,
	e_blockHoldTime,
	f_forcedFlag,
	g_blockPaddingChar,
	h_fileSize,
	j_fileHoldTime,
	k_generationTime,
	l_resetFlag,
	m_fixedFileRecords,
	n_fileRecordLength,
	o_filePaddingChar,
	p_minFileSize,
	ap_apidentity
};

enum RTR_CS_SystemType_Identifier {
                SysType_BC =                                    0,
                SysType_CP =                                    1000,
                SysType_AP =                                    2000,
                SysType_NotSpecified =                  0xFFFE
        };

/*==================================================================
                          STRUCTURE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  RTRMS_Parameters
*/
/*=================================================================== */
struct RTRMS_Parameters
{
	RTROutputType	OutputType;
	char			TransferQueue[32]; // added 1 byte to align the struct to 32 bit arch.
	unsigned int		BlockLength;
	ACE_UINT32			BlockLengthType;
	unsigned int     	BlockHoldTime;
        unsigned int		BlPaddingChar;
	unsigned int		FileSize;
	unsigned int		FiFileHoldTime;
	char			GenerationTime[12];
	char			ResetFlag[4];
	ACE_UINT32		FixedFileRecords;
	unsigned int		FileRecordLength;
	unsigned int			FiPaddingChar;
	bool            FiCdrFormat;
	bool            HmacMd5;
	char			pad[2]; // orders the struct to 32 bit
	char            FiKey[64];
	unsigned int			MinFileSize;
};// 172 bytes
/*=================================================================== */
/**
   @brief  HMACMD5_keys
*/
/*=================================================================== */
struct HMACMD5_keys
{
	unsigned char	ipad[64];
	unsigned char	opad[64];
	unsigned char    skey[16];
};
/*==================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  RTRCommandType
*/
/*=================================================================== */
enum RTRCommandType
{
	rtrStartService = 0,
	rtrChangeService,
	rtrStopService,
	/*rtrStopServiceDenied,*/
	rtrListStatistics,
	rtrForcedFileEnd
};
/*=================================================================== */
/**
   @brief  fileRequests
*/
/*=================================================================== */
enum fileRequests
{
	createSF, // make suppression file
	removeSF, // remove it...
	existSF   // check if it exists.
};
/*==================================================================
                          TYPEDEF STRUCTURE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  rtr_def_req_t
*/
/*=================================================================== */
typedef struct
{
	RTRCommandType		cmdType;
	char				msname[16];
	char                site[16];
	char			    TransferQueue[33];
	bool                S_opt;
	char                pad[2];
	RTROutputType    	OutputType;	
	RTRMS_Parameters parameters;
	bool Filepar;
        bool Blockpar;
	int opt[128];
} rtr_def_req_t; 
/*=================================================================== */
/**
   @brief  rtr_ls_req_t
*/
/*=================================================================== */
typedef struct
{
	RTRCommandType  cmdType;
	char            msname[16];
	char            site[16];
	char            apIdentity[6];
	bool            StatOpt;
	bool            DefOpt;
	bool            mAPOpt;
} rtr_ls_req_t; 
/*=================================================================== */
/**
   @brief  rtr_rm_req_t
*/
/*=================================================================== */
typedef struct
{
	RTRCommandType  cmdType;
	char            msname[16];
	char            site[16];
	char            apIdentity[7];
	bool            f_opt; //Forcefull option from COM not a valid case
} rtr_rm_req_t; 
/*=================================================================== */
/**
   @brief  rtr_ch_req_t
*/
/*=================================================================== */
typedef struct
{
	RTRCommandType  cmdType;
	int opt[128];
	RTRMS_Parameters parameters;
	char msname[16];
	char site[16];
	bool Filepar;
	bool Blockpar;
    char pad[2];
} rtr_ch_req_t; 
/*=================================================================== */
/**
   @brief  rtr_fe_req_t
*/
/*=================================================================== */
typedef struct
{
	RTRCommandType cmdType;
	char msname[16];
	char site[16];
} rtr_fe_req_t;
/*==================================================================
                          ENUMERATION DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  respCode
*/
/*=================================================================== */
enum respCode {
	//rtrdef
	EXECUTED		= 0,	//EXECUTED
	WRFILE_FAILED	= 1,	// Writing to file failed
	INCORRECTUSAGE	= 2,	// Usage
	MSNOTEXISTS		= 3,	// Message Store does not exist
	TQUNDEF			= 4,	// Transfer queue not defined
	DATATRANSFERERROR		= 5,	// No DataTransfer access
	INTERNALERR		= 6,	// Internal error code
	DIRECTORYERR	= 7,	// Directory structure faulty
	MSDEFERROR		= 8,	// Message Store already defined
	MS_BUFFER_BLK	= 9,	// Message Store buffer larger than block size
	DBOERROR		= 10,	// No DBO server access
	MS_BUFFER_REC	= 11,	// Message Store buffer larger than fixed record size
	NOTIFYERR		= 12,	// Notifying RTR service failed
	TOOMANYREQUESTS	= 13,	// Too many requests

	//rtrfe
	RTRRM_CMDFAILED		= 1, // Command executtion failed
	RTRFE_NOTQ			= 3, // Message store has no Transfer queue definition
	RTRFE_NOMS			= 4, // Message Store not defined
	RTRFE_NOTALLOWED	= 5, // Not allowed for Block Based Type
		
	//rtrrm
	NOTEXMS         = 3,
	MSNOTQ          = 4,
	RMDENIED        = 5,
	SERVICENOTRUN   = 6,

	//rtrch
	FILEERR           = 1,
	UNREASVALUE       = 3,
	MSNOTEX           = 4,
	NOMSFORTQ         = 5,
	MSBUFSIZE         = 6,
	ILLEGALPARTYPE    = 7,
	FIXRECSIZE        = 8,
	
	
    //rtrls 
	FILE_STRUCT_ERR   = 1,
	GENERAL_ERR       = 2,
	NOMS              = 3,
	NOTQFORMS         = 4,
	END_MESSAGE       = 10,
   	READ_DATA         = 11,
    NODATA            = 12,

	//generic codes
	SITE_ERROR      = 15,   //error in site name
	APNOTDEFINED	= 113,	// AP is not defined
	ILLEGALOPT		= 116,	// Illegal option in this system configuration
	UNABLECONNECT	= 117	// Unable to connect to server
};

/*=================================================================== */
/**
   @brief        getRTRdir

   @param        RTRdataHome

   @param        cpSystemId

   @return       int 
*/
/*=================================================================== */
int getRTRdir(char* RTRdataHome, unsigned cpSystemId = ~0U);
/*=================================================================== */
/**
   @brief        getACAparameters

   @param        msName

   @param        siteName

   @oaram        recSize

   @return       bool
*/
/*=================================================================== */
bool getACAparameters(const char*	 msName,
							  const char*	 siteName,
							  unsigned short &recSize);
/*=================================================================== */
/**
   @brief        getACASite

   @param        msName

   @param        siteName

   @param        count

   @return       bool
*/
/*=================================================================== */
bool getACASite(const char * msName, char * siteName, size_t count);
/*=================================================================== */
/**
   @brief        getRTRparameters

   @param        msName

   @param        parameters

   @param        suppAlarm

   @param        errstr

   @param        errLen

   @param        siteName  

   @return       bool
*/
/*=================================================================== */
bool getRTRparameters(const char*		msName,
							  RTRMS_Parameters*	parameters,
							  bool&				suppAlarm,
							  char*				errstr,
							  unsigned			errLen,
							  const char*		siteName = 0,
							  bool 			jobDefine = false);
/*=================================================================== */
/**
   @brief        setRTRparameters

   @param        msName

   @param        parameters

   @param        siteName

   @return       bool
*/
/*=================================================================== */
 bool setRTRparameters(const char*		msName,
							  RTRMS_Parameters* parameters,
							  const char*		siteName = 0);
/*=================================================================== */
/**
   @brief        commitCommand

   @return       bool
*/
/*=================================================================== */
 bool commitCommand(void);
/*=================================================================== */
/**
   @brief        setDefaultParValue

   @param        parsIssued

   @param        parValue

   @param        errstr

   @return       bool
*/
/*=================================================================== */
 bool setDefaultParValue(int* parsIssued,RTRMS_Parameters* parValue, char * errstr);
/*=================================================================== */
/**
   @brief        getDefaultParValues

   @param        bpar

   @param        errstr

   @return       bool
*/
/*=================================================================== */
 bool getDefaultParValues(RTRMS_Parameters& bpar, char * errstr);
/*=================================================================== */
/**
   @brief        suppressFile

   @param        fr

   @return       bool
*/
/*=================================================================== */
 bool suppressFile(fileRequests fr);
/*=================================================================== */
/**
   @brief        encr

   @param        e

   @return       void
*/
/*=================================================================== */
 void encr(unsigned char e[32]);
/*=================================================================== */
/**
   @brief        get_HMACMD5Keys

   @param        msName

   @param        siteName

   @param        ck

   @param        errstr

   @param        errLen

   @return       bool
*/
/*=================================================================== */
 bool get_HMACMD5Keys(const char*	msName,
					  const char*	siteName,
					  HMACMD5_keys&	ck,
					  const char * _filejobbasedDN,
					  char*			errstr,
					  unsigned		errLen);
/*=================================================================== */
/**
   @brief        getRTRConfigDir

   @param        RTRdataHome

   @return       int
*/
/*=================================================================== */
int getRTRConfigDir(char* RTRdataHome);
/*=================================================================== */
/**
   @brief        createDir

   @param        dirpath

   @param        err

   @return       bool
*/
/*=================================================================== */
 bool createDir(const char* dirpath, ACE_INT32& err);
/*=================================================================== */
/**
   @brief        getStatiticsFileAbsolutePath

   @param        msName

   @param        cpSite

   @param        filepath

   @return       bool
*/
/*=================================================================== */
bool getStatiticsFileAbsolutePath(const char* msName,
										  const char* cpSite,
										  char* filepath);
/*=================================================================== */
/**
   @brief        getCPlevelStatiticsAbsPath

   @param        msName

   @param        cpSystemId

   @param        filepath

   @return       bool
*/
/*=================================================================== */
 bool getCPlevelStatiticsAbsPath(const char* msName, 
								unsigned cpSystemId,
								char* filepath);
/*=================================================================== */
/**
   @brief        deleteConfigurationFile

   @param        msName

   @param        cpSite

   @return       bool
*/
/*=================================================================== */
 bool deleteConfigurationFile(const char* msName,
									 const char* cpSite = 0);
/*=================================================================== */
/**
   @brief        validateParameters

   @param        parameters

   @return       bool
*/
/*=================================================================== */
//-----------------------------------------------------------------------------
 bool validateParameters(RTRMS_Parameters* parameters);

#endif
