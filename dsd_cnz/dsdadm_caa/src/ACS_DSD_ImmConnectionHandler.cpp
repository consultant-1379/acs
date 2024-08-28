#include "ACS_DSD_ImmConnectionHandler.h"

bool __CLASS_NAME__ ::is_critical_error( int apgcc_err_code)
{
	bool retval = false;

	switch( - apgcc_err_code )
	{
		case 0 /*ACS_APGCC_NO_ERR)*/:
			break;
		case 1 /*ACS_APGCC_ERR_UNKNOW*/:
			break;
		case 2 /*ACS_APGCC_ERR_IMM_LIBRARY */:
			break;
		case 3 /*ACS_APGCC_ERR_IMM_VERSION*/:
			break;
		case 4 /*ACS_APGCC_ERR_IMM_INIT*/:
			break;
		case 5 /*ACS_APGCC_ERR_IMM_TIMEOUT*/:
			retval = true;
			break;
		case 6 /*ACS_APGCC_ERR_IMM_TRY_AGAIN*/:
			break;
		case 7 /*ACS_APGCC_ERR_IMM_INVALID_PARAM*/:
			break;
		case 8 /*ACS_APGCC_ERR_IMM_NO_MEMORY*/:
			retval = true;
			break;
		case 9 /*ACS_APFCC_ERR_IMM_BAD_HANDLE*/:
			retval = true;
			break;
		case 10 /*ACS_APGCC_ERR_IMM_BUSY*/:
			break;
		case 11 /*ACS_APGCC_ERR_IMM_ACCESS*/:
			break;
		case 12 /*ACS_APGCC_ERR_IMM_NOT_EXIST*/:
			break;
		case 13 /*ACS_APGCC_ERR_IMM_NAME_TOO_LONG*/:
			break;
		case 14 /*ACS_APGCC_ERR_IMM_EXIST"*/:
			break;
		case 15 /*ACS_APGCC_ERR_IMM_NO_SPACE*/:
			break;
		case 16 /*ACS_APGCC_ERR_IMM_INTERRUPT*/:
			break;
		case 17 /*ACS_APGCC_ERR_NAME_NOT_FOUND*/:
			break;
		case 18 /*ACS_APGCC_ERR_IMM_NO_RESOURCES*/:
			retval = true;
			break;
		case 19 /*ACS_APGCC_ERR_IMM_NOT_SUPPORTED*/:
			break;
		case 20 /*ACS_APGCC_ERR_IMM_BAD_OPERATION*/:
			break;
		case 21 /*ACS_APGCC_ERR_IMM_FAILED_OPERATION*/:
			break;
		case 22 /*ACS_APGCC_ERR_IMM_MESSAGE_ERROR*/:
			break;
		case 23 /*ACS_APGCC_ERR_IMM_QUEUE_FULL*/:
			break;
		case 24 /*ACS_APGCC_ERR_IMM_QUEUE_NOT_AVAILABLE*/:
			break;
		case 25 /*ACS_APGCC_ERR_IMM_BAD_FLAGS*/:
			break;
		case 26 /*ACS_APGCC_ERR_IMM_TOO_BIG*/:
			break;
		case 27 /*ACS_APGCC_ERR_IMM_NO_SECTIONS*/:
			break;
		case 28 /*ACS_APGCC_ERR_IMM_NO_OP*/:
			break;
		case 29 /*ACS_APGCC_ERR_IMM_REPAIR_PENDING*/:
			break;
		case 30 /*ACS_APGCC_ERR_IMM_NO_BINDINGS*/:
			break;
		case 31 /*ACS_APGCC_ERR_IMM_UNAVAILABLE*/:
			retval = true;
			break;
		case 32 /*ACS_APGCC_ERR_IMM_CAMPAIGN_ERR_DETECTED*/:
			break;
		case 33 /*ACS_APGCC_ERR_IMM_CAMPAIGN_PROC_FAILED*/:
			break;
		case 34 /*ACS_APGCC_ERR_IMM_CAMPAIGN_CANCELED*/ :
			break;
		case 35 /*ACS_APGCC_ERR_IMM_CAMPAIGN_FAILED*/:
			break;
		case 36 /*ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDED*/:
			break;
		case 37 /*ACS_APGCC_ERR_IMM_CAMPAIGN_SUSPENDING*/:
			break;
		case 38 /*ACS_APGCC_ERR_IMM_ACCESS_DENIED*/:
			break;
		case 39 /*ACS_APGCC_ERR_IMM_NOT_READY*/:
			break;
		case 40 /*ACS_APGCC_ERR_IMM_DEPLOYMENT*/:
			break;
		case 41 /*ACS_APGCC_ERR_NOT_FOUND*/:
			break;
		case 42 /*ACS_APGCC_ERR_TRANSACTION_NOT_FOUND*/:
			break;
		case 43 /*ACS_APGCC_ERR_IMM_SAVE*/:
			break;
		case 44 /*ACS_APGCC_ERR_GENERIC*/:
			break;
		default:
			break;
	}

	return retval;
}
