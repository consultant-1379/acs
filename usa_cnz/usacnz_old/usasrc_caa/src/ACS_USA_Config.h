//******************************************************************************
// 
// .NAME 
//  	  ACS_USA_Config - Handles initialisation from ACF.
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Config
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_Config.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	    This class handles configuration of USA. It reads an ACF file 
//	    validates the contents of it and passes data to Analysis Object
//	    and criterions.

// .ERROR HANDLING
//
//	    General rule:
//	    The error handling is specified for each method.
//
//	    No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	    190 89-CAA 109 0259

// AUTHOR 
// 	    1995-06-26 by ETX/TX/T XKKHEIN

// REV	DATE 	NAME	 DESCRIPTION
// A	950829	XKKHEIN	 First version.
// B	990915	UABDMT	 Ported to Windows NT4.0.
// C	030212	UABPEK   New AP VIRUS alarms.
// D	040429	QVINKAL	 Removal of RougeWave Tools.h++
// E	071231	EKAMSBA  General Error filtering.
// F	080215	EKAMSBA  Auto cease alarm.

// .LINKAGE
//	

// .SEE ALSO 
// 

//******************************************************************************

#ifndef ACS_USA_CONFIG_H 
#define ACS_USA_CONFIG_H

#pragma warning (disable: 4786)
#include <map>
#include <string>
#include <functional>
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_ObjectManager.h"
#include "ACS_USA_Global.h"
#include "ACS_USA_Common.h"
#include "ACS_USA_ATLRegExp.h"
#include "ACS_USA_Regexp.h"
#include "ACS_USA_Tokenizer.h"
using namespace std;

typedef string String;
typedef	 int IndexType;	   		  // Type for indexes
typedef	 int LineStatus;		  // type returned from readLine
typedef  long LineNumber;		  // line numbers


//******************************************************************************
// Keywords used in the ACF
//******************************************************************************
// Keywords for lists
const 	char* const 	Key_objectList   = "objectList";
const 	char* const	Key_criteriaList = "criteriaList";
const 	char* const	Key_commandList  = "commandList";
const 	char* const	Key_bootEventString = "bootEventRegularExpression";
const 	char* const	Key_RestartEventString = "restartEventRegularExpression";  

const IndexType unknownList 		= 0;
const IndexType objectList   		= 1;
const IndexType criteriaList 		= 2;
const IndexType commandList			= 3;
const IndexType bootEventList		= 4;
const IndexType RestartEventList = 5;

enum 
{
  Line_error,
  Line_eof,
  Line_list,
  Line_parameter
};

// Keywords in ACF related to Analysis Objects
const 	char* const	Key_logFile	   = "logFile";
const 	char* const	Key_TSposition = "TSposition";
const 	char* const	Key_TSformat	 = "TSformat";

// Identifiers related to Analysis Objects
enum
{
  ID_logFile,
  ID_TSposition,
  ID_TSformat
};

// Keywords in ACF that are related to Criteria
const char* const	Key_object	           = "object";
const char* const	Key_scope	             = "scope";
const char* const	Key_maxAllowed	       = "maxAllowed";
const char* const	Key_matchType	         = "matchType";
const char* const	Key_pattern	           = "pattern";
const char* const	Key_specificProblem    = "specificProblem";
const char* const	Key_perceivedSeverity	 = "perceivedSeverity";
const char* const	Key_probableCause	     = "probableCause";
const char* const	Key_objectOfRefSuffix	 = "objectOfRefSuffix";
const char* const	Key_problemText	       = "problemText";
const char* const	Key_command	           = "command";
const char* const	Key_node			   = "node";
const char* const	Key_disableAlarmFilter = "disableAlarmFilter";
const char* const	Key_generalErrorFilter = "generalErrorFilter";
const char* const	Key_ceaseDuration      = "ceaseDuration";

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

// identifier for command
enum
{
  ID_commandString
};

// Keyword in ACF for command definition
const 	char* const	Key_commandString = "commandString";


// Some character constants
const	char	Char_asterisk 	 = '*';
const	char	Char_escape	 = '\\';
const	char	Char_space	 = ' ';
const	char	Char_colon	 = ':';
const	char	Char_singleQuote = '\'';
const	char	Char_doubleQuote = '"';
const	char	Char_dot	 = '.';
const	char	Char_tab	 = '\t';
const	char	Char_underscore  = '_';
const	char	Char_slash	 = '/';


//Forward declaration(s)

class RWInteger;
class ACS_USA_ObjectManager;
class ACS_USA_Criterion;

//Member functions, constructors, destructors, operators
struct StringHash
{
	bool operator()(string a, string b) const
	{  
		return true;
	}
};

int findValue(map<string,int> ls, const string pkey);

String findValue(map<string,string> ls, const string pkey);

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
  
	ACS_USA_StatusType loadACF(ACS_USA_ObjectManager& manager, 
			     const char* fileName,
				 String& bootEventRegExp,
				 String& RestartEventRegExp
				 );
  // Description:
  // 	Loads the ACF file.
  // Parameters: 
  //    fileName		Full path and name of ACF file
  //	manager			Reference to Object Manager
  // Return value: 
  //    none
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

  void ACS_USA_Config::initDictionaries();
  // Description:
  // 	Initialises parameter name  dictionaries.
  // Parameters: 
  //    none
  // Return value: 
  //    none
  // Additional information:
  //
	ACS_USA_StatusType addListMember(const String& strlist, 
				   String& element);
  // Description:
  // 	Adds a new member to one of internal vectors
  // Parameters: 
  //    list			string indicationg which list
  //	element			new element in the list 
  // Return value: 
  //    ACS_USA_ok		successful
  //	ACS_USA_error		something went wrong 
  // Additional information:
  //   
	ACS_USA_StatusType addParameterValue(const String& ref, 
				       const String& name, 
				       String& value);
  // Description:
  // 	Fills a parameter value in one of internal lists (vectors)
  // Parameters: 
  //    <Parameter name>	<parameter description, in/out> 
  // Return value: 
  //    ref			reference to a list element
  //	name			name of a parameter
  //	value 			value for a parameter
  // Additional information:
  //
  
  ACS_USA_StatusType check(
						String& bootEventRegExp,
						String& RestartEventRegExp
						);
  // Description:
  // 	Checks all data for correctness. Must be called after configuration
  //	file is completely read.
  // Parameters: 
  //    bootEventRegExp	
  // Return value: 
  //    ACS_USA_ok		parameters ok
  //	ACS_USA_error		parameter(s) missing 
  // Additional information:
  //
  ACS_USA_StatusType fillAnalysisObjects(ACS_USA_ObjectManager& manager,
										 String& bootEventRegExp,
										 String& RestartEventRegExp
										 );
  // Description:
  // 	Moves configuration data from vectors to Object Manager
  // Parameters: 
  //    manager			reference to object manager
  // Return value: 
  //    ACS_USA_ok		success
  //	ACS_USA_error		something went wrong
  // Additional information:
  //
  LineStatus readLine(std::ifstream&  stream,
		      String& ref,
		      String& name,
		      String& value);
  // Description:
  // 	Reads a line from ACF file
  // Parameters: 
  //    stream			reference to a file stream
  //	ref			string containing a name of parameter group, out
  //	name			parameter name, out
  //	value			parameter value, out 
  // Return value: 
  //	Line_error		error occurred
  //	Line_eof		eon of file reached
  //	Line_list		value contains a list of parameter groups
  //	Line_parameter		returned info concerns a single parameter
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

  IndexType getListReference(const String& str) const;
  
  // Description:
  // 	Determines which list
  // Parameters: 
  //    str			a string object to compare with
  // Return value: 
  //	listIndex		index of the list    
  // Additional information:
  //

  // Dictionaries for keywords in the ACF file 
  typedef int RWInteger;

  typedef map<String, ACS_USA_Criterion,StringHash > CCriteriaListDictionary;
  typedef CCriteriaListDictionary::iterator CCriteriaListDictionaryIterator;
  typedef pair<String, ACS_USA_Criterion> CriteriaListPair;
  CCriteriaListDictionary criteriaListDictionary;

  typedef map<String, ACS_USA_AnalysisObject,StringHash> CObjectListDictionary;
  typedef CObjectListDictionary::iterator CObjectListDictionaryIterator;
  typedef pair<String, ACS_USA_AnalysisObject> ObjectListPair;
  CObjectListDictionary objectListDictionary;

  typedef map<String, String,StringHash>  CCommandListDictionary;
  typedef CCommandListDictionary::iterator CCommandListDictionaryIterator;
  typedef pair<String, String>  CommandListPair;
  CCommandListDictionary commandListDictionary;

  typedef map<String, String>  CBootEventListDictionary;
  typedef pair<String, String>  CBootEventListPair;
  CBootEventListDictionary bootEventListDictionary;

  typedef map<String, String>  CRestartEventListDictionary;
  typedef pair<String, String>  RestartEventListPair;
  CRestartEventListDictionary RestartEventListDictionary;

  typedef map<String, RWInteger>  CCommandDictionary;
  typedef pair<String, RWInteger>  CommandPair;
  CCommandDictionary commandDictionary;

  typedef map<String, RWInteger>  CCriteriaDictionary;
  typedef pair<String, RWInteger> CriteriaPair;
   CCriteriaDictionary criteriaDictionary;
  
  typedef map<String, RWInteger>  CObjectDictionary;
  typedef pair<String , RWInteger > ObjectPair;
  CObjectDictionary objectDictionary;

  LineNumber lineNr;			// current line in ACF  
};

#endif
