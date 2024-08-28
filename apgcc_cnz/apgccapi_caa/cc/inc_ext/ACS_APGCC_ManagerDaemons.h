#ifndef ACS_APGCC_MANAGERDAEMONS_H_
#define ACS_APGCC_MANAGERDAEMONS_H_
#include "ACS_TRA_trace.h"
/** @class ACS_APGCC_ManagerDaemons
 *	@brief ACS_APGCC_ManagerDaemons class
 *	@author xantior (Antonio Iorio)
 *	@date 2011-05-06
 *	@version R1A
 *
 *	ACS_APGCC_ManagerDaemons Class detailed description
 */
class ACS_APGCC_ManagerDaemons
{
public:
	/**	@brief daemonize method
		 *	daemonize method: perform a demonize of application.
		 *	daemonize method: It used to transform the application in a daemon.
		 *
		 *  @param user_name: the name of the user will be started the application
		 *  @log_trace: trace functionality
		 *	@return void
		 *
		 */
	static void daemonize(const char* user_name, ACS_TRA_trace* log_trace);
};
#endif /* ACS_APGCC_MANAGERDAEMONS_H_*/
