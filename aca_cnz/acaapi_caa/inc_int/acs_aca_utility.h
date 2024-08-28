#ifndef ACS_ACA_Utility_H
#define ACS_ACA_Utility_H

#include "ace/ACE.h"

#include "acs_aca_common.h"
#include "acs_aca_api_tracer.h"

#define OPEN 0

namespace {
	inline ULONGLONG convertToUll(const unsigned char * strMap) {
		unsigned shifter = 0;
		const unsigned char * p = 0;
		unsigned i = 0;
		ULONGLONG result = 0ULL;

		for (i = 0, p = strMap + 4; i++ < 4; result |= (static_cast<ULONGLONG>(*p++) << (shifter++ << 3))){}
		for (i = 0, p = strMap; i++ < 4; result |= (static_cast<ULONGLONG>(*p++) << (shifter++ << 3))){}

		return result;
	}

	inline void setBufferFromUll(ULONGLONG value, unsigned char * buffer) {
		ULONGLONG half = (value & 0xFFFFFFFF00000000ULL) >> 32;
		for (int i = 0; i++ < 4; half >>= 8) *buffer++ = static_cast<unsigned char>(half & 0xFFULL);
		half = value & 0xFFFFFFFFULL;
		for (int i = 0; i++ < 4; half >>= 8) *buffer++ = static_cast<unsigned char>(half & 0xFFULL);
	}

	inline bool isValidHandle (
			const char *_file,
			const char *_method,
			int _line,
			ACE_HANDLE &handle,
			const char* pathname = 0,
			mode_t mode = 0,
			int type = 1)
	{
		//we will open again only if we are checking in open mode
		//////////////
		// To remove compile warnings
		(void) _file; (void) _method; (void) _line;
		//ACS_ACA_TRACE_MESSAGE("handle %d at %s -> %s -> %d", handle, _file, _method, _line);

		switch(handle) {
		case 0:
		case 1:
		case 2:
		{
			freopen( "/dev/null", "r", stdin);
			freopen( "/dev/null", "w", stdout);
			freopen( "/dev/null", "w", stderr);

			if (type == OPEN)	{
				handle = ACE_OS::open(pathname,mode,ACE_DEFAULT_OPEN_PERMS);
				
				if (handle == ACE_INVALID_HANDLE)	return false;
				
				return true;
			}
			else
				return false;
		}

		break;

		case -1:
		      return false;

		default:
		      return true;
		}
		//////////////
	}
}


namespace ACS_ACA_CS_API
{
	// CS API Result Codes
	enum CS_API_Result_ACA
	{
		Result_Success =			0,
		Result_NoEntry =			3,
		Result_NoValue =			4,
		Result_NoAccess =			12,
		Result_Failure =			15
	};
	//using namespace ACS_CS_API_NS;	// Import global namespace

	enum ACA_CS_SystemType_Identifier {
		SysType_BC =					0,
		SysType_CP =					1000,
		SysType_AP =					2000,
		SysType_NotSpecified =			0xFFFE
	};
}

#endif
