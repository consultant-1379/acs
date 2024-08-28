//======================================================================
//
// NAME
//      PrintoutParserMml.cpp
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
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-05-18 by EEDKBR
// CHANGES
//     
//======================================================================


#include <fstream>
#include <iostream>

#include "acs_hcs_documentprintout.h"
#include "acs_hcs_printoutparser.h"
#include "acs_hcs_printoutparsermml.h"
#include "acs_hcs_jobscheduler.h"

using namespace std;
using namespace AcsHcs;

/*
 * parse the printout according to the advices in the rule set file and store the data in an object of class DocumentPrintout
 * param[out] documentPrintout the memory representation of the printout file
 * param[in]  printout		    the printout section in the rule set file
 * param[in]  cmdResult        the printout file, i.e. the result given from the mml command
 */

const char* PrintoutParserMml::_tableStrings[] = 
{ 
	"./begin", 
	"./end/failed", 
	"./ignore", 
	"./end/error", 
	"./end/normal" 
};
//SUMANA changing
void PrintoutParserMml::parse( DocumentPrintout& documentPrintout,const DocumentXml::Node& printout, const string& cmdResult )
{
	//DEBUG("%s","Entering  PrintoutParserMml::parse( ");

    // read from Rule Set File and store internally:
	// - begin of the printoutFile
	// - end of the printoutFile
	// - ignore lines that stop the evaluation of the current header
	// - error lines that stop the evaluation of the complete printout



	initTableTags( _tableTags[TTBEGINS],		printout, _tableStrings[TTBEGINS],		false, 1);
	initTableTags( _tableTags[TTENDNORMALS],	printout, _tableStrings[TTENDNORMALS],	false, -1);
	initTableTags( _tableTags[TTIGNORES],		printout, _tableStrings[TTIGNORES],		false, -1);
	initTableTags( _tableTags[TTERRORS],		printout, _tableStrings[TTERRORS],		false, -1);
	initTableTags( _tableTags[TTFAILEDS],		printout, _tableStrings[TTFAILEDS],		true, -1);

	_documentFound = false;

	// initialize the parser 
	// this is a virtual function which is implemented by the printoutParser specializations
	initParser();
	// read from the rule set and build the complete internal structure for headers and columns of the printout
	analyseHeader( printout );
	
	// parse the printout file and store the result in object documentPrintout
	parsePrintout(  documentPrintout,cmdResult);
	if ( !_documentFound )
	{
		cout << "No corresponding printout in file: " << endl;
		ACS_HCS_THROW(PrintoutParser::ExceptionParseError, "parse()", "No corresponding printout in file: '" << cmdResult << "' ", "");
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::parse( ");
}

void PrintoutParserMml::initTableTags( std::list<TableTag>& tableTag, const DocumentXml::Node& printout, const char* tagString, bool inTextDefault, int maxLinesDefault )
{
	//DEBUG("%s","Entering PrintoutParserMml::initTableTags ");
	DocumentXml::Nodes tagItems;
	//printout.getNodes(tagItems, tagString );
	printout.getNodesList(tagItems,tagString,printout);
	setTableTags( tableTag, tagItems, inTextDefault, maxLinesDefault );
	//DEBUG("%s","Leaving PrintoutParserMml::initTableTags ");
}

void PrintoutParserMml::setTableTags( std::list<TableTag>& tableTagList, DocumentXml::Nodes& endNode, bool inTextDefault, int maxLinesDefault )
{
	//DEBUG("%s","Entering PrintoutParserMml::setTableTags( ");
	for ( DocumentXml::Nodes::Iterator end = endNode.begin(); end != endNode.end(); ++end )
	{
		TableTag tableTag; 

		std::string itemText = (*end).getValue();
		tableTag.setItemText(itemText);
		DocumentXml::Node maxLines = (*end).getAttribute( "maxlines", false );
		(tableTag).setMaxLines( maxLines.isValid() ? atoi( maxLines.getValue().c_str()) : maxLinesDefault );

		DocumentXml::Node inText = (*end).getAttribute( "intext", false );

		tableTag.setInText( inText.isValid() ? (inText.getValue().compare( "true" ) == 0) : inTextDefault );

		tableTagList.push_back( tableTag );
	}
	//DEBUG("%s","Leaving PrintoutParserMml::setTableTags( ");
}

// this method is empty for PrintoutParserMml, it is used for PrintoutParserMmlAllip
void PrintoutParserMml::initParser()
{
	//DEBUG("%s","In PrintoutParserMml::initParser() ");
}

// this is the main function for parsing the printout
// cmdResult is the file containing the printout
// documentPrintout is the object that will contain the data extracted data from the printout
void PrintoutParserMml::parsePrintout( DocumentPrintout& documentPrintout, const string& cmdResult )
{	
	//DEBUG("%s","Entering  PrintoutParserMml::parsePrintout ");
	// some initializations
	_currentHeader = 0;
	_currentStartHeader = &(*_headers.begin());
	_currentEndHeader = &(*_headers.begin());

	// open the printoutFile for reading
	filebuf fb;
	//****************************
	//const string cmdResult1 = "/data/opt/ap/internal_root/health_check/toTest1.txt";
    	fb.open (cmdResult.c_str(),ios::in);
	//fb.open (cmdResult1.c_str(),ios::in);
	
    	istream is(&fb);
	string line;					// line to be investigated
	_fileEnd = false;			
	int maxLines = -1;   			// defines the maximum data lines that belong to a header in the table
	_initialHeaderFound = false;		// set when the first header inside a printout is found
	_ongoing = false;			// set when a begin line is found, cleared when an end line is found

	// read the printout line-by-line
	while ( readNextLine( line, is ) )
	{
		if ( isTagHandled( _tableTags[TTERRORS], line, is ) )
		{	
			fb.close();
			ACS_HCS_THROW(PrintoutParser::ExceptionPrintoutError, "parsePrintout()", _errorText, "");
		};

		if ( isTagHandled( _tableTags[TTFAILEDS], line, is ) )
		{
			fb.close();
			ACS_HCS_THROW(PrintoutParser::ExceptionPrintoutFailed, "parsePrintout()", "", "");
		};  
		
		
		if (skipLine( line, is ))
		{	
			if ( !_ongoing && _initialHeaderFound )	// printout has ended and header found
			{			
				addSectorToPrintout( documentPrintout ); 
				// clean everything after printout finished
				for ( vector<Header>::iterator it = _headers.begin();  it != _headers.end(); it++ )
					it->cleanDataLines();
				_currentHeader = NULL;
				documentPrintout.closePrintout(); // close the printout
				_initialHeaderFound = false;
			}
		}
		else
		{
			Header* header = checkHeader( line ); // check whether line is a header line
			if ( header != 0 ) // header line
			{
				_initialHeaderFound = true;
				processHeader( documentPrintout, header );	// initialize the header
				maxLines = header->getMaxLines();	// store the maximum data lines that the header accepts				
			}
			else // no header, candidate for a data line
			{
				if ( addPotentialHeaderLine( documentPrintout, line ))
				{
					maxLines = _currentHeader->getMaxLines();
				}
				// if maxlines for the header is set, then only maxlines are valid, further
				// data lines are ignored.
				// isDataLine decides, whether the line is a data line. Note that this decision
				// has a different implementation for PrintoutParserMmlAllip
				// if no header was found by now, the line is also ignored

				if ( (maxLines!=0) && isDataLine( line ) && _initialHeaderFound )
				{
					maxLines--;
					// a section is written in case the header already has a data line and
					// shall get a new one. In that case the new line is attached to the header
					if ( _currentHeader->hasDataLine() )
					{	
						
						// if leading columns are missing in the line, try to add them to the line
						// in case the new line does not add any information to the previous one,
						// e.g. all further columns are not required, then the line is skipped
						if ( _currentHeader->isNewOrPaddedLine( line ) && _currentHeader->hasRequiredColumn() )
						{
							addSectorToPrintout( documentPrintout ); // flash the current object
							_currentHeader->setDataLine( line );     // assign the line to the header
						}
					}
					else
					{
						_currentHeader->setDataLine( line );		 // assign the line to the header
					}
				}
			}
		}
		
	}
	
	fb.close();
	//DEBUG("%s","Leaving  PrintoutParserMml::parsePrintout ");

}

bool PrintoutParserMml::addPotentialHeaderLine( DocumentPrintout& documentPrintout, string& line )
{
	//DEBUG("%s","Entering  PrintoutParserMml::parsePrintout ");
	if ( !_initialHeaderFound )
	{
		//DEBUG("%s"," ");
		return false;
	}
	int firstCharInLine = line.find_first_not_of( " " );
	if ( firstCharInLine < _currentHeader->getFirstChar())
	{
		// find the potential header
		int bestGuess = 0;
		Header* newHeader = 0;

		for ( vector<Header>::iterator it = _headers.begin();  it != _headers.end(); it++ )
		{
			Header* header = &(*it);
			int newGuess = header->getFirstChar();
			if ( firstCharInLine >= newGuess )
			{
				if ( newGuess >= bestGuess )
				{
					newHeader = header;
					bestGuess = newGuess;
				}
			}
		}				
		if ( newHeader )
		{
			// found the correct header
			processHeader( documentPrintout, newHeader );
			//DEBUG("%s","Leaving PrintoutParserMml::addPotentialHeaderLine ");
			return true;
		}
	}
	//DEBUG("%s","Leaving PrintoutParserMml::addPotentialHeaderLine ");
	return false;
}

bool PrintoutParserMml::isDataLine( string& line )
{
	//DEBUG("%s","Leaving  PrintoutParserMml::isDataLine ");
//	return !_emptyLineFound; // if an empty line was found, then ignore all data lines until next header
	return true;

}


void PrintoutParserMml::processHeader( DocumentPrintout& documentPrintout, Header* header )
{
	//DEBUG("%s"," Entering void PrintoutParserMml::processHeader(");
	// in case the new header is not a subheader, flash the old object to the printout
	// and clean the data lines of all headers having lower level
	if ( _currentHeader != 0 && ( header->getLevel() <= _currentHeader->getLevel() ))
	{
		addSectorToPrintout( documentPrintout ); // flash the old object to the printout
				
		//for ( vector<Header>::iterator it = _currentStartHeader;  it != _currentEndHeader+1; it++ )
                vector<Header>::iterator end(_currentEndHeader+1);
	        for ( vector<Header>::iterator it ( _currentStartHeader);  it != end ; it++ )
        	{
			if (it->getLevel() >= header->getLevel() )
			{
				it->cleanDataLines();;
			}
                        vector<Header>::iterator check(_currentHeader);
			if ( it == check ) break;
		}
	}

	// in case header has level 1, set currentStartHeader
	// Explanation: There can be several tables with subtables inside the printout
	// e.g. level1, level2, level3, level1, level2
	// currentStartHeader points to the header with current start level1
	if ( header->getLevel() == "1" )
	{
		_currentStartHeader = header;
	}
	_currentEndHeader = header;
	_currentHeader = header;
	//DEBUG("%s","Leaving void PrintoutParserMml::processHeader( ");
}

bool PrintoutParserMml::lineEmpty( std::string& line )
{
	//DEBUG("%s","Entering  PrintoutParserMml::lineEmpty( ");
	if ( line == "WAIT" )
	{
		//DEBUG("%s","Leaving  PrintoutParserMml::lineEmpty( "); 
		return true;
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::lineEmpty( ");
	return ( line.find_first_not_of( " " ) == string::npos );
}

// check if the line is an error line
// in case there has been an error line found before, concatenate all error lines
// until one of the following conditions apply:
// an empty line or an end line is found
// maxlines was found in the XML for that error: Then read this number of lines
// a line starts with text "FAULT CODE" then one more line is read
bool PrintoutParserMml::isTagHandled( std::list<TableTag>& tableTagList, string& line, istream& is )
{
	//DEBUG("%s","Entering  PrintoutParserMml::lineEmpty( ");

	for ( std::list<TableTag>::iterator tableTag = tableTagList.begin(); tableTag != tableTagList.end(); ++tableTag )
	{
		if(line == "PROGRAM CORRECTIONS")
		{
		}
		DEBUG("Item TAG to match:: %s",line.c_str());
                DEBUG("Item TAG::%s",((*tableTag).getItemText()).c_str());
		int maxLines = (*tableTag).getMaxLines();
		const string faultCode = "FAULT CODE";
		const char prompt = '<';
		
		size_t startPos = line.find((*tableTag).getItemText());				
		if (startPos != string::npos )
		{	
			if ( (startPos == 0) || (*tableTag).isInText() )
			{
				int maxLines = (*tableTag).getMaxLines();
				_errorText = line;
				while ( ( --maxLines !=0 ) && readNextLine( line, is ) && !lineEmpty(line) && (line[0] != prompt) )
				{
					_errorText += ":";
					_errorText += line;
					if ( line.compare( 0, faultCode.size(), faultCode ) == 0 )
					{ 
						maxLines = 2;
					}

				}	
				//DEBUG("%s","Leaving  PrintoutParserMml::isTagHandled( ");
				return true;
			}

		}
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::isTagHandled( ");
	return false;
}

bool PrintoutParserMml::skipLine( string& line, std::istream& is )
{
	//DEBUG("%s","Entering PrintoutParserMml::skipLine( ");
	// if printout is ongoing, check if the line is an end line
	if ( lineEmpty( line ) || isTagHandled( _tableTags[TTIGNORES], line, is ) ) return true; // skip the line
	if ( _ongoing )
	{
		if( isTagHandled( _tableTags[TTENDNORMALS], line, is ))
		{
			_ongoing = false;
			//DEBUG("%s","Leaving PrintoutParserMml::skipLine( ");
			return true; // skip the line
		}
		//DEBUG("%s","Leaving PrintoutParserMml::skipLine( ");
		return false; // process the line
	}

	// if printout is not ongoing, check if the line is a begin line
	if (isTagHandled( _tableTags[TTBEGINS], line, is))		
	{
		_documentFound = true;
		_ongoing = true;		
	}
	//DEBUG("%s","Leaving PrintoutParserMml::skipLine( ");
	return true; // skip the line
}

// Scan all headers to find out, if the line fits to the columns of that header
PrintoutParserMml::Header* PrintoutParserMml::checkHeader ( const string& line )
{
	//DEBUG("%s","Entering  PrintoutParserMml::Header* PrintoutParserMml::checkHeader ( ");
	for ( vector<Header>::iterator it = _headers.begin();  it != _headers.end(); it++ )
	{
		Header* header = (*it).checkHeader( line );
		if ( header != 0)
		{
			//DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::checkHeader ( ");
			return header;
		}
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::checkHeader ( ");
	return 0;
}



// check whether the line fits to the header.
PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( const string& line )
{	
	//DEBUG("%s","Entering  PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( ");
	if ( _myHeaderLine.empty())
	{
		// first time a data line is found for the header
		// check all columns and evaluate the latest starting and ending points for the data
		// as well as the start and ending position of the columns 
		int startPos = 0;
		for ( vector<Column>::iterator it = _columns.begin(); it != _columns.end(); it++ )
		{
			startPos = getPosition( startPos, line, (*it).getTitle() );
	
			
			if ( startPos != string::npos )		// column found in line
			{
				it->setStartHeader( startPos );
				it->setEndHeader( startPos + (int) (*it).getTitle().size());
			}
			else
			{
			 	 //DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( ");
				 return 0;
			}						// not a header line

			if ( it != _columns.begin() )
			{
				it->setEarliestStart( (it-1)->getEndHeader() + 1 );
				(it-1)->setLatestEnd( it->getStartHeader() -1 );
			}
		}

		Column& lastColumn = _columns.back();

		lastColumn.setLatestEnd( (int) line.size() -1 ) ; 

    		_myHeaderLine = line; // store the header line
		_firstChar = _myHeaderLine.find_first_not_of( ' ' );
	    	//DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( ");
		return this;
	}
	

	if ( _myHeaderLine.compare( line ) == 0 )
	{
		 //DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( ");
		 return this;
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::Header* PrintoutParserMml::Header::checkHeader ( ");
	return 0;

}

// get position of column title in the line.
int PrintoutParserMml::Header::getPosition (  int startPosition, const string& line, const string& title )
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::getPosition ( ");
	int startPos = startPosition;
	int iend = startPos ? 2 : 1;

	for ( int i = 0; i < iend; i ++ )
	{
		while ( startPos != string::npos )
		{
			startPos = (int) line. find(title, startPos); // first try from latest position
			if ( startPos != string::npos )
			{
				try
				{
					if ( line.at(startPos + title.length()) ==  ' ' )
					{
						//DEBUG("%s","Leaving PrintoutParserMml::Header::getPosition ( ");
						return startPos;
					}
				}
				catch(...)
				{
					//DEBUG("%s","Leaving PrintoutParserMml::Header::getPosition ( ");
					return startPos;
				}
				startPos++;
			}
		}
		startPos = 0;
	}
	//DEBUG("%s","Leaving PrintoutParserMml::Header::getPosition ( ");
	return string::npos;

}

// return the next key and value. They are only valid, if the method returns true
bool PrintoutParserMml::Header::moreData( string& key, string& value )
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::moreData( ");
	std::vector<Column>::iterator check(0); 
        if (_currentColumn == check )
      	{
		//DEBUG("%s","Leaving PrintoutParserMml::Header::moreData( ");
		return false;
	}

	// skip not required columns
	while ( _currentColumn != _columns.end() && !(*_currentColumn).getRequired())
	{
		_currentColumn++;
	}

	// all columns scanned, no data found
	if ( _currentColumn == _columns.end() )
	{
		_currentColumn = _columns.begin();
		//DEBUG("%s","Leaving PrintoutParserMml::Header::moreData( ");
		return false;
	}

	// read the according value from the line
	readValue(_myDataLine,value); 

	// add the key
	key = _currentColumn->getTitle();
	_currentColumn++;
	//DEBUG("%s","Leaving PrintoutParserMml::Header::moreData( ");
	return true;
}

// in case dataLine has some missing leading columns which can be taken from the previous line
// and one of the available columns is required, then the missing leading columns are copied from
// the previous line.
// In case of no missing leading columns the method returns true
// In case some columns were copied the method returns true
// In case of missing leading columns but no other columns are required, the method returns false
// In the latter case the dataLine is not added to the printout
bool PrintoutParserMml::Header::isNewOrPaddedLine( std::string& dataLine )
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::isNewOrPaddedLine( ");
	bool retVal = true; // none of the following columns is required, so this line does not need to be taken into the printout
	// find the leading character not equal to space
	size_t firstPos = dataLine.find_first_not_of( " " );

	// find how many columns are empty in the new data line
	for ( _currentColumn = _columns.begin(); _currentColumn!=_columns.end(); _currentColumn++ )
	{
		if ( _currentColumn->getLatestEnd() > firstPos ) break;
	}


	if ( _currentColumn!=_columns.begin() && _currentColumn!=_columns.end())
	{
		retVal = false;
		// check if at least one of the following columns is required
		for( std::vector<Column>::iterator it = _columns.begin(); !retVal && (it != _currentColumn); it ++ )
		{
			if ( (*it).getRequired() )
			{
				_currentColumn--;
				dataLine.replace( 0, _currentColumn->getLatestEnd(), _myDataLine, 0, _currentColumn->getLatestEnd() );
				retVal = true;
			}
		}
	}

	_currentColumn = _columns.begin(); // restore _currentColumn
	//DEBUG("%s","Leaving PrintoutParserMml::Header::isNewOrPaddedLine( ");
	return retVal; // dataline is a new one
}

// add the data line to the header
void PrintoutParserMml::Header::setDataLine( std::string& dataLine ) 
{ 
	//DEBUG("%s","Entering PrintoutParserMml::Header::setDataLine( ");
	_myDataLine = dataLine; 
	_currentColumn = _columns.begin();
	//DEBUG("%s","Leaving PrintoutParserMml::Header::setDataLine( ");	
};


// return a list of all titles found in the columns of a header
void PrintoutParserMml::Header::getTitles( std::list<string>& titles )
{
	//DEBUG("%s","Entering  PrintoutParserMml::Header::getTitles( ");
    for ( vector<Column>::iterator it = _columns.begin(); it != _columns.end(); it++ )
	{
			titles.push_back((*it).getTitle());
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::Header::getTitles( ");
}


/**
  * check the boundaries of a column. Find a position in line where the substring until the next space
  * is included in the boundaries. In case there is no valid data, startPos indicates empty
  * @param[in] latestEnd two bytes before the position of the succeeding header
  * @param[out] value the string found
  */

void PrintoutParserMml::Header::readValue(string& line, string& value)
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::readValue( ");
	if(jobScheduler::stopRequested)
	{
		DEBUG("%s","statement in PrintoutParserMml");
		return;
	}
	

	readValueLeftHeader( line, value );

/*	if ( _currentColumn->getRegEx() == "" )
	{
		readValueLeftHeader( line, value );
	}
	else
	{
		readValueRegEx( line, value );
	}
	*/
	//DEBUG("%s","Leaving PrintoutParserMml::Header::readValue( ");
}

// Assume that the header titles are left-aligned, which is the normal case for MML commands
// Read the columns from the start of the title until one position before the next title
// eliminate spaces from start and end
void PrintoutParserMml::Header::readValueLeftHeader( string& line, string& value )
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::readValueLeftHeader( ");
	size_t headerStart   = _currentColumn->getStartHeader();
	size_t latestEnd	 = _currentColumn->getLatestEnd();


	if ( _currentColumn == (_columns.end()-1) ) 
	{
		latestEnd = line.size();
	}
	else if ( latestEnd < line.size() ) 
		// check whether the next column exceeds its header to the left
		// sometimes data start in a column left of the according header.
		// in this case the data of the previous column must end before.
	{
		while( line[latestEnd]!= ' ' ) latestEnd--;
	}

	size_t startPos = line. find_first_not_of( ' ', headerStart ); // point to the first non-space character 
	if (startPos > latestEnd )
	{
		value = "";
		//DEBUG("%s","Leaving PrintoutParserMml::Header::readValueLeftHeader( ");
		return;
	}

	// check if the column starts before the header start
	if ( startPos == headerStart )
		while ( (startPos > 0) && (line[startPos-1] != ' ') )
			startPos--;

	size_t endPos = line.find_last_not_of(' ', latestEnd); // point to the last non-space character

	if ( startPos <= endPos )
	{
		value = line.substr( startPos, endPos - startPos + 1 );
	}
	else
	{
		value = "";
	}
	//DEBUG("%s","Leaving PrintoutParserMml::Header::readValueLeftHeader( ");
}

void PrintoutParserMml::Header::readValueRegEx( string& line, string& value )
{
	//DEBUG("%s","Entering  PrintoutParserMml::Header::readValueRegEx( ");
	size_t earliestStart = _currentColumn->getEarliestStart();
	size_t headerStart   = _currentColumn->getStartHeader();
	size_t headerEnd     = _currentColumn->getEndHeader();
	size_t latestEnd	 = _currentColumn->getLatestEnd();

	if ( _currentColumn == (_columns.end()-1) ) latestEnd = line.size();

	size_t firstCheck    = (earliestStart > 0) ? earliestStart-1 : 0;
	size_t firstStart    = line.find_first_not_of( ' ', firstCheck );
	size_t firstEnd;
	bool firstValid = false;

	size_t secondStart;
	size_t secondEnd;
	bool secondValid = false;

	size_t startPos = -1; // initialize: the string found is emppty

	if ( firstStart  >= earliestStart ) // there was no string ongoing from column before
	{
		firstEnd = line. find_first_of( ' ', firstStart);
		if ( firstEnd == string::npos ) firstEnd = line.size()-1;
		firstValid = (firstEnd <= latestEnd );  // first string ends within column
	}
	else
		firstEnd = line.find_first_of( ' ', earliestStart ); // point to the string behind column before

	// trial

	if ( !firstValid )
	{ 
		firstStart = line. find_first_not_of( ' ', firstEnd ); // point to the first string after first string

		if ( firstStart != string::npos ) // found a new first string
		{
			firstEnd = line. find_first_of( ' ', firstStart );
			if (firstEnd == string::npos) firstEnd = line.size()-1; // first string ends at line end,
																			// if necessary
			firstValid = ( firstEnd <= latestEnd );
		}
	}

	// end trial

	secondStart = line. find_first_not_of( ' ', firstEnd ); // point to the first string after first string

	if ( secondStart != string::npos ) // found a second string
	{
		secondEnd = line. find_first_of( ' ', secondStart );
		if (secondEnd == string::npos) secondEnd = line.size()-1; // second string ends at line end,
		                                                                   // if necessary
		secondValid = ( secondEnd <= latestEnd );
	}

	if ( firstValid && secondValid )
	{
		// find decision: which string overlaps the header?
		firstValid =  !( firstEnd < headerStart ) || ( firstStart > headerEnd );
		secondValid =  !( secondEnd < headerStart ) || ( secondStart > headerEnd );
	}

	size_t endPos;
	if ( firstValid )
	{
		if ( secondValid ) 
		{
			endPos =  _myDataLine.find_first_of (' ', secondStart );			
		}
		else
		{
			endPos =  _myDataLine.find_first_of (' ', firstStart );
		}
		startPos = firstStart;
	}
	else if ( secondValid )
	{
		startPos = secondStart;
		endPos   =_myDataLine.find_first_of (' ', secondStart );
	}
	
	if ( startPos != string::npos )
	{
		value = _myDataLine.substr( startPos, endPos - startPos );
	}
	else
	{
		value = " ";
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::Header::readValueRegEx( ");

}

// build the internal header and column representation from the Rule Set File
void PrintoutParserMml::analyseHeader( const DocumentXml::Node& printout )
{	
	//DEBUG("%s","Entering PrintoutParserMml::analyseHeader( ");
	DocumentXml::Nodes headers;	

	//printout.getNodes(headers, "./header");
	printout.getNodesList(headers, "./header",printout);	
	DocumentXml::Nodes::Iterator headerNode;
	string lvalue;
	for ( headerNode = headers.begin(); headerNode!=headers.end(); ++headerNode )
	{
		PrintoutParserMml::Header header;
		
		DocumentXml::Node level = (*headerNode).getAttribute( "level", false );
                if(level.isValid())
		{
                	lvalue=level.getValue();
		}
                else
		{
                             lvalue="1";
		}
                header.setLevel(lvalue);

		//header.setLevel( level.isValid() ? levelValue : "1" );
		
		DocumentXml::Node maxLine = (*headerNode).getAttribute( "maxlines", false );		
		header.setMaxLines( maxLine.isValid() ? atoi( maxLine.getValue().c_str()) : -1);

		string emptyString = "";
		header.setHeader( emptyString );
		header.cleanDataLines();
		analyzeColumn( *headerNode, header  );
		_headers.push_back( header );
	}
	//DEBUG("%s","Leaving PrintoutParserMml::analyseHeader( ");
}

// build the columns inside a header from the rule set file
//void PrintoutParserMml::analyzeColumn( DocumentXml::Node& headerNode, PrintoutParserMml::Header& header )
void PrintoutParserMml::analyzeColumn( DocumentXml::Node headerNode, PrintoutParserMml::Header& header )
{
	//DEBUG("%s","Entering  PrintoutParserMml::analyzeColumn( ");
	DocumentXml::Nodes columns;
	//headerNode.getNodes(columns, "./column");
	headerNode.getNodesList(columns, "./column",headerNode);
	DocumentXml::Nodes::Iterator columnNode;

	header.clearHasKey();
	header.clearRequiredColumn();
	string columnNodeValue;
	string dataFormatValue = "";
	string emptyValue = "";
	for ( columnNode = columns.begin(); columnNode!=columns.end(); ++columnNode )
	{
		PrintoutParserMml::Column column;

		DocumentXml::Node required = (*columnNode).getAttribute( "required", false );

		if ( required.isValid() && required.getValue().compare( "true" ) == 0 )
		{		
			column.setRequired();
			header.setRequiredColumn();
		}
		else
			column.clearRequired();

		DocumentXml::Node hasKey = (*columnNode).getAttribute( "key", false );

		if ( hasKey.isValid() && hasKey.getValue().compare( "true" ) == 0 )
		{
			header.setHasKey();
			column.setHasKey();
		}
		else
			column.clearHasKey();

		DocumentXml::Node dataFormat = (*columnNode).getAttribute( "data-format", false );
		if(dataFormat.isValid())
		{
			dataFormatValue = dataFormat.getValue();
			column.setRegEx(dataFormatValue);
		}
		else
		{
			column.setRegEx(emptyValue);
		}
		//column.setRegEx( dataFormat.isValid() ? dataFormat.getValue() : "" );
		columnNodeValue = (*columnNode).getValue();
		column.setTitle(columnNodeValue);

		column.setStartHeader( 0 );
		column.setEndHeader( 0 );
		column.setEarliestStart( 0 );
		column.setLatestEnd( 0 );
		header.addColumn( column );
	}
	//DEBUG("%s","Leaving  PrintoutParserMml::analyzeColumn( ");	
}

// add an object to the internal printout representation
// this comprises the header and subheaders with their stored datalines
// the object is closed then, meaning that all further data belong to a new object
void PrintoutParserMml::addSectorToPrintout( DocumentPrintout& documentPrintout )
{
	//DEBUG("%s","Entering  PrintoutParserMml::addSectorToPrintout( ");
	//for ( vector<Header>::iterator it = _currentStartHeader; it != _currentEndHeader+1; it++ )
        vector<Header>::iterator end(_currentEndHeader+1);
        for ( vector<Header>::iterator it( _currentStartHeader); it != end; it++ )
	{
	    	adjustSector( documentPrintout, *it );
		string key, value;
		while(moreData( &(*it), key, value ) )
		{
			documentPrintout.add( key, value );
		}
		if ( it->getLevel().compare( _currentHeader->getLevel()) > 0 ) 
			break;
	}
   	documentPrintout.closeSector(); // close the object
	//DEBUG("%s","Leaving  PrintoutParserMml::addSectorToPrintout( ");
}

// in case more data shall be added to a special object, this object is re-opened
// this is done in case the header has a key defined in the rule set file
// Follow-up tables can be associated this way
void PrintoutParserMml::adjustSector( DocumentPrintout& documentPrintout, PrintoutParserMml::Header& header )
{
	//DEBUG("%s","Entering PrintoutParserMml::adjustSector( ");
	if (!header.getHasKey()) // no column has a key, normal handling applies
	{
		//DEBUG("%s","Leaving PrintoutParserMml::adjustSector( ");
		return;			
	}

	header.adjustSector( documentPrintout );    // one column has a key, object must be re-opened
	//DEBUG("%s","Leaving PrintoutParserMml::adjustSector( ");
}

// re-open an object
void PrintoutParserMml::Header::adjustSector( DocumentPrintout& documentPrintout )
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::adjustSector( DocumentPrintout& documentPrintout ) ");
	// find the column that has the key
	for ( _currentColumn = _columns.begin(); _currentColumn!= _columns.end(); _currentColumn++ )
	{
		string value, dataLine;
		if ( _currentColumn->getHasKey() )
		{
			dataLine = getDataLine();
			readValue(dataLine, value);	// read the key
			documentPrintout.adjustSector(_currentColumn->getTitle(), value); // re-open the object
			//DEBUG("%s","Leaving PrintoutParserMml::Header::adjustSector( DocumentPrintout& documentPrintout ) ");
			return;
		}
	}
	//DEBUG("%s","Leaving PrintoutParserMml::Header::adjustSector( DocumentPrintout& documentPrintout ) ");
}

// get more data from the header's dataline. Returns false when finished
bool PrintoutParserMml::moreData( Header *header, string& key, string& value )
{
	//DEBUG("%s","In PrintoutParserMml::moreData( Header *header, string& key, string& value ) ");
	return header->moreData( key, value );
}

// this method writes the internal data structure to cout. This can be used for diagnostics.
void PrintoutParserMml::printAll()
{
	//DEBUG("%s","Entering PrintoutParserMml::printAll( ");
	cout << "This is the structure of headers and columns found in the rule set file" << endl;
	
	vector<Header>::iterator it;

	for ( it = _headers.begin(); it != _headers.end(); it++ )
	{
		it->printAll();
	}
	cout << endl;
	//DEBUG("%s","Leaving PrintoutParserMml::printAll( ");
}


// this method writes the internal data structure to cout. This can be used for diagnostics.
void PrintoutParserMml::Header::printAll()
{
	//DEBUG("%s","Entering PrintoutParserMml::Header::printAll() ");
	cout << endl << "Header of level " << getLevel() << endl;

	vector<Column>::iterator it;

	cout << " Title    Required startPos endPos regExpression " << endl;

	for ( it = _columns.begin(); it!= _columns.end(); it++ )
	{
		it->printAll();
	}
	//DEBUG("%s","Leaving PrintoutParserMml::Header::printAll() ");
}

// this method writes the internal data structure to cout. This can be used for diagnostics.
void PrintoutParserMml::Column::printAll()
{
	//DEBUG("%s","In PrintoutParserMml::Column::printAll( ");
	cout<<  getTitle() << "\t" << getRequired () << "\t" <<  (int) getStartHeader() << "\t" << (int) getEndHeader() << "\t" << getRegEx() << endl; 
}

bool PrintoutParserMml::readNextLine( std::string& line, istream& is )
{	
	//DEBUG("%s","Entering PrintoutParserMml::readNextLine ");
	if(is.eof())
	{
		_fileEnd=true;
		//DEBUG("%s","Leaving PrintoutParserMml::readNextLine ");
		return false;
	}

	getline( is, line);
	//DEBUG("%s","Leaving PrintoutParserMml::readNextLine ");
	return true;
}
