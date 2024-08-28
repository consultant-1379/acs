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

#include <cstdlib>

#include "ACS_SCH_ScalingCommands.h"

namespace scaling_command_interface
{

const std::string tftp_path_ = "/data/apz/data";


ScalingCommand::ScalingCommand(ScalingCommandReceiver* pReceiver):
		pReceiver_(pReceiver)
{

}

ScalingCommand::~ScalingCommand()
{

}


CopyFilesFromTrafficLeader::CopyFilesFromTrafficLeader(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string trafficLeaderBcName, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), trafficLeader_(trafficLeaderBcName)
{
	char lower_bc_name [4*1024] = {0};
	for (int i = 0; (lower_bc_name[i] = tolower(bc_name[i])); ++i);
	bc_name_ = lower_bc_name;
}

CopyFilesFromTrafficLeader::~CopyFilesFromTrafficLeader()
{

}

ScalingCommandResult CopyFilesFromTrafficLeader::execute()
{
	return (pReceiver_->*pAction_)(tftp_path_.c_str(), bc_name_.c_str(), trafficLeader_.c_str());
}


DeleteBladeFiles::DeleteBladeFiles(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction)
{
	char lower_bc_name [4*1024] = {0};
	for (int i = 0; (lower_bc_name[i] = tolower(bc_name[i])); ++i);
	bc_name_ = lower_bc_name;
}

DeleteBladeFiles::~DeleteBladeFiles()
{

}

ScalingCommandResult DeleteBladeFiles::execute()
{
	return (pReceiver_->*pAction_)(tftp_path_.c_str(), bc_name_.c_str());
}


IntroduceNewBladeIntoQuorum::IntroduceNewBladeIntoQuorum(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), bc_name_(bc_name)
{

}

IntroduceNewBladeIntoQuorum::~IntroduceNewBladeIntoQuorum()
{

}

ScalingCommandResult IntroduceNewBladeIntoQuorum::execute()
{
	return (pReceiver_->*pAction_)(bc_name_.c_str());
}

ExecuteCACLP::ExecuteCACLP(ScalingCommandReceiver* pReceiver, CommandAction pAction):
                ScalingCommand(pReceiver), pAction_(pAction)
{

}

ExecuteCACLP::~ExecuteCACLP()
{

}

ScalingCommandResult ExecuteCACLP::execute()
{
        return (pReceiver_->*pAction_)();
}


TakeNewBladeToClusterCpStateActive::TakeNewBladeToClusterCpStateActive(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), bc_id_(0)
{
	std::string id_string = bc_name.substr(2);
	bc_id_ = atoi(id_string.c_str());
}

TakeNewBladeToClusterCpStateActive::~TakeNewBladeToClusterCpStateActive()
{

}

ScalingCommandResult TakeNewBladeToClusterCpStateActive::execute()
{
	return (pReceiver_->*pAction_)(bc_id_);
}


ReconfigureBladeForReplacement::ReconfigureBladeForReplacement(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), bc_id_(0)
{
	std::string id_string = bc_name.substr(2);
	bc_id_ = atoi(id_string.c_str());
}

ReconfigureBladeForReplacement::~ReconfigureBladeForReplacement()
{

}

ScalingCommandResult ReconfigureBladeForReplacement::execute()
{
	return (pReceiver_->*pAction_)(bc_id_);
}


EraseBladeInfoFromQuorum::EraseBladeInfoFromQuorum(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), bc_id_(0)
{
	std::string id_string = bc_name.substr(2);
	bc_id_ = atoi(id_string.c_str());
}

EraseBladeInfoFromQuorum::~EraseBladeInfoFromQuorum()
{

}

ScalingCommandResult EraseBladeInfoFromQuorum::execute()
{
	return (pReceiver_->*pAction_)(bc_id_);
}


TakeBladeToHaltedState::TakeBladeToHaltedState(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name):
		ScalingCommand(pReceiver), pAction_(pAction), bc_name_(bc_name)
{

}

TakeBladeToHaltedState::~TakeBladeToHaltedState()
{

}

ScalingCommandResult TakeBladeToHaltedState::execute()
{
	return (pReceiver_->*pAction_)(bc_name_.c_str());
}

}
