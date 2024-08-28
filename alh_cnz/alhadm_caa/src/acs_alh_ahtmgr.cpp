/*
 * acs_alh_ahtmgr.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: efabron
 */

#include "acs_alh_ahtmgr.h"
#include "acs_alh_macroconfig.h"

#include "acs_apgcc_omhandler.h"

//========================================================================================
//	Constructor
//========================================================================================

acs_alh_ahtmgr::acs_alh_ahtmgr() : alhList_Already_loaded(false), ahtList_(), util_("acs_alh_ahtmgr") {
}

//========================================================================================
//	Destructor
//========================================================================================

acs_alh_ahtmgr::~acs_alh_ahtmgr(){
    // Remove and deallocate all items from Alarm Handler Tab
	alhList_Already_loaded = false;
    ahtList_.clear();
}

void acs_alh_ahtmgr::setAlarmTableFlag ( bool value ){
	alhList_Already_loaded = value;
}

//========================================================================================
//	Get parameters
//========================================================================================

bool acs_alh_ahtmgr::getParameters()
{
	OmHandler omhandler;
	ACS_CC_ReturnType result;

	std::vector<std::string> rdnObjectList;

	//Initialize IMM Handler
	result = omhandler.Init();
	if(result != ACS_CC_SUCCESS){
		setError(acs_alh::ERR_SAF_IMM_OM_INIT);
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "object ahtMgr.getParameters problem\nIMM error, Init Failure, ERROR CODE:%d, %s",
				omhandler.getInternalLastError(),
				omhandler.getInternalLastErrorText());
		setErrorText(problemData);
		return false;
	}

	//Retrieve from IMM the RDN object of class AlarmConfigurationTable
	result = omhandler.getClassInstances(ACS_ALH_CONFIG_IMM_ALARMCONFIGURATIONTABLE_CLASS_NAME, rdnObjectList);
	if(result != ACS_CC_SUCCESS && omhandler.getInternalLastError() != -41){
		setError(acs_alh::ERR_SAF_IMM_OM_GET);
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "object ahtMgr.getParameters problem\nIMM error, getClassInstances Failure, ERROR CODE:%d, %s",
				omhandler.getInternalLastError(),
				omhandler.getInternalLastErrorText());
		setErrorText(problemData);
		return false;
	}else if (result != ACS_CC_SUCCESS && omhandler.getInternalLastError() == -41){


		//TODO
		//Loggare che la Alarm Handler Table e vuota

		return true;
	}

	size_t numAhtRecords = rdnObjectList.size();
	//printf("Number of object %d", numAhtRecords);

	if( numAhtRecords != 0 ){

		//const char specifiProblemNameAttr[128] = ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_SPECIFIC_PROBLEM;

		//Analyse each single record
		for(size_t i = 0; i < numAhtRecords; i++){

			long specifiProblem = 0;
			char *problemText = 0;
			char *probableCause = 0;
			char *severity = 0;

			ACS_APGCC_ImmObject object;

			//Retrieve from IMM the single record of Alarm Handler Table
			object.objName = rdnObjectList[i].c_str();
			result = omhandler.getObject(&object);
			if(result != ACS_CC_SUCCESS){
				setError(acs_alh::ERR_SAF_IMM_OM_GET);
				char problemData[DEF_MESSIZE] = {0};
				snprintf(problemData, sizeof(problemData) - 1, "object ahtMgr.getParameters problem\nIMM error, getObject Failure, ERROR CODE:%d, %s",
						omhandler.getInternalLastError(),
						omhandler.getInternalLastErrorText());
				setErrorText(problemData);
				return false;
			}

			for ( size_t j = 0; j < object.attributes.size(); j++ ){

					if((strcmp(ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_SPECIFIC_PROBLEM, object.attributes[j].attrName.c_str())==0)){
						if(object.attributes[j].attrValuesNum != 0){
							specifiProblem = *(reinterpret_cast<long long*>(object.attributes[j].attrValues[0]));
						}
					}else if((strcmp(ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBLEM_TEXT, object.attributes[j].attrName.c_str())==0)){
						if(object.attributes[j].attrValuesNum != 0){
							problemText = (char*)(object.attributes[j].attrValues[0]);
						}
					}else if((strcmp(ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PROBABLE_CAUSE, object.attributes[j].attrName.c_str())==0)){
						if(object.attributes[j].attrValuesNum != 0){
							probableCause = (char*)(object.attributes[j].attrValues[0]);
						}

					}else if((strcmp(ACS_ALH_CONFIG_IMM_ACT_ATTR_NAME_PERCEIVED_SEVERITY, object.attributes[j].attrName.c_str())==0)){
						if(object.attributes[j].attrValuesNum != 0){
							severity = (char*)(object.attributes[j].attrValues[0]);
						}

					}

				}//end for internal

			acs_alh_ahtrecord ahtPtr(specifiProblem, severity, probableCause, problemText);

			//Check syntax of record
			if (syntaxCheck(ahtPtr) == false) {
				char problemData[DEF_MESSIZE] = {0};
				snprintf( problemData, sizeof(problemData) - 1, "object ahtMgr.getParameters problem\n%s", getErrorText());
				setError(getError());
				setErrorText(problemData);
			}else{
				ahtList_.push_back(ahtPtr);
			}


		}//end for external

	}//end if


	//Finalize IMM Handler
	result = omhandler.Finalize();
	if(result != ACS_CC_SUCCESS){
		setError(acs_alh::ERR_SAF_IMM_OM_INIT);
		char problemData[DEF_MESSIZE] = {0};
		snprintf(problemData, sizeof(problemData) - 1, "object ahtMgr.getParameters problem\nIMM error, Finalize Failure, ERROR CODE:%d, %s",
				omhandler.getInternalLastError(),
				omhandler.getInternalLastErrorText());
		setErrorText(problemData);
		return false;
	}

    return true;
}


//========================================================================================
//	getMatch
//========================================================================================

void acs_alh_ahtmgr::getMatch(ACS_ALH_AlarmMessage& alarmMessage) const{

    // If perceivedSeverity of event Message <> CEASING
    // for all records in Alarm Handler Table, compare the specificProblem
    // with specificProblem in eventMessage
    // if same
    //     substitute perceived Severity in eventMessage with the one
    //     in Alarm Handler Table

    // First check that perceived Severity of eventMessage <> CEASING
    // (we don't treat these!)
    if (!strcmp(alarmMessage.event.percSeverity, STRING_CEASING))
    {
		return;
    }

    size_t ahtTabCnt = ahtList_.size();

    // Check for every item in Alarm Handler Tab if match
    for (size_t i = 0; i < ahtTabCnt; i++)
    {
		acs_alh_ahtrecord ahtRec;
		ahtRec = ahtList_[i];

		if (alarmMessage.event.specificProblem == ahtRec.getSpecProb())
		{
			memset(alarmMessage.event.percSeverity, 0, sizeof(alarmMessage.event.percSeverity));
			snprintf(alarmMessage.event.percSeverity, sizeof(alarmMessage.event.percSeverity) - 1, "%s", ahtRec.getPercSev());

			// If probable cause present, substitute old
			if (strlen(ahtRec.getProbCause()) > 0) {
				memset(alarmMessage.event.probableCause, 0, sizeof(alarmMessage.event.probableCause));
				snprintf(alarmMessage.event.probableCause, sizeof(alarmMessage.event.probableCause) - 1, "%s", ahtRec.getProbCause());
			}

			// If problem Text present, substitute old
			if (strlen(ahtRec.getProbText()) > 0) {
				memset(alarmMessage.event.problemText, 0, sizeof(alarmMessage.event.problemText));
				snprintf(alarmMessage.event.problemText, sizeof(alarmMessage.event.problemText) - 1, "%s", ahtRec.getProbText());
			}
			break;
		}
    }
}


//========================================================================================
//	syntaxCheck
//========================================================================================

bool acs_alh_ahtmgr::syntaxCheck(const acs_alh_ahtrecord& ahtPtr)
{
    acs_alh_ahtrecord ahtOldPtr;
	std::string str(ahtPtr.getPercSev());

    // First check that specificProblem no of parameter does not
    // occur in previously stored item in Alarm Handler tab
	size_t entries = ahtList_.size();
    for (size_t i = 0; i < entries; i++)
    {
		ahtOldPtr = ahtList_[i];
		if (ahtPtr.getSpecProb() == ahtOldPtr.getSpecProb())
		{
			char problemData[DEF_MESSIZE];
			snprintf( problemData, sizeof(problemData) - 1, "SyntaxCheck problem. Multiple specific problem %ld", ahtPtr.getSpecProb() );
			setError(ACS_ALH_alarmHandTabMultipleError);
			setErrorText(problemData);
			return false;
		}
    }

    // Now check that percSeverity of parameter is
    // one of [A1, A2, A3, O1, O2, EVENT]
	if ((str == STRING_A1) || (str == STRING_A2) || (str == STRING_A3) ||
	    (str == STRING_O1) || (str == STRING_O2) ||
	    (str == STRING_EVENT))
	{
	   return true;
	}
	else
    {
		char problemData[DEF_MESSIZE];
		snprintf( problemData, sizeof(problemData) - 1, "syntaxCheck problem. Illegal severity %s", ahtPtr.getPercSev());
		setError(ACS_ALH_alarmHandTabSyntaxError);
		setErrorText(problemData);
		return false;
    }
}
