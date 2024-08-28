/*
 * acs_alh_ahtrecord.cpp
 *
 *  Created on: Oct 31, 2011
 *      Author: efabron
 */


#include "acs_alh_ahtrecord.h"
#include "acs_alh_common.h"
#include "ace/OS.h"
#include <stdio.h>

//========================================================================================
//	Constructors
//========================================================================================

acs_alh_ahtrecord::acs_alh_ahtrecord() : specificProblem_(0)
{
	strncpy(percSeverity_, "", sizeof(percSeverity_) - 1);
	strncpy(probableCause_, "", sizeof(probableCause_) - 1);
	strncpy(problemText_, "", sizeof(problemText_) - 1);
}

acs_alh_ahtrecord::acs_alh_ahtrecord(long specProb, const char percSev[],
									 const char probCause[], const char probText[]) : specificProblem_(specProb)
{
	snprintf(percSeverity_, sizeof(percSeverity_) - 1, "%s", percSev);
	snprintf(probableCause_, sizeof(probableCause_) - 1, "%s", probCause);
	snprintf(problemText_, sizeof(problemText_) - 1, "%s", probText);
}


acs_alh_ahtrecord::acs_alh_ahtrecord(const acs_alh_ahtrecord& other) : specificProblem_(other.specificProblem_)
{
	strncpy(percSeverity_, other.percSeverity_, sizeof(percSeverity_) - 1);
	strncpy(probableCause_, other.probableCause_, sizeof(probableCause_) - 1);
	strncpy(problemText_, other.problemText_, sizeof(problemText_) - 1);
}


//========================================================================================
//	Destructor
//========================================================================================

acs_alh_ahtrecord::~acs_alh_ahtrecord()
{
}


//========================================================================================
//	Assignment operator
//========================================================================================

acs_alh_ahtrecord& acs_alh_ahtrecord::operator=(const acs_alh_ahtrecord& other)
{
	specificProblem_ = other.specificProblem_;
	strncpy(percSeverity_, other.percSeverity_, sizeof(percSeverity_) - 1);
	strncpy(probableCause_, other.probableCause_, sizeof(probableCause_) - 1);
	strncpy(problemText_, other.problemText_, sizeof(problemText_) - 1);

	return *this;
}


//========================================================================================
//	Equality operator
//========================================================================================

bool acs_alh_ahtrecord::operator==(const acs_alh_ahtrecord& other) const
{
	if (specificProblem_ == other.specificProblem_ &&
		!strcmp(percSeverity_, other.percSeverity_) &&
		!strcmp(probableCause_, other.probableCause_) &&
		!strcmp(problemText_, other.problemText_))
	{
		return true;
	}
	else
	{
		return false;
	}
}


//========================================================================================
//	Un-equality operator
//========================================================================================

bool acs_alh_ahtrecord::operator!=(const acs_alh_ahtrecord& other) const
{
	return !(*this == other);
}


//========================================================================================
//	Get specific problem
//========================================================================================

long acs_alh_ahtrecord::getSpecProb() const
{
	return specificProblem_;
}


//========================================================================================
//	Get percieved severity
//========================================================================================

const char* acs_alh_ahtrecord::getPercSev() const
{
	return percSeverity_;
}


//========================================================================================
//	Get probable cause
//========================================================================================

const char* acs_alh_ahtrecord::getProbCause() const
{
	return probableCause_;
}


//========================================================================================
//	Get problem text
//========================================================================================

const char* acs_alh_ahtrecord::getProbText() const
{
	return problemText_;
}
