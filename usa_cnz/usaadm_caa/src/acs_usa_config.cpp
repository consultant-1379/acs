//******************************************************************************
//******************************************************************************


#include <stdio.h>
#include <ctype.h>

#include <iostream>
#include <fstream>
#include "acs_usa_config.h"
#include "acs_usa_common.h"
#include "acs_apgcc_omhandler.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_error.h"
#include "acs_usa_logtrace.h"

using namespace std;

ACS_USA_Trace traCnfg("ACS_USA_Config            ");


//******************************************************************************
//	ACS_USA_Config()
//******************************************************************************
ACS_USA_Config::ACS_USA_Config() :
	criteriaDictionary(),
	m_ctrObj(0)	
{
	USA_TRACE_ENTER();
	
    	initDictionaries();

	USA_TRACE_LEAVE();
}  

//******************************************************************************
//	initdictionaries()
//******************************************************************************
void ACS_USA_Config::initDictionaries() {

  	USA_TRACE_ENTER();

  	// Set the key-value pairs in keyword dictionaries

  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_object),
		RWInteger(ID_object)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_scope),
		RWInteger(ID_scope)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_maxAllowed),
     		RWInteger(ID_maxAllowed)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_matchType),
		RWInteger(ID_matchType)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_pattern),
		RWInteger(ID_pattern)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_specificProblem),
		RWInteger(ID_specificProblem)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_perceivedSeverity),
		RWInteger(ID_perceivedSeverity)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_probableCause),
		RWInteger(ID_probableCause)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_objectOfRefSuffix),
		RWInteger(ID_objectOfRefSuffix)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_problemText),
		RWInteger(ID_problemText)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_command),
		RWInteger(ID_command)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_node),
		RWInteger(ID_node)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_disableAlarmFilter),
		RWInteger(ID_disableAlarmFilter)));
  	criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_ceaseDuration),
		RWInteger(ID_ceaseDuration)));

	USA_TRACE_LEAVE();
}

//******************************************************************************
//	loadIMMConfig()
//******************************************************************************
ACS_USA_ReturnType ACS_USA_Config::loadIMM( ACS_USA_AnalysisObject *configObject) {

	USA_TRACE_ENTER();

	// Load Objects from IMM
	OmHandler immHandle;
	char *className = const_cast<char*>("USASupervison");
	String char_value;
	unsigned int int_value=0;
	ACS_USA_Boolean noCriteria = ACS_USA_True;

	ACS_CC_ReturnType result;
	std::vector<std::string> p_dnList;
	result = immHandle.Init();
	if ( result != ACS_CC_SUCCESS ){
		traCnfg.traceAndLogFmt(ERROR,"ACS_USA_Config:%s() Initialization FAILURE",__func__);
		USA_TRACE_1("ACS_USA_Config:%s() Initialization FAILURE",__func__);
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_ImmInitFailed);
		return ACS_USA_Error;
	}		

	result = immHandle.getClassInstances(className, p_dnList);
	int instancesNumber  = p_dnList.size();

	USA_TRACE_1("ACS_USA_Config:%s() Instances Found:[%d]", __func__, instancesNumber);
	String temp;

	for (unsigned int k=0; k<p_dnList.size(); k++){

		// Create a new crititerion object for each object of IMM
		ACE_NEW_NORETURN(this->m_ctrObj, ACS_USA_Criterion());
		if (0 == this->m_ctrObj){
			traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s()->ACS_USA_Criterion Memory Allocation failed", __func__);
			USA_TRACE_LEAVE2("ACS_USA_Config:%s()->ACS_USA_Criterion Memory Allocation failed" ,__func__);
			error.setError(ACS_USA_Memory);
			error.setErrorText("ACE_NEW_NORETURN() Failed in acs_usa_config:loadIMM()");
			return ACS_USA_Error;
		}
		ACS_APGCC_ImmObject object;
		char *nameObject =  const_cast<char*>( p_dnList[k].c_str());
		object.objName = nameObject;
		result = immHandle.getObject(&object);
		bool isvalid=true;
		
		/* fetch each IMM Object Variables */
		for (unsigned int i = 0; i < object.attributes.size(); i++ ){
			RWInteger p = findValue(criteriaDictionary, String(object.attributes[i].attrName));
			temp = object.attributes[i].attrName;
			if (p == rwnil){
				//traCnfg.traceAndLogFmt(INFO, "ACS_USA_Config:%s() - New Object[%s] found. Might set to defaults if applicable", __func__,object.attributes[i].attrName.data());
				continue;
				//error.setError(ACS_USA_SyntaxError);
				//error.setErrorText(ACS_USA_unknownParameter);	
			}	

			/* fetch Object value from IMM */
			for (unsigned int j = 0; j < object.attributes[i].attrValuesNum; j++){
				if ( (object.attributes[i].attrType == 9) || (object.attributes[i].attrType == 6)  ){
					char_value=(char*)(object.attributes[i].attrValues[j]);
				}else if (object.attributes[i].attrType == 2){
					int_value=*(reinterpret_cast<unsigned int*>(object.attributes[i].attrValues[j]));
				}		
			} // end of inner for

			switch(p) {

				case ID_object:
						m_ctrObj->setObject(char_value);
						break;

				case ID_scope:
						m_ctrObj->setScope(int_value);
						break;

				case ID_maxAllowed:
						if (m_ctrObj->setMaxAllowed(int_value) == ACS_USA_Error) {
							traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s() - setMaxAllowed [%d] FAILED", __func__, int_value);
							USA_TRACE_LEAVE2("ACS_USA_Config:%s() - setMaxAllowed [%d] FAILED", __func__, int_value);
							error.setError(ACS_USA_SyntaxError);
							error.setErrorText(ACS_USA_IllegalValue);
							return ACS_USA_Error;
						}
						break;

				case ID_matchType:
						if (m_ctrObj->setMatchType(char_value) == ACS_USA_Error) {
							traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s() - setMatchType[%s] FAILED", __func__, char_value.data());
							USA_TRACE_LEAVE2("ACS_USA_Config:%s() - setMatchType[%s] FAILED", __func__, char_value.data());
							error.setError(ACS_USA_SyntaxError);
							error.setErrorText(ACS_USA_IllegalMatchType);
							return ACS_USA_Error;
						}
						break;

				case ID_pattern:
						m_ctrObj->setPattern(char_value);
						break;

				case ID_specificProblem:
						if (m_ctrObj->setSpecificProblem(int_value) == ACS_USA_Error) {
							traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s() - setSpecificProblem [%d] FAILED", __func__, int_value);
							USA_TRACE_LEAVE2("ACS_USA_Config:%s() - setSpecificProblem [%d] FAILED", __func__, int_value);
							error.setError(ACS_USA_SyntaxError);
							error.setErrorText(ACS_USA_IllegalValue);
							return ACS_USA_Error;
						}
						break;	

				case ID_perceivedSeverity:
						if (m_ctrObj->setPerceivedSeverity(char_value) == ACS_USA_Error) {
							traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s() - setPerceivedSeverity[%s] FAILED", __func__, char_value.data());
							USA_TRACE_LEAVE2("ACS_USA_Config:%s() - setPerceivedSeverity[%s] FAILED", __func__, char_value.data());
							error.setError(ACS_USA_SyntaxError);
							error.setErrorText(ACS_USA_IllegalServerity);
							return ACS_USA_Error;
						}
						break;

				case ID_probableCause:
						if (m_ctrObj->setProbableCause(char_value) == ACS_USA_Error){
							traCnfg.traceAndLogFmt(ERROR, "ACS_USA_Config:%s() - setProbableCause[%s] FAILED", __func__, char_value.data());
							USA_TRACE_LEAVE2("ACS_USA_Config:%s() - setProbableCause[%s] FAILED", __func__, char_value.data());
							error.setError(ACS_USA_SyntaxError);
							error.setErrorText(ACS_USA_IllegalProbableCause);
							return ACS_USA_Error;
						}
						break;
				
				case ID_objectOfRefSuffix:
						m_ctrObj->setObjectOfRefSuffix(char_value);
						break;

				case ID_problemText:
						m_ctrObj->setProblemText(char_value);
						break;

				case ID_command:
						m_ctrObj->setCommand(char_value);
						break;

				case ID_node:
						m_ctrObj->setNode(char_value);
						break;

				case ID_disableAlarmFilter:
						m_ctrObj->setDisableAlarmFilter(char_value);
						break;
						
				case ID_ceaseDuration:
						 if (m_ctrObj->setCeaseDuration(int_value) == ACS_USA_Error) {
							 return ACS_USA_Error;
						 }
						 break;
				default:
						USA_TRACE_1("ACS_USA_Config:%s() Indefault", __func__);
						isvalid=false;
						break;

			} // end switch		
					
		}// end of second for						

		// append criteria into criteria list
		/*bool critfound = false;
		CCriteriaListDictionaryIterator iter2;
		for(iter2 = configObject->criteriaListDictionary.begin();iter2 != configObject->criteriaListDictionary.end();iter2++) {
				if (iter2->first == m_ctrObj->getObject()){
					critfound = true;
					break;	
				}
			
		}*/
		if (isvalid) {
			USA_TRACE_1("ACS_USA_Config:%s() - Adding the criteria in criteriaList", __func__);

			configObject->criteriaListDictionary.insert(	configObject->criteriaListDictionary.end(), 
									CriteriaListPair(String(m_ctrObj->getObject()),
												(*m_ctrObj)));
			//configObject->criteriaList.push_back((*m_ctrObj));
			//configObject->ptrList.push_back(m_ctrObj);
			configObject->appendCriterion((*m_ctrObj));
			noCriteria = ACS_USA_False;
		} else {
			USA_TRACE_1("ACS_USA_Config:%s() - Bad paramter[%s]. Object is ignored",__func__, temp.data());
	 	}	
	}// end of first for

	// Check if there is no Criteria specified in IMM, set an error
	if (noCriteria == ACS_USA_True){
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_NoCriteria);
		return ACS_USA_Error;
	}	

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}


//******************************************************************************
//	stripWhiteSpace()
//******************************************************************************
String& 
ACS_USA_Config::stripWhiteSpace(String& str) const
{
	USA_TRACE_ENTER();

	String tmp;
	usacc_strip(str, Char_space);
	tmp = str;
	String abc;
	usacc_strip(tmp, Char_tab);
	abc = tmp;
	str = abc;

	USA_TRACE_LEAVE();

	return str;
}



//******************************************************************************
//	validateName()
//******************************************************************************
int
ACS_USA_Config::validateName(String& str) const
{
	USA_TRACE_ENTER();

    // Check all characters in the string. Allow only alphanumeric
    // underscore
	for(unsigned int i = 0; i < str.length(); i++) {
		char c = str[(size_t)i];
		if (!isalnum(c)) {
			if (c != Char_underscore) {
				return TRUE;
			}
		}
	}    

	USA_TRACE_LEAVE();
	return FALSE;
}

//******************************************************************************
//	~ACS_USA_Config()
//******************************************************************************
ACS_USA_Config::~ACS_USA_Config()
{
	USA_TRACE_ENTER();

	criteriaDictionary.clear();

	USA_TRACE_LEAVE();
}

//******************************************************************************

int findValue(map<string,int> ls, const string pkey)
{
	USA_TRACE_ENTER();

	map<string,int>:: const_iterator iter;
	iter = ls.find(pkey);
	if (iter != ls.end()) {
		USA_TRACE_LEAVE();
		return iter->second;
	}	
	else
	{
		USA_TRACE_LEAVE();
		return rwnil;
	}	
}

