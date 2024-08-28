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

#include "ACS_SCH_ScalingCommandInterface.h"
#include "ACS_SCH_ScalingCommands.h"
#include "ACS_CS_API.h"
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Trace.h"
#include <sstream>
ACS_SCH_Trace_TDEF(ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE); 
namespace scaling_command_interface
{

ScalingCommandInterfaceImpl::ScalingCommandInterfaceImpl():invoker_(), receiver_()
{

}

ScalingCommandInterfaceImpl::~ScalingCommandInterfaceImpl()
{

}

bool ScalingCommandInterfaceImpl::makeDumpBeLoadedByNewBlade(const std::string bc_name, std::string& op_result_str)
{
	op_result_str.clear();
	bool result = false;
	std::string trafficLeaderBcName;
	
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Getting the traffic leader name..."));
	if(getTrafficLeaderBcName(trafficLeaderBcName))
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Obtain the traffic leader."));
		ScalingCommand* cmd = new CopyFilesFromTrafficLeader(&receiver_, &ScalingCommandReceiver::copyFilesFromTrafficLeader, trafficLeaderBcName, bc_name);

		ScalingCommandResult commandResult = invoker_.executeCommand(cmd);

		if(0 == commandResult.result_code)
			result = true;

		op_result_str = commandResult.result_str;
		delete cmd;
	}
	else
	{
		op_result_str = "CS API could not fetch traffic leader id";
	}

	return result;
}

bool ScalingCommandInterfaceImpl::introduceNewBladeIntoQuorum(const std::string bc_name, std::string& op_result_str)
{
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Adding %s to the quorum...",bc_name.c_str()));
	op_result_str.clear();
	bool result = false;

	ScalingCommand* cmd = new IntroduceNewBladeIntoQuorum(&receiver_, &ScalingCommandReceiver::introduceNewBladeIntoQuorum, bc_name);

	ScalingCommandResult commandResult = invoker_.executeCommand(cmd);

	if(0 == commandResult.result_code)
		result = true;

	op_result_str = commandResult.result_str;
	delete cmd;

	return result;
}

bool ScalingCommandInterfaceImpl::executeCACLP(std::string& op_result_str)
{
        op_result_str.clear();
        bool result = false;

        ScalingCommand* cmd = new ExecuteCACLP(&receiver_, &ScalingCommandReceiver::executeCACLP);

        ScalingCommandResult commandResult = invoker_.executeCommand(cmd);

        if(0 == commandResult.result_code)
                result = true;

        op_result_str = commandResult.result_str;
        delete cmd;

        return result;
}



bool ScalingCommandInterfaceImpl::takeNewBladeToClusterCpStateActive(const std::string bc_name, std::string& op_result_str)
{
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_ERROR,"Making the %s ACTIVE...",bc_name.c_str()));
	op_result_str.clear();
	bool result = false;

	ScalingCommand* cmd = new TakeNewBladeToClusterCpStateActive(&receiver_, &ScalingCommandReceiver::activateNewBlade, bc_name);

	ScalingCommandResult commandResult = invoker_.executeCommand(cmd);

	if(0 == commandResult.result_code)
		result = true;

	op_result_str = commandResult.result_str;
	delete cmd;

	return result;
}

bool ScalingCommandInterfaceImpl::takeBladeOutOfQuorum(const std::string bc_name, std::string& op_result_str)
{
	op_result_str.clear();
	op_result_str.append("\n***** takeBladeOutOfQuorum - START *****\n");
	bool result = false;

	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Executing REMBI command on %s .....",bc_name.c_str()));
	// REMBI
	ScalingCommand* cmd1 = new ReconfigureBladeForReplacement(&receiver_, &ScalingCommandReceiver::reconfigureBladeForReplacement, bc_name);

	ScalingCommandResult commandResult1 = invoker_.executeCommand(cmd1);

	result = ((0 == commandResult1.result_code) ? true : false);

	op_result_str.append(commandResult1.result_str);
	delete cmd1;

	if(!result)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Failed in executing REMBI command on %s",bc_name.c_str()));
		return result;
	}

	// Append LF
	op_result_str.append("\n");

	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"REMBI command executed successfully on %s",bc_name.c_str()));
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Executing CQMSR command on %s ...",bc_name.c_str()));

	// CQMSR
	ScalingCommand* cmd2 = new EraseBladeInfoFromQuorum(&receiver_, &ScalingCommandReceiver::eraseBladeInfoFromQuorum, bc_name);

	ScalingCommandResult commandResult2 = invoker_.executeCommand(cmd2);

	result = ((0 == commandResult2.result_code) ? true : false);

	op_result_str.append(commandResult2.result_str);
	delete cmd2;

	if(!result)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_ERROR,"CQMSR execution failed on %s",bc_name.c_str()));
		return result;
	}

	// Append LF
	op_result_str.append("\n");

	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"CQMSR command executed successfully on %s",bc_name.c_str()));
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Executing PTHAS on %s in PTCOI session...",bc_name.c_str()));

	// PTHAS
	ScalingCommand* cmd3 = new TakeBladeToHaltedState(&receiver_, &ScalingCommandReceiver::haltBlade, bc_name);

	ScalingCommandResult commandResult3 = invoker_.executeCommand(cmd3);

	result = ((0 == commandResult3.result_code) ? true : false);

	op_result_str.append(commandResult3.result_str);
	delete cmd3;

	if(!result)
	{
		ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_ERROR,"PTHAS execution failed on %s",bc_name.c_str()));
		return result;
	}

	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"PTHAS command executed successfully on %s",bc_name.c_str()));
	ACS_SCH_FTRACE((ACS_SCH_SCALINGCOMMANDINTERFACE_TRACE,LOG_LEVEL_INFO,"Performing %s APG cleanup(). ",bc_name.c_str()));

	// Append LF
	op_result_str.append("\n");

	// Cluster files deletion
	ScalingCommand* cmd4 = new DeleteBladeFiles(&receiver_, &ScalingCommandReceiver::deleteBladeFiles, bc_name);

	ScalingCommandResult commandResult4 = invoker_.executeCommand(cmd4);

	result = ((0 == commandResult4.result_code) ? true : false);

	op_result_str.append(commandResult4.result_str);
	delete cmd4;

	op_result_str.append("\n***** takeBladeOutOfQuorum - END *****\n");
	return result;
}

bool ScalingCommandInterfaceImpl::getTrafficLeaderBcName (std::string& trafficLeaderBcName)
{
	ACS_CS_API_NS::CS_API_Result csResult;
	bool result = false;
	trafficLeaderBcName.clear();
	CPID cpId(0);
	csResult = ACS_CS_API_NetworkElement::getTrafficLeader(cpId);
	if (ACS_CS_API_NS::Result_Success == csResult)
	{
		std::ostringstream sstream;
		sstream << "bc" << cpId;
		trafficLeaderBcName = sstream.str();
		result = true;
	}

	return result;
}

}
