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

#ifndef SCHADM_CAA_INC_SCALING_COMMANDS_H_
#define SCHADM_CAA_INC_SCALING_COMMANDS_H_

#include "ACS_SCH_ScalingCommandReceiver.h"
#include <string>

namespace scaling_command_interface
{

class ScalingCommand
{
public:
	ScalingCommand(ScalingCommandReceiver* pReceiver);

	virtual ~ScalingCommand();

	virtual ScalingCommandResult execute() = 0;

protected:
	ScalingCommandReceiver* pReceiver_;
};


class CopyFilesFromTrafficLeader: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(const char *, const char *, const char *);

public:
	 CopyFilesFromTrafficLeader(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string trafficLeaderBcName, const std::string bc_name);

	~CopyFilesFromTrafficLeader();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	std::string trafficLeader_;
	std::string bc_name_;
};


class DeleteBladeFiles: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(const char *, const char *);

public:
	DeleteBladeFiles(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~DeleteBladeFiles();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	std::string bc_name_;
};


class IntroduceNewBladeIntoQuorum: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(const char *);

public:
	IntroduceNewBladeIntoQuorum(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~IntroduceNewBladeIntoQuorum();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	std::string bc_name_;
};

class ExecuteCACLP: public ScalingCommand
{
        typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)();

public:
        ExecuteCACLP(ScalingCommandReceiver* pReceiver, CommandAction pAction);

        ~ExecuteCACLP();

        ScalingCommandResult execute();

private:
        CommandAction pAction_;
};


class TakeNewBladeToClusterCpStateActive: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(unsigned);

public:
	TakeNewBladeToClusterCpStateActive(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~TakeNewBladeToClusterCpStateActive();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	unsigned bc_id_;
};


class ReconfigureBladeForReplacement: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(unsigned);

public:
	ReconfigureBladeForReplacement(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~ReconfigureBladeForReplacement();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	unsigned bc_id_;
};


class EraseBladeInfoFromQuorum: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(unsigned);

public:
	EraseBladeInfoFromQuorum(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~EraseBladeInfoFromQuorum();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	unsigned bc_id_;
};


class TakeBladeToHaltedState: public ScalingCommand
{
	typedef ScalingCommandResult(ScalingCommandReceiver:: *CommandAction)(const char *);

public:
	TakeBladeToHaltedState(ScalingCommandReceiver* pReceiver, CommandAction pAction, const std::string bc_name);

	~TakeBladeToHaltedState();

	ScalingCommandResult execute();

private:
	CommandAction pAction_;
	std::string bc_name_;
};

}

#endif /* SCHADM_CAA_INC_SCALING_COMMANDS_H_ */
