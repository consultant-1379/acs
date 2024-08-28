//******************************************************************************

// NAME
//      ACS_USA_Config.cpp

// COPYRIGHT Ericsson AB, Sweden 1995, 1999, 2003-2004.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.

// .DESCRIPTION
// 	    This class handles configuration of USA. It reads an ACF file 
//	    validates the contents of it and passes data to Analysis Object
//	    and criterions.

// DOCUMENT NO
//	    190 89-CAA 109 0545

// AUTHOR 
// 	    1995-08-08 by ETX/TX/T XKKHEIN

// REV	DATE	NAME 	 DESCRIPTION
// A	950829	XKKHEIN	 First version.
// B	990915	UABDMT	 Ported to Windows NT4.0.
// C	030212	UABPEK	 New AP VIRUS alarms.
// D	040429	QVINKAL	 Removal of RougeWave Tools.h++.
// E	071231	EKAMSBA	 General Error filtering.
// F	080215	EKAMSBA	 Auto cease alarm.

//******************************************************************************

#pragma warning (disable: 4786)


#include <stdio.h>
#include <ctype.h>

#include <iostream>
#include <fstream>
#include "ACS_USA_Config.h"
#include "ACS_USA_Common.h"

using namespace std;					// Using std/stl adapted RW


//******************************************************************************
//	ACS_USA_Config()
//******************************************************************************
ACS_USA_Config::ACS_USA_Config() :
objectListDictionary(),
criteriaListDictionary(),
commandListDictionary(),
bootEventListDictionary(),
RestartEventListDictionary(),
commandDictionary(),
criteriaDictionary(),
objectDictionary(), 
lineNr(0)
{
    initDictionaries();
}  

//******************************************************************************
//	initdictionaries()
//******************************************************************************
void
ACS_USA_Config::initDictionaries()
{
  //
  // Set the key-value pairs in keyword dictionaries
  //
  objectDictionary.insert(objectDictionary.end(),ObjectPair( String(Key_logFile),
		RWInteger(ID_logFile)));
  objectDictionary.insert(objectDictionary.end(),ObjectPair(String(Key_TSposition),
		 RWInteger(ID_TSposition)));
  objectDictionary.insert(objectDictionary.end(),ObjectPair(String(Key_TSformat),
		 RWInteger(ID_TSformat)));

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
  criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_generalErrorFilter),
		 RWInteger(ID_generalErrorFilter)));
  criteriaDictionary.insert(criteriaDictionary.end(),CriteriaPair(String(Key_ceaseDuration),
		 RWInteger(ID_ceaseDuration)));

  commandDictionary.insert(commandDictionary.end(),CommandPair(String(Key_commandString), 
		 RWInteger(ID_commandString)));
}

//******************************************************************************
//	loadACF()
//******************************************************************************
ACS_USA_StatusType ACS_USA_Config::loadACF(
  ACS_USA_ObjectManager& manager, 
  const char* fileName,
  String& bootEventRegExp,
  String& RestartEventRegExp)
{
	std::ifstream stre;	// Using std/stl.
	try {
		stre.open(fileName);
		if(!stre.good()) {
			// ok
    }
	} 
	catch (...) {
	  // Ignore exception at this stage.
	}
  String refer;
  String name;
  String val;
  // Test if the stream is valid.
  // The ios(3C++) operator '!' returns non-zero if the stream is invalid.
  if (!stre) {
	  String a(fileName);
	  error.setError(ACS_USA_FileIOError);
	  a += ioErrorPrefix;
	  error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
	  return ACS_USA_error;
  }
  // Read the ACF file
  while(TRUE) {
    LineStatus ls = readLine(stre, refer, name, val);
  	if (ls == Line_eof) {
      // End of file.
	    if (fillAnalysisObjects(manager, bootEventRegExp, RestartEventRegExp)
        == ACS_USA_error)
      {
		    // Something wrong.
		    if (error.getError() == ACS_USA_SyntaxError) {
		      String a(fileName);
		      a += syntaxErrorPrefix;
		      error.setErrorText(a.data(), error.getErrorText());
		    }
		    return ACS_USA_error;
	    }
	    return ACS_USA_ok;
	  }
    else if (ls == Line_list) {
	  	ACS_USA_Tokenizer tokenizer(val);
	    ACS_USA_Boolean notFinish = ACS_USA_True;
	    while (notFinish) {
		    String element(tokenizer(" "));
        if (element.empty()) {
          notFinish = ACS_USA_False;
		    } 
		    else if (addListMember(refer, element) == ACS_USA_error) {
		      // error is set already make a prefix with line nr.
		      String a(fileName);
		      String b(error.getErrorText());
		      a += syntaxErrorPrefix;
		      b += lineNumberPrefix;
		      error.setErrorText(a.data(), b.data(), lineNr);
		      return ACS_USA_error;
		    }
        else {
		      // do nothing
		    }
	    }
	  }
    else if (ls == Line_parameter) { 
		  if (addParameterValue(refer, name, val) == ACS_USA_error) {
		    String a(fileName);
		    String b(error.getErrorText());
		    a += syntaxErrorPrefix;
		    b += lineNumberPrefix;
		    error.setErrorText(a.data(), b.data(), lineNr);
		    return ACS_USA_error;
	    }
	  }
    else {
      // Error when reading line.
	    ACS_USA_StatusType err = error.getError();
	    if (err == ACS_USA_FileIOError) {
		    String a(fileName);
		    a += ioErrorPrefix;
		    error.setErrorText(a.data(), error.getErrorText());
	    }
      else if (err == ACS_USA_SyntaxError) {
		    String a(fileName);
		    String b(error.getErrorText());
		    a += syntaxErrorPrefix;
		    b += lineNumberPrefix;
		    error.setErrorText(a.data(), b.data(), lineNr);
	    }
	    return ACS_USA_error;
	  }
	}
  return ACS_USA_ok;
}

//******************************************************************************
//	readLine()
//******************************************************************************
LineStatus ACS_USA_Config::readLine(
  std::ifstream& stream, // Using stl/std.
  String& ref,
  String& name,
  String& value)
{
  String strbuf;
  String line;
  int longLine = FALSE;
  int notFinish = TRUE;
  // Get a line of interest.
  while (notFinish) {
    if (getline(stream,strbuf, '\n')) {	// Successful.
			++lineNr;
      if (!(strbuf.empty())) {
        if (longLine || (longLine == FALSE && strbuf[(size_t)0] == Char_asterisk)) {
			    // Char_asterisk  is '*'.
			    size_t idx = last(strbuf,Char_escape);
		      stripWhiteSpace(strbuf); // Remove whitespaces.
			    if ( idx != string::npos && idx == (strbuf.length() - 1U)) {
				    strbuf.erase(idx, 1);	// Remove '\\' char.
		        longLine = TRUE;
		      }
          else {
		        notFinish = FALSE;
		      }
          line += strbuf;
		    }
      }
    }
    else { // Not ok.
      if (stream.eof()) {
        return Line_eof;
      }
      else {
        error.setError(ACS_USA_FileIOError);
        error.setErrorText(ACS_USA_ReadFailed);
        return Line_error;
      }
    }
  }
  // Split the line into components.
  size_t idx = first(line,Char_colon);
	if (idx == String::npos) {
	  error.setError(ACS_USA_SyntaxError);
	  error.setErrorText(ACS_USA_UnrecognizedParameter);
    return Line_error;
  }
  String head = line.substr(1, idx - 1U);
  ++idx;
	value = line.substr(idx, line.length() - idx);
	stripWhiteSpace(value);
	if (value.empty()) {
	  error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_MissingParameterValue);
		return Line_error;
  }
  char b = value[(size_t) 0];
  char e = value[value.length() - 1U];
  if (b == Char_singleQuote || b == Char_doubleQuote ||
	    e == Char_singleQuote || e == Char_doubleQuote) {
    if (e != b) {
	    error.setError(ACS_USA_SyntaxError);
	    error.setErrorText(ACS_USA_QuoteImbalance);
      return Line_error;
    }
		value = strip(value, b);
  }
	if ((idx = first(head,Char_dot)) == String::npos) {
    if (validateName(head)) { // Invalid name.
	    error.setError(ACS_USA_SyntaxError);
	    error.setErrorText(ACS_USA_IllegalName);
      return Line_error;
		}
    else {
      ref = head;
      return Line_list;
    }
  }
  else {
		ref = head.substr(0, idx);
    ++idx;
		name = head.substr(idx, head.length() - idx);
    if (validateName(ref) || validateName(name)) { // Invalid name.
      error.setError(ACS_USA_SyntaxError);
      error.setErrorText(ACS_USA_IllegalName);
      return Line_error;
    }
    else {
      return Line_parameter;
    }
  }
}

//******************************************************************************
//	stripWhiteSpace()
//******************************************************************************
String& 
ACS_USA_Config::stripWhiteSpace(String& str) const
{
	String tmp;
	strip(str, Char_space);
	tmp = str;
    String abc;
	strip(tmp, Char_tab);
	abc = tmp;
    str = abc;
    return str;
}



//******************************************************************************
//	validateName()
//******************************************************************************
int
ACS_USA_Config::validateName(String& str) const
{
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
    return FALSE;
}



//******************************************************************************
//	addListMember()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Config::addListMember(
			      const String& strlist,
			      String& element)
{
	bool critfound = false;
	bool objfound = false;
	CCriteriaListDictionaryIterator iter2;
	for(iter2 = criteriaListDictionary.begin();iter2 != criteriaListDictionary.end();
	  iter2++)
	{
		if(iter2->first == element)
		{
			critfound = true;
			break;
		}
	}
	
	CObjectListDictionaryIterator objIter;
	for(objIter = objectListDictionary.begin();objIter != objectListDictionary.end();
	  objIter++)
	{
		if(objIter->first == element)
		{
			objfound = true;
			break;
		}
	}

    // Check if there are same names in the list already
    if (objfound || critfound ||
		commandListDictionary.find(element)  != commandListDictionary.end()
    || bootEventListDictionary.find(element) != bootEventListDictionary.end()
	|| RestartEventListDictionary.find(element) != RestartEventListDictionary.end()
	) 
	{
	// Multiple objects with same name
	// handle
	error.setError(ACS_USA_SyntaxError);
	error.setErrorText(ACS_USA_MultipleNames);
	return ACS_USA_error;
    }
    //
    // Find the list the element belongs to and insert it
    //
    IndexType listIdx = getListReference(strlist);
    ACS_USA_StatusType ret = ACS_USA_ok;
    switch(listIdx) {
    
	case objectList:    
			    objectListDictionary.insert(objectListDictionary.end(),
								ObjectListPair(String(element), 
								ACS_USA_AnalysisObject()));
			    break;
			    
	case criteriaList:  
				criteriaListDictionary.insert(criteriaListDictionary.end(),
							CriteriaListPair(element, 
							ACS_USA_Criterion()));
			    break;
			    
	case commandList:    
			    commandListDictionary.insert(commandListDictionary.end(),CommandListPair(String(element), 
				     String()));
    			break;
	case bootEventList:    
			    bootEventListDictionary.insert(CBootEventListPair(String(strlist),   
					 String(element))); 
    			break;

	case RestartEventList:   
			    RestartEventListDictionary.insert(RestartEventListPair(
					 String(strlist),   
					 String(element))); 
    			break;
			    
	default:	    // error
			    ret = ACS_USA_error;
			    error.setError(ACS_USA_SyntaxError);
			    error.setErrorText(ACS_USA_IllegalList);
			    break;
    }
    return ret;
}


//******************************************************************************
//	addParameterValue()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Config::addParameterValue(
				          const String& ref, 
				          const String& name, 
				          String& value)
{
  IndexType listIdx = unknownList;
  CObjectListDictionaryIterator iter1;
  bool objfound = false;
  ACS_USA_AnalysisObject object;

  for(iter1 = objectListDictionary.begin();iter1 != objectListDictionary.end();
	  iter1++)
  {
	  if(iter1->first == ref)
	  {
		objfound = true;
		object = iter1->second;
		break;
	  }
  }

  CCriteriaListDictionaryIterator iter2;
  ACS_USA_Criterion criterion ;
  bool critfound = false;

  for(iter2 = criteriaListDictionary.begin();iter2 != criteriaListDictionary.end();
	  iter2++)
  {
	  if(iter2->first == ref)
	  {
		critfound = true;
		criterion = iter2->second;
		break;
	  }
  }
  
  String *emptyStr = 0;

  CCommandListDictionaryIterator citer ;
  citer = commandListDictionary.find(ref);	
  const String *command = &(citer->first);
  emptyStr = &(citer->second);


  if(objfound)
  {
	  listIdx = objectList;
  }
  //else if (criterion != rwnil)
  //else if (iter2 != criteriaListDictionary.end())
  else if (critfound)
  {
	  listIdx = criteriaList;
  }
  //else if (command != rwnil)
  else if (commandListDictionary.find(ref) != commandListDictionary.end())
  {
	  listIdx = commandList;
  }
  else 
	{
	  // Parameter was not found
	  error.setError(ACS_USA_SyntaxError);
	  error.setErrorText(ACS_USA_UndefinedListMember);
	  return ACS_USA_error;
  }
  if (listIdx == objectList)
  {
	RWInteger p = findValue(objectDictionary,name);
	if (p == rwnil)
    {
	    // unknown parameter name
	    error.setError(ACS_USA_SyntaxError);
	    //set the ref - to indicate which one
	    error.setErrorText(ACS_USA_unknownParameter);
	    return ACS_USA_error;
	  }
	  switch(p)
    {
      case ID_logFile:
        object.setLogFile(value);
				break;
		  case ID_TSposition:
        object.setPosition(value);
				break;
		  case ID_TSformat:
        object.setFormat(value);
				break;
		  default:
        break;
	  }
	
	iter1->second = object  ;
  }
  else if (listIdx == criteriaList)
  {
	  RWInteger p =findValue( criteriaDictionary,name);
	  if (p == rwnil)
	  {
		  // unknown parameter name
		  error.setError(ACS_USA_SyntaxError);
		  //set the ref - to indicate which one
		  error.setErrorText(ACS_USA_unknownParameter);
		  return ACS_USA_error;
	  }
	  switch(p)
	  {
	  case ID_object:
		  criterion.setObject(value); 
		  break;
	  case ID_scope:
		  if (criterion.setScope(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_maxAllowed:
		  if (criterion.setMaxAllowed(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_matchType:
		  if (criterion.setMatchType(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_pattern:
		  criterion.setPattern(value);
		  break;
	  case ID_specificProblem:
		  if (criterion.setSpecificProblem(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_perceivedSeverity:
		  if (criterion.setPerceivedSeverity(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_probableCause:
		  if (criterion.setProbableCause(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_objectOfRefSuffix:
		  criterion.setObjectOfRefSuffix(value);
		  break;
	  case ID_problemText:
		  criterion.setProblemText(value);
		  break;
	  case ID_command:
		  criterion.setCommand(value);
		  break;
	  case ID_node:
		  criterion.setNode(value);
		  break;
	  case ID_disableAlarmFilter:
		  criterion.setDisableAlarmFilter(value);
		  break;
	  case ID_generalErrorFilter:
		  if (criterion.setGeneralErrorFilter(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  case ID_ceaseDuration:
		  if (criterion.setCeaseDuration(value) == ACS_USA_error)
		  {
			  return ACS_USA_error;
		  }
		  break;
	  default:
		  break;
	  }
	  iter2->second = criterion ;
  }
  else if (listIdx == commandList)
  {
    RWInteger p = findValue(commandDictionary,name);
	 if (p == rwnil)
    {
	    // unknown parameter name
	    error.setError(ACS_USA_SyntaxError);
	    //set the ref - to indicate which one
	    error.setErrorText(ACS_USA_unknownParameter);
	    return ACS_USA_error;
	  }
	  switch(p)
    {
      case ID_commandString: delete emptyStr; // delete previous one
				commandListDictionary.insert(commandListDictionary.end(),CommandListPair(
				 String(*command),
				 //String(*value)));
				String(value)));
				break;
		  default:
        break;
	  }
  } 
	else
  {		    
	  return ACS_USA_error;
  }
  return ACS_USA_ok;
}
			       
//******************************************************************************
//	check()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Config::check(
					String& bootEventRegExp,
					String& RestartEventRegExp
)
{
    // Checking consistency of parameters in all list dictionaries
    
    //
    // Check criteria
    //
    {
		CCriteriaListDictionaryIterator iter = criteriaListDictionary.begin();

    	if (criteriaListDictionary.size() == 0) 
		{
			// No criteria specified
			error.setError(ACS_USA_SyntaxError);
			error.setErrorText(ACS_USA_NoCriteria);
			return ACS_USA_error;
		}
		
		for( ;iter != criteriaListDictionary.end();iter++)
		{			
			ACS_USA_Criterion *criterion = &(iter->second);
			String *object = &(criterion->getObject());
		
			if (!(object->empty())) {	// check object reference 
				CObjectListDictionaryIterator objIter;
				bool objfound = false;
				for(objIter = objectListDictionary.begin();objIter != objectListDictionary.end();
				objIter++)
				{
					if(objIter->first == *object)
					{
						objfound = true;
						break;
					}
				}
				if(!objfound)
				{
					// Reference to unknown AnalysisObject
					String a(ACS_USA_UnknownReference);
					error.setError(ACS_USA_SyntaxError);
					a += beginQuote;
					a += *object;
					a += endQuote;
					error.setErrorText(a.data());
					return ACS_USA_error;
				}
			} 
			else 
			{			
				// Missing object reference
				String a(ACS_USA_MissingObjectReference);
				error.setError(ACS_USA_SyntaxError);
				a += beginQuote;
				a += criterion->getObject();
				a += endQuote;
				error.setErrorText(a.data());
				return ACS_USA_error;
			}
		
			String *command = &(criterion->getCommand());
		
			if (!(command->empty())) {
		
			if (commandListDictionary.find(*command) == commandListDictionary.end()) {
				// A criterion refers to unknown command
				String a(ACS_USA_MissingCommandReference);
				error.setError(ACS_USA_SyntaxError);
				a += beginQuote;
				a += criterion->getCommand();
				a += endQuote;
				error.setErrorText(a.data());
				return ACS_USA_error;
			}
			}

			if (criterion->validate() == ACS_USA_error) {
			String a(error.getErrorText());
			a += beginQuote;
			a += (iter->first);
			a += endQuote;
			error.setErrorText(a.data());
			return ACS_USA_error;
			}
		}
    }
    
    //
    // Check Analysis Objects
    //
    {
	CObjectListDictionaryIterator ator =objectListDictionary.begin();
	if (objectListDictionary.size() == 0) 
	{
	    error.setError(ACS_USA_SyntaxError);
	    error.setErrorText(ACS_USA_NoAnalysisObjects);
	    return ACS_USA_error;
	}
    
	for(; ator != objectListDictionary.end();ator++) 
	{
		ACS_USA_AnalysisObject *obj = &(ator->second);
	    if (obj->validate() == ACS_USA_error) {
		String a(error.getErrorText());
		a += beginQuote;
		a += (ator->first);
		a += endQuote;
		error.setErrorText(a.data());
		return ACS_USA_error;
	    }
	}
    }
    
    //
    // Check Commands
    //
    {
	CCommandListDictionaryIterator iterator=commandListDictionary.begin();
    
	for(; iterator != commandListDictionary.end();++iterator)
	{
		String *str = &(iterator->second);
		if (str->empty()) {
	    String a(ACS_USA_MissingCommand);
		error.setError(ACS_USA_SyntaxError);
		a += beginQuote;
		a += (iterator->first);
	
		a += endQuote;
		error.setErrorText(a.data());
		return ACS_USA_error;
	    }
	}
    } 
    //
    // Check boot event regexp
    //
	// Is there an entry in boot event dictionary?
	//
	int entries = 0;
	if ((entries = bootEventListDictionary.size()) < 1)
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText("No value for bootEventRegularExpression in ACF");
		return ACS_USA_error;
	}
	// Set variable to the boot event regular expression defined in ACF,
	// by reading key value bootEventRegularExpression
	//
	const String keyName = Key_bootEventString;	
	String regexp = (findValue(bootEventListDictionary,keyName)); 
	bootEventRegExp = regexp;

	// Is there an entry in RestartEventListDictionary?
	//
	int entr = 0;
	if ((entr = RestartEventListDictionary.size()) < 1)
	//if ((entr = RestartEventListDictionary.entries()) < 1)
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText("No value for RestartEventRegularExpression in ACF");
		return ACS_USA_error;
	}
	// Set variable to the cleared Seclog event regexp defined in ACF,
	// by reading key value bootEventRegularExpression
	//
	const String keyNameRestart = Key_RestartEventString; 	
	String restartRegexp = (findValue(RestartEventListDictionary,keyNameRestart)); 
	RestartEventRegExp = restartRegexp;
    return ACS_USA_ok;
}


//******************************************************************************
//	fillAnalysisObjects()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Config::fillAnalysisObjects(ACS_USA_ObjectManager& manager,
									String& bootEventRegExp,
									String& RestartEventRegExp
)
{
    CObjectListDictionaryIterator aoIter = objectListDictionary.begin();
    if (check(
				bootEventRegExp,
				RestartEventRegExp
		) == ACS_USA_error) 
	{
		// Something wrong in parameters its handled already
		return ACS_USA_error;
    }
    
	for( ;aoIter != objectListDictionary.end();aoIter++)
	{
	//
	// Get next object from object dictionary. Create new AnalysisObject
	// calling the copy constructor. Insert all criteria that refer to
	// Analysis Object.
	//
	ACS_USA_AnalysisObject 	*configObject  = &(aoIter->second);
	const String 		*key = &(aoIter->first);

	ACS_USA_Flag		noCriteria = ACS_USA_True;
	ACS_USA_AnalysisObject  *managerObject = new ACS_USA_AnalysisObject(*configObject);
	if (managerObject == NULL) 
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Config::fillAnalysisObjects().");
		return ACS_USA_error;	
	}

	CCriteriaListDictionaryIterator crIter = criteriaListDictionary.begin();
	
	// Insert criteria	
	for(;crIter != criteriaListDictionary.end();++crIter) 
	{
		ACS_USA_Criterion *criterion = &(crIter->second);
	    if (criterion->getObject() == *key) {

		// See if a command is attached to the current criterion.
		// Find it if needed and put in . 
		// Add the criterion to the current AnalysisObject.
		//
		String *command = &(criterion->getCommand());
		if (!(command->empty())) {
			CCommandListDictionaryIterator cmdIter = commandListDictionary.begin();
			bool found=false;
			for(;cmdIter!= commandListDictionary.end();cmdIter++)
			{
				if(cmdIter->first == *command)
				{
					*command = cmdIter->second;
					found=true;
					break;
				}
			}
				
			if (!found) {
		        // error
			return ACS_USA_error;
		    }
		    criterion->setCommand(*command);
		}
		noCriteria = ACS_USA_False;
		managerObject->appendCriterion(*criterion);
	    }
	}
	
	// 
	// Add the current Analysis Object to Object Manager
	//
	if (noCriteria == ACS_USA_True){
	    error.setError(ACS_USA_SyntaxError);
	    error.setErrorText(key->data(), ACS_USA_NoCriteria);
	    return ACS_USA_error;
	}

	manager.append(managerObject);
    }
    
    return ACS_USA_ok;
}


//******************************************************************************
//	getListReference()
//******************************************************************************
IndexType
ACS_USA_Config::getListReference(const String& str) const
{
    if (str == Key_objectList) {
	return objectList;
    } else if (str == Key_criteriaList) {
	return criteriaList;
    } else if (str == Key_commandList) {
	return commandList;
    } else 
	if (str == Key_bootEventString){
	return bootEventList;
	}
	else
	if (str == Key_RestartEventString){
	return RestartEventList;
	}
	else
	{
	return unknownList;
    }
}


//******************************************************************************
//	~ACS_USA_Config()
//******************************************************************************
ACS_USA_Config::~ACS_USA_Config()
{
	objectListDictionary.clear();
    criteriaListDictionary.clear();
    commandListDictionary.clear();  
    commandDictionary.clear();
    criteriaDictionary.clear();
    objectDictionary.clear();
	bootEventListDictionary.clear();
	RestartEventListDictionary.clear();
}

//******************************************************************************

int findValue(map<string,int> ls, const string pkey)
{
	 map<string,int>:: const_iterator iter;
	iter = ls.find(pkey);
	if (iter != ls.end())
		return iter->second;
	else
		return rwnil;
}

String findValue(map<string,string> ls, const string pkey)
{
	map<string,string>:: const_iterator iter;
	iter = ls.find(pkey);
	if (iter != ls.end())
		return iter->second;
	else
		return nilstring;
}
