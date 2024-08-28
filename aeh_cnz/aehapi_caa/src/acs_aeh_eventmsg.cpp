//========================================================================================
/** @file  acs_aeh_eventmsg.cpp
 *	@brief
 *	@author xfabron (Fabio Ronca)
 *	@date 2011-05-18
 *	@version 1.0.1
 *
 *	COPYRIGHT Ericsson AB, 2011
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and dissemination to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| P1.0.1 | 2011-05-18 | xfabron      | File created.                       |
 *	+--------+------------+--------------+-------------------------------------+
 *	| P1.0.1 |            | xfabron      | Released for ITSTEAM2 sprint 12     |
 *	+--------+------------+--------------+-------------------------------------+
 *	|        |            |              |                                     |
 *	+========+============+==============+=====================================+
 *
 * SEE ALSO
 *		-
 *
 */
//========================================================================================

#include "acs_aeh_eventmsg.h"
#include <sstream>
#include <string.h>
#include <iostream>

#ifndef APEVENT_MSG_LABEL
#define APEVENT_MSG_LABEL 		" MSG: "
#endif

#ifndef PROCESS_NAME_LABEL
#define PROCESS_NAME_LABEL 		" -PNAME: "
#endif

#ifndef SPEC_PROBLEM_LABEL
#define SPEC_PROBLEM_LABEL 		" -SPROB: "
#endif

#ifndef SEVERITY_LABEL
#define SEVERITY_LABEL 			" -SEV: "
#endif

#ifndef MANUAL_CEASE_LABEL
#define MANUAL_CEASE_LABEL 		" -CEASE: "
#endif

#ifndef PROBABLE_CAUSE_LABEL
#define PROBABLE_CAUSE_LABEL 	" P_CAUSE: "
#endif

#ifndef CLASS_OF_REF_LABEL
#define CLASS_OF_REF_LABEL 		" CLASS_REF: "
#endif

#ifndef OBJ_OF_REF_LABEL
#define OBJ_OF_REF_LABEL 		" OBJ_REF: "
#endif

#ifndef PROB_DATA_LABEL
#define PROB_DATA_LABEL 		" P_DATA: "
#endif

#ifndef PROB_TEXT_LABEL
#define PROB_TEXT_LABEL 		" P_TEXT: "
#endif

#ifndef ACS_AEH_DELIMITER
#define ACS_AEH_DELIMITER 	"##"
#endif

//========================================================================================
//	Default constructor
//========================================================================================

acs_aeh_eventmsg::acs_aeh_eventmsg(): priority(), nodeName(), userName(), processName(), percSeverity(), probableCause(), objClassOfReference(), objectOfReference(), problemData(), problemText(), eventMessage()
{
	specificProblem = 0;
	manualCease = false;
}


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_eventmsg::acs_aeh_eventmsg(const char * p_processName, long p_specificProblem,
					const char * p_percSeverity, const char * p_probableCause,
					const char * p_objClassOfReference, const char * p_objectOfReference,
					const char * p_problemData, const char * p_problemText,
					bool p_manualCease): eventMessage()
{

	setProcessName(p_processName);
	setSpecificProblem(p_specificProblem);
	setPercSeverity(p_percSeverity);
	setProbableCause(p_probableCause);
	setObjClassOfReference(p_objClassOfReference);
	setObjectOfReference(p_objectOfReference);
	setProblemData(p_problemData);
	setProblemText(p_problemText);
	setManualCease(p_manualCease);

}


//========================================================================================
//	Constructor
//========================================================================================

acs_aeh_eventmsg::acs_aeh_eventmsg(const char* p_priority, const char* p_nodeName,
					const char * p_nodeStatus, const char * p_userName,
					const char * p_processName, long p_specificProblem,
					const char * p_percSeverity, const char * p_probableCause,
					const char * p_objClassOfReference, const char * p_objectOfReference,
					const char * p_problemData, const char * p_problemText,
					bool p_manualCease): eventMessage()
{
	setPriorityMsg(p_priority);
	setNodeName(p_nodeName);
	setNodeStatus(p_nodeStatus);
	setUserName(p_userName);
	setProcessName(p_processName);
	setSpecificProblem(p_specificProblem);
	setPercSeverity(p_percSeverity);
	setProbableCause(p_probableCause);
	setObjClassOfReference(p_objClassOfReference);
	setObjectOfReference(p_objectOfReference);
	setProblemData(p_problemData);
	setProblemText(p_problemText);
	setManualCease(p_manualCease);

}


//========================================================================================
//	Destructor
//========================================================================================

acs_aeh_eventmsg::~acs_aeh_eventmsg()
{
}


//========================================================================================
//	Copy constructor
//========================================================================================

acs_aeh_eventmsg::acs_aeh_eventmsg(const acs_aeh_eventmsg& p_eventMsg)
{

	priority = p_eventMsg.priority;
	nodeName = p_eventMsg.nodeName;
	nodeStatus = p_eventMsg.nodeStatus;
	userName = p_eventMsg.userName;
	processName = p_eventMsg.processName;
	specificProblem = p_eventMsg.specificProblem;
	percSeverity = p_eventMsg.percSeverity;
	probableCause = p_eventMsg.probableCause;
	objClassOfReference = p_eventMsg.objClassOfReference;
	objectOfReference = p_eventMsg.objectOfReference;
	problemData = p_eventMsg.problemData;
	problemText = p_eventMsg.problemText;
	manualCease = p_eventMsg.manualCease;
	eventMessage = p_eventMsg.eventMessage;

}


//========================================================================================
//	Assignment operator
//========================================================================================

acs_aeh_eventmsg&
acs_aeh_eventmsg::operator=(const acs_aeh_eventmsg& p_eventMsg)
{
	if(this == &p_eventMsg){
		return *this;
	}else{
		priority = p_eventMsg.priority;
		nodeName = p_eventMsg.nodeName;
		nodeStatus = p_eventMsg.nodeStatus;
		userName = p_eventMsg.userName;
		processName = p_eventMsg.processName;
		specificProblem = p_eventMsg.specificProblem;
		percSeverity = p_eventMsg.percSeverity;
		probableCause = p_eventMsg.probableCause;
		objClassOfReference = p_eventMsg.objClassOfReference;
		objectOfReference = p_eventMsg.objectOfReference;
		problemData = p_eventMsg.problemData;
		problemText = p_eventMsg.problemText;
		manualCease = p_eventMsg.manualCease;
		eventMessage = p_eventMsg.eventMessage;

		return *this;
	}

}


//========================================================================================
//	Comparison operator==
//========================================================================================

bool acs_aeh_eventmsg::operator==(const acs_aeh_eventmsg& p_msg){

//	std::cout<<"1"<<std::endl;
	if( (priority.compare(p_msg.priority))!=0  )
		return false;
//	std::cout<<"2"<<std::endl;
	if( (nodeName.compare(p_msg.nodeName))!=0 )
		return false;
//	std::cout<<"3"<<std::endl;
	if( (nodeStatus.compare(p_msg.nodeStatus))!=0 )
		return false;
//	std::cout<<"4"<<std::endl;
	if( (userName.compare(p_msg.userName))!=0 )
		return false;
//	std::cout<<"5"<<std::endl;
	if( (processName.compare(p_msg.processName))!=0 )
		return false;
//	std::cout<<"6"<<std::endl;
	if( specificProblem!=p_msg.specificProblem)
		return false;
//	std::cout<<"7"<<std::endl;
	if( (percSeverity.compare(p_msg.percSeverity))!=0 )
		return false;
//	std::cout<<"8"<<std::endl;
	if( (probableCause.compare(p_msg.probableCause))!=0 )
		return false;
//	std::cout<<"9"<<std::endl;
	if( (objClassOfReference.compare(p_msg.objClassOfReference))!=0 )
		return false;
//	std::cout<<"10"<<std::endl;
	if( (objectOfReference.compare(p_msg.objectOfReference))!=0 )
		return false;
//	std::cout<<"11"<<std::endl;
	if( (problemData.compare(p_msg.problemData))!=0 )
		return false;
//	std::cout<<"12"<<std::endl;
	if( (problemText.compare(p_msg.problemText))!=0 )
		return false;
//	std::cout<<"13"<<std::endl;
	if( manualCease!=p_msg.manualCease)
		return false;
//	std::cout<<"14"<<std::endl;
//	std::cout<<"eventMessage: "<<eventMessage<<endl;
//	std::cout<<"p_msg.eventMessage: "<<p_msg.eventMessage<<endl;
//	if( (eventMessage.compare(p_msg.eventMessage))!=0 )
//		return false;
//	std::cout<<"15"<<std::endl;

	return true;


}

//========================================================================================
//	Comparison operator!=
//========================================================================================

bool acs_aeh_eventmsg::operator!=(const acs_aeh_eventmsg& p_msg){

	return !(*this == p_msg);
}


//========================================================================================
//	Set Priority message
//========================================================================================

void acs_aeh_eventmsg::setPriorityMsg(const char* p_priority)
{
	if(p_priority){
		priority = p_priority;
	}
}


//========================================================================================
//	Set Node Name
//========================================================================================

void acs_aeh_eventmsg::setNodeName(const char* p_nodeName)
{
	if(p_nodeName){
		nodeName = p_nodeName;
	}
}


//========================================================================================
//	Set Node Status
//========================================================================================

void acs_aeh_eventmsg::setNodeStatus(const char* p_nodeStatus)
{
	if(p_nodeStatus){
		nodeStatus = p_nodeStatus;
	}
}


//========================================================================================
//	Set Node Status
//========================================================================================

void acs_aeh_eventmsg::setUserName(const char* p_userName)
{
	if(p_userName){
		userName = p_userName;
	}
}


//========================================================================================
//	Set ProcessName
//========================================================================================

void acs_aeh_eventmsg::setProcessName(const char* p_processName)
{
	if(p_processName){
		processName = p_processName;
	}
}


//========================================================================================
//	Set Specific Problem
//========================================================================================

void acs_aeh_eventmsg::setSpecificProblem(long p_sepecificProblem)
{
	specificProblem = p_sepecificProblem;
}


//========================================================================================
//	Set Perceived severity
//========================================================================================

void acs_aeh_eventmsg::setPercSeverity(const char* p_percSeverity)
{
	if(p_percSeverity){
		percSeverity = p_percSeverity;
	}
}


//========================================================================================
//	Set Probable cause
//========================================================================================

void acs_aeh_eventmsg::setProbableCause(const char* p_probableCause)
{
	if(p_probableCause){
		probableCause = p_probableCause;
	}
}


//========================================================================================
//	Set Object Class of Reference
//========================================================================================

void acs_aeh_eventmsg::setObjClassOfReference(const char* p_objClassOfReference)
{
	if(p_objClassOfReference){
		objClassOfReference = p_objClassOfReference;
	}

}


//========================================================================================
//	Set Object of Reference
//========================================================================================

void acs_aeh_eventmsg::setObjectOfReference(const char* p_objectOfReference)
{
	if(p_objectOfReference){
		objectOfReference = p_objectOfReference;
	}
}


//========================================================================================
//	Set Problem data
//========================================================================================

void acs_aeh_eventmsg::setProblemData(const char* p_problemData)
{
	if(p_problemData){
		problemData = p_problemData;
	}
}


//========================================================================================
//	Set Problem text
//========================================================================================

void acs_aeh_eventmsg::setProblemText(const char* p_problemText)
{
	if(p_problemText){
		problemText = p_problemText;
	}
}


//========================================================================================
//	Set Manual cease
//========================================================================================

void acs_aeh_eventmsg::setManualCease(bool p_manualCease)
{
	manualCease = p_manualCease;
}


//========================================================================================
//	Get Node Name
//========================================================================================

std::string acs_aeh_eventmsg::getNodeName()
{
	return nodeName;
}


//========================================================================================
//	Get Priority event
//========================================================================================

std::string acs_aeh_eventmsg::getPriorityMsg()
{
	return priority;
}


//========================================================================================
//	Get Node Status
//========================================================================================

std::string acs_aeh_eventmsg::getNodeStatus()
{
	return nodeStatus;
}


//========================================================================================
//	Get User Name
//========================================================================================

std::string acs_aeh_eventmsg::getUserName()
{
	return userName;
}


//========================================================================================
//	Get Process Name
//========================================================================================

std::string acs_aeh_eventmsg::getProcessName()
{
	return processName;
}


//========================================================================================
//	Get Specific Problem
//========================================================================================

long  acs_aeh_eventmsg::getSpecificProblem()
{
	return specificProblem;
}


//========================================================================================
//	Get Perceived Severity
//========================================================================================

std::string acs_aeh_eventmsg::getPercSeverity()
{
	return percSeverity;
}


//========================================================================================
//	Get Probable Cause
//========================================================================================

std::string acs_aeh_eventmsg::getProbableCause()
{
	return probableCause;
}


//========================================================================================
//	Get Object Class of Reference
//========================================================================================

std::string acs_aeh_eventmsg::getObjClassOfReference()
{
	return objClassOfReference;
}


//========================================================================================
//	Get Object of Reference
//========================================================================================

std::string acs_aeh_eventmsg::getObjectOfReference()
{
	return objectOfReference;
}


//========================================================================================
//	Get Problem Data
//========================================================================================

std::string acs_aeh_eventmsg::getProblemData()
{
	return problemData;
}


//========================================================================================
//	Get problem text
//========================================================================================

std::string acs_aeh_eventmsg::getProblemText()
{
	return problemText;
}


//========================================================================================
//	Get Manual Cease
//========================================================================================

bool acs_aeh_eventmsg::getManualCease()
{
	return manualCease;
}


//========================================================================================
//	Get Event Message
//========================================================================================

std::string acs_aeh_eventmsg::getEventMessage(){

	std::stringstream s;
	s << "AEHUSOCK " <<getNodeName()<< " " << getNodeStatus() << " "
			<<getPriorityMsg()<< " " << getUserName() << " "
			<< getProcessName() << " " << getSpecificProblem() <<" "
			<< getPercSeverity() << " " << getManualCease()
			<< PROBABLE_CAUSE_LABEL << getProbableCause()
			<< CLASS_OF_REF_LABEL << getObjClassOfReference()
			<< OBJ_OF_REF_LABEL << getObjectOfReference()
			<< PROB_DATA_LABEL << getUnFormatterProblemData()
			<< PROB_TEXT_LABEL << getUnFormatterProblemText();

	eventMessage = s.str();

	return eventMessage;
}


/*int buffReplace(std::string &buffer, const std::string &strFind, const std::string &strRepl)
{
   int    count = 0;
   size_t pos   = 0;
   if(0 == buffer.size() || 0 == strFind.size())
		return -1;

   while((pos = buffer.find(strFind, pos)) != std::string::npos)
   {
		buffer.replace( pos, strFind.size(), strRepl.c_str() );
		pos += strRepl.size();
		count++;
   }
   return count;
}*/

//========================================================================================
//	Find and replace in a string
//========================================================================================
void acs_aeh_eventmsg::myReplace(std::string &str, const std::string &oldStr, const std::string &newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

//========================================================================================
//	Get unformatted Problem Text
//========================================================================================
std::string acs_aeh_eventmsg::getUnFormatterProblemText(){
	std::string newProblemText;

	newProblemText = problemText;
	myReplace(newProblemText, "\n", ACS_AEH_DELIMITER);

//	char str[problemText.length()];
//	::sprintf(str, "%s",problemText.c_str());
//
//	char *pch = 0;
//	pch = strtok(str, "\n");
//
//	while (pch != 0){
//
//		newProblemText += pch;
//		pch = strtok(0, "\n");
//		if(pch !=0 ){
//			newProblemText += ACS_AEH_DELIMITER;
//		}
//	}

	return newProblemText;
}


//========================================================================================
//	Get unformatted Problem Data
//========================================================================================
std::string acs_aeh_eventmsg::getUnFormatterProblemData(){
	std::string newProblemData;

	newProblemData = problemData;
	myReplace(newProblemData, "\n", ACS_AEH_DELIMITER);

//	char str[problemData.length()];
//	::sprintf(str, "%s",problemData.c_str());
//
//	char *pch = 0;
//	pch = strtok(str, "\n");
//
//	while (pch != 0){
//
//		newProblemData += pch;
//		pch = strtok(0, "\n");
//		if(pch !=0 ){
//			newProblemData += ACS_AEH_DELIMITER;
//		}
//	}

	return newProblemData;
}

