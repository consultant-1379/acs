//======================================================================
//
// NAME
//      RuleChecker.cpp
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      -
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-17 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map> 
#include <syslog.h>

#include "acs_hcs_commandclientmml.h"
#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentprintout.h"
#include "acs_hcs_printoutparsermml.h"
#include "acs_hcs_printoutparsermmlallip.h"
#include "acs_hcs_rulechecker.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class RuleChecker
	//================================================================================

	RuleChecker::RuleChecker() : cmdResultFiles(0)
	{
		//DEBUG("%s","In Default constructor of RuleChecker ");
	}

	RuleChecker::RuleChecker(Directory::FileNames& cmdResultFiles) : cmdResultFiles(&cmdResultFiles)
	{
		//DEBUG("%s","Enetering RuleChecker(Directory::FileNames& cmdResultFiles) ");
		this->cmdResultFiles->clear();
		//DEBUG("%s","Leaving RuleChecker(Directory::FileNames& cmdResultFiles) ");
	}

	RuleChecker::~RuleChecker()
	{
		//DEBUG("%s","In Destructor RuleChecker ");
	}

	const DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key)  ");
		const DocumentPrintout::Filter* result = this->resFilters.get(key);

		if (result == 0)
			ACS_HCS_THROW(RuleChecker::ExceptionUnknownResult, "getResultFilter()", "Unable to fetch result for key '" << key << "'.", "Key not found.");
		//DEBUG("%s","Leaving DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key)  ");
		return *result;
	}
	bool RuleChecker::isPassed(const DocumentXml::Node& ruleCheck, const std::string& cpName)
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key)  ");
		cout << "isPassed" << endl;
		if (! ruleCheck.isValid())
		{
			//DEBUG("%s","Leaving DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key) ");
			return false;
		}
		if (ruleCheck.getName() != "check")
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "isPassed()", "Undefined node-tag '" << ruleCheck.getName() << "'.", "");
		cout<<"ruleCheck.getName()->"<<ruleCheck.getName()<<endl;
		//DEBUG("%s","Leaving DocumentPrintout::Filter& RuleChecker::getResultFilter(const string& key)  ");
		return this->evaluateNodeCheck(ruleCheck.getFirstChild(), cpName); 
	}

	void RuleChecker::setCmdResultFiles(Directory::FileNames& cmdResultFiles)
	{
		//DEBUG("%s","Entering RuleChecker::setCmdResultFiles(Directory::FileNames& cmdResultFiles ");
		this->cmdResultFiles = &cmdResultFiles;
		this->cmdResultFiles->clear();
		//DEBUG("%s","Leaving RuleChecker::setCmdResultFiles(Directory::FileNames& cmdResultFiles ");
	}
		
	bool RuleChecker::evaluateNodeCheck(const DocumentXml::Node& node, const std::string& cpName)
	{	
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeCheck ");
		if (!node.isValid())
		{
			//DEBUG("%s","Leaving RuleChecker::evaluateNodeCheck");
			return true;
		}
		bool result;
		string nodeName = node.getName();
		if (nodeName == "condition")
		{
			result = evaluateNodeCondition(node.getFirstChild());

			if (!result)
			{
				//DEBUG("%s","Leaving RuleChecker::evaluateNodeCheck");
				return true;
			}
		}
		else if (nodeName == "mml")
		{
			result = evaluateNodeMml(node, cpName);
			cout<<"return is "<<  result <<endl;
		}
		else if (nodeName == "eval")
		{
			result = evaluateNodeEval(node.getFirstChild());
			syslog(LOG_INFO, "result of evaluateNodeEval is %d", result);	
			cout<<"result of evaluateNodeEval is "<<result <<endl;	
			DocumentXml::Node attrResult = node.getAttribute("result", false);

			if (attrResult.isValid())
			{
				cout<<"after evall , calling add"<<endl;
				this->resBools.add(attrResult.getValue(), new bool(result));
			}
		}
		else if (nodeName == "check")
		{
			result = evaluateNodeCheck(node.getFirstChild(), cpName);
			DocumentXml::Node attrResult = node.getAttribute("result", false);

			if (attrResult.isValid())
				this->resBools.add(attrResult.getValue(), new bool(result));
		}
		else
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeCheck()", "Undefined tag '" << nodeName << "'.", "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeCheck ");
		return (result && evaluateNodeCheck(node.getNextSibling(), cpName));
	}


	bool RuleChecker::evaluateNodeMml(const DocumentXml::Node& node, const std::string& cpName)
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeMml ");
		//const DocumentXml::Node& command = node.getNode("./command");
		const DocumentXml::Node& command = node.getSpecifiedNode("./command",node,false);
		
		string fileNameCmdResult;

		{
			// Evaluate Node mml/command.

			CommandClientMml client;
			fileNameCmdResult = client.send(command, cpName);
			cout<<"now the file name is :"<< fileNameCmdResult<<endl;
			this->cmdResultFiles->push_back(fileNameCmdResult);
		}

		DocumentPrintout* docPrintout;

		{
			// Evaluate Node mml/printout.
			
			docPrintout = this->resPrintouts.add(new DocumentPrintout());
			//const DocumentXml::Node& printout = node.getNode("./printout");
			const DocumentXml::Node& printout = node.getSpecifiedNode("./printout",node,NULL);

			try
			{
				if (command.getValue().find("ALLIP") != string::npos)
				{
					PrintoutParserMmlAllip parser;
					parser.parse(*docPrintout, printout, fileNameCmdResult);
				}
				else
				{
					PrintoutParserMml parser;
					parser.parse(*docPrintout, printout, fileNameCmdResult);
					cout<<"parsing done"<<endl;
				}
			}
			catch (PrintoutParser::ExceptionParseError&)
			{
				syslog(LOG_INFO, "Parse error exception");
				ACS_HCS_THROW(PrintoutParser::ExceptionParseError, "evaluateNodeMml()", "No corresponding printout", "");				
			}
			catch (const PrintoutParser::ExceptionPrintoutFailed&)
			{
				//DEBUG("%s"," RuleChecker::evaluateNodeMml");
				return false;
			}

			DocumentXml::Node attrResult = printout.getAttribute("result", false);

			if (attrResult.isValid())
				this->resFilters.add(attrResult.getValue(), new DocumentPrintout::Filter(*docPrintout));
		}

		vector<string> filterNames;

		/*if (Configuration::Singleton::get()->is//DEBUGEnabled())
		{
			// Print the internal printout representation to file.

			string printoutFile = fileNameCmdResult + ".docPrintout.txt";
			ofstream file;
			file.open (printoutFile.c_str(),ios_base::app);
			streambuf* sbuf = cout.rdbuf();
			cout.rdbuf(file.rdbuf());  
			docPrintout->printAll();
			cout.rdbuf(sbuf);
			file.close();
		}*/

		{
			// Evaluate Nodes mml/filter.

			DocumentXml::Nodes filters;
			node.getNodesList(filters, "./filter",node);
			DocumentXml::Nodes::Iterator filter;

			for (filter = filters.begin(); filter != filters.end(); ++filter)
			{
				cout<<"Inside filter iterator"<<endl;
				this->resFilters.add((*filter).getAttribute("result").getValue(), new DocumentPrintout::Filter(*docPrintout, *filter));

				/*if (Configuration::Singleton::get()->is//DEBUGEnabled())
				{
					filterNames.push_back((*filter).getAttribute("result").getValue());
				}*/
			}
		}

		/*if (Configuration::Singleton::get()->is//DEBUGEnabled())
		{
			// Print the internal printout representation to file.

			string printoutFile = fileNameCmdResult + ".docPrintout.txt";
			ofstream file;

			file.open (printoutFile.c_str(),ios_base::app);
			streambuf* sbuf = cout.rdbuf();
			cout.rdbuf(file.rdbuf());  

			for (vector<string>::iterator filterName = filterNames.begin(); filterName != filterNames.end(); ++filterName)
			{
				cout << "output from Filter: " << *filterName << endl;
				DocumentPrintout::Filter *dFilter = resFilters.get( *filterName );
				dFilter->printAll();
			}

			filterNames.clear();
			cout.rdbuf(sbuf);
			file.close();
		}*/
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeMml ");
		cout << "Returning from Evaluate mml" << endl;
		return true;
	}


	bool RuleChecker::evaluateNodeCondition(const DocumentXml::Node& node)
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeCondition ");
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeCondition ");
		return this->evaluateNodeEval(node);
	}

	bool RuleChecker::evaluateNodeEval(const DocumentXml::Node& node)
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeEval ");
		if (!node.isValid())
		{	
			//DEBUG("%s","Leaving RuleChecker::evaluateNodeEval");
			return true;
		}
		bool result;
		string nodeName = node.getName();
		if (nodeName == "andNODE" || nodeName == "and")
		{
			result = this->evaluateNodeAnd(node);
		}
		else if (nodeName == "orNODE" || nodeName == "or")
		{
			result = this->evaluateNodeOr(node);
		}
		else if (nodeName == "notNODE" || nodeName == "not")
		{ 
			result = this->evaluateNodeNot(node);
		}
		else if (nodeName == "is-empty")
		{
			result = this->evaluateNodeIsEmpty(node);
		}
		else if (nodeName == "is-not-empty")
		{
			result = this->evaluateNodeIsNotEmpty(node);
		}
		else if (nodeName == "is-equal")
		{
			result = this->evaluateNodeIsEqual(node);
		}
		else if (nodeName == "is-not-equal")
		{
			result = this->evaluateNodeIsNotEqual(node);
		}
		else if (nodeName == "all-in-period")
		{
			result = this->evaluateNodeIsInPeriod(node);
		}
		else if (nodeName == "is-in-period")
		{
			result = this->evaluateNodeIsInPeriod(node);
		}
		else if (nodeName == "is-subset")
		{
			result = this->evaluateNodeIsSubset(node);
		}
		else if (nodeName == "is-true")
		{
			result = this->evaluateNodeIsTrue(node);
		}
		else if (nodeName == "columns-equal")
		{
			result = this->evaluateNodeColumnsEqual(node);
		}
		else if (nodeName == "columns-greater")
		{
			result = this->evaluateNodeColumnsGreater(node);
		}
		else if (nodeName == "columns-less")
		{
			result = this->evaluateNodeColumnsLess(node);
		}
		else if (nodeName == "all-in-range")
		{
			result = this->evaluateNodeIsInRange(node);
		}
		else
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeEval()", "Undefined tag '" << nodeName << "'.", "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeEval( ");
		return result;
	}

	bool RuleChecker::evaluateNodeAnd(const DocumentXml::Node & andNODE)
	{	
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeAnd( ");
		const DocumentXml::Node& child = andNODE.getFirstChild();

		if (!child.isValid())
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeAnd()", "Invalid number of operands: operator 'and' must have at least one operand: " << andNODE, "");

		bool result = this->evaluateNodeEval(child);

		for (DocumentXml::Node sibling = child.getNextSibling(); result && sibling.isValid(); sibling = sibling.getNextSibling())
			result = (result && this->evaluateNodeEval(sibling));
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeAnd(");
		return result;
	}

	bool RuleChecker::evaluateNodeOr(const DocumentXml::Node& orNODE)
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeOr ");
		const DocumentXml::Node& child = orNODE.getFirstChild();

		if (!child.isValid())
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeOr()", "Invalid number of operands: operator 'or' must have at least one operand: " << orNODE, "");

		bool result = this->evaluateNodeEval(child);

		for (DocumentXml::Node sibling = child.getNextSibling(); (!result) && sibling.isValid(); sibling = sibling.getNextSibling())
			result = (result || this->evaluateNodeEval(sibling));
		//DEBUG("%s","Leaving RuleChecker::evaluateNodeOr ");
		return result;
	}

	bool RuleChecker::evaluateNodeNot(const DocumentXml::Node& notNODE)
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeNot ");
		const DocumentXml::Node& child = notNODE.getFirstChild();

		if (!child.isValid() || child.getNextSibling().isValid())
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeNot()", "Invalid number of operands: operator 'not' must have exactly one operand: " << notNODE, "");
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeNot ");
		return (! this->evaluateNodeEval(child));
	}

	bool RuleChecker::evaluateNodeIsEmpty(const DocumentXml::Node& isEmpty) const
	{	
		//DEBUG("%s","Entering RuleChecker::evaluateNodeIsEmpty ");
		try
		{
			const DocumentXml::Node& nodeSet = isEmpty.getAttribute("set");
			const string& valueSet = nodeSet.getValue();
			const DocumentPrintout::Filter* filterSet = this->resFilters.get(valueSet);
			if (filterSet == 0)
			{
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsEmpty()", "Undefined variable '" << valueSet << "': " << nodeSet, "");
			}
			cout<<"printing filterSet->printAll() "<<endl;
			filterSet->printAll() ;
			cout<<"printig done"<<endl;
			bool ret = filterSet->isEmpty();
			syslog(LOG_INFO, "Filter is %d", ret);
			cout<<"return of filterSet->isEmpty() is "<<ret;
			//DEBUG("%s","Leaving RuleChecker::evaluateNodeIsEmpty ");
			return ret;
			//filterSet->isEmpty();
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsEmpty()", "Invalid number of arguments. Required argument: 'set': " << isEmpty, "");
		}
		//DEBUG("%s","Leaving RuleChecker::evaluateNodeIsEmpty ");
	}

	bool RuleChecker::evaluateNodeIsEqual(const DocumentXml::Node& isEqual) const
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeIsEqual ");
		try
		{
			const DocumentXml::Node& nodeSetLeft = isEqual.getAttribute("set-left");
			const DocumentXml::Node& nodeSetRight = isEqual.getAttribute("set-right");
			const string& valueSetLeft = nodeSetLeft.getValue();
			const string& valueSetRight = nodeSetRight.getValue();
			const DocumentPrintout::Filter* filterSetLeft = this->resFilters.get(valueSetLeft);
			const DocumentPrintout::Filter* filterSetRight = this->resFilters.get(valueSetRight);

			if (filterSetLeft == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsEqual()", "Undefined variable '" << valueSetLeft << "': " << nodeSetLeft, "");

			if (filterSetRight == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsEqual()", "Undefined variable '" << valueSetRight << "': " << nodeSetRight, "");
			 //DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsEqual ");
			return filterSetLeft->isEqual(*filterSetRight);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsEqual()", "Invalid number of arguments. Required arguments: 'set-left', 'set-right': " << isEqual, "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsEqual ");
	}

	bool RuleChecker::evaluateNodeIsInPeriod(const DocumentXml::Node& isInPeriod) const
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeIsInPeriod( ");
		string nodeName = isInPeriod.getName();

		try
		{
			const DocumentXml::Node& nodeSetTime = isInPeriod.getAttribute("set-time");
			const DocumentXml::Node& nodeSetTimeCurrent = isInPeriod.getAttribute("reference-time");
			const string& valueSetTime = nodeSetTime.getValue();
			const string& valueSetTimeCurrent = nodeSetTimeCurrent.getValue();
			const DocumentPrintout::Filter* filterSetTime = this->resFilters.get(valueSetTime);
			const DocumentPrintout::Filter* filterSetTimeCurrent = this->resFilters.get(valueSetTimeCurrent);

			if (filterSetTime == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Undefined variable '" << valueSetTime << "': " << nodeSetTime, "");

			if (filterSetTimeCurrent == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Undefined variable '" << valueSetTimeCurrent << "': " << nodeSetTimeCurrent, "");

			time_t valuePeriod  = 24 * 3600; // Optional attribute 'period'  , default is 24h.
			string valueColDate;             // Optional attribute 'col-date', for implicit default see DocumentPrintout::Filter::areAllInPeriod().
			string valueColTime;             // Optional attribute 'col-time', for implicit default see DocumentPrintout::Filter::areAllInPeriod().

			{
				const DocumentXml::Node& nodePeriod = isInPeriod.getAttribute("period", false);

				if (nodePeriod.isValid())
				{
					const string& str = nodePeriod.getValue();

					if (str.empty() || (str[0] != '0' && (valuePeriod = ACE_OS::strtoul(str.c_str(),0, 10)) == 0))
						ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Invalid value of argument 'period': " << nodePeriod, "");

					valuePeriod *= 3600; // attribute 'period' is given in hours.
				}
			}
			{
				const DocumentXml::Node& nodeColDate = isInPeriod.getAttribute("col-date", false);

				if (nodeColDate.isValid())
				{
					valueColDate = nodeColDate.getValue();

					if (valueColDate.empty())
						ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Invalid value of argument 'col-date': " << nodeColDate, "");
				}
			}
			{
				const DocumentXml::Node& nodeColTime = isInPeriod.getAttribute("col-time", false);

				if (nodeColTime.isValid())
				{
					valueColTime = nodeColTime.getValue();

					if (valueColTime.empty())
						ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Invalid value of argument 'col-time': " << nodeColTime, "");
				}
			}
			//DEBUG("%s","Leaving RuleChecker::evaluateNodeIsInPeriod( ");
			return filterSetTime->isInPeriod(valuePeriod, *filterSetTimeCurrent, valueColDate, valueColTime, nodeName == "all-in-period");

		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInPeriod()", "Invalid number of arguments. Required arguments: 'set-time', 'reference-time', ['period'], ['col-date'], ['col-time']: " << isInPeriod, "");
		}
		//DEBUG("%s","Leaving RuleChecker::evaluateNodeIsInPeriod( ");
	}

	bool RuleChecker::evaluateNodeIsNotEmpty(const DocumentXml::Node& isNotEmpty) const
	{
		//DEBUG("%s"," Entered RuleChecker::evaluateNodeIsNotEmpty");
		cout<<"Entered RuleChecker::evaluateNodeIsNotEmpty"<<endl;
		 //DEBUG("%s"," Leaving RuleChecker::evaluateNodeIsNotEmpty");
		return (! this->evaluateNodeIsEmpty(isNotEmpty));
	}

	bool RuleChecker::evaluateNodeIsNotEqual(const DocumentXml::Node& isNotEqual) const
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeIsNotEqual(c ");
		 //DEBUG("%s","Leaving RuleChecker::evaluateNodeIsNotEqual(c ");
		return (! this->evaluateNodeIsEqual(isNotEqual));
	}

	bool RuleChecker::evaluateNodeIsSubset(const DocumentXml::Node& isSubset) const
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeIsSubset(const DocumentXml::Node& isSubset) ");
		try
		{
			const DocumentXml::Node& nodeSetLeft = isSubset.getAttribute("subset");
			const DocumentXml::Node& nodeSetRight = isSubset.getAttribute("superset");
			const string& valueSetLeft = nodeSetLeft.getValue();
			const string& valueSetRight = nodeSetRight.getValue();
			const DocumentPrintout::Filter* filterSetLeft = this->resFilters.get(valueSetLeft);
			const DocumentPrintout::Filter* filterSetRight = this->resFilters.get(valueSetRight);

			if (filterSetLeft == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsSubset()", "Undefined variable '" << valueSetLeft << "': " << nodeSetLeft, "");

			if (filterSetRight == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsSubset()", "Undefined variable '" << valueSetRight << "': " << nodeSetRight, "");
			 //DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsSubset(const DocumentXml::Node& isSubset) ");
			return filterSetLeft->isSubset(*filterSetRight);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsSubset()", "Invalid number of arguments. Required arguments: 'subset', 'superset': " << isSubset, "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsSubset(const DocumentXml::Node& isSubset) ");
	}

	bool RuleChecker::evaluateNodeIsTrue(const DocumentXml::Node& isTrue) const
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeIsTrue(const DocumentXml::Node& isTrue)  ");
		try
		{
			const DocumentXml::Node& nodeBoolResult = isTrue.getAttribute("bool-result");
			const string& valueBoolResult = nodeBoolResult.getValue();
			const bool* boolResult = this->resBools.get(valueBoolResult);

			if (boolResult == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsTrue()", "Undefined variable '" << valueBoolResult << "': " << nodeBoolResult, "");
			 //DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsTrue(const DocumentXml::Node& isTrue)  ");
			return *boolResult;
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsTrue()", "Invalid number of arguments. Required arguments: 'bool-result': " << isTrue, "");
		}
		//DEBUG("%s","LEaving  RuleChecker::evaluateNodeIsTrue(const DocumentXml::Node& isTrue)  ");	
	}

	bool RuleChecker::evaluateNodeColumnsEqual(const DocumentXml::Node& columnsEqual) const
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeColumnsEqual(const DocumentXml::Node& columnsEqual) ");
		try
		{
			string valueColLeft = columnsEqual.getAttribute("col-left").getValue();
			string valueColRight = columnsEqual.getAttribute("col-right").getValue();
			string valueSet = columnsEqual.getAttribute("set").getValue();

			const DocumentPrintout::Filter* filterSet = this->resFilters.get(valueSet);

			if (filterSet == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsEqual()", "Undefined variable '" << valueSet << "': " << columnsEqual, "");

			DocumentPrintout::References references;

			filterSet->compareColumns(references, valueColLeft, valueColRight, DocumentPrintout::/*Operator::*/EQUAL, "number", "100");
			 //DEBUG("%s","Leaving RuleChecker::evaluateNodeColumnsEqual(const DocumentXml::Node& columnsEqual) ");
			// If original references in filter are equal to the references after the operation
			// then all columns fulfill the condition
			return filterSet->isEqual(references);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsEqual()", "Invalid number of arguments. Required arguments: 'set', 'col-left', 'col-right': " << columnsEqual, "");
		}
		//DEBUG("%s","Leaving RuleChecker::evaluateNodeColumnsEqual(const DocumentXml::Node& columnsEqual) ");
	}

	bool RuleChecker::evaluateNodeColumnsGreater(const DocumentXml::Node& columnsGreater) const
	{
		//DEBUG("%s","Entering RuleChecker::evaluateNodeColumnsGreater(const DocumentXml::Node& columnsGreater) const ");
		try
		{
			string valueColLeft = columnsGreater.getAttribute("col-left").getValue();
			string valueColRight = columnsGreater.getAttribute("col-right").getValue();
			string valueSet = columnsGreater.getAttribute("set").getValue();
			DocumentXml::Node nodeDataType = columnsGreater.getAttribute("type", false);
			string valueDataType = nodeDataType.isValid() ? nodeDataType.getValue() : "number"; // standard value is number

			if (valueDataType != "string")
				valueDataType = "number"; // if the value is not string, then assign number

			DocumentXml::Node nodePercent = columnsGreater.getAttribute("percent", false);
			string valuePercent = nodePercent.isValid() ? nodePercent.getValue() : "100";			

			const DocumentPrintout::Filter* filterSet = this->resFilters.get(valueSet);

			if (filterSet == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsGreater()", "Undefined variable '" << valueSet << "': " << columnsGreater, "");

			DocumentPrintout::References references;

			filterSet->compareColumns(references, valueColLeft, valueColRight, DocumentPrintout::/*Operator::*/GREATER, valueDataType, valuePercent);
			//DEBUG("%s","Leaving RuleChecker::evaluateNodeColumnsGreater(const DocumentXml::Node& columnsGreater) const ");	
			// If original references in filter are equal to the references after the operation
			// then all columns fulfill the condition
			return filterSet->isEqual(references);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsGreater()", "Invalid number of arguments. Required arguments: 'set', 'col-left', 'col-right', ['type'], ['percent']: " << columnsGreater, "");
		}
		//DEBUG("%s","Leaving RuleChecker::evaluateNodeColumnsGreater(const DocumentXml::Node& columnsGreater) const ");
	}

	bool RuleChecker::evaluateNodeColumnsLess(const DocumentXml::Node& columnsLess) const
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeColumnsLess(const DocumentXml::Node& columnsLess) ");
		try
		{
			string valueColLeft = columnsLess.getAttribute("col-left").getValue();
			string valueColRight = columnsLess.getAttribute("col-right").getValue();
			string valueSet = columnsLess.getAttribute("set").getValue();
			DocumentXml::Node nodeDataType = columnsLess.getAttribute("type", false);
			string valueDataType = nodeDataType.isValid() ? nodeDataType.getValue() : "number"; // standard value is number

			if (valueDataType != "string")
				valueDataType = "number"; // if the value is not string, then assign number

			DocumentXml::Node nodePercent = columnsLess.getAttribute("percent", false);
			string valuePercent = nodePercent.isValid() ? nodePercent.getValue() : "100";			

			const DocumentPrintout::Filter* filterSet = this->resFilters.get(valueSet);

			if (filterSet == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsLess()", "Undefined variable '" << valueSet << "': " << columnsLess, "");

			DocumentPrintout::References references;

			filterSet->compareColumns(references, valueColLeft, valueColRight, DocumentPrintout::/*Operator::*/LESS, valueDataType, valuePercent);
			//DEBUG("%s","LEaving  RuleChecker::evaluateNodeColumnsLess(const DocumentXml::Node& columnsLess) ");
			// If original references in filter are equal to the references after the operation
			// then all columns fulfill the condition
			return filterSet->isEqual(references);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeColumnsLess()", "Invalid number of arguments. Required arguments: 'set', 'col-left', 'col-right', 'operation', ['type'], ['percent']: " << columnsLess, "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeColumnsLess(const DocumentXml::Node& columnsLess) ");
	}
	

	bool RuleChecker::evaluateNodeIsInRange(const DocumentXml::Node& range) const
	{
		//DEBUG("%s","Entering  RuleChecker::evaluateNodeIsInRange(const DocumentXml::Node& range) ");
		try
		{
			string valueCol = range.getAttribute("col").getValue();
			string valueSet = range.getAttribute("set").getValue();
			DocumentXml::Node nodeDataType = range.getAttribute("type", false);
			string valueDataType = nodeDataType.isValid() ? nodeDataType.getValue() : "number"; // standard value is number

			if (valueDataType != "string")
				valueDataType = "number"; // if the value is not string, then assign number

			string valueMin = range.getAttribute("min").getValue();
			string valueMax = range.getAttribute("max").getValue();

			const DocumentPrintout::Filter* filterSet = this->resFilters.get(valueSet);

			if (filterSet == 0)
				ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInRange()", "Undefined variable '" << valueSet << "': " << range, "");

			DocumentPrintout::References references;

			filterSet->range(references, valueCol, valueMin, valueMax, valueDataType);
			//DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsInRange(const DocumentXml::Node& range) ");
			// If original references in filter are equal to the references after the operation
			// then all columns fulfill the condition
			return filterSet->isEqual(references);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(RuleChecker::ExceptionRuleCheckSyntax, "evaluateNodeIsInRange()", "Invalid number of arguments. Required arguments: 'set', 'col', ['type'], 'min', 'max': " << range, "");
		}
		//DEBUG("%s","Leaving  RuleChecker::evaluateNodeIsInRange(const DocumentXml::Node& range) ");
	}

	//================================================================================
	// Class RuleChecker::Results<T>
	//================================================================================

	template <class T> RuleChecker::Results<T>::Results()
	{
		//DEBUG("%s","In RuleChecker::Results<T>::Results() ");
	}

	template <class T> RuleChecker::Results<T>::~Results()
	{
		//DEBUG("%s","Entering RuleChecker::Results<T>::~Results() ");
	        //map<string, T*>::iterator result ;
                typename map<string, T*>::iterator( result) ;

		for (result = this->results.begin(); result != this->results.end(); ++result)
			delete result->second;
		//DEBUG("%s","Leaving RuleChecker::Results<T>::~Results() ");
	}

	template <class T> T* RuleChecker::Results<T>::add(T* result)
	{
		//DEBUG("%s","Entering  T* RuleChecker::Results<T>::add(T* result) ");
		stringstream s;
		s << "RuleChecker::Results[" << this->results.size() << "]";
		//DEBUG("%s"," Leaving T* RuleChecker::Results<T>::add(T* result)");
		return this->add(s.str(), result);
	}

	template <class T> T* RuleChecker::Results<T>::add(const string& name, T* result)
	{
		//DEBUG("%s","Entering T* RuleChecker::Results<T>::add((const string& name, T* result) ");
                typename map<string, T*>::iterator it = this->results.find(name);

		if (it != this->results.end())
			delete it->second;

		this->results[name] = result;
		//DEBUG("%s","Leaving T* RuleChecker::Results<T>::add((const string& name, T* result) ");
		return result;
	}

	template <class T> T* RuleChecker::Results<T>::get(const string& name) const
	{
		//DEBUG("%s","Entering T* RuleChecker::Results<T>::get ");
                           
                typename map<string, T*>::const_iterator it = this->results.find(name); 
		if (it == this->results.end())
		{
			//DEBUG("%s","Leaving  T* RuleChecker::Results<T>::get ");
			return 0;
		}
		//DEBUG("%s","LEaving  T* RuleChecker::Results<T>::get ");
		return it->second;
	}
}
