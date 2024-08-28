//======================================================================
//
// NAME
//      RuleSetToTxt.h
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
//      Implementation of Rules from xml to text conversion.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_RuleSetToTxt_h
#define AcsHcs_RuleSetToTxt_h

#include <sstream>
#include <string>

#include "acs_hcs_documentxml.h"
#include "acs_hcs_exception.h"
#include "acs_hcs_formattertxt.h"

namespace AcsHcs
{
	/**
	* RuleSetToTxt - Convert the XML-formatted content of a rule set file to text.
	*/
	class RuleSetToTxt
	{
		friend std::ostream& operator<<(std::ostream& os, const RuleSetToTxt& converter);

	public:

		/**
		* RuleSetToTxt::ExceptionFileNotFound - Exception indicating that a file has not been found.
		* RuleSetToTxt::ExceptionFileNotFound is thrown in the case of a file has not been found.
		*/
		class ExceptionFileNotFound : public Exception
		{
		public:
			ExceptionFileNotFound( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
			~ExceptionFileNotFound() throw(){}
		};

		/**
		* Constructor.
		* @param[in] ruleSetId   The ID of the rule set file to be converted.
		* @param[in] showDetails Indicates whether or not details shall be shown.
		*/
		RuleSetToTxt(const std::string& ruleSetId = "", bool showDetails = false);

		/**
		* Destructor.
		*/
		~RuleSetToTxt();

		/**
		* Serializes the RuleSet.
		* @param[inout] ss    The stream to which to write the rule set data.
		* @param[in]    level
		* @return The stream as passed in 'ss'.
		*/
		std::stringstream& toString(std::stringstream& ss, unsigned int level = 0) const;

	private:
		static std::string toRuleSetId(const std::string& ruleSetFileName);

		static const FormatterTxt::Table::Column  colsRuleSet[];
		static const FormatterTxt::Table::Column  colsRuleSetRuleSlogan[];
		static const FormatterTxt::Table::Column  colsRuleSetRuleSloganCategory[];

		RuleSetToTxt(const RuleSetToTxt&);
		RuleSetToTxt& operator=(const RuleSetToTxt&);

		void printFormatRuleSetList(std::stringstream& ss) const;
		void printFormatRuleSetDetails(std::stringstream& ss, const FormatterTxt::Table::Column cols[]) const;

		const std::string fileName;
		const bool        showDetails;
	};
}

#endif // AcsHcs_RuleSetToTxt_h
