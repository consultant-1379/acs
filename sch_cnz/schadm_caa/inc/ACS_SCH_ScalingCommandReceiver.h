//	********************************************************
//
//	 COPYRIGHT Ericsson 2016
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2016.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2016 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#ifndef SCHADM_CAA_INC_SCALING_COMMAND_RECEIVER_H_
#define SCHADM_CAA_INC_SCALING_COMMAND_RECEIVER_H_

#include <string>
#include <vector>
#include <sstream>


#include "MCS_MML_Printout.h"
#include <ace/Get_Opt.h>
#include <ace/Task.h>
#include "mcs_mmlapi_session.h"



namespace scaling_command_interface {
	class ScalingCommandResult {
	public:
		inline ScalingCommandResult () : result_code(0), result_str() {}
		inline explicit ScalingCommandResult (int code) : result_code(code), result_str() {}
		inline explicit ScalingCommandResult (const std::string & str) : result_code(0), result_str(str) {}
		inline explicit ScalingCommandResult (const char * str) : result_code(0), result_str(str) {}
		inline ScalingCommandResult (int code, const std::string & str) : result_code(code), result_str(str) {}
		inline ScalingCommandResult (int code, const char * str) : result_code(code), result_str(str) {}

		inline ScalingCommandResult (const ScalingCommandResult & rhs) : result_code(rhs.result_code), result_str(rhs.result_str) {}

		inline ~ScalingCommandResult () {}

		ScalingCommandResult & operator= (const ScalingCommandResult & rhs) {
			if (this != &rhs) { result_code = rhs.result_code; result_str = rhs.result_str; }
			return *this;
		}

		int result_code;
		std::string result_str;
	};

	class ScalingCommandReceiver
	{
	public:
		ScalingCommandReceiver ();

		~ScalingCommandReceiver ();

		ScalingCommandResult copyFilesFromTrafficLeader (const char * data_root, const char * bc_name, const char * traffic_leader);

		ScalingCommandResult deleteBladeFiles (const char * data_root, const char * bc_name);

		ScalingCommandResult introduceNewBladeIntoQuorum (const char * bc_name);
		
		ScalingCommandResult executeCACLP ();

		ScalingCommandResult activateNewBlade (unsigned bc_id);

		ScalingCommandResult reconfigureBladeForReplacement (unsigned bc_id);

		ScalingCommandResult eraseBladeInfoFromQuorum (unsigned bc_id);

		ScalingCommandResult haltBlade (const char * bc_name);

	private:
		ScalingCommandResult copy_directory_contents (const char * src_dir, const char * dst_dir);

		ScalingCommandResult remove_directory (const char * dir);

		// Pair of command and command output string to be matched
		typedef std::pair<std::string, std::string> cmd_output_pair;

		int execute_ptcoi_commands_sequence(const char * bc_name, std::vector<cmd_output_pair>& cmd_seq, std::string& output_str);

		int executeCommand (const std::string& command, std::string& output);

		int executeMmlCommand (const std::string bc_name, const std::string& command, std::string& output);

		static ACE_THR_FUNC_RETURN ptcoi_command_thread (void* ptrParam);

		std::string intToString(int number);

		typedef struct{
			int fds;
			char* cp_name;
		} pt_session_struct;


	};
}


class SessionManager {
   public:
      SessionManager();
      int open (const std::string bc_scope);
      int close();
      int execMmlCmd(std::string cmd, MCS_MML_Printout& d_prt, MCS_MML_Printout& r_prt);

   private:
      MCS_MML_Session mml;



};


#endif /* SCHADM_CAA_INC_SCALING_COMMAND_RECEIVER_H_ */
