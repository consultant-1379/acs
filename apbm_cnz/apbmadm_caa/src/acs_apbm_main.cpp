#include <unistd.h>
#include <stdio.h>

#include "acs_apbm_programmacros.h"
#include "acs_apbm_programconstants.h"
#include "acs_apbm_programconfiguration.h"
#include "acs_apbm_cmdoptionparser.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_serverwork.h"
#include <boost/filesystem.hpp>	

bool is_swm_2_0 = true;
bool isSWM20();
const char* const SWMVERSION="/cluster/storage/system/config/apos/swm_version";
std::string NodeStateSu = "safSu=SC-%s,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service";
std::string NodeStateSi = "safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";
namespace {
	inline void print_command_usage (const char * program_name) {
		fprintf(stderr, "USAGE: %s [--noha]\n", program_name);
	}
}


/*
 * Application main entry point
 */
int main (int argc, char * argv []) {
	// Load the program configuration, if any, so we can overwrite the default behaviour
	acs_apbm_programconfiguration::load();

	acs_apbm_cmdoptionparser cmd_options(argc, argv);

	if (cmd_options.parse() < 0) { //Error on parsing command line
		print_command_usage(cmd_options.program_name());
		return acs_apbm::PROGRAM_EXIT_BAD_INVOCATION;
	}

	//SwM 2.0 impacts
	is_swm_2_0 = isSWM20();
	if ( is_swm_2_0 == true ){
		NodeStateSu = "safSu=SC-%s,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service";
		NodeStateSi = "safSi=apg.nbi.aggregation.service-2N-1,safApp=ERIC-apg.nbi.aggregation.service";
	}
	else{
		NodeStateSu = "safSu=%s,safSg=2N,safApp=ERIC-APG";
		NodeStateSi = "safSi=AGENT,safApp=ERIC-APG";
	}

//	// Initialize the server logger
//	acs_apbm_logger::open(acs_apbm_programconfiguration::logger_appender_name);

	ACS_APBM_SYSLOG(LOG_NOTICE, LOG_LEVEL_INFO, "APBM Server started (PID = %d)", getpid());

	// Initialize the net-snmp and ssh libraries
	acs_apbm_snmpmanager::initialize(acs_apbm_programconfiguration::netsnmp_initializer_name);
	acs_apbm_ironsidemanager::initialize();
	ACS_APBM_LOG(LOG_LEVEL_INFO, "NET-SNMP library initialized");

	acs_apbm_serverwork serverwork(cmd_options);

	//int return_code = cmd_options.noha() ? serverwork.work_noha() : serverwork.work_ha();
	const int return_code = serverwork.work();

	// Reset the net-snmp and ssh libraries
	acs_apbm_snmpmanager::shutdown(acs_apbm_programconfiguration::netsnmp_initializer_name);
	acs_apbm_ironsidemanager::shutdown();
	ACS_APBM_LOG(LOG_LEVEL_INFO, "NET-SNMP library shutted down");

	ACS_APBM_SYSLOG(LOG_NOTICE, LOG_LEVEL_INFO, "APBM Server exit (PID = %d)", getpid());

	acs_apbm_logger::close();

	return return_code;
}

bool isSWM20()
{
    if((boost::filesystem::exists(boost::filesystem::path(SWMVERSION)))== true)
    {
        return true;
    }
    else {
        return false;
    }
}

