//======================================================================
//
// NAME
//      RuleChecker.h
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
//      Implementation of Rule Validation and filtering of HC JOb for HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_RuleChecker_h
#define AcsHcs_RuleChecker_h

#include <map>

#include "acs_hcs_exception.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_documentprintout.h"

namespace AcsHcs
{
	/**
	* RuleChecker - Performs the rule-checking.
	*/
	class RuleChecker
	{
	public:
		/**
		* RuleChecker::ExceptionRuleCheckSyntax - Exception while checking a Rule.
		* RuleChecker::ExceptionRuleCheckSyntax is thrown on syntax problems while checking a Rule.
		*/
		class ExceptionRuleCheckSyntax : public Exception
		{
		public:
			ExceptionRuleCheckSyntax(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionRuleCheckSyntax() throw(){}
		};

		/**
		* RuleChecker::ExceptionUnknownResult - Thrown when trying to fetch an unknown result.
		* RuleChecker::ExceptionUnknownResult is thrown when trying to fetch an unknown result.
		*/
		class ExceptionUnknownResult : public Exception
		{
		public:
			ExceptionUnknownResult(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionUnknownResult() throw(){}
		};

		/**
		* Default constructor.
		*/
		RuleChecker();

		/**
		* Constructor.
		* @param[out] cmdResultFiles The names of all the command result files generated during the rule execution.
		*/
		RuleChecker(Directory::FileNames& cmdResultFiles);

		/**
		* Destructor.
		*/
		~RuleChecker();

		/**
		* Gets the filtered result for the key passed.
		* @param[in] key The key for which the filtered result shall be returned.
		* @return The filtered result for the key passed.
		* @throws RuleChecker::ExceptionUnknownResult
		*/
		const DocumentPrintout::Filter& getResultFilter(const std::string& key) const;

		/**
		* Performs the check of a Rule as defined in parameter ruleCheck and returns the result.
		* @param[in] ruleCheck       The definition of the check.
		* @param[in] cpName          The name of the CP where to send possible MML commands (only valid for blade cluster).
		* @return true on success, otherwise false.
		* @throws RuleChecker::ExceptionRuleCheckSyntax
		*/
		bool isPassed(const DocumentXml::Node& ruleCheck, const std::string& cpName);

		/**
		* Sets the container for the command result files.
		* @param[in] cmdResultFiles Holds the names of all the command result files generated during the rule execution.
		*/
		void setCmdResultFiles(Directory::FileNames& cmdResultFiles);

	private:
		template <class T>
		class Results
		{
		public:
			Results();
			~Results();

			T* add(T* result);
			T* add(const std::string& name, T* result);
			T* get(const std::string& name) const;

		private:
			std::map<std::string, T*> results;
		};

		RuleChecker(const RuleChecker&);
		RuleChecker& operator=(const RuleChecker&);

		bool evaluateNodeCheck(const DocumentXml::Node& node, const std::string& cpName);
		bool evaluateNodeCondition(const DocumentXml::Node& node);
		bool evaluateNodeMml(const DocumentXml::Node& node, const std::string& cpName);
		bool evaluateNodeEval(const DocumentXml::Node& node);
		bool evaluateNodeAnd(const DocumentXml::Node& node);
                bool evaluateNodeOr(const DocumentXml::Node& node);
		bool evaluateNodeNot(const DocumentXml::Node& node);
		bool evaluateNodeIsEmpty(const DocumentXml::Node& node) const;
		bool evaluateNodeIsEqual(const DocumentXml::Node& node) const;
		bool evaluateNodeIsInPeriod(const DocumentXml::Node& node) const;
		bool evaluateNodeIsNotEmpty(const DocumentXml::Node& node) const;
		bool evaluateNodeIsNotEqual(const DocumentXml::Node& node) const;
		bool evaluateNodeIsSubset(const DocumentXml::Node& node) const;
		bool evaluateNodeIsTrue(const DocumentXml::Node& node) const;
		bool evaluateNodeColumnsEqual(const DocumentXml::Node& node) const;
		bool evaluateNodeColumnsGreater(const DocumentXml::Node& node) const;
		bool evaluateNodeColumnsLess(const DocumentXml::Node& node) const;
		bool evaluateNodeIsInRange(const DocumentXml::Node& node) const;

		Directory::FileNames* cmdResultFiles;
		Results<DocumentPrintout> resPrintouts;
		Results<DocumentPrintout::Filter> resFilters;
		Results<bool> resBools;
	};
}

#endif // AcsHcs_RuleChecker_h
