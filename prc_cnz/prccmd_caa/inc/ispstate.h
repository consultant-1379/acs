//========================================================================================
// 
// NAME
//		ispstate.h - include file for ISP
//
// COPYRIGHT
//		Ericsson AB 2004 - All Rights Reserved.
//
//		The Copyright to the computer program(s) herein is the	
//		property of Ericsson AB, Sweden. The program(s) may be
//		used and/or copied only with the written permission from
//		Ericsson AB or in accordance with the terms and conditions 
//		stipulated in the agreement/contract under which the 
//		program(s) have been supplied.
//
// DESCRIPTION 
//		State class for ispprint	
//
// ERROR HANDLING
//		Exception handling, see exception.h for details
//
// DOCUMENT NO
//		190 89-CAA 109 0520 Ux 
//
// AUTHOR 
//		2004-02-10 by EAB/UZ/DH UABTSO
//
// REVISION
//		R1A	
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R1A		040210  UABTSO		First version
//
// LINKAGE
//		-
//
// SEE ALSO 
//		-
//
//========================================================================================

#ifndef ISPSTATE_H
#define ISPSTATE_H

#include "ACS_PRC_ispapi.h"

enum apStateType
{
	down,
	degraded_nonred,
	degraded_red,
	up_nonred,
	up_red,
	unknownApState
};

enum listopt_t
{
	e_apstate =			0x01,
	e_runlevel =		0x02,
	e_nodestate =		0x04,
	e_resourcestate =	0x08,
	e_reason =			0x10,
	e_details =			0x20
};

//using namespace ACS_PRC;

//========================================================================================
//	Class Ispstate
//========================================================================================

template<typename S, int size, int normal>
class Ispstate
{
	friend std::ostream& 
		operator<< <S, size, normal>(std::ostream& s, Ispstate<S, size, normal>& ispstate);

public:
	Ispstate();
	~Ispstate();

//	void setTime(const Time& time = Time::NULL_);
//	void addTime(const Time& time);
	void setState(const S& state, reasonType reason);
	void setNode(char node);
	S getState() const {return state_;}
	char getNode() const {return node_;}
	reasonType getReason() const {return reason_;}
	int getTotalTime() const {return ttotal_;}
	static void format(int listopt);

private:
	S state_;								// State
	char node_;								// Node
	reasonType reason_;						// Reason
//	TimerMatrix<S, size, normal> tmatrix_;	// Time matrix
//	Time last_;								// Last state change
	int ttotal_;					// Total time
};


//#include "ispstate.cc"
//
const int normalRunLevel = 1 << level_5;
typedef Ispstate<runLevelType, 7, normalRunLevel> RunLevel;

const int normalNodeState = 1 << active | 1 << passive;
typedef Ispstate<nodeStateType, 3, normalNodeState> NodeState;

const int normalResourceState = 1 << started;
typedef Ispstate<resourceStateType, 6, normalResourceState> ResourceState;

const int normalApState = 1 << up_red;
typedef Ispstate<apStateType, 6, normalApState> APstate;

#endif
