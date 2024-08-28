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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <cstring>

#include <utility>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>

#include "ACS_CS_API.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_ScalingCommandReceiver.h"

#include "ACS_SCH_Trace.h"

using namespace std;
int  _fd_master=-1;
ACS_SCH_Trace_TDEF(ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE); 

namespace scaling_command_interface {
	ScalingCommandReceiver::ScalingCommandReceiver () {}

	ScalingCommandReceiver::~ScalingCommandReceiver () {}

	ScalingCommandResult ScalingCommandReceiver::copyFilesFromTrafficLeader (const char * data_root, const char * bc_name, const char * traffic_leader)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Copying boot files for new blade from traffic leader..."));
		// Build the complete path for the bc_name
		char bc_complete_path [4*1024] = {0}; // A local temporary 16Kb buffer for path
		snprintf(bc_complete_path, sizeof(bc_complete_path), "%s/%s", data_root, bc_name);

		char traffic_leader_path [4*1024] = {0};
		snprintf(traffic_leader_path, sizeof(traffic_leader_path), "%s/%s", data_root, traffic_leader);

		// Build the absolute real path to the traffic_leader directory
		char traffic_leader_real_path [4*1024] = {0};
		if (! realpath(traffic_leader_path, traffic_leader_real_path)) {
			// ERROR: on building the path
			int errno_save = errno;
			/*char error_description [2*1024] = {0};

			snprintf(error_description, sizeof(error_description), "\nError on building the absolute path for the traffic leader files directory: "
					"data_root == '%s': traffic_leader == '%s': traffic_leader_path == '%s': errno == %d",
					data_root, traffic_leader, traffic_leader_path, errno_save);*/

			string error_description="\nError on building the absolute path for the traffic leader files directory: data_root="+string(data_root)+" traffic_leader="+string(traffic_leader)+" traffic_leader_path="+string(traffic_leader_path)+" errno="+intToString(errno_save);


			return ScalingCommandResult(-1, error_description);
		}

		// Make the copy of traffic leader files into the created directory
		ScalingCommandResult result = copy_directory_contents(traffic_leader_real_path, bc_complete_path);
		if (result.result_code) {
			// ERROR: On copying traffic leader files
			return result;
		}

		//update the board content "m0_0_1_0_s<n>
		int number;
		sscanf(bc_name,"bc%d", &number);
  	char board_content[128]={0};
		snprintf(board_content, sizeof(board_content), "m0_0_1_0_s%d",number+1);
		
		char command[4*1024]={0};
		snprintf(command, sizeof(command), "printf %s > %s/%s/cpa/board", board_content, data_root, bc_name);

		system(command);

		// Create the symbolic link
		char BC_NAME [4*1024] = {0};
		for (int i = 0; (BC_NAME[i] = toupper(bc_name[i])); ++i) ;

		char BC_COMPLETE_PATH [4*1024] = {0}; // A local temporary 16Kb buffer for path
		snprintf(BC_COMPLETE_PATH, sizeof(BC_COMPLETE_PATH), "%s/%s", data_root, BC_NAME);

		if (symlink(bc_name, BC_COMPLETE_PATH) && (errno != EEXIST)) {
			// ERROR: on creating the symbolic link
			int errno_save = errno;

			/*char error_description [2*1024] = {0};

			snprintf(error_description, sizeof(error_description), "\nError creating the symbolic link to the '%s' data files directory: "
					"data_root == '%s': bc_complete_path == '%s': traffic_leader == '%s': traffic_leader_path == '%s': "
					"traffic_leader_real_path == '%s': errno == %d", bc_name, data_root, bc_complete_path, traffic_leader,
					traffic_leader_path, traffic_leader_real_path, errno_save);*/

			string error_description="\nError creating the symbolic link to the "+string(bc_name)+" data files directory: data_root="+string(data_root)+" bc_complete_path="+string(bc_complete_path)+" traffic_leader == "+string(traffic_leader)+" traffic_leader_path =="+string(traffic_leader_path)+"traffic_leader_real_path"+string(traffic_leader_real_path)+" errno= "+intToString(errno_save);

			return ScalingCommandResult(-1, error_description);
		}

		//create the symbolic link at /data/apz/data/board path
		char board_name[2*1024]={0};
		char cpa_path[2*1024]={0};
		snprintf(board_name, sizeof(board_name), "%s/board/%s", data_root,board_content);
		snprintf(cpa_path, sizeof(cpa_path), "%s/%s/cpa", data_root, bc_name);
		
		
		if(symlink(cpa_path, board_name) && (errno != EEXIST)) {
			int errno_save = errno;
     /* char error_description [2*1024] = {0};

      snprintf(error_description, sizeof(error_description), "\nError creating the symbolic link to the '%s' source and '%s' destination"
					" with error number: %d", board_name,cpa_path, errno_save); */
			string error_description="\nnError creating the symbolic link to the "+string(board_name)+" source and "+string(cpa_path)+ "destination with error number: "+intToString(errno_save);

      return ScalingCommandResult(-1, error_description);

		}
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"files copied successfully."));
		return result;
	}

	ScalingCommandResult ScalingCommandReceiver::copy_directory_contents (const char * src_dir, const char * dst_dir)
	{
		char copy_dir_command [1024] = {0};
		snprintf(copy_dir_command, sizeof(copy_dir_command), "cp -rf %s %s", src_dir, dst_dir);

		std::string command(copy_dir_command);
		std::string command_output;
		int command_result = executeCommand(command, command_output);
		return ScalingCommandResult(command_result, command_output);
	}

	ScalingCommandResult ScalingCommandReceiver::deleteBladeFiles (const char * data_root, const char * bc_name)
	{
		string name(bc_name);
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Deleting the %s  files...",name.c_str()));
		// Build the complete path for the bc_name
		char bc_path [4*1024] = {0}; // A local temporary 16Kb buffer for path
		snprintf(bc_path, sizeof(bc_path), "%s/%s", data_root, bc_name);

		// Build the absolute real path to the blade directory
		char bc_real_path [4*1024] = {0};
		if (!realpath(bc_path, bc_real_path) && (errno != ENOENT))
		{
			// ERROR: on building the path
			int errno_save = errno;
			/*char error_description [2*1024] = {0};

			snprintf(error_description, sizeof(error_description), "\nError on building the absolute path for the blade files directory: "
					"data_root == '%s': bc_name == '%s': bc_path == '%s': errno == %d",
					data_root, bc_name, bc_path, errno_save);*/

			string error_description="\n Error on building the absolute path for the blade files directory: data_root = "+string(data_root)+"bc_name = "+string(bc_name)+" bc_path = "+string(bc_path)+"errno_save = "+intToString(errno_save);

			return ScalingCommandResult(-1, error_description);
		}

		// Remove the symbolic link
		char BC_NAME [4*1024] = {0};
		for (int i = 0; (BC_NAME[i] = toupper(bc_name[i])); ++i) ;

		char BC_COMPLETE_PATH [4*1024] = {0}; // A local temporary 16Kb buffer for path
		snprintf(BC_COMPLETE_PATH, sizeof(BC_COMPLETE_PATH), "%s/%s", data_root, BC_NAME);

		if (unlink(BC_COMPLETE_PATH) && (errno != ENOENT))
		{
			// ERROR: on removing the symbolic link
			int errno_save = errno;
			/*char error_description [2*1024] = {0};

			snprintf(error_description, sizeof(error_description), "\nError removing the symbolic link to the '%s' data files directory: "
					"data_root == '%s': bc_path == '%s': errno == %d", bc_name, data_root, bc_path, errno_save);*/

			string error_description="\n Error removing the symbolic link to the "+string(bc_name)+" data files directory: data_root == "+string(data_root)+"  bc_path = "+string(bc_path)+" errno == "+intToString(errno_save); 

			return ScalingCommandResult(-1, error_description);
		}

		//Remove softlink
		int magazineNumber;
		sscanf(bc_name, "bc%d", &magazineNumber);
		char board_path[1024]={0};
		sprintf(board_path,"%s/board/m0_0_1_0_s%d", data_root, magazineNumber+1);
    
		if (unlink(board_path) && (errno != ENOENT))
    {
      // ERROR: on removing the symbolic link
      int errno_save = errno;
      /*char error_description [2*1024] = {0};

      snprintf(error_description, sizeof(error_description), "\nError removing the symbolic link to the '%s' data files directory: "
          "data_root == '%s': bc_path == '%s': errno == %d", bc_name, data_root, bc_path, errno_save);*/

			string error_description="\n Error removing the symbolic link to the "+string(bc_name)+" data files directory: data_root == "+string(data_root)+"  bc_path = "+string(bc_path)+" errno == "+intToString(errno_save);

      return ScalingCommandResult(-1, error_description);
    }


		// Remove cluster blade directory
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Deleted Successfully."));
		return remove_directory(bc_real_path);
	}

	ScalingCommandResult ScalingCommandReceiver::remove_directory (const char * dir)
	{
		char remove_dir_command [1024] = {0};
		snprintf(remove_dir_command, sizeof(remove_dir_command), "rm -rf %s", dir);

		std::string command(remove_dir_command);
		std::string command_output;
		int command_result = executeCommand(command, command_output);
		return ScalingCommandResult(command_result, command_output);
	}

	ScalingCommandResult ScalingCommandReceiver::introduceNewBladeIntoQuorum (const char * bc_name)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Fetching the apt type..."));
		// Get the APT Type
		std::string aptType = "";
		ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getAPTType(aptType);
		if (ACS_CS_API_NS::Result_Success != result)
		{
			return ScalingCommandResult(-1, "CS API failed to get APT Type");
		}

		if(aptType.compare("HLR") && aptType.compare("MSC"))
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nAPT Type %s not expected", aptType.c_str());*/

			string error_description="\nAPT Type "+string(aptType)+" not expected ";
 
			return ScalingCommandResult(-1, error_description);
		}
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"aptType is: %s",aptType.c_str()));

		std::string cp_scope(bc_name);
		const std::string cp_not_obtainable		= "CP not obtainable";
		const std::string initiation_performed	= "INITIATION PERFORMED";
		
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Executing CQMSI on %s  command...",cp_scope.c_str()));
		char cqmsi_command [2*1024] = {0};
		snprintf(cqmsi_command, sizeof(cqmsi_command), "cqmsi:app=%s;", aptType.c_str());

		std::string command(cqmsi_command);
		std::string command_output;
		int command_result = -1;
		const int max_retries = 30;
		int retry = max_retries;
		int sleep_time = 20;
		
		do
		{
			//This piece of code is for logging purpose
      char log_msg[256]={0};
      sprintf(log_msg,"Iteration-%d : Executing CQMSP on %s to check blade state",max_retries-retry+1,cp_scope.c_str());

      string msg(log_msg);
      ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,msg.c_str()));

			sleep(sleep_time);
			command_result = executeMmlCommand(cp_scope, command, command_output);
			retry--;
		}
		while(command_output.find(cp_not_obtainable) != std::string::npos && retry!=0);

		if(0 == retry)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nmml -cp %s cqmsi:app=%s failed. CP still not obtainable after %i minutes",
					bc_name, aptType.c_str(), ((sleep_time*max_retries)/60));*/

		string error_description="\nmml -cp "+string(bc_name)+"  cqmsi:app="+string(aptType)+" failed. CP still not obtainable after "
			+intToString(((sleep_time*max_retries)/60))+" minutes";

			return ScalingCommandResult(-1, error_description);
		}

		if(0 != command_result || command_output.find(initiation_performed) == std::string::npos)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nmml -cp %s cqmsi:app=%s failed, result code: %i, command output: %s",
					bc_name, aptType.c_str(), command_result, command_output.c_str());*/

			string error_description="\nmml -cp "+string(bc_name)+" cqmsi:app= "+string(aptType)+" failed, result code: "+intToString(command_result)+" command output: "+string(command_output); 

			return ScalingCommandResult(-1, error_description);
		}

		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Execution of CQMSI is successful"));
		return ScalingCommandResult(command_result, command_output);
	}

 	ScalingCommandResult ScalingCommandReceiver::executeCACLP ()
        {
                char caclp_command [2*1024] = {0};
                snprintf(caclp_command, sizeof(caclp_command), "caclp;");

                std::string command(caclp_command);
                std::string command_output;
		std::string cp_scope;
                int command_result = -1;
	      	//char log_msg[256]={0};
      		//string msg(log_msg);
      		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Executing CACLP command..."));

                        command_result = executeMmlCommand(cp_scope,command, command_output);

                if(0 != command_result)
                {
                       /* char error_description [2*1024] = {0};
                        snprintf(error_description, sizeof(error_description), "CACLP failed, result code: %i, command output: %s",
                                        command_result, command_output.c_str());*/

												string error_description="CACLP failed, result code: "+intToString(command_result)+", command output: "+string(command_output);

                        return ScalingCommandResult(-1, error_description);
                }
                return ScalingCommandResult(command_result, command_output);
	}


	ScalingCommandResult ScalingCommandReceiver::activateNewBlade (unsigned bc_id)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Executing CQACI command...."));
		std::string bc_scope1 ("");

		// CQACI
		char cqaci_command [2*1024] = {0};
		snprintf(cqaci_command, sizeof(cqaci_command), "cqaci:cp=%u;", bc_id);

		std::string command1(cqaci_command);
		std::string command_output1;
		//const std::string session_locked = "SESSION LOCKED DURING CLONING";
		const std::string ordered = "ORDERED";

		int command_result1 = executeMmlCommand(bc_scope1, command1, command_output1);

		//if(command_output1.find(session_locked) == std::string::npos)
		if(command_output1.find(ordered) == std::string::npos)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nmml cqaci:cp=%u failed, result code: %i, command output: %s",
					bc_id, command_result1, command_output1.c_str());*/


			string error_description="\nmml cqaci:cp="+intToString(bc_id)+" failed, result code: "+intToString(command_result1)+" , command output: "+string(command_output1);

			return ScalingCommandResult(-1, error_description);
		}

		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"CQACI command ordered succssfully."));
		char cp_name [10] = {0};
		snprintf(cp_name, sizeof(cp_name), "BC%u", bc_id);

		std::string bc_scope2 (cp_name);
		// CQMSP
		std::string command2("cqmsp;");
		std::string command_output2;
		const std::string cp_not_obtainable	= "CP not obtainable";
		const std::string active			= "ACTIVE";
		int command_result2 = -1;
		const int max_retries = 15;
		int retry = max_retries;
		int sleep_time = 20;

		do
		{
			//This piece of code is for logging purpose
			/*char log_msg[256]={0};
			sprintf(log_msg,"Iteration-%d : Executing CQMSP after CQACI to check blade state",max_retries-retry+1);*/

			string log_msg="Iteration-"+intToString(max_retries-retry+1)+" : Executing CQMSP after CQACI to check blade state";

			string msg(log_msg);
			ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,msg.c_str()));

			sleep(sleep_time);
			command_result2 = executeMmlCommand(bc_scope2, command2, command_output2);
			if(0 != command_result2 && command_output2.find(cp_not_obtainable) == std::string::npos)
			{
				/*char error_description [2*1024] = {0};
				snprintf(error_description, sizeof(error_description), "\nmml -cp BC%u cqmsp failed, result code: %i, command output: %s",
						bc_id, command_result2, command_output2.c_str());*/


				string error_description="\nmml -cp BC "+intToString(bc_id)+"  cqmsp failed, result code: "+intToString(command_result2)+" , command output: "+string(command_output2);

				return ScalingCommandResult(-1, error_description);
			}
			retry--;
		}
		while(command_output2.find(active) == std::string::npos && retry!=0);

		if(0 == retry)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nCP still not in ACTIVE state after %i minutes, command output: %s",
					((sleep_time*max_retries)/60), command_output2.c_str());*/

			string error_description="\nCP still not in ACTIVE state after "+intToString(((sleep_time*max_retries)/60))+"  minutes, command output: "+string(command_output2);

			return ScalingCommandResult(-1, error_description);
		}
	
		return ScalingCommandResult(command_result2, command_output2);
	}

	ScalingCommandResult ScalingCommandReceiver::reconfigureBladeForReplacement (unsigned bc_id)
	{

		std::string bc_scope ("");

		// REMBI
		char rembi_command [2*1024] = {0};
		snprintf(rembi_command, sizeof(rembi_command), "rembi:cp=%u;", bc_id);

		std::string command(rembi_command);
		std::string command_output;
		const std::string cp_not_obtainable				= "CP not obtainable";
		const std::string function_busy					= "FUNCTION BUSY";
		const std::string intervention_not_permitted 	= "NOT SUCCESSFUL";
		int command_result = -1;
		const int max_retries = 3;
		int retry = max_retries;
		int sleep_time = 20;

		do
		{
			command_result = executeMmlCommand(bc_scope, command, command_output);

			if(0 == command_result) // REMBI ORDERED
			{
				if(command_output.find(intervention_not_permitted) == std::string::npos)
				{
					// Cluster CP manual intervention is permitted
					break;
				}
				else
				{
					// Cluster CP manual intervention is not permitted
				/*	char error_description [2*1024] = {0};
					snprintf(error_description, sizeof(error_description), "\nmml rembi:cp=%u not successful: Cluster CP manual intervention is not permitted, "
							"result code: %i, command output: %s",
							bc_id, command_result, command_output.c_str());*/

					string error_description="\nmml rembi:cp= "+intToString(bc_id)+"  not successful: Cluster CP manual intervention is not permitted,result code: "+intToString(command_result)+", command output:"+string(command_output);

					return ScalingCommandResult(-1, error_description);
				}
			}
			else if(0 != command_result && command_output.find(cp_not_obtainable) == std::string::npos && command_output.find(function_busy) == std::string::npos)
			{
				/*char error_description [2*1024] = {0};
				snprintf(error_description, sizeof(error_description), "\nmml rembi:cp=%u failed, result code: %i, command output: %s",
						bc_id, command_result, command_output.c_str());*/

			 string error_description="\nmml rembi:cp= "+intToString(bc_id)+"  failed, result code: "+intToString(command_result)+", command output: "+string(command_output);


				return ScalingCommandResult(-1, error_description);
			}
			else
			{
				// Either CP not obtainable or FUNCTION BUSY
				retry--;
				if(retry > 0)
					sleep(sleep_time);
			}
		}
		while(0 != retry);

		if(0 == retry)
		{
		/*	char error_description [2*1024] = {0};
		snprintf(error_description, sizeof(error_description), "\nMax retry attempts:<%i> reached, mml rembi:cp=%u still failing after %i minutes, command output: %s",
					max_retries, bc_id, ((sleep_time*max_retries)/60), command_output.c_str());*/

			string error_description="\nMax retry attempts:<"+intToString(max_retries)+"> reached, mml rembi:cp="+intToString(bc_id)+" still failing after "+intToString(((sleep_time*max_retries)/60))+", command output: "+string(command_output);


			return ScalingCommandResult(-1, error_description);
		}

		/*char success_description [2*1024] = {0};
		snprintf(success_description, sizeof(success_description), "\nmml rembi:cp=%u\n%s", bc_id, command_output.c_str());*/

		string success_description="\nmml rembi:cp= "+intToString(bc_id)+"\n"+string(command_output);

		return ScalingCommandResult(command_result, success_description);
	}

	ScalingCommandResult ScalingCommandReceiver::eraseBladeInfoFromQuorum (unsigned bc_id)
	{

		std::string bc_scope ("");

		// CQMSR
		char rembi_command [2*1024] = {0};
		snprintf(rembi_command, sizeof(rembi_command), "cqmsr:cp=%u;", bc_id);

		//string rembi_command="cqmsr:cp="+intToString(bc_id);
		std::string command(rembi_command);
		std::string command_output;
		const std::string cp_not_obtainable		= "CP not obtainable";
		const std::string function_busy			= "FUNCTION BUSY";
		const std::string cluster_handler_busy	= "CLUSTER HANDLER BUSY";
		const std::string executed 				= "EXECUTED";
		int command_result = -1;
		const int max_retries = 3;
		int retry = max_retries;
		int sleep_time = 20;

		do
		{
			command_result = executeMmlCommand(bc_scope, command, command_output);

			if(command_output.find(executed) != std::string::npos)
			{
				// CQMSR EXECUTED
				break;
			}
			else if(command_output.find(cp_not_obtainable) 		== std::string::npos &&
					command_output.find(function_busy) 			== std::string::npos &&
					command_output.find(cluster_handler_busy) 	== std::string::npos)
			{
				// CQMSR NOT ACCEPTED
				/*char error_description [2*1024] = {0};
				snprintf(error_description, sizeof(error_description), "\nmml cqmsr:cp=%u failed, result code: %i, command output: %s",
						bc_id, command_result, command_output.c_str());*/

        string error_description="\n mml cqmsr:cp= "+intToString(bc_id)+"  failed, result code: "+intToString(command_result)+" , command output: "+string(command_output);

				return ScalingCommandResult(-1, error_description);
			}
			else
			{
				// Either CP not obtainable or FUNCTION BUSY or CLUSTER HANDLER BUSY
				retry--;
				if(retry > 0)
					sleep(sleep_time);
			}
		}
		while(0 != retry);

		if(0 == retry)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nMax retry attempts:<%i> reached, mml cqmsr:cp=%u still failing after %i minutes, command output: %s",
					max_retries, bc_id, ((sleep_time*max_retries)/60), command_output.c_str());*/

			string error_description="\nMax retry attempts:<"+intToString(max_retries)+"> reached, mml cqmsr:cp="+intToString(bc_id)+" still failing after "+intToString(((sleep_time*max_retries)/60))+", command output: "+string(command_output);

			return ScalingCommandResult(-1, error_description);
		}

		/*char success_description [2*1024] = {0};
		snprintf(success_description, sizeof(success_description), "\nmml cqmsr:cp=%u\n%s", bc_id, command_output.c_str());*/
	
		string success_description="\n \nmml cqmsr:cp= "+intToString(bc_id)+" \n"+string(command_output);

		return ScalingCommandResult(command_result, success_description);
	}

	ScalingCommandResult ScalingCommandReceiver::haltBlade (const char * bc_name)
	{
		const std::string pthas_command			= "PTHAS;\r\n";
		const std::string expected_pthas_output	= "EXECUTED";
		const std::string function_busy			= "FUNCTION BUSY";
		std::string pthas_command_output;
		int command_result = -1;
		const int max_retries = 3;
		int retry = max_retries;
		int sleep_time = 20;

		std::vector<cmd_output_pair> cmd_seq;
		cmd_seq.push_back(cmd_output_pair(pthas_command, expected_pthas_output));

		do
		{
			command_result = execute_ptcoi_commands_sequence(bc_name, cmd_seq, pthas_command_output);

			if(0 == command_result)
			{
				// PTHAS EXECUTED
				//ACS_SCH_Logger::log("PTHAS executed successfully.",SCH_makeOrigin(),LOG_LEVEL_INFO);
				break;
			}
			else if(pthas_command_output.find(function_busy) == std::string::npos)
			{
				// PTHAS NOT ACCEPTED
				ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Failed in executing PTHAS command."));	
				/*char error_description [2*1024] = {0};
				snprintf(error_description, sizeof(error_description), "\nPTHAS failed for blade %s, result code: %i, command output: %s",
						bc_name, command_result, pthas_command_output.c_str());*/


				string error_description="\nPTHAS failed for blade "+string(bc_name)+" result code: "+intToString(command_result)+" , command output: "+string(pthas_command_output);
				return ScalingCommandResult(-1, error_description);
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Retrying the execution of PTHAS..."));	
				// FUNCTION BUSY
				retry--;
				if(retry > 0)
					sleep(sleep_time);
			}
		}
		while(0 != retry);

		if(0 == retry)
		{
			/*char error_description [2*1024] = {0};
			snprintf(error_description, sizeof(error_description), "\nMax retry attempts:<%i> reached, PTHAS for blade %s still failing after %i minutes, command output: %s",
					max_retries, bc_name, ((sleep_time*max_retries)/60), pthas_command_output.c_str());*/

			string error_description="\nMax retry attempts:<"+intToString(max_retries)+"> reached, PTHAS for blade "+string(bc_name)+" still failing after "+intToString(((sleep_time*max_retries)/60))+", command output: "+string(pthas_command_output);

			return ScalingCommandResult(-1, error_description);
		}

		/*char success_description [2*1024] = {0};
		snprintf(success_description, sizeof(success_description), "\n/opt/ap/mas/bin/mas_cptaspmml PTCOI -cp %s -echo 7\n%s", bc_name, pthas_command_output.c_str());*/

    string success_description="\n /opt/ap/mas/bin/mas_cptaspmml PTCOI -cp "+string(bc_name)+"  -echo 7\n "+string(pthas_command_output);

		return ScalingCommandResult(command_result, success_description);
	}

	int ScalingCommandReceiver::execute_ptcoi_commands_sequence(const char * bc_name, std::vector<cmd_output_pair>& cmd_seq, std::string& output_str)
	{
		// This function opens a PTY in order to control the interaction with a PTCOI session
		// The provided command sequence contains the pairs of commands to be executed in the cpt< prompt and the expected outputs to be matched
		// The YES command is automatically issued at the startup of the PTCOI session
		// The PTCOE is automatically issued either at the end of the sequence or if some output match fails.

		int fdm, fds;
		int rc;

		output_str.clear();

		std::string ptcoi_command_output;
		bool expected_string_matched = true;
		const char cpt_prompt 			= '<';
		const std::string yes			= "Y;\r\n";
		const std::string yes_output	= "EXECUTED";
		const std::string ptcoe			= "PTCOE;\r\n";
		const std::string ptcoe_output	= "EXECUTED";

		// Insert Y; to the top of the commands sequence
		std::vector<cmd_output_pair>::iterator it;
		it = cmd_seq.begin();
		cmd_seq.insert(it, cmd_output_pair(yes, yes_output));

		// Insert PTCOE; to the bottom of commands sequence
		cmd_seq.push_back(cmd_output_pair(ptcoe, ptcoe_output));

		int cmd_seq_dim = cmd_seq.size();

		const std::string ptcoi_warning = "CPT COMMANDS ARE DANGEROUS TO CLUSTER CP";
		bool ptcoi_warning_found = false;

		if(_fd_master == -1)
		{
			fdm = posix_openpt(O_RDWR);
			if (fdm < 0)
			{
				output_str.append("Error on posix_openpt() \n");
				return -1;
			}
			rc = grantpt(fdm);
			if (rc != 0)
			{
				output_str.append("Error on grantpt() \n");
				close(fdm);
				return -1;
			}

			rc = unlockpt(fdm);
			if (rc != 0)
			{
				output_str.append("Error on unlockpt() \n");
				close(fdm);
				return -1;
			}
		}
		else
			fdm=_fd_master;

		// Open the slave side ot the PTY
		fds = open(ptsname(fdm), O_RDWR);
		if (fds < 0)
		{
			output_str.append("Error on posix_openpt() \n");
			close(fdm);
			return -1;
		}


		std::string cp_name (bc_name);
		pt_session_struct *file_refs = NULL;
		file_refs = (pt_session_struct*) ::malloc(sizeof(pt_session_struct));

		if (file_refs == NULL) {
			output_str.append("ERROR: Failed to create the struct info for the thread! \n");
			close(fdm);
			close(fds);
			return -1;
		}

		file_refs->fds = fds;
		file_refs->cp_name = (char*)bc_name;

		if (ACE_Thread::spawn(ptcoi_command_thread, file_refs, THR_DETACHED) < 0) {
			output_str.append("ERROR: Failed to start ptcoi_command_thread thread! \n");
			close(fdm);
			close(fds);
			::free(file_refs);
			return -1;
		}
		//ACS_SCH_Logger::log("Thread spawned for ptcoi.",SCH_makeOrigin(),LOG_LEVEL_INFO);

		int i = 0;
		while (i <= cmd_seq_dim)
		{
			char buffer[4096] = {'\0'};
			ssize_t bytes_read = 0;
			std::string str_buffer="";

			int read_res = 0;
			do
			{
				ACS_SCH_TRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,"Waiting for cpt prompt/ warning message"));
				// get the output in buffer

				memset(buffer,0,4096);
				//if((read_res = read(fdm, (buffer + bytes_read), sizeof(buffer))) > 0)
				if((read_res = read(fdm, buffer, sizeof(buffer)-sizeof(char))) > 0)
				{
					// The number of bytes read is returned and the file position is advanced by this number.
					// Let's advance also buffer position.
					ACS_SCH_TRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,"bytes read= %d and read_res=%d",bytes_read,read_res));
					bytes_read += read_res;
					str_buffer=str_buffer+(string)buffer;
				}
				ACS_SCH_TRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,"Data in buffer is %s",str_buffer.c_str()));
				usleep(100);
			}
			while((read_res > 0) && !strchr(str_buffer.c_str(), cpt_prompt) && (str_buffer.find(ptcoi_warning) == std::string::npos));

			if (bytes_read > 0) // No error
			{

				// Send data on standard output or wherever you want
				ptcoi_command_output.append(str_buffer);
				output_str.append (ptcoi_command_output);

				if (0 == i)
				{
					// At the first successful read, PTCOI warning message is expected
					if(ptcoi_command_output.find(ptcoi_warning) != std::string::npos)
					{
						ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"ptcoi warning msg received"));
						ptcoi_warning_found = true;
					}
					else
					{
						output_str.append("\nUnexpected error: PTCOI warning message not received.\n");
						return -1;
					}
				}
				else if((!cmd_seq[i-1].second.empty()) && (ptcoi_command_output.find(cmd_seq[i-1].second) == std::string::npos) && (i < cmd_seq_dim))
				{
					// Output does not match expected string. Issue the PTCOE in order to close the session.
					expected_string_matched = false;
					i = (cmd_seq_dim -1);
				}
			}
			else
			{
				ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Enter into failed scenario"));
				output_str.append("\nFailed to read from master PTY \n");

				if(ptcoi_warning_found) // Here maybe due to child death after PTCOE
				{
					output_str.append("\nFailed to read from master PTY.Warning \n");
					break;
				}
				else // Here due to PTCOI failure
				{
					output_str.append("\nPTCOI failed\n");
					return -1; // Failure
				}
			}

			if(i < cmd_seq_dim)
			{
				// Send data on the master side of PTY
				//output_str.append("\nSend data on the master side of PTY\n");
				ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Executing %s command and expected output is %s",cmd_seq[i].first.c_str(),cmd_seq[i].second.c_str()));
				write(fdm, cmd_seq[i].first.c_str(), cmd_seq[i].first.length());
				ptcoi_command_output.clear();
			}
			++i;
		} // End while

		if(expected_string_matched)
		{
			ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Commands executed successfully"));
			output_str.append ("\nCPT session commands sequence successfully completed.\n");
			if( _fd_master == -1)
				_fd_master=fdm;
			return 0; // OK
		}
		else
		{
			output_str.append ("\nCPT printouts not expected.\n");
			return -1; // Failure
		}
	}


	int ScalingCommandReceiver::executeMmlCommand(const std::string bc_scope, const std::string& cmd, std::string& output)
	{
		int result = -1;
		output.clear();

		MCS_MML_Printout d_prt;
		MCS_MML_Printout r_prt;
        SessionManager sessionManager;

        result = sessionManager.open(bc_scope);
		if (result == 0) {  // Mml session opened correctly
			int cmdRes = sessionManager.execMmlCmd(cmd, d_prt, r_prt);
			output.append("\n");
			if (cmdRes == 0) {

				output.append("\n");

				// Direct Printout fetching
				for (int i=0; i<d_prt.lineCount(); i++) {
					output.append(d_prt.line[i]);
					output.append("\n");

				}

				// Result Printout fetching if exists
				if (r_prt.lineCount() > 0) {
					for (int i=0; i<r_prt.lineCount(); i++) {
						output.append(r_prt.line[i]);
						output.append("\n");

					}
				}

			}else {

				output.append("Session Opened but command not executed");
				result = -1;

			}

			sessionManager.close();

		}else {

			// Error code handling

			switch (result) {

				case MCS_MML_Session::RC_AUTHFAILURE:
				{
					output.append("Authorization failure");
					break;
				}
				case MCS_MML_Session::RC_ALREADYUSED:
				{

					output.append("Device is occupied");
					break;
				}
				case MCS_MML_Session::RC_INCOMPLETECPGROUP:
				{

					output.append("CP not obtainable");
						break;
				}

				case MCS_MML_Session::RC_SERVER_UNREACHABLE:
				{

					output.append("Unable to connect to server");
					break;
				}

				case MCS_MML_Session::RC_CPNOTDEFINED:
				{

					output.append("CP name or CP group is not defined");
					break;
				}

				case MCS_MML_Session::RC_CMD_EXECUTING_ERROR:
				{
					output.append("Error when executing ( general fault )");
					break;
				}

				case MCS_MML_Session::RC_FAILED:
				default:
				{
					output.append("CP not obtainable");
					break;

				}

			}

		}

		return result;

	}

	string ScalingCommandReceiver::intToString(int number)
	{
		std::ostringstream ss;
		ss<<number;
	  return ss.str();
	}



	int ScalingCommandReceiver::executeCommand(const std::string& command, std::string& output)
	{
		int result = -1;
		output.clear();
		FILE* pipe_ = popen(command.c_str(), "r");
		if(NULL != pipe_)
		{
			char rowOutput[1024]={'\0'};

			while(!feof(pipe_))
			{
				// get the cmd output
				if(fgets(rowOutput, 1023, pipe_) != NULL)
				{
					std::size_t len = strlen(rowOutput);
					// remove the newline
					if( rowOutput[len-1] == '\n' ) rowOutput[len-1] = 0;

					output.append(rowOutput);
					output.append(" ");
				}
			}
			// wait cmd termination
			int exitCode = pclose(pipe_);

			// get the exit code from the exit status
			result = WEXITSTATUS(exitCode);
		}
		else
		{
			output = "popen() failed";
		}
		return result;
	}

	/*============================================================================
			ROUTINE: ISF_UpdateCallbackHnd
		 ============================================================================ */
	ACE_THR_FUNC_RETURN ScalingCommandReceiver::ptcoi_command_thread(void* ptrParam)
	{
			//ACS_SCH_Logger::log("Inside ptcoi session.",SCH_makeOrigin(),LOG_LEVEL_INFO);
			pt_session_struct* fd_list  = (pt_session_struct*) ptrParam;

			struct termios slave_orig_term_settings; // Saved terminal settings
			struct termios new_term_settings; // Current terminal settings

			int fds = fd_list->fds;

			char* cp_name = fd_list->cp_name;

			::free (fd_list);


			// Save the defaults parameters of the slave side of the PTY
			tcgetattr(fds, &slave_orig_term_settings);

			// Set RAW mode on slave side of PTY
			new_term_settings = slave_orig_term_settings;
			cfmakeraw (&new_term_settings);
			tcsetattr (fds, TCSANOW, &new_term_settings);

			int stdinCopy, stdoutCopy, stdErr;

			stdinCopy = dup (0);

			stdoutCopy = dup (1);

			stdErr = dup (2);

			// The slave side of the PTY becomes the standard input and outputs of the child process
			close(0); // Close standard input (current terminal)
			close(1); // Close standard output (current terminal)
			close(2); // Close standard error (current terminal)

			dup(fds); // PTY becomes standard output (0)
			dup(fds); // PTY becomes standard output (1)
			dup(fds); // PTY becomes standard error (2)


			// Now the original file descriptor is useless
			close(fds);


			// Make the current process a new session leader
			//setsid();

			// As the child is a session leader, set the controlling terminal to be the slave side of the PTY
			// (Mandatory for programs like the shell to make them manage correctly their outputs)
			ioctl(0, TIOCSCTTY, 1);

			// Execution of the program
			/*char PTCOI [64] = {0};
			snprintf(PTCOI, sizeof(PTCOI), "/opt/ap/mas/bin/mas_cptaspmml PTCOI -cp %s -echo 7", cp_name);*/

			string PTCOI="/opt/ap/mas/bin/mas_cptaspmml PTCOI -cp "+string(cp_name)+"  -echo 7";

			ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Executing commands in ptcoi..."));
			system(PTCOI.c_str());
			//ACS_SCH_Logger::log("commands executed.",SCH_makeOrigin(),LOG_LEVEL_INFO);

			close(0); // Close standard input (current terminal)
			close(1); // Close standard output (current terminal)
			close(2); // Close standard error (current terminal)

			dup2 (stdinCopy, 0);
			dup2 (stdoutCopy, 1);
			dup2 (stdErr, 2);

			close (stdinCopy);
			close (stdoutCopy);
			close (stdErr);

			ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMADNRECEIVER_TRACE,LOG_LEVEL_INFO,"Leaving ptcoi session."));
			return 0;
	}
}

//----------------------------------------------------------------------------
SessionManager::SessionManager() {
   // Empty
}



//----------------------------------------------------------------------------
int SessionManager::open(const std::string bc_scope) {

	int res = mml.open_ext("",bc_scope.c_str(), MS_CPEX, MML_ANY_DEVICE, false, "ADMINISTRATOR", 0, false);
	return res;
}
//----------------------------------------------------------------------------
int SessionManager::close() {

	mml.close();
	return 0;
}



//-------------------------------------------------------
int SessionManager::execMmlCmd(std::string cmd, MCS_MML_Printout& d_prt, MCS_MML_Printout& r_prt) {

	MCS_MML_READORDER readOrder;

	readOrder = mml.execCmd(cmd.c_str(),d_prt, true);

	if (readOrder == MR_RDCMD) {
		if ( d_prt.cmdResult() == MC_ORDERED )  {
			mml.getPrintout(r_prt, MP_RESULT, -1);
		}
	}else
		return -1;


    return 0;
}
