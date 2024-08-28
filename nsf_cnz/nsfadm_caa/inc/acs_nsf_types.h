
#ifndef _ACS_NSF_TYPES_H
#define _ACS_NSF_TYPES_H


//!@todo To be removed once high level defines for boolean variables are defined for project
typedef bool BOOL;
#define TRUE true
#define FALSE false

#define DOUBLE double

// Result codes
#define NSF_RC_OK                            0
#define NSF_RC_NOK                           1


#define ACS_PHA_PARAM_RC_OK                  0
#define ACS_PHA_PARAM_RC_NOK                 1
#define ACS_PHA_PARAM_RC_ERROR               1



#define NSF_SERVICE_VER "1.0"


//! Internal Data structures
#include <string.h>
#include <ace/ACE.h>
#include <ace/Event.h>

#define NSF_PHA_PRODUCT "ACS/CXC1371974"

#define ACS_NSF_SHELL_PROG  "/bin/sh"

#define NSF_PHA_BASICDATA    "NSF_PHA_BasicData"

class ACS_NSF_Server;

typedef struct _NSF_BASIC_DATA
{
   ACE_UINT64   pingPeriod;
   ACE_UINT64   routerResponse;
   ACE_UINT64   failOvers;
   ACE_UINT64   resetTime;
   bool         ActiveFlag;
} NSF_BASIC_DATA, *LPNSF_BASIC_DATA ;

struct gwStruct
{
  ACE_INT32    gwIndex;                                 // Bit number in the alarmState
  std::string IPAddrStr;                       // IP address in string format (e.g. "134.138.80.1)".
  ACE_HANDLE threadH;                             // Thread handler
  ACE_HANDLE stopEvent[2];                   // Set to stop the thread before a failover
  ACE_INT32 network;			//It refers to the network.
};

struct eventStruct
{
        ACE_UINT32                      eventId;        // specific problem
        time_t                  reportTime; // Reporting time
        char                    objRef[64];     // Object of reference
        eventStruct*    next;           // Pointer to next in the list
};

struct threadParm
{
        ACS_NSF_Server* objPtr;
        int index;
};


#endif  // _ACS_NSF_TYPES_H


