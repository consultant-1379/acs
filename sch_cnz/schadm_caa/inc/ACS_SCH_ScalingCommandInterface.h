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

#ifndef SCHADM_CAA_INC_SCALING_COMMAND_INTERFACE_H_
#define SCHADM_CAA_INC_SCALING_COMMAND_INTERFACE_H_

#include "ACS_SCH_ScalingCommandInvoker.h"
#include "ACS_SCH_ScalingCommandReceiver.h"
#include <ace/Singleton.h>
#include <string>

namespace scaling_command_interface
{

class ScalingCommandInterfaceImpl
{

	friend class ACE_Singleton<ScalingCommandInterfaceImpl, ACE_Recursive_Thread_Mutex>;

public:
	// Scale-out operations
	bool makeDumpBeLoadedByNewBlade(const std::string bc_name, std::string& op_result_str);

	bool introduceNewBladeIntoQuorum(const std::string bc_name, std::string& op_result_str);

	bool executeCACLP(std::string& op_result_str);

	bool takeNewBladeToClusterCpStateActive(const std::string bc_name, std::string& op_result_str);

	// Scale-in operations
	bool takeBladeOutOfQuorum(const std::string bc_name, std::string& op_result_str);

private:
	ScalingCommandInterfaceImpl();

	~ScalingCommandInterfaceImpl();

	bool getTrafficLeaderBcName (std::string& trafficLeaderBcName);

	ScalingCommandInvoker invoker_;

	ScalingCommandReceiver receiver_;
};

typedef ACE_Singleton<ScalingCommandInterfaceImpl, ACE_Recursive_Thread_Mutex> ScalingCommandInterface;

}

#endif /* SCHADM_CAA_INC_SCALING_COMMAND_INTERFACE_H_ */
