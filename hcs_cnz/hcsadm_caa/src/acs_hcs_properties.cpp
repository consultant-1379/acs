//======================================================================
//
// NAME
//      Properties.cpp
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
//      2011-06-15 by EEDSTL
// CHANGES
//     
//======================================================================

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <map>

#include "acs_hcs_tracer.h"
#include "acs_hcs_directory.h"
#include "acs_hcs_properties.h"

using namespace std;

namespace AcsHcs
{
	//ACS_HCS_TRACE_INIT;

	//================================================================================
	// Friends
	//================================================================================

	ostream& operator<<(ostream& os, const Properties& props)
	{
		stringstream s;

		return os << props.toString(s).str();
	}

	//================================================================================
	// Class Properties
	//================================================================================

	Properties::Properties(const string& path, const string& fileName) : modificationTime(0)
	{
		DEBUG("%s","Entering  Properties::Properties(const string& path, const string& fileName ");
		std::list<std::string> items;

		Properties::split(items, path, ';');

		for (std::list<string>::const_iterator item = items.begin(); item != items.end(); ++item)
		{
			Directory dir(*item);

			if (dir.exists() && dir.exists(fileName))
			{
				this->fileName = (*item) + "/" + fileName;
				break;
			}
		}
		DEBUG("%s","Leaving  Properties::Properties(const string& path, const string& fileName ");
	}

	Properties::~Properties()
	{
		DEBUG("%s","In destructor Properties ");
	}

	string Properties::get(const string& propName) const
	{
		DEBUG("%s"," Entering  string Properties::get");
		map<string, string>::const_iterator it = this->props.find(propName);

		if (it == this->props.end())
			ACS_HCS_THROW(Properties::ExceptionPropertyUndefined, "get()", "Undefined property '" << propName << "'.", "");
		DEBUG("%s","Leaving  string Properties::get ");
		return it->second; 
	}

	void Properties::read()
	{
		DEBUG("%s","Entering Properties::read( ");
		Directory dir("");
		time_t modificationTime = dir.getModificationTime(this->fileName);

		if (modificationTime <= this->modificationTime)
		{
			DEBUG("%s"," return  Property-file has not been updated lately, no need for re-reading");
			// Property-file has not been updated lately, no need for re-reading.
			return;
		}

		this->modificationTime = modificationTime;
		this->props.clear();

		ifstream ifs(this->fileName.c_str());

		if (!ifs.good())
			ACS_HCS_THROW(Properties::ExceptionFileNotFound, "read()", "Property file '" << this->fileName << "' not found.", "");

		try
		{
			while (!ifs.eof())
			{
				string line;
				getline (ifs, line);

				//unsigned int posEqual;
				size_t posEqual;
				if (line[0] == '#' || (posEqual = line.find_first_of("=")) == string::npos)
					continue;

				string str1 = line.substr(0, posEqual);
                                string str2 = line.substr(posEqual + 1);

                                string& key = str1;
                                string& val = str2;
                                
                                //string& key = line.substr(0, posEqual);
				//string& val = line.substr(posEqual + 1);

				key.erase(key.find_last_not_of(" \t\r\n") + 1).erase(0, key.find_first_not_of(" \t\r\n"));
				val.erase(val.find_last_not_of(" \t\r\n") + 1).erase(0, val.find_first_not_of(" \t\r\n"));

				this->props[key] = val;
			}
		}
		catch (...)
		{
			ifs.close();
			throw;
		}

		ifs.close();

		//ACS_HCS_TRACE("read(): Properties read from file '" << this->fileName << "': " << *this);
		cout << "read(): Properties read from file '" << this->fileName << "': " << *this << endl;
		DEBUG("%s","Leaving Properties::read( ");
	}

	void Properties::split(std::list<std::string>& items, string list, char sep)
	{
		DEBUG("%s","Entering  Properties::split( ");
		size_t pos;
		size_t posStart = 0;

		do
		{
			pos = list.find_first_of(&sep, posStart);

			const string& item = list.substr(posStart, pos - posStart);

			posStart = pos + 1;

			items.push_back(item);
		}
		while (posStart);
		DEBUG("%s","Leaving  Properties::split( ");
	}

	stringstream& Properties::toString(stringstream& s) const
	{
		DEBUG("%s","Entering Properties::toString( ");
		s << "{";

		for (map<string, string>::const_iterator prop = this->props.begin(); prop != this->props.end(); ++prop)
		{
			if (prop != props.begin())
				s << ",";

			s << (*prop).first << "='" << (*prop).second << "'";
		}

		s << "}";
		DEBUG("%s","Leaving Properties::toString( ");
		return s;
	}
}
