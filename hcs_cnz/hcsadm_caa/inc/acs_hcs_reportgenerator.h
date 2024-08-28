//======================================================================
//
// NAME
//      ReportGenerator.h
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
//      Implementation of Report Generation of a HC Job for HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_ReportGenerator_h
#define AcsHcs_ReportGenerator_h

#include <fstream>
#include <string>

#include "acs_hcs_exception.h"
#include "acs_hcs_tra.h"

namespace AcsHcs
{
	/**
	* ReportGenerator - Generate the report file in XML format.
	*/
	class ReportGenerator
	{
	public:
		/**
		* ReportGenerator::ExceptionIo - IO Exception.
		* ReportGenerator::ExceptionIo is thrown in the case of IO problems.
		*/
		class ExceptionIo : public Exception
		{
		public:
			ExceptionIo(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionIo() throw() {}
		};

		/**
		* ReportGenerator::ExceptionXml - XML Exception.
		* ReportGenerator::ExceptionXml is thrown in the case of wrong sequence of writing XML.
		*/
		class ExceptionXml : public Exception
		{
		public:
			ExceptionXml(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionXml() throw(){}
		};

		/**
		* Constructor.
		* @param[in] fileName The name of the report file without suffix.
		*/
		ReportGenerator(const std::string& fileName, string time_Stamp);

		/**
		* Destructor.
		*/
		~ReportGenerator();

		/**
		* Opens the report.
		* @param[in] jobName  Name of the job.
		* @param[in] date     Start date for the job execution.
		* @throws ReportGenerator::ExceptionIo
		*/
		void open(const std::string& jobName, const std::string& date);

		/**
		* Closes the Report.
		* @throws ReportGenerator::ExceptionIo
		*/
		void close();

		/**
		* Opens a rule set part in the report.
		* @param[in] id Rule set identity.
		* @throws ReportGenerator::ExceptionIo
		*/
		void openRuleSet(const std::string& id);

		/**
		* Closes a rule set part in the report.
		* @throws ReportGenerator::ExceptionIo
		*/
		void closeRuleSet();

		/**
		* Opens a rule set part for non-matching rules/categories in the report.
		* @param[in] id Rule set identity.
		* @throws ReportGenerator::ExceptionIo
		*/
		void openRuleSetNoMatch(const std::string& id);

		/**
		* Closes a rule set part for non-matching rules/categories in the report.
		* @throws ReportGenerator::ExceptionIo
		*/
		void closeRuleSetNoMatch();

		/**
		* Add context attributes to the report.
		* @param[in] apzType  The type of APZ which the rule is executed for.
		* @param[in] nodeType The type of node which the rule is executed for.
		* @param[in] swLevel  The SW level which the rule is executed for.
		* @param[in] opMode   The operation mode which the rule is executed for.
		* @throws ReportGenerator::ExceptionIo
		*/
		void addAttributes(const std::string& apzType, const std::string& nodeType, const std::string& swLevel, const std::string& opMode);

		/**
		* Add a rule part to the report.
		* @param[in] id             Rule identity.
		* @param[in] name           Rule name.
		* @param[in] slogan         Rule slogan.
		* @param[in] result         Rule check result.
		* @param[in] recommendation Rule recommendation.
		* @throws ReportGenerator::ExceptionIo
		*/
		void addRule(const std::string& id, const std::string& name, const std::string& slogan, const std::string& result, const std::string& recommendation);

		/**
		* Add a rule-no-match part to the report.
		* @param[in] id Rule identity.
		* @throws ReportGenerator::ExceptionIo
		*/
		void addRuleNoMatch(const std::string& id);

		/**
		* Add a category-no-match part to the report.
		* @param[in] id Rule category.
		* @throws ReportGenerator::ExceptionIo
		*/
		void addCategoryNoMatch(const std::string& id);

		/**
		* Adds interruption from exception or from remove user command.
		* @param[in] reason Reason for interruption.
		* @throws ReportGenerator::ExceptionIo
		*/
		void addInterruption(const std::string& reason);

	private:
		struct State
		{
			State() : isReportOpen(false), isReportRuleSetOpen(false), isReportRuleSetNoMatchOpen(false)
			{
			}

			bool isReportOpen;
			bool isReportRuleSetOpen;
			bool isReportRuleSetNoMatchOpen;
		};

		ReportGenerator(const ReportGenerator&);
		ReportGenerator& operator=(const ReportGenerator&);

		State             state;
		const std::string fileName;
		ofstream          ofs;
	};
}

#endif // AcsHcs_ReportGenerator_h
