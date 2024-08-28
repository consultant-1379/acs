//======================================================================
//
// NAME
//      FormatterTxt.cpp
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
//      2011-07-22 by EEDSTL
// CHANGES
//     
//======================================================================

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

#include "acs_hcs_formattertxt.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class FormatterTxt
	//================================================================================

	FormatterTxt::FormatterTxt()
	{
		DEBUG("%s","In constructor FormatterTxt ");
	}

	FormatterTxt::~FormatterTxt()
	{
		DEBUG("%s","in destructor FOrmatterTxt ");
	}

	//================================================================================
	// Class FormatterTxt::Table
	//================================================================================

	const char* FormatterTxt::Table::S_C  = " | "; // Separator for columns.
	const char* FormatterTxt::Table::S_R  = "-";   // Separator for rows.
	const char* FormatterTxt::Table::S_CR = "-+-"; // Separator for crossings of columns and rows.

	FormatterTxt::Table::Table(const Column cols[]) : cols(cols)
	{
		DEBUG("%s","In constructor FormatterTxt::Table::Table(c ");
	}

	FormatterTxt::Table::~Table()
	{
		DEBUG("%s","In destructor FormatterTxt::Table::Table(c ");
	}

	unsigned int FormatterTxt::Table::printHeader(std::stringstream& ss) const
	{
		DEBUG("%s","Entering  FormatterTxt::Table::printHeader( ");
		unsigned int numCols = 0;
		vector<string> colTexts;

		for (int i = 0; cols[i].isValid(); i++)
		{
			colTexts.push_back(cols[i].header);
			numCols++;
		}

		this->printLine(ss, colTexts);

		ss << setfill(*S_R);

		for (int i = 0; cols[i].isValid(); i++)
			ss << left << setw(cols[i].width) << "" << cols[i].sepRow;
		DEBUG("%s","Leaving  FormatterTxt::Table::printHeader( ");
		return numCols;
	}

	void FormatterTxt::Table::printLine(std::stringstream& ss, const std::vector<std::string>& colTexts) const
	{
		DEBUG("%s","entering  FormatterTxt::Table::printLine( ");
		size_t sizeMax = 0;

		for (size_t i = 0; cols[i].isValid(); i++)
		{
			if (i < colTexts.size())
				sizeMax = max(sizeMax, this->wrapColumn(i, colTexts[i], cols[i].width));
			else
				sizeMax = max(sizeMax, this->wrapColumn(i, "", cols[i].width));
		}

		for (size_t i = 0; i < sizeMax; i++)
		{
			vector<string>* col = 0;

			ss << setfill(' ');

			for (int j = 0; cols[j].isValid(); j++)
			{
				col = &this->wrappedCols[j];
				ss << (cols[j].align == FormatterTxt::Table::Column::/*Align::*/RIGHT ? right : left) << setw(cols[j].width) << (i < col->size() ? (*col)[i] : "") << cols[j].sepCol;
			}
		}
		DEBUG("%s","Leaving  FormatterTxt::Table::printLine( ");
	}

	size_t FormatterTxt::Table::wrapColumn(size_t colIndex, const std::string& colText, size_t colWidth) const
	{
		DEBUG("%s","Entering FormatterTxt::Table::wrapColumn( ");
		vector<string>& col = this->wrappedCols[colIndex];
		col.clear();

		size_t posStart = 0;
		size_t posEnd   = posStart + colWidth;

		while (posEnd < colText.size())
		{
			if (colText[posEnd] == ' ')
			{
				string line = colText.substr(posStart, posEnd - posStart);
				//cout << "wrapColumn()1: line=" << line << ", posStart=" << posStart << ", posEnd=" << posEnd << endl;
				col.push_back(line);
				posStart = posEnd + 1;
			}
			else
			{
				posEnd = colText.find_last_of(" .,/\\", posEnd - 1);

				if (posEnd != string::npos && posStart <= posEnd)
				{
					string line = colText.substr(posStart, posEnd - posStart + 1);
					//cout << "wrapColumn()2: line=" << line << ", posStart=" << posStart << ", posEnd=" << posEnd << endl;
					col.push_back(line);
					posStart = posEnd + 1;
				}
				else
				{
					posEnd = posStart + colWidth;
					string line = colText.substr(posStart, posEnd - posStart);
					//cout << "wrapColumn()3: line=" << line << ", posStart=" << posStart << ", posEnd=" << posEnd << endl;
					col.push_back(line);
					posStart = posEnd;
				}
			}

			posEnd = posStart + colWidth;
		}

		if (posStart < colText.size())
		{
			string line = colText.substr(posStart);
			//cout << "wrapColumn()4: line=" << line << ", posStart=" << posStart << ", posEnd=" << posEnd << endl;
			col.push_back(line);
		}
		DEBUG("%s"," Leaving FormatterTxt::Table::wrapColumn(");
		return col.size();
	}

	//================================================================================
	// Class FormatterTxt::Table::Column
	//================================================================================

	FormatterTxt::Table::Column::Column()
		: header(0)
		, width (0)
		, align (/*Align::*/LEFT)
		, sepCol(0)
		, sepRow(0)
	{
		DEBUG("%s","In  FormatterTxt::Table::Column::Column()");
	}

	FormatterTxt::Table::Column::Column(const char* header, int width, Align align, const char* sepCol, const char* sepRow)
		: header(header)
		, width (width)
		, align (align)
		, sepCol(sepCol)
		, sepRow(sepRow)
	{
		DEBUG("%s","In  FormatterTxt::Table::Column::Column(");
	}

	bool FormatterTxt::Table::Column::isValid() const
	{
		DEBUG("%s"," return from FormatterTxt::Table::Column::isValid(");
		return (this->header != 0);
	}
}
