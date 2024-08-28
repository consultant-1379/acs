//======================================================================
//
// NAME
//      RuleSetToTxt.cpp
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
//      2011-07-26 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_formattertxt.h"
#include "acs_hcs_rulesettotxt.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const RuleSetToTxt& converter)
	{
		stringstream ss;

		return os << converter.toString(ss, 0).str();
	}

	//================================================================================
	// Class RuleSetToTxt
	//================================================================================

	const FormatterTxt::Table::Column RuleSetToTxt::colsRuleSet[] = 
	{
		FormatterTxt::Table::Column("No."     ,  4, FormatterTxt::Table::Column::/*Align::*/RIGHT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule Set", 40, FormatterTxt::Table::Column::/*Align::*/LEFT , "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column RuleSetToTxt::colsRuleSetRuleSlogan[] = 
	{
		FormatterTxt::Table::Column("Rule Set", 25, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule"    ,  4, FormatterTxt::Table::Column::/*Align::*/RIGHT, " "                     , FormatterTxt::Table::S_R),
		FormatterTxt::Table::Column(""        , 16, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Slogan"  , 43, FormatterTxt::Table::Column::/*Align::*/LEFT , "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column RuleSetToTxt::colsRuleSetRuleSloganCategory[] = 
	{
		FormatterTxt::Table::Column("Rule Set", 25, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule"    ,  4, FormatterTxt::Table::Column::/*Align::*/RIGHT, " "                     , FormatterTxt::Table::S_R),
		FormatterTxt::Table::Column(""        , 16, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Slogan"  , 25, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Category", 15, FormatterTxt::Table::Column::/*Align::*/LEFT , "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	RuleSetToTxt::RuleSetToTxt(const string& ruleSetId, bool showDetails)
		: fileName(ruleSetId.empty() ? "" : Configuration::Singleton::get()->getRulesDirName() + "/" + ruleSetId + ".xml")
		, showDetails(showDetails)
	{
		DEBUG("%s","In Constructor RuleSetToTxt ");
	}

	RuleSetToTxt::~RuleSetToTxt()
	{
		DEBUG("%s","In destructor RuleSetToTxt ");
	}

	void RuleSetToTxt::printFormatRuleSetList(stringstream& ss) const
	{
		DEBUG("%s","entering  RuleSetToTxt::printFormatRuleSetList( ");
		Directory dirRules(Configuration::Singleton::get()->getRulesDirName());
		Directory::FileNames ruleSets;
		dirRules.ls(ruleSets);

		if (ruleSets.size() == 0)
		{
			ss << "No rule sets found." << endl;
			DEBUG("%s","return after no rulsets found in  RuleSetToTxt::printFormatRuleSetList( ");
			return;
		}

		FormatterTxt::Table table(RuleSetToTxt::colsRuleSet);

		table.printHeader(ss);

		int i = 0;

		for (Directory::FileNames::const_iterator ruleSet = ruleSets.begin(); ruleSet != ruleSets.end(); ++ruleSet)
		{
			vector<string> colTexts;
			char buf[16];

			colTexts.push_back(RuleSetToTxt::toRuleSetId(ACE_OS::itoa(++i, buf, 10)));
			colTexts.push_back(RuleSetToTxt::toRuleSetId(*ruleSet));
			table.printLine(ss, colTexts);
		}
		DEBUG("%s","leaving  RuleSetToTxt::printFormatRuleSetList( ");
	}

	void RuleSetToTxt::printFormatRuleSetDetails(stringstream& ss, const FormatterTxt::Table::Column cols[]) const
	{
		DEBUG("%s","Entering RuleSetToTxt::printFormatRuleSetDetails ");
		Directory::FileNames ruleSetFiles;

		if (this->fileName.empty())
		{
			Directory dirRules(Configuration::Singleton::get()->getRulesDirName());
			dirRules.ls(ruleSetFiles);
		
			if (ruleSetFiles.size() == 0)
			{
				ss << "No rule sets found." << endl;	
				DEBUG("%s","return after no rulesets found in RuleSetToTxt::printFormatRuleSetDetails ");
				return;
			}
		}
		else
		{
			Directory dir(Configuration::Singleton::get()->getRulesDirName());

			if (!dir.exists(this->fileName))
			{
				ACS_HCS_THROW( RuleSetToTxt::ExceptionFileNotFound, "printFormatRuleSetDetails()", "Rule set '" << RuleSetToTxt::toRuleSetId(this->fileName) << "' does not exist.", "File not found.");
				 DEBUG("%s","return after File not found-ruleset doesnot exist.");
				return;
			}
			
			ruleSetFiles.push_back(this->fileName);
		}

		FormatterTxt::Table table(cols);
		unsigned int numCols = table.printHeader(ss);

		for (Directory::FileNames::const_iterator ruleSetFile = ruleSetFiles.begin(); ruleSetFile != ruleSetFiles.end(); ++ruleSetFile)
		{
			DocumentXml docRuleSet(*ruleSetFile);
			DocumentXml::Node ruleSet = docRuleSet.getNode("/ruleset");
			DocumentXml::Nodes rules;
			ruleSet.getNodes(rules, "./rule");

			set<string> rulesHandled;

			for (DocumentXml::Nodes::Iterator rule = rules.begin(); rule != rules.end(); ++ rule)
			{
				string ruleId = (*rule).getAttribute("id").getValue();
				ruleId = ruleId.substr(0, ruleId.find_first_of("."));

				if (rulesHandled.find(ruleId) != rulesHandled.end())
					continue;

				rulesHandled.insert(ruleId);

				vector<string> colTexts;

				colTexts.push_back(ruleSet.getAttribute("id").getValue());
				colTexts.push_back(ruleId);
				colTexts.push_back((*rule).getAttribute("name").getValue());
				colTexts.push_back((*rule).getNode("./slogan").getValue());

				if (numCols > 4)
				{
					DocumentXml::Nodes cats;
					(*rule).getNodes(cats, "./category");

					stringstream s;

					for (DocumentXml::Nodes::Iterator cat = cats.begin(); cat != cats.end(); ++cat)
					{
						if (cat == cats.begin())
							s << (*cat).getValue();
						else
							s << ", " << (*cat).getValue();
					}

					colTexts.push_back(s.str());
				}

				table.printLine(ss, colTexts);
			}
		}
		DEBUG("%s","Leaving RuleSetToTxt::printFormatRuleSetDetails ");
	}

	string RuleSetToTxt::toRuleSetId(const std::string& ruleSetFileName)
	{
		DEBUG("%s","Entering  RuleSetToTxt::toRuleSetId ");
		size_t posStart = ruleSetFileName.find_last_of("/\\");

		if (posStart == string::npos)
			posStart = 0;
		else
			posStart++;

		size_t posEnd = ruleSetFileName.find_last_of(".");
		DEBUG("%s","Leaving  RuleSetToTxt::toRuleSetId ");
		return ruleSetFileName.substr(posStart, posEnd - posStart);
	}

	stringstream& RuleSetToTxt::toString(stringstream& ss, unsigned int level) const
	{
		DEBUG("%s","Entering RuleSetToTxt::toString( ");
		if (this->showDetails)
			this->printFormatRuleSetDetails(ss, RuleSetToTxt::colsRuleSetRuleSloganCategory);
		else if (this->fileName.empty())
			this->printFormatRuleSetList(ss);
		else
			this->printFormatRuleSetDetails(ss, RuleSetToTxt::colsRuleSetRuleSlogan);
		DEBUG("%s","Leaving RuleSetToTxt::toString( ");
		return ss;
	}
}
