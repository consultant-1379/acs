//======================================================================
//
// NAME
//      FormatterTxt.h
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
//      Implementation of the Table format of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_FormatterTxt_h
#define AcsHcs_FormatterTxt_h

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "acs_hcs_tra.h"
namespace AcsHcs
{
	/**
	* FormatterTxt - Encapsulates the formatting of text.
	*/
	class FormatterTxt
	{
	public:
		class Table
		{
		public:
			/**
			* defines the columns for the formatted text
			*/
			class Column
			{
			public:
				enum Align
				{
					LEFT,
					RIGHT
				};

				Column();
				/** constructor that sets the parameters for the colum */
				Column(const char* header, int width, Align align, const char* sepCol, const char* sepRow);

				/** check if the column is initialized */
				bool isValid() const;

				const char* header; // Defines the column's headers.
				const int   width;  // Defines the column's width.
				const Align align;  // Defines the column's alignment.
				const char* sepCol; // Defines the column's separator.
				const char* sepRow; // Defines the column's row separator.

			private:
				Column& operator=(const Column&);
			};

			/**
			* Constructor.
			*/
			Table(const Column cols[]);
			
			/**
			* Destructor.
			*/
			~Table();

			/**
			* Prints the table header.
			* param[inout] ss The stream to output the header.
			* @return The number of table columns.
			*/
			unsigned int printHeader(std::stringstream& ss) const;

			/**
			* Prints a table line.
			* @param[inout] ss       The stream to output the line.
			* @param[in]    colTexts The column texts to be written.
			*/
			void printLine(std::stringstream& ss, const std::vector<std::string>& colTexts) const;

			static const char* S_C;  // Separator for columns.
			static const char* S_R;  // Separator for rows.
			static const char* S_CR; // Separator for crossings of columns and rows.

		private:
			Table(const Table&);
			Table& operator=(const Table&);

			size_t wrapColumn(size_t colIndex, const std::string& colText, size_t colWidth) const;
			
			mutable std::map<int, std::vector<std::string> > wrappedCols;
			const Column* const cols;
		};

		/**
		* Constructor.
		*/
		FormatterTxt();

		/**
		* Destructor.
		*/
		~FormatterTxt();

	private:
		FormatterTxt(const FormatterTxt&);
		FormatterTxt& operator=(const FormatterTxt&);
	};
}

#endif // AcsHcs_FormatterTxt_h
