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

#ifndef AcsHcs_PrintoutParserMml_h
#define AcsHcs_PrintoutParserMml_h

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "acs_hcs_documentprintout.h"
#include "acs_hcs_printoutparser.h"

namespace AcsHcs
{
	/** PrintoutParserMml is the Printout Parser for printouts derived from MML commands.
	  * The data found in the printout are analyzed accourding to the rule set file and stored
	  * in the local object documentPrintout.
	  * The strategy for parsing the file is as follows:
	  * The rule set file is read for node <printout> and all data related to headers
	  * and their columns are read from that file and stored inside the private object
	  * collections Header and Column.
	  * One or more headers plus their data form an object. This is achieved by assigning levels to the headers:
	  * A header of level n+1 is assumed as a subheader of a header with level n.
	  * The data of an object is stored locally inside the headers. Once a section is finished, the collected data is
	  * written to the object documentPrintout.
	  */
	class PrintoutParserMml : public PrintoutParser
	{
	private:
		/**
		  * Column takes the column related parameters
		  * some are related to the rule set file
		  * others are calculated from the location of the
		  * own column title and the titles of the predecessor and successor
		  */
		class Column
		{
			std::string	_title;
			bool	    _required;
			bool		_hasKey;
			size_t	    _startHeader;
			size_t	    _endHeader;
			size_t      _earliestStart;
			size_t      _latestEnd;
			std::string	_regex;
		public:
			void setTitle( std::string& s ){ _title = s; };
                        //void setTitle( std::string s ){ _title = s; };
			std::string getTitle() const { return _title; };
			void setRequired( ){ _required = true; };
			void clearRequired( ){ _required = false; };
			bool getRequired () const { return _required; };
			void setHasKey(){ _hasKey = true; };
			void clearHasKey(){ _hasKey = false; };
			bool getHasKey() const { return _hasKey; };
			void setStartHeader( size_t i ) { _startHeader = i; };
			size_t getStartHeader() const { return _startHeader; };
			void setEndHeader( size_t i ) { _endHeader = i; };
			size_t getEndHeader() const { return _endHeader; };
			void setRegEx( std::string& regex ) { _regex = regex; };
			//void setRegEx( std::string regex ) { _regex = regex; };
                        std::string getRegEx() const { return _regex; };
			void setEarliestStart( size_t start ) { _earliestStart = start; };
			void setLatestEnd( size_t end ) { _latestEnd = end; };
			size_t getEarliestStart() { return _earliestStart; };
			size_t getLatestEnd() { return _latestEnd; };


			void printAll();

		};

		/**
		  * Header takes the description of a header line in an MML printout
		  * some data are taken from the rule set file, others are calculated
		  * at arrival of the first line containing the columns of the header
		  *
		  * It contains the objects which contain the description of the columns
		  */
		class Header
		{
			std::vector<Column> _columns;
			std::string	_level;
			std::string _myHeaderLine;
			std::string _myDataLine;
			std::vector<Column>::iterator _currentColumn;
			bool _hasKey; // defined if any of the columns has a key defined
			bool _hasRequiredColumn;
			int getPosition( int startPosition, const std::string& line, const std::string& title );


			int _maxLines;
			int _firstChar;

		public:
                	int getFirstChar() { return _firstChar; };
			void clearHasKey(){ _hasKey = false; };
			void setHasKey(){ _hasKey = true ;};
			bool getHasKey() const { return _hasKey; };
			void setLevel( std::string& l ) { _level = l; };
			//void setLevel( std::string l ) { _level = l; };
                        std::string getLevel() const { return _level; };		
			void addColumn( Column& column ){ _columns.push_back(column);};
			void printAll();
			void setHeader( std::string& header ){ _myHeaderLine = header;};
			void setDataLine( std::string& dataLine );
			void cleanDataLines() { _myDataLine = ""; };
			std::string getDataLine( ) const { return _myDataLine ;};
			bool hasDataLine() { return !_myDataLine.empty();};
			bool isNewOrPaddedLine( std::string& dataLine );
		    Header* checkHeader( const std::string& line );
			void addSectorToPrintout();
			bool moreData( std::string& key, std::string& value );
			int getMaxLines() const { return _maxLines; };
			void setMaxLines( int maxLines ) { _maxLines = maxLines; };
			/**
			* check the boundaries of a column. Find a position in line where the substring until the next space
			* is included in the boundaries. In case there is no valid data, startPos indicates empty
			* @param[in] latestEnd two bytes before the position of the succeeding header
			* @param[out] startPos the starting position found for the column (-1 indicates empty)
			*/
			void readValue( std::string& line, std::string& value );
                        //void readValue( std::string line, std::string value );
			void readValueLeftHeader( std::string& line, std::string& value );
			void readValueRegEx( std::string&line, std::string& value );

		    void adjustSector( DocumentPrintout& documentPrintout );
			void getTitles( std::list<std::string>& titles );
			bool hasRequiredColumn() { return _hasRequiredColumn; };
			void setRequiredColumn() { _hasRequiredColumn = true; };
			void clearRequiredColumn() { _hasRequiredColumn = false; };


		};




	public:
		
		/**
		  * parse the printout according to the advices in the rule set file and store the data in an object of class DocumentPrintout
		  * @param[out] documentPrintout the memory representation of the printout file
		  * @param[in]  printout		    the printout section in the rule set file
		  * @param[in]  cmdResult        the printout file, i.e. the result given from the mml command
		  */
		void parse( DocumentPrintout& documentPrintout, const DocumentXml::Node& printout, const std::string& cmdResult );

		/**
		  * print the structure of headers and columns. This function is meant for testing purposes
		  */
		void printAll();



	private:

		void analyseHeader( const DocumentXml::Node& printout );
		//void analyzeColumn( DocumentXml::Node& headerNode, PrintoutParserMml::Header& header );
		void analyzeColumn( DocumentXml::Node headerNode, PrintoutParserMml::Header& header );
                void parsePrintout( DocumentPrintout& documentPrintout, const std::string& cmdResult );
		void addSectorToPrintout( DocumentPrintout& documentPrintout );
		bool addPotentialHeaderLine( DocumentPrintout& documentPrintout, std::string& line );

		void adjustSector( DocumentPrintout& documentPrintout, PrintoutParserMml::Header& header );

		void processHeader( DocumentPrintout& documentPrintout, Header* header );

		bool lineEmpty( std::string& line );

		Header* _currentStartHeader;
		Header* _currentEndHeader;
		bool moreData( Header *header, std::string& key, std::string& value );
		bool skipLine( std::string& line, std::istream& is );
		std::string _begin;

		class TableTag
		{
			std::string _itemText;
			int _maxLines;
			bool        _inTextFlag;
		public:
			void setMaxLines( int maxLines ) { _maxLines = maxLines; };
			int getMaxLines() { return _maxLines; };
			void setInText( bool inTextFlag ) { _inTextFlag = inTextFlag; };
			bool isInText() { return _inTextFlag; };
			std::string& getItemText() { return _itemText; };
			void setItemText( std::string& itemText) { _itemText = itemText; };
                        //void setItemText( std::string itemText) { _itemText = itemText; };
		};		
		
		enum TABLETAGS{
			TTBEGINS,
			TTFAILEDS,
			TTIGNORES,
			TTERRORS,
			TTENDNORMALS,
			TTSIZE
		} ;

		static const char* _tableStrings[];

		std::list<TableTag> _tableTags[TTSIZE];
		void setTableTags( std::list<TableTag>& tableTagList, DocumentXml::Nodes& endNode, bool inTextDefault, int maxLinesDefault );
		void initTableTags( std::list<TableTag>& tableTag, const DocumentXml::Node& printout, const char* tagString, bool inTextDefault, int maxLinesDefault );


		bool isTagHandled( std::list<TableTag>& TableTag, std::string& line, std::istream& is );


		bool _ongoing;
		std::string _errorText;

	protected:
		Header* _currentHeader;
		std::vector<Header> _headers;
		/** decides whether a line from the printout is regarded as a data line.
		*  For normal MML commands all lines that are not headers are data lines.
		*  Exception: An <end/ignore> tag had been found in the current table.
		*  In that case all further lines are ignored until the next header line is found.
		* @param[in] the line to be investigated
		* @returns true if line is a data line
		*/
		virtual bool isDataLine( std::string& line );
		/** initParser is used to do specific initialization for the PrintoutParser in use
		* in case of PrintoutParserMml nothing is done
		*/
		virtual void initParser();
		/** this method checks whether the line read from the printout is a header line of a table.
		* In case it is, it returns a pointer to this according header object.
		*@param the line to be investigated
		*@returns the header applicable for line
		*         NULL in case the line is not a header line
		*/
		Header* checkHeader( const std::string& line );
		/** check whether a header has found found so far in the printout
		* this indication is set, if a header line is found. It is false outside the
		* begin and end of a printout inside the printoutFile
		*/
		bool _initialHeaderFound;  // first header inside the printout found
		/** is set when a line matching and <ignore> line has been found. 
		* it is cleared at the next header line
		*/

		bool _documentFound; // indicates if any data have been found in the document

		bool _fileEnd;
		bool _continueReading;

		bool readNextLine( std::string& line, std::istream& is );



	};
};


#endif
