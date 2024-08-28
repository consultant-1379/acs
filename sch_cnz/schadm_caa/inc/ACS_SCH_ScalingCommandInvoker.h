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

#ifndef SCHADM_CAA_INC_SCALING_COMMAND_INVOKER_H_
#define SCHADM_CAA_INC_SCALING_COMMAND_INVOKER_H_

#include "ACS_SCH_ScalingCommands.h"

namespace scaling_command_interface
{

class ScalingCommandInvoker
{
public:
	ScalingCommandInvoker();

	~ScalingCommandInvoker();

	ScalingCommandResult executeCommand(ScalingCommand* cmd);
};

}

#endif /* SCHADM_CAA_INC_SCALING_COMMAND_INVOKER_H_ */
