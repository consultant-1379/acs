//======================================================================
//
// NAME
//      ReportToTxt.h
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
//      Implementation of a report from XMl to text format of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_ReportToTxt_h
#define AcsHcs_ReportToTxt_h

#include <sstream>
#include <string>
#include "acs_hcs_tra.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_formattertxt.h"

namespace AcsHcs
{
	/**
	* ReportToTxt - Convert the XML-formatted content of a report file to text.
	*/
	class ReportToTxt
	{
		friend std::ostream& operator<<(std::ostream& os, const ReportToTxt& converter);

	public:

		/**
		* ReportToTxt::ExceptionFileNotFound - Exception indicating that a file has not been found.
		* ReportToTxt::ExceptionFileNotFound is thrown in the case of a file has not been found.
		*/
		class ExceptionFileNotFound : public Exception
		{
		public:
			ExceptionFileNotFound( const std::string& where, const std::string& what, const std::string& cause ) : Exception( where, what, cause ) {}
			~ExceptionFileNotFound() throw(){}
		};

		/**
		* Constructor.
		* @param[in] reportId The ID of the report to be converted.
		*/
		ReportToTxt(const std::string& reportId);

		/**
		* Destructor.
		*/
		~ReportToTxt();

		/**
		* Serializes the Report.
		* @param[inout] ss    The stream to which to write the report data.
		* @param[in]    level
		* @return The stream as passed in 'ss'.
		*/
		std::stringstream& toString(std::stringstream& ss, unsigned int level = 0) const;

	private:
		static std::string toReportId(const std::string& reportFileName);

		static const FormatterTxt::Table::Column colsReportList[];
		static const FormatterTxt::Table::Column colsReportDetails[];
		static const FormatterTxt::Table::Column colsReportNoMatch[];

		ReportToTxt(const ReportToTxt&);
		ReportToTxt& operator=(const ReportToTxt&);

		void printFormatReportList(std::stringstream& ss) const;
		void printFormatReportDetails(std::stringstream& ss) const;
		void printFormatReportNoMatch(std::stringstream& ss, const DocumentXml::Node& report) const;

		const std::string fileName;
	};
}

#endif // AcsHcs_ReportToTxt_h
