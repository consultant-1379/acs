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

#ifndef AcsHcs_PrintoutParserMmlAllip_h
#define AcsHcs_PrintoutParserMmlAllip_h

#include "acs_hcs_printoutparsermml.h"

namespace AcsHcs
{
    /**
	* PrintoutParserMmlAllip is a specialization of PrintoutParserMml.
	* This parser is meant for parsing of command ALLIP.
	* As there is no header in this output file, the header is inserted
	* artificially by the parser. The data lines that are applicable from
	* ALLIP are the ones that start with CLASS/CATEGORY. All other lines
	* are ignored. These data lines are prepared in a way that the usual
	* PrintoutParserMml methods can parse them.
	*/
	class PrintoutParserMmlAllip : public PrintoutParserMml
	{

	public:

	private:
		virtual bool isDataLine( std::string& line );
		bool _firstCall;
		std::list<size_t> _fromPositions;
		std::list<size_t> _toPositions;
		void initParser();
		void setCurrentHeader( std::string& line );
		void adjustLine( std::string& line );


	};
}
#endif
