/*=================================================================== */
/**
   @file   jtp.h

   @brief  Header file for APJTP type module.

		   This file contains common utilities.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/26/2010     XTANAGG        APG43 on Linux.
==================================================================== */
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_JTP_H_)
#define _JTP_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_JTP.h>
#include <ace/ACE.h>
#include <ACS_TRA_trace.h>
#include <ACS_JTP_CriticalSectionGuard.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/**
 *@brief	TranspErr
 */
#define TranspErr 2

/**
 * @brief	CorruptMessage
 *
 */
#define CorruptMessage (len > 33000)

/**
 * @brief	h2AXE
 */
#define h2AXE short2AXE

/**
 * @brief	AXE2h
 */
#define AXE2h AXE2short

/**
 * @brief	APZ_FUNCTIONS_CLASS_NM
 */
#define APZ_FUNCTIONS_CLASS_NM	"AxeFunctions"

/**
 * @brief	AP_NODE_NUMBER_ATTR_NAME
 */
#define AP_NODE_NUMBER_ATTR_NAME	"apNodeNumber"


/*=================================================================== */
	/**

		@brief			This method converts the short value to AXE format.

		@pre			None

		@post			None

		@param			I
						Short value which is to be converted into AXE format.

		@return			AXE value.

		@exception		None
	*/
/*=================================================================== */
short short2AXE(short I);

/*=================================================================== */
	/**

		@brief			This method converts the value in AXE format to short.

		@pre			None

		@post			None

		@param			I
						Value in AXE format which is to be converted into short.

		@return			Short value.

		@exception		None
	*/
/*=================================================================== */
short AXE2short(short I);

/*=================================================================== */
	/**

		@brief			This method converts the value in long to RFC format.

		@pre			None

		@post			None

		@param			I
						long value which is to be converted into RFC format.

		@return			RFC value.

		@exception		None
	*/
/*=================================================================== */
unsigned long long2RPC(unsigned long I);

/*=================================================================== */
	/**

		@brief			This method converts the value in RFC format to long.

		@pre			None

		@post			None

		@param			I
						Value in RFC format which is to be converted into long format.

		@return			long value.

		@exception		None
	*/
/*=================================================================== */
unsigned long RPC2long(unsigned long I);

//============================================//
// Global Tracing Objects and Helper Function //
//============================================//
/**
 * @brief	ACS_JTP_InformationTrace
 */
extern ACS_TRA_trace ACS_JTP_InformationTrace;

/**
 * @brief	ACS_JTP_WarningTrace
 */
extern ACS_TRA_trace ACS_JTP_WarningTrace;

/**
 * @brief	ACS_JTP_ErrorTrace
 */
extern ACS_TRA_trace ACS_JTP_ErrorTrace;

/**
 *
 * @brief	ACS_JTP_TRACE_INFORMATION_ACTIVE
 */
#define ACS_JTP_TRACE_INFORMATION_ACTIVE	(ACS_JTP_InformationTrace.ACS_TRA_ON())

/**
 * @brief	ACS_JTP_TRACE_WARNING_ACTIVE
 */
#define ACS_JTP_TRACE_WARNING_ACTIVE		(ACS_JTP_WarningTrace.ACS_TRA_ON())

/**
 * @brief ACS_JTP_TRACE_ERROR_ACTIVE
 */
#define ACS_JTP_TRACE_ERROR_ACTIVE			(ACS_JTP_ErrorTrace.ACS_TRA_ON())

/*=================================================================== */
	/**

		@brief			This method to trace informational messages.

		@pre			None

		@post			None

		@param			messageFormat
						Message format.

		@return			void.

		@exception		None
	*/
/*=================================================================== */
void  helperTraceInformation(const char* messageFormat, ...);

/*=================================================================== */
	/**

		@brief			This method to trace warning messages.

		@pre			None

		@post			None

		@param			messageFormat
						Message format.

		@return			void.

		@exception		None
	*/
/*=================================================================== */
void  helperTraceWarning(const char* messageFormat, ...);

/*=================================================================== */
	/**

		@brief			This method to trace error messages.

		@pre			None

		@post			None

		@param			messageFormat
						Message format.

		@return			void.

		@exception		None
	*/
/*=================================================================== */
void  helperTraceError(const char* messageFormat, ...);

/*=================================================================== */
	/**

		@brief			This method is used to help in tracing warning messages.

		@pre			None

		@post			None

		@param			sourceName
						FileName

		@param			sourceLine
						Line Number

		@param			message
						Message to be traced.

		@return			void.

		@exception		None
	*/
/*=================================================================== */
inline void helperTraceWarningSource(const char* sourceName, unsigned long sourceLine, const char* message = 0)
{
	if (message)
	{
		helperTraceWarning("Source coordinate (%s, %u): %s", sourceName && *sourceName ? sourceName : "NO_SOURCE", sourceLine, message);
	}
	else
	{
		helperTraceWarning("Source coordinate (%s, %u)", sourceName && *sourceName ? sourceName : "NO_SOURCE", sourceLine);
	}
}

/*=================================================================== */
	/**

		@brief			This method is used to help in tracing error messages.

		@pre			None

		@post			None

		@param			sourceName
						FileName

		@param			sourceLine
						Line Number

		@param			message
						Message to be traced.

		@return			void.

		@exception		None
	*/
/*=================================================================== */
inline void helperTraceErrorSource(const char* sourceName, unsigned long sourceLine, const char* message = 0)
{
	if (message)
	{
		helperTraceError("Source coordinate (%s, %u): %s", sourceName && *sourceName ? sourceName : "NO_SOURCE", sourceLine, message);
	}
	else
	{
		helperTraceError("Source coordinate (%s, %u)", sourceName && *sourceName ? sourceName : "NO_SOURCE", sourceLine);
	}
}


/*=================================================================== */
	/**
		@struct		JSLCL

		@brief		Information for disconnected sessions.

		@par		Free

		@par		APNo

		@par		APRestartCntr

		@par		PID

		@par		ConnectionCntr

	*/
/*=================================================================== */
typedef struct {
	/**
	 * @brief	Free
	 */
	int	Free;

	/**
	 * @brief	APNo
	 */
	unsigned char	APNo;

	/**
	 * @brief	APRestartCntr
	 */
	unsigned char	APRestartCntr;

	/**
	 * @brief	PID
	 */
	unsigned short	PID;

	/**
	 * @brief	ConnectionCntr
	 */
	unsigned short  ConnectionCntr;
} JSLCL;


/*=================================================================== */
/**
 * @brief	JSL_init
 *
 */
extern int JSL_init; //Empty slots in the list have NULL value

/**
 * @brief	g_JSL_init_Sync
 */
extern ACS_JTP_CriticalSection g_JSL_init_Sync;

/**
 * @brief	CL
 * 			Client List. The list of disconnected sessions
 */
extern JSLCL* CL;

/**
 * @brief 	CLSize
 * 			The current size of the list
 */
extern int CLSize;

/**
 * @brief	g_CLSync
 */
extern ACS_JTP_CriticalSection g_CLSync;

//changes for sts hang issue
/**
 * @brief       g_CLSync_new
 */
extern ACS_JTP_CriticalSection g_CLSync_new;

class JTP_Session;

/**
 * @brief	SL
 * 			Server List. The list of active sessions
 */
extern JTP_Session** SL;

/**
 * @brief	SLSize
 * 			The current size of the list
 */
extern int SLSize;

/**
 * @brief	g_SLSync
 */
extern ACS_JTP_CriticalSection g_SLSync;

//changes for sts hang issue
/**
 * @brief       g_SLSync_new
 */
extern ACS_JTP_CriticalSection g_SLSync_new;

/**
 * @brief	JSLAPNo
 */
extern unsigned char JSLAPNo;

/**
 * @brief	JSLAPRestartCntr
 */
extern unsigned char JSLAPRestartCntr;

/**
 * @brief	JSLPID
 */
extern unsigned short JSLPID;

/**
 * @brief	JSLConnectionCntr
 */
extern unsigned short JSLConnectionCntr;

/*=================================================================== */
	/**
		@brief		This represents invalid system ID.

	*/
/*=================================================================== */
const unsigned long INVALID_SYSTEM_ID = -1;

#endif //!defined(_JTP_H_)
