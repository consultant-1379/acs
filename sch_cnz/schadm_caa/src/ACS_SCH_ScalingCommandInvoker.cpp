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

#include "ACS_SCH_ScalingCommandInvoker.h"

namespace scaling_command_interface
{

ScalingCommandInvoker::ScalingCommandInvoker()
{

}

ScalingCommandInvoker::~ScalingCommandInvoker()
{

}

ScalingCommandResult ScalingCommandInvoker::executeCommand(ScalingCommand* cmd)
{
	return cmd->execute();
}

}
