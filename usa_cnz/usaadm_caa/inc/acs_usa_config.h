
#ifndef ACS_USA_CONFIG_H 
#define ACS_USA_CONFIG_H

#include <map>
#include <string>
#include <functional>
#include "acs_usa_analysisObject.h"
#include "acs_usa_criterion.h"
#include "acs_usa_types.h"
#include "acs_usa_common.h"
#include "acs_usa_atlRegexp.h"
#include "acs_usa_regexp.h"
#include "acs_usa_tokenizer.h"

using namespace std;

typedef string String;


//******************************************************************************
// Keywords used in the IMM
//******************************************************************************
const char* const	Key_object	        = "object";
const char* const	Key_scope	        = "scope";
const char* const	Key_maxAllowed	       	= "maxAllowed";
const char* const	Key_matchType	        = "matchType";
const char* const	Key_pattern	        = "pattern";
const char* const	Key_specificProblem    	= "specificProblem";
const char* const	Key_perceivedSeverity	= "perceivedSeverity";
const char* const	Key_probableCause	= "probableCause";
const char* const	Key_objectOfRefSuffix	= "objectOfRefSuffix";
const char* const	Key_problemText	       	= "problemText";
const char* const	Key_command	        = "command";
const char* const	Key_node		= "node";
const char* const	Key_disableAlarmFilter 	= "disableAlarmFilter";
const char* const	Key_ceaseDuration      	= "ceaseDuration";

// Identifiers related to criteria
enum 
{
  ID_object,
  ID_scope,
  ID_maxAllowed,
  ID_matchType,
  ID_pattern,
  ID_specificProblem,
  ID_perceivedSeverity,
  ID_probableCause,
  ID_objectOfRefSuffix,  // For extended Obj of Ref description
  ID_problemText,
  ID_command,
  ID_node,               // Is the criterion for active or passive node?
  ID_disableAlarmFilter, // Can the alarm filter be ignored?
  ID_generalErrorFilter, // The criterion is to be filtered or not.
  ID_ceaseDuration       // Time after which an auto cease event is sent.
};

// Some character constants
const	char	Char_space	 = ' ';
const	char	Char_tab	 = '\t';
const	char	Char_underscore  = '_';
const	char	Char_camma	 = ',';


//Forward declaration(s)

class RWInteger;
class ACS_USA_AnalysisObject;
class ACS_USA_Criterion;

int findValue(map<string,int> ls, const string pkey);

class  ACS_USA_Config
{

 public:

  	ACS_USA_Config();
  	// Description:
  	// 	Default constructor
  	// Parameters: 
  	//    none
  	// Return value: 
  	//    none
  
	ACS_USA_ReturnType loadIMM(ACS_USA_AnalysisObject *configObject);
  	// Description:
  	// 	Loads the IMM Objects
  	// Parameters: 
	// 	ACS_USA_AnalysisObject Anaysis Object
  	// Return value: 
  	//    ACS_USA_Ok/ACS_USA_Error
  	// Additional information:
  	//

  	virtual ~ACS_USA_Config();
	// Description:
  	// 	Destructor. Releases all allocated memory.
  	// Parameters: 
  	//    none
  	// Return value: 
  	//    none
  	// Additional information:
  	//

private:

  	void initDictionaries();
  	// Description:
  	// Initialises parameter name  dictionaries.
  	// Parameters: 
  	//    none
  	// Return value: 
  	//    none
  	// Additional information:
  	//
  
  	ACS_USA_ReturnType check(ACS_USA_AnalysisObject *configObject);
  	// Description:
  	// 	Checks all data for correctness. Must be called after configuration
  	//	file is completely read.
  	// Parameters: 
  	// Return value: 
  	//    ACS_USA_ok		parameters ok
  	//	ACS_USA_error		parameter(s) missing 
  	// Additional information:
  	//

  	String& stripWhiteSpace(String& str) const;
  
 	// Description:
  	// 	Removes leading and trailing whitespaces characters
  	// Parameters: 
  	//    str			a string object to compare with
  	// Return value: 
  	//	str			self    
  	// Additional information:
  	//

  	int validateName(String& str) const;
  	// Description:
  	// 	Checks if a parameter name contains illegal characters
  	// Parameters: 
  	//    str			a string object to compare with
  	// Return value: 
  	//	TRUE			str contains an invalid name
  	//	FALSE			str contains a valid name  
  	// Additional information:
 	//

  	typedef int RWInteger;

  	typedef map<String, RWInteger>  CCriteriaDictionary;
  	typedef pair<String, RWInteger> CriteriaPair;
  	CCriteriaDictionary criteriaDictionary;
  
  	ACS_USA_Criterion* m_ctrObj;;
};

#endif
//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

