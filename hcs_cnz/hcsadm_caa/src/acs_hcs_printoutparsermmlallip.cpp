//======================================================================
//
// NAME
//      PrintoutParserMmlAllip.cpp
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
//      2011-07-22 by EEDKBR
// CHANGES
//     
//======================================================================

#include "acs_hcs_documentprintout.h"
#include "acs_hcs_printoutparser.h"
#include "acs_hcs_printoutparsermmlallip.h"

using namespace std;

namespace AcsHcs
{
	void PrintoutParserMmlAllip::initParser()
	{
		//DEBUG("%s","Entering PrintoutParserMmlAllip::initParser() ");
		_firstCall = true;
		//DEBUG("%s","Leaving PrintoutParserMmlAllip::initParser() ");
	}

	bool PrintoutParserMmlAllip::isDataLine( string& line )
	{
		//DEBUG("%s","Entering PrintoutParserMmlAllip::isDataLine ");
		
		if ( line[2] != '/' )
		{
			DEBUG("%s"," Leaving PrintoutParserMmlAllip::isDataLine");	
			return false;
		}

		line[2] = ' ';

		if (_firstCall)
		{
			setCurrentHeader(line);
			_firstCall = false;

		}
		
		adjustLine( line );
		//DEBUG("%s","Leaving PrintoutParserMmlAllip::isDataLine ");
		return true;
	}

	void PrintoutParserMmlAllip::setCurrentHeader( string& line )
	{
		//DEBUG("%s"," Entering  PrintoutParserMmlAllip::setCurrentHeader");
		(void)line;

		string headerLine;
		std::list<size_t>::iterator toPos;

		_toPositions.push_back(0);
		_toPositions.push_back(10);
		_toPositions.push_back(40);
		_toPositions.push_back(60);
		_toPositions.push_back(80);
		_toPositions.push_back(100);

		_currentHeader = &(*_headers.begin());

		std::list<string> titles;
		_currentHeader->getTitles( titles );

		for ( toPos = _toPositions.begin(); toPos!=_toPositions.end(); toPos++ )
		{
			headerLine.append( *toPos-headerLine.size(),' ' );
			headerLine += titles.front();
			titles.pop_front();

		}
		checkHeader( headerLine );
		_initialHeaderFound = true;
		_documentFound = true;
		//DEBUG("%s","Leaving  PrintoutParserMmlAllip::setCurrentHeader ");
	}


	void PrintoutParserMmlAllip::adjustLine( string& line )
	{
		//DEBUG("%s","Entering PrintoutParserMmlAllip::adjustLine( ");
		string newLine;
		std::list<size_t>::iterator toPos;
		size_t fromPos = 0;
		size_t toCopy = 2;

		toPos = _toPositions.begin();

		
		newLine += line.substr( fromPos, toCopy );
		toPos++;
		fromPos = 3;

		for ( ; toPos!=_toPositions.end(); toPos++ )
		{
			newLine.append( *toPos-newLine.size(),' ' );
			fromPos = line.find_first_not_of( ' ', fromPos );
			toCopy = line.find_first_of(' ', fromPos );
			newLine += line.substr( fromPos, toCopy-fromPos );
			fromPos = toCopy+1;

		}
		line = newLine;
		//DEBUG("%s"," Leaving PrintoutParserMmlAllip::adjustLine(");
	}
}
