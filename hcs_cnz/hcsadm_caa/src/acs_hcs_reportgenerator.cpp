//======================================================================
//
// NAME
//      ReportGenerator.cpp
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
//      2011-06-16 by EEDSTL
// CHANGES
//     
//======================================================================

#include <fstream>
#include <sstream>
#include <string>

#include "ACS_APGCC_Util.H"
#include "acs_hcs_configuration.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_reportgenerator.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class ReportGenerator
	//================================================================================

	ReportGenerator::ReportGenerator(const string& fileName, string time_Stamp)
		: fileName(Configuration::Singleton::get()->getReportsTmpDirName(fileName) + "/" + fileName + "_" + time_Stamp +".xml")
	{
	}

	ReportGenerator::~ReportGenerator()
	{
	}

	void ReportGenerator::open(const string& jobName, const string& date)
	{
		DEBUG("%s","Entering ReportGenerator::open(const string& jobName, const string& date)");
 
		if (this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "open()", "Could not open <report> in report file '" << this->fileName << "'.", "<report> already opened.");

		this->ofs.open(this->fileName.c_str());

		if (!this->ofs.is_open())
			ACS_HCS_THROW(ReportGenerator::ExceptionIo, "open()", "Could not open report file '" << this->fileName << "'.", "");

		this->ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
		this->ofs << "<report>"<<endl;
		this->ofs << "<jobname>"   << jobName      << "</jobname>"<<endl;
		this->ofs << "<date>"      << date         << "</date>"<<endl;

		this->state.isReportOpen = true;
		DEBUG("%s","Leaving ReportGenerator::open(const string& jobName, const string& date) ");
	}

	void ReportGenerator::close()
	{
		DEBUG("%s","Entering ReportGenerator::close() ");
		if (!this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "close()", "Could not close <report> in report file '" << this->fileName << "'.", "<report> already closed.");

		try
		{
			this->closeRuleSet();
		}
		catch (const ReportGenerator::ExceptionXml&)
		{
			try
			{
				this->closeRuleSetNoMatch();
			}
			catch (const ReportGenerator::ExceptionXml&)
			{
				// Ignore
			}
		}

		this->ofs << "</report>";

		this->ofs.close();

		this->state.isReportOpen = false;
		DEBUG("%s","Leaving ReportGenerator::close()  ");
	}

	void ReportGenerator::openRuleSet(const string& id)
	{
		DEBUG("%s","Entering void ReportGenerator::openRuleSet(const string& id ");
		if (!this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "openRuleSet()", "Could not open <ruleset> in report file '" << this->fileName << "'.", "<report> not opened.");

		if (this->state.isReportRuleSetOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "openRuleSet()", "Could not open <ruleset> in report file '" << this->fileName << "'.", "<ruleset> already opened.");

		this->ofs << "<ruleset id=\"" << id << "\">"<<endl;

		this->state.isReportRuleSetOpen = true;
		DEBUG("%s","Leaving void ReportGenerator::openRuleSet(const string& id ");
	}

	void ReportGenerator::closeRuleSet()
	{
		DEBUG("%s","Entering  void ReportGenerator::closeRuleSet( ");
		if (!this->state.isReportRuleSetOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "closeRuleSet()", "Could not close <ruleset> in report file '" << this->fileName << "'.", "<ruleset> already closed.");

		this->ofs << "</ruleset>"<<endl;

		this->state.isReportRuleSetOpen = false;
		DEBUG("%s","Leaving  void ReportGenerator::closeRuleSet( ");
	}

	void ReportGenerator::addAttributes(const string& apzType, const string& nodeType, const string& swLevel, const string& opMode)
	{
		DEBUG("%s","Entering void ReportGenerator::addAttributes ");
		if (!this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "addAttributes()", "Could not add <attributes> to report file '" << this->fileName << "'.", "<report> not opened.");

		this->ofs << "<attributes>"<<endl;
		this->ofs << "<apz-type>"  << apzType  << "</apz-type>"<<endl;
		this->ofs << "<node-type>" << nodeType << "</node-type>"<<endl;
		this->ofs << "<sw-level>"  << swLevel  << "</sw-level>"<<endl;

		if (!opMode.empty())
			this->ofs << "<op-mode>" << opMode << "</op-mode>"<<endl;

		this->ofs << "</attributes>"<<endl;
		DEBUG("%s","Leaving void ReportGenerator::addAttributes ");
	}

	void ReportGenerator::addRule(const string& id, const string& name, const string& slogan, const string& result, const string& recommendation)
	{
		DEBUG("%s","Entering void ReportGenerator::addRule ");
		if (!this->state.isReportOpen || ! this->state.isReportRuleSetOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "addRule()", "Could not add <rule> to report file '" << this->fileName << "'.", "<report> or <ruleset> not opened.");

		this->ofs << "<rule id=\"" << id.substr(0, id.find_first_of(".")) << "\" name=\"" << name << "\">"<<endl;
		this->ofs << "<slogan>"          << slogan          << "</slogan>"<<endl;
		this->ofs << "<result>"          << result          << "</result>"<<endl;
		this->ofs << "<recommendation>"  << recommendation  << "</recommendation>"<<endl;
		this->ofs << "</rule>"<<endl;
		DEBUG("%s","Leaving void ReportGenerator::addRule ");
	}

	void ReportGenerator::openRuleSetNoMatch(const string& id)
	{
		DEBUG("%s","Entering void ReportGenerator::openRuleSetNoMatch(const string& id) ");
		if (!this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "openRuleSetNoMatch()", "Could not open <ruleset-no-match> in report file '" << this->fileName << "'.", "<report> not opened.");

		if (this->state.isReportRuleSetNoMatchOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "openRuleSetNoMatch()", "Could not open <ruleset-no-match> in report file '" << this->fileName << "'.", "<ruleset-no-match> already opened.");

		this->ofs << "<ruleset-no-match id=\"" << id << "\">";

		this->state.isReportRuleSetNoMatchOpen = true;
		DEBUG("%s","Leaving void ReportGenerator::openRuleSetNoMatch(const string& id) ");
	}

	void ReportGenerator::closeRuleSetNoMatch()
	{	
		DEBUG("%s","Entering void ReportGenerator::closeRuleSetNoMatch() ");
		if (!this->state.isReportRuleSetNoMatchOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "closeRuleSetNoMatch()", "Could not close <ruleset-no-match> in report file '" << this->fileName << "'.", "<ruleset-no-match> already closed.");

		this->ofs << "</ruleset-no-match>";

		this->state.isReportRuleSetNoMatchOpen = false;
		DEBUG("%s","Leaving void ReportGenerator::closeRuleSetNoMatch() ");
	}

	void ReportGenerator::addRuleNoMatch(const string& id)
	{
		DEBUG("%s","entering void ReportGenerator::addRuleNoMatch(const string& id) ");
		if (!this->state.isReportOpen || ! this->state.isReportRuleSetNoMatchOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "addRuleNoMatch()", "Could not add <rule-no-match> to report file '" << this->fileName << "'.", "<report> or <ruleset> not opened.");

		this->ofs << "<rule-no-match id=\"" << id.substr(0, id.find_first_of(".")) << "\"/>";
		DEBUG("%s","Leaving void ReportGenerator::addRuleNoMatch(const string& id ");
	}

	void ReportGenerator::addCategoryNoMatch(const string& id)
	{
		DEBUG("%s","Entering void ReportGenerator::addCategoryNoMatch(const string& id) ");
		if (!this->state.isReportOpen || ! this->state.isReportRuleSetNoMatchOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "addCategoryNoMatch()", "Could not add <category-no-match> to report file '" << this->fileName << "'.", "<report> or <ruleset> not opened.");

		this->ofs << "<category-no-match id=\"" << id.substr(0, id.find_first_of(".")) << "\"/>";
		DEBUG("%s","Leaving void ReportGenerator::addCategoryNoMatch(const string& id) ");
	}

	void ReportGenerator::addInterruption(const string& reason)
	{
		DEBUG("%s","Entering void ReportGenerator::addInterruption(const string& reason) ");
		if (!this->state.isReportOpen)
			ACS_HCS_THROW(ReportGenerator::ExceptionXml, "addInterruption()", "Could not add <interruption> to report file '" << this->fileName << "'.", "<report> not open.");

		this->ofs << "<interruption>" << reason << "</interruption>";
		DEBUG("%s","Leaving void ReportGenerator::addInterruption(const string& reason) ");
	}
}
