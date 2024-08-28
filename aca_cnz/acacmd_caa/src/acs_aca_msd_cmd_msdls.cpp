#include "acs_aca_msd_cmd_msdls.h"
#include "acs_aca_utility.h"
#include <acs_apgcc_omhandler.h>

static ACS_TRA_trace ACS_ACA_CMD_MSDLS_DebugTrace("ACS_ACA_CMD_MSDLS_DebugTrace", "C512");
static ACS_TRA_trace ACS_ACA_CMD_MSDLS_ErrorTrace("ACS_ACA_CMD_MSDLS_ErrorTrace", "C512");
#define NO_OF_AP 16
//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_MSDCMD_msdls::ACS_ACA_MSDCMD_msdls(ACE_INT32 argc, ACE_TCHAR* argv [], ACS_ACA_MSDCMD_Stub* stub) :
ACS_ACA_Command (argc, argv) {
	m_stub = stub;
	argc_ = argc;
	argv_ = argv;
	ms = const_cast<char *>("");
	ap = const_cast<char *>("");
	cp = const_cast<char *>("");
	data = const_cast<char *>("");
	option = 0;
	apid = 0;
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_MSDCMD_msdls::~ACS_ACA_MSDCMD_msdls(void){ }

//------------------------------------------------------------------------------
//                            Command parse
// parsing of the command line
void ACS_ACA_MSDCMD_msdls::parse () {
	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering into  ACS_ACA_MSDCMD_msdls::parse()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	//check CPSystem
	bool bMultipleCpSys = isMultipleCpSystem();
	m_stub->MCP = bMultipleCpSys;

	//cmd line option flags
	bool m_opt = false, d_opt = false, st_opt = false, n_opt = false,t_opt = false, a_opt = false, ap_opt = false, cp_opt = false;
	ACE_INT32 m_ind = 0, d_ind = 0, a_ind = 0, ap_ind = 0, cp_ind = 0, argInd = 1;
	//Analyze cmd line

	while(argInd < argc_) {
		if(!ACE_OS::strcmp(argv_[argInd],"-m")) {
			if(!m_opt) {
				m_opt = true;
				m_ind = argInd;
			}
			else
				usage();
		}
/*		else if(!ACE_OS::strcmp(argv_[argInd],"-s"))
		{
			if(!s_opt)
			{
				s_opt = true;
				s_ind = argInd;
			}
			else
				usage();
		}*/
/*		else if(!ACE_OS::strcmp(argv_[argInd],"-d"))
		{
			if(!d_opt)
			{
				d_opt = true;
				d_ind = argInd;
			}
			else
				usage();
		}
		else if(!ACE_OS::strcmp(argv_[argInd],"-S"))
		{
			if(!st_opt)
			{
				st_opt = true;
				st_ind = argInd;
			}
			else
				usage();
		}
		else if(!ACE_OS::strcmp(argv_[argInd],"-n"))
		{
			if(!n_opt)
			{
				n_opt = true;
				n_ind = argInd;
			}
			else
				usage();
		}
		else if(!ACE_OS::strcmp(argv_[argInd],"-t"))
		{
			if(!t_opt)
			{
				t_opt = true;
				t_ind = argInd;
			}
			else
				usage();
		}*/
		else if(!ACE_OS::strcmp(argv_[argInd],"-a")) {
			if(!a_opt) {
				a_opt = true;
				a_ind = argInd;
			}
			else
				usage();
		}
		else if(!ACE_OS::strcmp(argv_[argInd],"-ap")) {
			if(!ap_opt)	{
// XCASALE: -ap option is available also on SCP according to SPOE adaptation
//				if (!bMultipleCpSys) throw OPTION_EXCEPTION;
				ap_opt = true;
				ap_ind = argInd;
			}
			else
				usage();
		}
		else if(!ACE_OS::strcmp(argv_[argInd],"-cp")) {
			if(!cp_opt) {
				cp_opt = true;
				cp_ind = argInd;
			}
			else
				usage();
		}
		argInd++;
	}

	//check option
	if((argc_ == 3) && (m_opt == true)) {	//msdls -m MS ( common for single and multi CP)
		if(bMultipleCpSys == true) {	//msdls -m MS (only multiple)
			//check errors
			if(m_ind == (argc_ - 1))
				usage();

			if(!CheckData(argv_[m_ind + 1]))
				usage();

			//no errors
			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);//case insensitive
			SET_OPT_MS(option);
			m_stub->CURRENTAP = true;
		}
		else {	//msdls -m MS (only single)
			if(m_ind == (argc_ - 1))
				usage();

			if(!CheckData(argv_[m_ind + 1]))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			SET_OPT_MS(option);
			SET_OPT_SRC(option);	
			m_stub->CURRENTAP = true;
			//throw USAGE_EXCEPTION;
		}
	}
	else if((argc_ == 5) && (ap_opt == true) && (m_opt == true)) {	//msdls -ap AP -m MS (SCP and MCP) [SPOE adaptation]
		//check errors
		if((ap_ind == (argc_ -1)) || (m_ind == (argc_ - 1)))
			usage();

		if((!CheckData(argv_[ap_ind + 1])) || (!CheckData(argv_[m_ind +1])))
			usage();

		//no errors
		ap = argv_[ap_ind + 1];
		ap = to_upper(ap);
		CheckApName(ap,apid);
		ms = argv_[m_ind + 1];
//		*ms = toupper((char)*ms);
		SET_OPT_MS(option);
		SET_OPT_AP(option);

		if (!bMultipleCpSys)
			SET_OPT_SRC(option);

		m_stub->SPECIFICAP = true;
	}
	else if((argc_ == 3) && (cp_opt == true)) {	//msdls -cp CP (only multiple)
		if(bMultipleCpSys == true) {
			//check errors
			if(cp_ind == (argc_ -1))
				usage();

			if(!CheckData(argv_[cp_ind + 1]))
				usage();

			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_CP(option);
			m_stub->CURRENTAP = true;
		}
		else {//error
			throw OPTION_EXCEPTION;
		}
	}
	else if ((argc_ == 3) && (ap_opt == true)) {	// msdls -ap AP (both SCP and MCP)
		// Check errors
		if (ap_ind == (argc_ - 1))
			usage();

		if (!CheckData(argv_[ap_ind + 1]))
			usage();

		ap = argv_[ap_ind + 1];
		ap = to_upper(ap);
		CheckApName(ap, apid);
		SET_OPT_AP(option);
		m_stub->SPECIFICAP = true;
	}
	else if((argc_ == 5) && (m_opt == true) && (cp_opt == true)) {	//msdls -m MS -cp CP (only multiple)
		if(bMultipleCpSys == true) {
			//check errors
			if((m_ind == (argc_ - 1)) || (cp_ind == (argc_ - 1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
/*	else if((argc_ == 5) && (m_opt == true) && (s_opt == true))
	{//msdls -m MS -s site(only single)
		if(!bMultipleCpSys)
		{
			if((m_ind == (argc_ - 1)) || (s_ind == (argc_ - 1)))
				usage();
			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[s_ind + 1])))
				usage();
			ms = argv_[m_ind + 1];
			*ms = toupper((char)*ms);
			//ms = strupr(ms);
			cp = argv_[s_ind + 1];//site
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION; }
	}*/
	else if((argc_ == 8) && (m_opt == true) && (cp_opt == true) && (d_opt == true) && (t_opt == true)) {	//msdls -m MS -cp CP -d file -t (only multiple)
		if(bMultipleCpSys == true) {
			//check errors
			if((m_ind == (argc_ -1)) || (cp_ind == (argc_ -1)) || (d_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])) ||	(!CheckData(argv_[d_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];	
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_DATA(option);
			SET_OPT_TRUNC(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
/*	else if((argc_ == 8) && (m_opt == true) && (s_opt == true) && (d_opt == true) &&
		(t_opt == true))*/
#if 0
	else if((argc_ == 6) && (m_opt == true) && (d_opt == true) && (t_opt == true))
	{//msdls -m MS -s site -d file -t (only single)
		if(!bMultipleCpSys)
		{
			if((m_ind == (argc_ -1)) /*|| (s_ind == (argc_ -1))*/ || (d_ind == (argc_ -1)))
				usage();
			if((!CheckData(argv_[m_ind + 1])) /*|| (!CheckData(argv_[s_ind + 1]))*/ ||
				(!CheckData(argv_[d_ind + 1])))
				usage();
			ms = argv_[m_ind + 1];
			*ms = toupper((char)*ms);
			//ms = strupr(ms);
//			cp = argv_[s_ind + 1];//site
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			SET_OPT_DATA(option);
			SET_OPT_TRUNC(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION; }
	}
#endif
	else if((argc_ == 6) && (m_opt == true) && (cp_opt == true) && (st_opt == true)) {	//msdls -m MS -cp CP -S (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ -1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_STAT(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
#if 0
	else if((argc_ == 4) && (m_opt == true) /*&& (s_opt == true)*/ && (st_opt == true))
	{//msdls -m MS -s site -S (only single)
		if(!bMultipleCpSys)
		{
			if((m_ind == (argc_ -1)) /*|| (s_ind == (argc_ -1))*/)
				usage();
			if((!CheckData(argv_[m_ind + 1])) /*|| (!CheckData(argv_[s_ind + 1]))*/)
				usage();
			ms = argv_[m_ind + 1];
			*ms = toupper((char)*ms);
			//ms = strupr(ms);
//			cp = argv_[s_ind + 1];//site
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			SET_OPT_STAT(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION; }
	}
#endif
	else if((argc_ == 5) && (ap_opt == true) && (cp_opt == true)) {	//msdls -ap AP -cp CP (only multiple)
		if(bMultipleCpSys == true) {
			if((ap_ind == (argc_ - 1)) || (cp_ind == (argc_ - 1)))
				usage();

			if((!CheckData(argv_[ap_ind + 1])) || (!CheckData(argv_[ap_ind + 1])))
				usage();

			ap = argv_[ap_ind + 1];
			ap = to_upper(ap);
			CheckApName(ap,apid);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_AP(option);
			SET_OPT_CP(option);
			m_stub->SPECIFICAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if((argc_ == 6) && (m_opt == true) && (cp_opt == true) && (a_opt == true)) {	//msdls -m MS -cp CP -a (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (cp_ind == (argc_ - 1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_ALL(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if ((argc_ == 6) && ap_opt && m_opt && a_opt) {	// msdls -ap AP -m MS -a (only single-cp)
		if (!bMultipleCpSys) {
			if((m_ind == (argc_ - 1)) || (m_opt == (argc_ - 1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[m_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			ap = argv_[ap_ind + 1];
			ap = to_upper(ap);
			CheckApName(ap,apid);
			SET_OPT_MS(option);
			SET_OPT_AP(option);
			SET_OPT_ALL(option);
			SET_OPT_SRC(option);
			m_stub->SPECIFICAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if((argc_ == 4) && (m_opt == true) /*&& (s_opt == true)*/ && (a_opt == true)) {	//msdls -m MS -a (only single)
		if(!bMultipleCpSys)	{
			if((m_ind == (argc_ - 1)) /*|| (s_ind == (argc_ - 1))*/)
				usage();

			if((!CheckData(argv_[m_ind + 1])) /*|| (!CheckData(argv_[s_ind + 1]))*/)
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			SET_OPT_ALL(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if((argc_ == 7) && (ap_opt == true) && (m_opt == true) && (cp_opt == true)) {	//msdls -ap AP -m MS -cp CP (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (ap_ind == (argc_ - 1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[ap_ind + 1])) || (!CheckData(argv_[cp_ind + 1])))
				usage();

			ap = argv_[ap_ind + 1];
			ap = to_upper(ap);
			CheckApName(ap,apid);
			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_AP(option);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			m_stub->SPECIFICAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if((argc_ == 8) && (ap_opt == true) && (m_opt == true) && (cp_opt == true) && (a_opt == true)) {	//msdls -ap AP -m MS -cp CP -a (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (ap_ind == (argc_ - 1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[ap_ind + 1])) || (!CheckData(argv_[cp_ind + 1])))
				usage();

			ap = argv_[ap_ind + 1];
			ap = to_upper(ap);
			CheckApName(ap,apid);
			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_AP(option);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_ALL(option);
			m_stub->SPECIFICAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if((argc_ == 7) && (m_opt ==true) && (cp_opt == true) && (d_opt == true)) {	//msdls -m MS -cp CP -d file (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (d_ind == (argc_ - 1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])) || (!CheckData(argv_[d_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_DATA(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
#if 0
	else if((argc_ == 5) && (m_opt == true) /*&& (s_opt == true)*/ && (d_opt == true))
	{//msdls -m MS -s site -d file (only single)
		if(!bMultipleCpSys)
		{
			if((m_ind == (argc_ - 1)) || (d_ind == (argc_ - 1)) /*|| (s_ind == (argc_ -1))*/)
				usage();
			if((!CheckData(argv_[m_ind + 1])) /*|| (!CheckData(argv_[s_ind + 1])) */ ||
				(!CheckData(argv_[d_ind + 1])))
				usage();
			ms = argv_[m_ind + 1];
			*ms = toupper((char)*ms);
			//ms = strupr(ms);
//			cp = argv_[s_ind + 1]; //site
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			SET_OPT_DATA(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION; }
	}
#endif
	else if((argc_ == 8) && (m_opt == true) && (cp_opt == true) && (d_opt == true) && (n_opt == true)) {	//msdls -m MS -cp CP -d file -n (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (d_ind == (argc_ - 1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[cp_ind + 1])) ||	(!CheckData(argv_[d_ind + 1])))
				usage();

			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_DATA(option);
			SET_OPT_NUM(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
#if 0
	else if((argc_ == 6) && (m_opt == true) /*&& (s_opt == true)*/ && (d_opt == true) &&
		(n_opt == true))
	{//msdls -m MS -s site -d file -n (only single)
		if(!bMultipleCpSys)
		{
			if((m_ind == (argc_ - 1)) || (d_ind == (argc_ - 1)) /*|| (s_ind == (argc_ -1))*/)
				usage();
			if((!CheckData(argv_[m_ind + 1])) || /*(!CheckData(argv_[s_ind + 1])) ||*/
				(!CheckData(argv_[d_ind + 1])))
				usage();
			ms = argv_[m_ind + 1];
			*ms = toupper((char)*ms);
			//ms = strupr(ms);
//			cp = argv_[s_ind + 1];//site
			data = argv_[d_ind + 1];
			SET_OPT_MS(option);
			SET_OPT_SRC(option);
			SET_OPT_DATA(option);
			SET_OPT_NUM(option);
			m_stub->CURRENTAP = true;
		}
		else {
			throw OPTION_EXCEPTION; }
	}
#endif
	else if((argc_ == 8) && (m_opt == true) && (cp_opt == true) && (ap_opt == true) && (st_opt == true)) {	//msdls -ap AP -m MS -cp CP -S (only multiple)
		if(bMultipleCpSys == true) {
			if((m_ind == (argc_ - 1)) || (ap_ind == (argc_ - 1)) || (cp_ind == (argc_ -1)))
				usage();

			if((!CheckData(argv_[m_ind + 1])) || (!CheckData(argv_[ap_ind + 1])) ||	(!CheckData(argv_[cp_ind + 1])))
				usage();

			ap = argv_[ap_ind + 1];
			ap = to_upper(ap);
			CheckApName(ap,apid);
			ms = argv_[m_ind + 1];
//			*ms = toupper((char)*ms);
			cp = argv_[cp_ind + 1];
			cp = to_upper(cp);
			SET_OPT_AP(option);
			SET_OPT_MS(option);
			SET_OPT_CP(option);
			SET_OPT_STAT(option);
			m_stub->SPECIFICAP = true;
		}
		else {
			throw OPTION_EXCEPTION;
		}
	}
	else if(argc_ == 1) {	//msdls
		m_stub->ALLAP = true;
	}
	else {
		usage();
	}

	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::parse()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
}

//------------------------------------------------------------------------------
//	Execute command
//------------------------------------------------------------------------------
void ACS_ACA_MSDCMD_msdls::execute () {
	//calling to msdls in ACA_ACA_MSDCMD_Stub
	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering into  ACS_ACA_MSDCMD_msdls::execute()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if (ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[2048];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "Calling ACA_ACA_MSDCMD_Stub::msdls() with the following parameters: "
				"[options == '%hu'][ap == '%s'][apid == '%u'][cp == '%s'][ms == '%s']",
				option, ap, apid, cp, ms);
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	m_stub->msdls(option, ap, apid, cp, ms, data);

	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::execute()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
}

//------------------------------------------------------------------------------
//	Print command usage
//------------------------------------------------------------------------------
void ACS_ACA_MSDCMD_msdls::usage() {
	if(m_stub->MCP == true)//multiple
		throw USAGE_EXCEPTION_NEW;
	else //single
		throw USAGE_EXCEPTION;
}
//-------------------------------------------------------------------------------
//           CheckCPSystem
//---------------------------------------------------------------------------------
bool ACS_ACA_MSDCMD_msdls::isMultipleCpSystem() {
	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	bool isMultipleCPSystem = false;
	ACS_ACA_CS_API::CS_API_Result_ACA result = (ACS_ACA_CS_API::CS_API_Result_ACA) ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);

	switch (result)	{
		case ACS_ACA_CS_API::Result_Success:
		{
			if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
				char traceBuffer[512];
				ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Returning  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()with Result_Success");
				ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			return isMultipleCPSystem;
		}

		case ACS_ACA_CS_API::Result_NoEntry:
		{
			if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
				char traceBuffer[512];
				ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Returning  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()with Result_NoEntry");
				ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			throw CS_EXCEPTION56;
		}

		case ACS_ACA_CS_API::Result_NoValue:
		{
			if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
				char traceBuffer[512];
				ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Returning  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()with Result_NoValue");
				ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			throw CS_EXCEPTION55;
		}

		case ACS_ACA_CS_API::Result_NoAccess:
		{
			if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
				char traceBuffer[512];
				ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Returning  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()with Result_NoAccess");
				ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			throw CS_EXCEPTION55;
		}

		case ACS_ACA_CS_API::Result_Failure:
		{
			if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
				char traceBuffer[512];
				ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Returning  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()with Result_Failure");
				ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			throw CS_EXCEPTION55;
		}
	}

	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::isMultipleCpSystem()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	return false;
}

//***************************************************************************************
//             CheckApName
//***************************************************************************************
void ACS_ACA_MSDCMD_msdls::CheckApName(char* apname, unsigned int & apid) {
	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering  ACS_ACA_MSDCMD_msdls::CheckApName()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if(apname) {
		string ap(apname);
		string str;

		//check len of the name
		if((ap.length() > 5) || (ap.length() < 3))
			throw APNAME_EXCEPTION;

		//check first 2 chars
		str = ap.substr(0,2);
		if((str.compare("AP") != 0) && (str.compare("ap") != 0) && (str.compare("Ap") != 0)	&& (str.compare("aP") != 0))
			throw APNAME_EXCEPTION;

		//check last chars
		str = ap.substr(2,(ap.length() - 2));

		// check if the string contains not only numbers
		if (str.find_first_not_of("0123456789") != std::string::npos)
			throw APNAME_EXCEPTION;

		// check if the string has the format AP0XX
		if (!str.substr(0, 1).compare("0"))
			throw APNAME_EXCEPTION;

		if(( atoi(str.c_str()) > NO_OF_AP ) || ( atoi(str.c_str()) < 1))
			throw APNAME_EXCEPTION;

		//set id
		apid = 2000 + atoi(str.c_str());
	}
	else
		throw APNAME_EXCEPTION;

	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::CheckApName()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
}

//******************************************************************************
//                         CheckData
//*******************************************************************************
bool ACS_ACA_MSDCMD_msdls::CheckData(const char* data) {
	if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering  ACS_ACA_MSDCMD_msdls::CheckData()");
		ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	if(data) {
		string str(data);

		if(str.compare("") == 0) return false;

		//find char -
		ACE_INT32 ind = (int)str.find_first_of("-", 0);

		if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON()) {
			char traceBuffer[512];
			ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::CheckData()");
			ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}

		return (ind == 0) ? false : true;
	}

	if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON()) {
		char traceBuffer[512];
		ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving  ACS_ACA_MSDCMD_msdls::CheckData()");
		ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	}
	return false;
}

char * ACS_ACA_MSDCMD_msdls::to_upper(char * str) {
	int str_len = ::strlen(str);
	char tmp_str[str_len + 1];

	for (int i = 0; i < str_len; i++)
		tmp_str[i] = ::toupper(str[i]);
	tmp_str[str_len] = 0;

	::strncpy(str, tmp_str, str_len);

	return str;
}
