//======================================================================
//
// NAME
//      PrintoutParser.h
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
//      Implementation of Printout Parser for XML Handling.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDKBR
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_PrintoutParser_h
#define AcsHcs_PrintoutParser_h

#include <string>

#include "acs_hcs_exception.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_documentprintout.h"

namespace AcsHcs
{
	/**
	 * PrintoutParser is the interface for all printout parsers that will be used for different classes
	 * of printouts. The list of specializations will be continued during design. 
	 * Specializations are for instance
	 * - PrintoutParserMml which can parse a lot of general MML printouts
	 * - PrintoutParserAllip which can parse an ALLIP printout.
	 * - PrintoutParserApg which can parse APG printouts
	 */
	class PrintoutParser
	{
	public:
		/**
		* PrintoutParser::ExceptionParseError - Exception indicating an error when parsing a printout.
		* PrintoutParser::ExceptionParseError is thrown in the case of an error when parsing a printout.
		*/
		class ExceptionParseError : public Exception
		{
		public:
			ExceptionParseError(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionParseError() throw(){}
		};

		/**
		* PrintoutParser::ExceptionPrintoutError - Exception indicating error printout received.
		* PrintoutParser::ExceptionPrintoutError is thrown in the case of an error printout has been received from the external system.
		*/
		class ExceptionPrintoutError : public Exception
		{
		public:
			ExceptionPrintoutError(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionPrintoutError() throw(){}
		};

		/**
		* PrintoutParser::ExceptionPrintoutFailed - Exception indicating failed printout received.
		* PrintoutParser::ExceptionPrintoutFailed is thrown in the case of a failed printout has been received from the external system.
		*/
		class ExceptionPrintoutFailed : public Exception
		{
		public:
			ExceptionPrintoutFailed(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionPrintoutFailed() throw(){}
		};

		PrintoutParser();
		virtual ~PrintoutParser();

		/**
		 * parse is the common interface for all parsing specializations.
		 * The method throws an exeception in case an error string is found in the printout. The applicable error strings
		 * must be listed in the printout part of the rule set file.
		 * @param[out] documentPrintout is the internal representation of the printout after parsing
		 * @param[in]  printout is the XML node in the rule set file representing the description of the printout needed for parsing
		 * @param[in] cmdResult the file containing the printout
		 *
		 * 
		 */
		virtual void parse(DocumentPrintout& documentPrintout, const DocumentXml::Node& printout, const std::string& cmdResult ) = 0;
	};
}

#endif // AcsHcs_PrintoutParser_h
