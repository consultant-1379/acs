//======================================================================
//
// NAME
//      ReportToTxt.cpp
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
//      2011-07-22 by EEDSTL
// CHANGES
//     
//======================================================================

#include <iostream>
#include <sstream>
#include <string>

#include <ace/ACE.h>

#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_formattertxt.h"
#include "acs_hcs_reporttotxt.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const ReportToTxt& converter)
	{
		stringstream ss;

		return os << converter.toString(ss, 0).str();
	}

	//================================================================================
	// Class ReportToTxt
	//================================================================================

	const FormatterTxt::Table::Column ReportToTxt::colsReportList[] = 
	{
		FormatterTxt::Table::Column("Report Name", 40, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("PASSED"     ,  6, FormatterTxt::Table::Column::/*Align::*/RIGHT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("FAILED"     ,  6, FormatterTxt::Table::Column::/*Align::*/RIGHT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("ERROR"      ,  6, FormatterTxt::Table::Column::/*Align::*/RIGHT, "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column ReportToTxt::colsReportDetails[] = 
	{
		FormatterTxt::Table::Column("Rule Set"             , 25, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule"                 ,  4, FormatterTxt::Table::Column::/*Align::*/RIGHT, " "                     , FormatterTxt::Table::S_R),
		FormatterTxt::Table::Column(""                     , 16, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Result"               , 12, FormatterTxt::Table::Column::/*Align::*/LEFT , FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Reason/Recommendation", 23, FormatterTxt::Table::Column::/*Align::*/LEFT , "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	const FormatterTxt::Table::Column ReportToTxt::colsReportNoMatch[] = 
	{
		FormatterTxt::Table::Column("Rule Set"  , 25, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Rule IDs"  , 21, FormatterTxt::Table::Column::/*Align::*/LEFT, FormatterTxt::Table::S_C, FormatterTxt::Table::S_CR),
		FormatterTxt::Table::Column("Categories", 38, FormatterTxt::Table::Column::/*Align::*/LEFT, "\n"                    , "\n"),
		FormatterTxt::Table::Column()
	};

	ReportToTxt::ReportToTxt(const string& reportId)
		: fileName(reportId.empty() ? "" : Configuration::Singleton::get()->getReportsDirName() + "/" + reportId + "/" + reportId + ".xml")
	{
	}

	ReportToTxt::~ReportToTxt()
	{
	}

	void ReportToTxt::printFormatReportList(stringstream& ss) const
	{
		DEBUG("%s","entering  ReportToTxt::printFormatReportList( ");
		Directory dirReports(Configuration::Singleton::get()->getReportsDirName());
		Directory::FileNames reports;
		dirReports.ls(reports, "*.xml", true, true);

		if (reports.size() == 0)
		{
			ss << "No reports found." << endl;
			DEBUG("%s","Leaving after return no reports found in  ReportToTxt::printFormatReportList ");
			return;
		}
	
		FormatterTxt::Table table(ReportToTxt::colsReportList);

		table.printHeader(ss);

		for (Directory::FileNames::const_iterator report = reports.begin(); report != reports.end(); ++report)
		{
			vector<string> colTexts;
			
			DocumentXml docReport(*report);
			DocumentXml::Nodes rules;
			docReport.getNodes(rules, "/report/ruleset/rule");
			
			int numRulesPassed = 0;
			int numRulesFailed = 0;
			int numRulesError  = 0;

			for (DocumentXml::Nodes::Iterator rule = rules.begin(); rule != rules.end(); ++rule)
			{
				const string& ruleResult = (*rule).getNode("./result").getValue();

				if (ruleResult == "PASSED")
					numRulesPassed++;
				else if (ruleResult == "FAILED")
					numRulesFailed++;
				else if (ruleResult == "ERROR")
					numRulesError++;
			}

			char buf[16];
			colTexts.push_back(ReportToTxt::toReportId(*report));
			colTexts.push_back(ACE_OS::itoa(numRulesPassed, buf, 10));
			colTexts.push_back(ACE_OS::itoa(numRulesFailed, buf, 10));
			colTexts.push_back(ACE_OS::itoa(numRulesError , buf, 10));

			table.printLine(ss, colTexts);
		}
		DEBUG("%s","Leaving  ReportToTxt::printFormatReportList( ");
	}

	void ReportToTxt::printFormatReportDetails(stringstream& ss) const
	{
		DEBUG("%s","Entering ReportToTxt::printFormatReportDetails( ");
		Directory dir(Configuration::Singleton::get()->getReportsDirName());

		if (!dir.exists(this->fileName))
		{
			ACS_HCS_THROW( ReportToTxt::ExceptionFileNotFound, "printFormatReportDetails()", "Report '" << ReportToTxt::toReportId(this->fileName) << "' does not exist.", "File not found.");
			return;
		}

		DocumentXml docReport(this->fileName);
		const DocumentXml::Node& report = docReport.getNode("/report");
		ss << "Health-Check Report for Job '" << report.getNode("./jobname").getValue() << "'" << endl;
		ss << endl;
		ss << endl;
		ss << "1 General Information" << endl;
		ss << endl;
		ss << "Execution date and time: " << report.getNode("./date").getValue() << endl;
		ss << endl;
		ss << "The job has been executed on the following configuration:" << endl;
		const DocumentXml::Node& attributes = report.getNode("./attributes");
		ss << "HW: " << attributes.getNode("./apz-type").getValue() << " " << attributes.getNode("./node-type").getValue() << endl;
		ss << "SW: " << attributes.getNode("./sw-level").getValue() << endl;
		ss << endl;
		ss << "Additional information for the rules that have a result other than PASSED" << endl;
		ss << "can be found next to this report file in the report directory of the job." << endl;
		ss << endl;
		ss << endl;
		ss << "2 Check-Results" << endl;
		ss << endl;

		FormatterTxt::Table table(ReportToTxt::colsReportDetails);

		table.printHeader(ss);

		DocumentXml::Nodes ruleSets;
		report.getNodes(ruleSets, "./ruleset");
		DocumentXml::Nodes::Iterator ruleSet;

		for (ruleSet = ruleSets.begin(); ruleSet != ruleSets.end(); ++ ruleSet)
		{
			DocumentXml::Nodes rules;
			(*ruleSet).getNodes(rules, "./rule");
			DocumentXml::Nodes::Iterator rule;

			for (rule = rules.begin(); rule != rules.end(); ++ rule)
			{
				vector<string> colTexts;

				colTexts.push_back((*ruleSet).getAttribute("id").getValue());
				colTexts.push_back((*rule).getAttribute("id").getValue());
				colTexts.push_back((*rule).getAttribute("name").getValue());
				colTexts.push_back((*rule).getNode("./result").getValue());
				colTexts.push_back((*rule).getNode("./recommendation").getValue());

				table.printLine(ss, colTexts);
			}
		}

		this->printFormatReportNoMatch(ss, report);

		try
		{
			const DocumentXml::Node& interruption = report.getNode(".//interruption");

			ss << endl;
			ss << "The job-execution has been interrupted for the reason below:" << endl;
			ss << interruption.getValue() << endl;
		}
		catch (const DocumentXml::ExceptionNodeUndefined&)
		{
		}
		DEBUG("%s","Leaving ReportToTxt::printFormatReportDetails( ");
	}

	void ReportToTxt::printFormatReportNoMatch(stringstream& ss, const DocumentXml::Node& report) const
	{
		DEBUG("%s","Entering ReportToTxt::printFormatReportNoMatch( ");
		ss << endl;
		ss << "Rule IDs and categories for which no matching rule has been found:" << endl;
		ss << endl;

		FormatterTxt::Table table(ReportToTxt::colsReportNoMatch);

		bool found = false;
		DocumentXml::Nodes ruleSets;
		report.getNodes(ruleSets, "./ruleset-no-match");
		DocumentXml::Nodes::Iterator ruleSet;

		for (ruleSet = ruleSets.begin(); ruleSet != ruleSets.end(); ++ ruleSet)
		{
			{
				DocumentXml::Nodes rules;
				(*ruleSet).getNodes(rules, "./rule-no-match");
				DocumentXml::Nodes::Iterator rule;

				stringstream s;

				for (rule = rules.begin(); rule != rules.end(); ++rule)
				{
					if (rule != rules.begin())
						s << ", ";

					s << (*rule).getAttribute("id").getValue();
				}

				string ids = s.str();

				if (!ids.empty())
				{
					if (!found)
					{
						table.printHeader(ss);
						found = true;
					}

					vector<string> colTexts;

					colTexts.push_back((*ruleSet).getAttribute("id").getValue());
					colTexts.push_back(ids);
					colTexts.push_back("");

					table.printLine(ss, colTexts);
				}
			}
			{
				DocumentXml::Nodes cats;
				(*ruleSet).getNodes(cats, "./category-no-match");
				DocumentXml::Nodes::Iterator cat;

				stringstream s;

				for (cat = cats.begin(); cat != cats.end(); ++cat)
				{
					if (cat != cats.begin())
						s << ", ";

					s << (*cat).getAttribute("id").getValue();
				}

				string ids = s.str();

				if (!ids.empty())
				{
					if (!found)
					{
						table.printHeader(ss);
						found = true;
					}

					vector<string> colTexts;

					colTexts.push_back((*ruleSet).getAttribute("id").getValue());
					colTexts.push_back("");
					colTexts.push_back(ids);

					table.printLine(ss, colTexts);
				}
			}
		}

		if (!found)
			ss << "None." << endl;
		DEBUG("%s","Leaving ReportToTxt::printFormatReportNoMatch( ");
	}

	stringstream& ReportToTxt::toString(stringstream& ss, unsigned int level) const
	{
		DEBUG("%s","Entering stringstream& ReportToTxt::toString( ");
		if (this->fileName.empty())
			this->printFormatReportList(ss);
		else
			this->printFormatReportDetails(ss);
		DEBUG("%s","Leaving stringstream& ReportToTxt::toString( ");
		return ss;
	}

	string ReportToTxt::toReportId(const std::string& reportFileName)
	{
		DEBUG("%s","Entering  string ReportToTxt::toReportId( ");
		size_t posStart = reportFileName.find_last_of("/\\");

		if (posStart == string::npos)
			posStart = 0;
		else
			posStart++;

		size_t posEnd = reportFileName.find_last_of(".");
		DEBUG("%s","Leaving  string ReportToTxt::toReportId( ");
		return reportFileName.substr(posStart, posEnd - posStart);
	}
}
