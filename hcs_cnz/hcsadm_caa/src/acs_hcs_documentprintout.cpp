//======================================================================
//
// NAME
//      DocumentPrintout.cpp
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
//      2011-05-03 by EEDKBR
// CHANGES
//     
//======================================================================

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include<stdlib.h>
#include <ace/ACE.h>

#include "acs_hcs_exception.h"
#include "acs_hcs_documentxml.h"
#include "acs_hcs_documentprintout.h"

using namespace std;

namespace AcsHcs
{
	//================================================================================
	// Class DocumentPrintout
	//================================================================================

	const string DocumentPrintout::_printoutNumber = "printoutNumber";

	// constructor: initialize some variables
	DocumentPrintout::DocumentPrintout()
	{
		//DEBUG("%s","Entering DocumentPrintout::DocumentPrintout()");
		_currentSector = -1;
		_maxSector = -1;
		_sectorOpen = false;
		_currentPrintout = 0;
		//DEBUG("%s","Leaving DocumentPrintout::DocumentPrintout()");
	}

	// destructor: clear the collection
	DocumentPrintout::~DocumentPrintout()
	{
		//DEBUG("%s","Entering DocumentPrintout Destructor");
		_collection.clear();
		//DEBUG("%s","Leaving DocumentPrintout Destructor");
	}

	// close an object, for new data a new sector is opened
	void DocumentPrintout::closeSector()
	{
		//DEBUG("%s","Entering DocumentPrintout::closeSector(");
		_sectorOpen = false;
		//DEBUG("%s","Leaving  DocumentPrintout::closeSector()");
	}

	// re-open an object for taking more data, search for the object with the according <key, value> pair
	// this search is done by some filtering of the references
	void DocumentPrintout::adjustSector( string key, string value )
	{
		//DEBUG("%s","Entering DocumentPrintout::adjustSector()");
		//char buffer[3];
		//string poValue = itoa(_currentPrintout, buffer, 10);
		//snprintf(buffer, 3, "%d", _currentPrintout);
		stringstream ss;
		ss << _currentPrintout;
		string poValue = ss.str();
		DocumentPrintout::References references, refsCurrentPrintout;
		copySectorsKeyValue( references, key, value, EQUAL );	// filter all references with key, value
		copySectorsKeyValue( refsCurrentPrintout, _printoutNumber, poValue, EQUAL  ); // filter current printout
		references.intersect( refsCurrentPrintout );					// intersect the results

		if ( !references.isEmpty())										// found an object to add the data
		{
			_currentSector = references.getFirstReference();
			_sectorOpen = true;											// mark the object as open, new data will be filled in
		}
		//DEBUG("%s","Leaving DocumentPrintout::adjustSector(");
	}

	// printout ended, a new printout will start if new data come in
	void DocumentPrintout::closePrintout()
	{
		//DEBUG("%s","Entering DocumentPrintout::closePrintout(");
		_currentPrintout++;				
		//DEBUG("%s","Leaving DocumentPrintout::closePrintout()");								
	}

	// check if there is a key for association of different tables
	bool DocumentPrintout::hasKey( string key ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::hasKey");
		PrintoutCollection::const_iterator it;
		it = _collection.find( key );
		// if key found, return true, else false
		//DEBUG("%s","Leaving DocumentPrintout::hasKey(");
		return ( it != _collection.end());
	}

	// list all values found for a key
	void DocumentPrintout::getValues( std::set<string>& values, const string& key ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::getValues");
		PrintoutCollection::const_iterator it;
		it = _collection.find( key );

		Values value = it->second;
		value.getValues( values );
		//DEBUG("%s","Leaving DocumentPrintout::getValues");
	}



	// add a key and a value to the current object
	void DocumentPrintout::add( string key, string value )
	{
		//DEBUG("%s","Entering  void DocumentPrintout::add(");
		string tempPrintout = "";
		
		// if old sector was closed, start a new one
		if ( !_sectorOpen )
		{
			char buffer[3];
			_sectorOpen = true;
			_currentSector = ++_maxSector;

			//add( "printoutNumber", itoa(_currentPrintout, buffer, 10) );
			stringstream ss;
			ss << _currentPrintout;
			tempPrintout = ss.str();		
			add("printoutNumber", tempPrintout);
			//_maxSector = ( _currentSector > _maxSector ) ? _currentSector : _maxSector;		
		}

		PrintoutCollection::iterator it;
		it = _collection.find( key );
		// if key already known, add value and sector to it
		if ( it != _collection.end())
		{
			it->second.addValue( value, _currentSector );
		}
		// create a new collection
		else
		{	
			Values values;
			values.addValue( value, _currentSector );
			_collection[key] = values;
		}
		//DEBUG("%s","Leaving  void DocumentPrintout::add(");
	}

	// clear the memory of the printout
	void DocumentPrintout::clear()
	{
		//DEBUG("%s","Entering DocumentPrintout::clear()");
		PrintoutCollection::iterator it;

		for( it = _collection.begin(); it!=_collection.end(); it++ )
		{
			it->second.clear();
		}

		_collection.clear();
		//DEBUG("%s","Leaving DocumentPrintout::clear()");
	}

	// print the collected data to cout. Useful for diagnostics, only.
	void DocumentPrintout::printAll() const
	{
		//DEBUG("%s","Entering  DocumentPrintout::printAll(");
		PrintoutCollection::const_iterator it;

		for ( it = _collection.begin(); it!=_collection.end(); it++ )
		{
			cout << endl << "key: " << it->first <<endl;
			it->second.printAll();
		}
		//DEBUG("%s","Leaving DocumentPrintout::printAll()");
	}

	// enhance object references: All all references that belong to special key
	void DocumentPrintout::copySectorsKey( DocumentPrintout::References& references, const string& key ) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::copySectorsKey(");
		PrintoutCollection::const_iterator it = _collection.find(key);  // find the collection for the key

		if ( it != _collection.end())								    // collection found
		{
			Values values = it->second;
			values.copySectors( references );						    // add all references found to object references
		}
		//DEBUG("%s","Leaving  DocumentPrintout::copySectorsKey(");
		return;
	}

	// enhance object references: All all references that belong to special key and value pair (value being a string type)
	void DocumentPrintout::copySectorsKeyValue( DocumentPrintout::References& references, const string& key, const string& value, Operator op ) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::copySectorsKeyValue(");
		PrintoutCollection::const_iterator it = _collection.find(key);  // find the collection for the key
		if ( it != _collection.end())									// collection found
		{
			Values values = it->second;
			values.copySectorsValue( references, value, op );			// copy all references valid for value
		}
		//DEBUG("%s","Leaving  DocumentPrintout::copySectorsKeyValue(");
		return;
	}

	// enhance object references: Copy all references that belong to a special key where the value is an a special range )

	void DocumentPrintout::copySectorsKeyValueRange( DocumentPrintout::References& references, const string& key, 
		const string& minimum, const string& maximum, bool numeric ) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::copySectorsKeyValueRange(");
		PrintoutCollection::const_iterator it = _collection.find(key);  // find the collection for the key

		if ( it != _collection.end())									// collection found
		{
			Values values = it->second;
			values.copySectorsValueRange( references, minimum, maximum, numeric );			// copy all references valid for value
		}
		//DEBUG("%s","Leaving  DocumentPrintout::copySectorsKeyValueRange(");
		return;
	}

	// enhance object references: All all references that belong to special key and value pair (value being a double type)
	void DocumentPrintout::copySectorsKeyValue( DocumentPrintout::References& references, const string& key, double value, Operator op ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::copySectorsKeyValue(");
		PrintoutCollection::const_iterator it = _collection.find(key);

		if ( it != _collection.end())
		{
			Values values = it->second;
			values.copySectorsValue( references, value, op );
		}
		//DEBUG("%s","Leaving DocumentPrintout::copySectorsKeyValue(");
		return;
	}

	// recursively evaluate the filter that is found in node
	// return all references that are found in the printout that fit to the filter
	void DocumentPrintout::evaluateNode(DocumentPrintout::References& references, const DocumentXml::Node& node) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNode(");
		if (!node.isValid() )
		 // not a valid node, filtering stops
		{
			//DEBUG("%s","Leaving DocumentPrintout::evaluateNode(");
			return;
		}

		evaluateNodeFilter(references, node.getFirstChild()); // take the first child of the filter for futher filtering
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNode(");
	}

	// evaluate the filter according to the filter's name
	void DocumentPrintout::evaluateNodeFilter(DocumentPrintout::References& references, const DocumentXml::Node& filter) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeFilter(");
		if (!filter.isValid())
		{
			//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeFilter(D");
			return;
		}
		string nodeName = filter.getName();

		if (nodeName == "intersect")
		{ 
			evaluateNodeIntersect(references, filter);		// evaluate filter, intersect with references 															
		}
		else if (nodeName == "unite")
		{ 
			evaluateNodeUnite(references, filter);			// evaluate filter , unite references
		}
		else if (nodeName == "equal")
		{ 
			evaluateNodeEqual(references, filter);			// evaluate filter equal
		}
		else if (nodeName == "not-equal")
		{ 
			evaluateNodeNotEqual(references, filter);		// evaluate filter not-equal
		}
		else if (nodeName == "greater")
		{ 
			evaluateNodeGreater(references, filter);		// evaluate filter greater
		}
		else if (nodeName == "less")
		{ 
			evaluateNodeLess(references, filter);			// evaluate filter less
		}
		else if (nodeName == "match")
		{ 
			evaluateNodeMatch(references, filter);			// evaluate filter match
		}
		else if (nodeName == "range" )
		{
			evaluateNodeRange(references, filter);			// evaluate filter range`
		}
		else												// unknown filter name, error
		{
			cout << "Undefined tag" << endl;
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeFilter()", "Undefined tag '" << nodeName << "'.", "");
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeFilter(D");
		return;
	}

	// evaluate the filter and build intersection with input references
	void DocumentPrintout::evaluateNodeIntersect(DocumentPrintout::References& references, const DocumentXml::Node& intersect) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::evaluateNodeIntersect(");
		const DocumentXml::Node& child = intersect.getFirstChild();

		// one operand is needed
		if (!child.isValid())
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeIntersect()", "Invalid number of operands: operator 'intersect' must have at least one operand: " << intersect, "");

		// evaluate first operand (first child)
		this->evaluateNodeFilter(references, child);

		// evaluate all further operands (siblings) and intersect them with references
		for (DocumentXml::Node sibling = child.getNextSibling(); !references.isEmpty() && sibling.isValid(); sibling = sibling.getNextSibling())
		{
			References tmpReferences;
			this->evaluateNodeFilter(tmpReferences, sibling);
			references.intersect(tmpReferences);
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeIntersect(");
		return;
	}

	// evaluate the filter and unite with input references
	void DocumentPrintout::evaluateNodeUnite(DocumentPrintout::References& references, const DocumentXml::Node& unite) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::evaluateNodeUnite(");
		const DocumentXml::Node& child = unite.getFirstChild();

		// one operand is needed
		if (!child.isValid())
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeUnite()", "Invalid number of operands: operator 'unite' must have at least one operand: " << unite, "");

		// evaluate the first operand (first child)
		this->evaluateNodeFilter(references, child);

		// evaluate all siblings and unite with references
		for (DocumentXml::Node sibling = child.getNextSibling(); sibling.isValid(); sibling = sibling.getNextSibling())
		{
			References tmpReferences;
			this->evaluateNodeFilter(tmpReferences, sibling);
			references.unite(tmpReferences);
		}
		//DEBUG("%s","Leaving  DocumentPrintout::evaluateNodeUnite(");
		return;
	}

	// evaluate filter equal and store result in references
	void DocumentPrintout::evaluateNodeEqual(DocumentPrintout::References& references, const DocumentXml::Node& equal) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::evaluateNodeEqual(");
		// name and value must be found in the filter
		try
		{
			const string& strName  = equal.getAttribute("name").getValue();
			const string& strValue = equal.getAttribute("value").getValue();

			if (strName.empty() || strValue.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeEqual()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'value': " << equal, "");

			// add all references that fit to name and value
			copySectorsKeyValue(references, strName, strValue, EQUAL);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeEqual()", "Invalid number of arguments. Required arguments: 'name', 'value': " << equal, "");
		}
		//DEBUG("%s","Leaving  DocumentPrintout::evaluateNodeEqual(");
	}

	// evaluate filter not equal and store result in references
	void DocumentPrintout::evaluateNodeNotEqual(DocumentPrintout::References& references, const DocumentXml::Node& notEqual) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeNotEqual");
		// name and value must be found in the filter
		try
		{
			const string& strName  = notEqual.getAttribute("name").getValue();
			const string& strValue = notEqual.getAttribute("value").getValue();

			if (strName.empty() || strValue.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeEqual()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'value': " << notEqual, "");

			// add all references that fit to name and value
			copySectorsKeyValue(references, strName, strValue, DocumentPrintout::/**/NOTEQUAL);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeEqual()", "Invalid number of arguments. Required arguments: 'name', 'value': " << notEqual, "");
		}
		//DEBUG("%s","Leaving  DocumentPrintout::evaluateNodeNotEqual(");	
	}

	// evaluate filter greater and store result in references
	// greater can be performed for strings and for numbers
	void DocumentPrintout::evaluateNodeGreater(DocumentPrintout::References& references, const DocumentXml::Node& greater) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeGreater(");
		try
		{
			const string& strName  = greater.getAttribute("name").getValue();
			const string& strValue = greater.getAttribute("value").getValue();

			if (strName.empty() || strValue.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeGreater()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'value', ['type']: " << greater, "");

			DocumentXml::Node dataType = greater.getAttribute("type", false);

			if (!dataType.isValid() || dataType.getValue() != "string") // Treat value as number
			{
				double num = /*ACE_OS::*/atof(strValue.c_str());

				if (num == 0.0 && strValue[0] != '0')
					ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeGreater()", "Could not perform 'greater' operation. Cause: Invalid number '" << strValue << "': " << greater, "");

				// add all references that fit to key=strName and value > num
				copySectorsKeyValue(references, strName, num, GREATER);
			}
			else // Treat value as string
			{
				// add all references that fit to key=strName and value > strValue
				copySectorsKeyValue(references, strName, strValue, GREATER);
			}
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeGreater()", "Invalid number of arguments. Required arguments: 'name', 'value', ['type']: " << greater, "");
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeGreater(");
	}

	// evaluate filter less and store result in references
	// less can be performed for strings and for numbers
	void DocumentPrintout::evaluateNodeLess(DocumentPrintout::References& references, const DocumentXml::Node& less) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeLess(");
		try
		{
			const string& strName  = less.getAttribute("name").getValue();
			const string& strValue = less.getAttribute("value").getValue();

			if (strName.empty() || strValue.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeLess()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'value', ['type']: " << less, "");

			DocumentXml::Node dataType = less.getAttribute("type", false);

			if (!dataType.isValid() || dataType.getValue() != "string") // Treat as number
			{
				double num = /*ACE_OS::*/atof(strValue.c_str());

				if (num == 0.0 && strValue[0] != '0')
					ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeLess()", "Could not perform 'less' operation. Cause: Invalid number '" << strValue << "': " << less, "");

				// add all references that fit to key=strName and value < num
				copySectorsKeyValue(references, strName, num, LESS);
			}
			else // Treat as string
			{
				// add all references that fit to key=strName and value < strValue
				copySectorsKeyValue(references, strName, strValue, LESS);
			}
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeLess()", "Invalid number of arguments. Required arguments: 'name', 'value', ['type']: " << less, "");
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeLess(");
	}

	// evaluate filter match and store result in references
	void DocumentPrintout::evaluateNodeMatch(DocumentPrintout::References& references, const DocumentXml::Node& match) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeMatch(");
		try
		{
			const string& strName  = match.getAttribute("name").getValue();
			const string& strValue = match.getAttribute("value").getValue();

			if (strName.empty() || strValue.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeMatch()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'value': " << match, "");

			// add all references that fit to key = strName and match to strValue
			copySectorsKeyValue(references, strName, strValue, MATCH);
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeMatch()", "Invalid number of arguments. Required arguments: 'name', 'value': " << match, "");
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeMatch(");
	}

	// evaluate filter range and store result in references
	void DocumentPrintout::evaluateNodeRange(DocumentPrintout::References& references, const DocumentXml::Node& range) const
	{
		//DEBUG("%s","Entering DocumentPrintout::evaluateNodeRange(");
		try
		{
			const string& strName = range.getAttribute("name").getValue();
			const string& strMin  = range.getAttribute("min").getValue();
			const string& strMax  = range.getAttribute("max").getValue();

			if (strName.empty() || strMin.empty() || strMax.empty())
				ACS_HCS_THROW(DocumentPrintout::ExceptionArgumentInvalid, "evaluateNodeRange()", "Invalid arguments. Arguments must not be empty. Required arguments: 'name', 'min', 'max', ['type']: " << range, "");

			DocumentXml::Node dataType = range.getAttribute("type", false);

			if (!dataType.isValid() || dataType.getValue() != "string") // Treat as number
			{
				copySectorsKeyValueRange(references, strName, strMin, strMax, true);
			}
			else // Treat as string
			{
				copySectorsKeyValueRange(references, strName, strMin, strMax, false);
			}
		}
		catch (const DocumentXml::ExceptionAttributeUndefined&)
		{
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "evaluateNodeRange()", "Invalid number of arguments. Required arguments: 'name', 'min', 'max', ['type']: " << range, "");
		}
		//DEBUG("%s","Leaving DocumentPrintout::evaluateNodeRange(");
	}


	//================================================================================
	// Class DocumentPrintout::Values
	//================================================================================


	/** @brief add a reference for the value, in case value is not yet referenced, 
	* then it is created.
	* param[in] value the value to get a new sector reference
	* param[in] the sector to be added 
	*/	
	void DocumentPrintout::Values::addValue( string& value, int sector )
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::addValue(");
		ValueCollection::iterator it;
		it = _references.find( value );		// search value in _references collection
		if ( it != _references.end())		// value found
		{
			it->second.addSector(sector);	// add sector to references
		}
		else
		{
			References references;			 // create new references
			references.addSector(sector);	 // add sector to references
			_references[value] = references; // add value to _references collection
		}
		//DEBUG("%s","Leaving DocumentPrintout::Values::addValue(");
	}


	// get the value that has a specified sector
	string DocumentPrintout::Values::getValueForSector( int sector )
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::getValueForSector(");
		ValueCollection::iterator it;

		for ( it = _references.begin(); it!=_references.end(); it++ )
		{
			if ( it->second.hasReference( sector ))
			{
				//DEBUG("%s","Leaving DocumentPrintout::Values::getValueForSector(");
				return it->first;
			}
		}
		//DEBUG("%s","Leaving DocumentPrintout::Values::getValueForSector(");
		return "";
	}


	// clear the references inside a Value
	void DocumentPrintout::Values::clear()
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::clear()");
		ValueCollection::iterator it;

		for( it = _references.begin(); it!=_references.end(); it++ )
		{
			it->second.clear();
		}

		_references.clear();
		//DEBUG("%s","Leaving DocumentPrintout::Values::clear()");
	}


	// print all references to cout, used for diagnotics
	void DocumentPrintout::Values::printAll() const
	{
		//DEBUG("%s","Entering  DocumentPrintout::Values::printAll()");
		for ( ValueCollection::const_iterator it = _references.begin(); it!=_references.end(); it++ )
		{
			cout << "    value: " << it->first << endl;
			it->second.printAll();
		}
		//DEBUG("%s","Leaving  DocumentPrintout::Values::printAll()");
	}

	// copy all references to Values
	void DocumentPrintout::Values::copySectors( DocumentPrintout::References& references )
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::copySectors(");
		for ( ValueCollection::iterator it = _references.begin(); it!=_references.end(); it++ )
		{
			references.addSectors( it->second );
		}
		//DEBUG("%s","Leaving DocumentPrintout::Values::copySectors(");
		return;
	}

	// copy all sectors that fit to a specific value according to the specified operator, values of type string
	void DocumentPrintout::Values::copySectorsValue( DocumentPrintout::References& references, const string& value, DocumentPrintout::Operator op )
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::copySectorsValue(");
		ValueCollection::iterator it;

		switch (op)
		{
		// operator EQUAL, take all references for the value
		case DocumentPrintout::EQUAL :		
			it = _references.find( value );	// search the value
			if ( it != _references.end() )								// if found
				references.addSectors( it->second );					// copy all sectors
			
			break;
		
		// operator GREATER, take all references for the value
		case DocumentPrintout::GREATER :
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;		

				if (key.compare(value) > 0)								// if key > value copy all sectors
					references.addSectors( it->second );
			}
			break;

		// operator LESS, take all references for the value
		case DocumentPrintout::LESS :							
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;

				if (key.compare(value) < 0)								// if key < value copy all sectors
					references.addSectors( it->second );
			}
			break;

		// operator MATCH, take all references for the value
		// value can be found at the start, at the end or somewhere in the key
		case DocumentPrintout::MATCH :
			{
				string str = value;
				
				bool matchFromStart = (*(str.begin())  == '^');				// check if match starts at the start
				bool matchTillEnd   = (*(str.rbegin()) == '^');				// check if match to the end

				str.erase(str.find_last_not_of("^") + 1).erase(0, str.find_first_not_of("^"));

				for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
				{
					const string& key = (*it).first;

					if (key.size() < str.size())
					{
						// key too short, take next reference
						continue;
					}
					if (matchFromStart && matchTillEnd)						  // match from start and end is same as equal	  
					{
						if (key.compare(str) == 0)
							references.addSectors(it->second);
					}
					else if (matchFromStart)								  // check if string is found at the start
					{
						if (key.find(str.c_str(), 0, str.size()) != string::npos)
							references.addSectors(it->second);
					}
					else if (matchTillEnd)									  // check if string is found at the end
					{
						if (key.find(str.c_str(), key.size() - str.size(), str.size()) != string::npos)
							references.addSectors(it->second);
					}
					else													  // check if string is found somewhere
					{
						if (key.find(str) != string::npos)
							references.addSectors(it->second);
					}			
				}
			}
			break;

			// operator NOTEQUAL, take all references for the value
		case DocumentPrintout::NOTEQUAL :							
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;

				if (key.compare(value) != 0)								// if key < value copy all sectors
					references.addSectors( it->second );
			}
			break;

		default :
			break;
			;
		}
		//DEBUG("%s","Leaving DocumentPrintout::Values::copySectorsValue(");
		return;
	}

	// copy all sectors that fit to a specific value according to the specified operator, values of type double
	void DocumentPrintout::Values::copySectorsValue( DocumentPrintout::References& references, double value, Operator op )
	{
		//DEBUG("%s","Entering  DocumentPrintout::Values::copySectorsValue(");
		ValueCollection::iterator it;

		switch (op)
		{
		// operator GREATER, take all references for the value
		case DocumentPrintout::GREATER :
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;

				if (/*ACE_OS::*/atof(key.c_str()) > value)					// if key > value copy all sectors
					references.addSectors(it->second);
			}
			break;

		// operator LESS, take all references for the value
		case DocumentPrintout::LESS :
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;

				if (/*ACE_OS::*/atof(key.c_str()) < value)					  // if key < value copy all sectors
					references.addSectors(it->second);
			}
			break;

		default :
			;
		}
		//DEBUG("%s","Leaving  DocumentPrintout::Values::copySectorsValue(");
		return;
	}

		// copy all sectors that fit into a specific range
	void DocumentPrintout::Values::copySectorsValueRange( DocumentPrintout::References& references, const string& minimumStr, const string& maximumStr, bool numeric )
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::copySectorsValueRange( ");

		ValueCollection::iterator it;
		if ( numeric )
		{
			double minimum = /*ACE_OS::*/atof(minimumStr.c_str());
			double maximum = /*ACE_OS::*/atof(maximumStr.c_str());

			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				const string& key = (*it).first;
				double value = /*ACE_OS::*/atof(key.c_str());

				if ( (minimum < value) && (value < maximum ))
					references.addSectors(it->second);
			}
		}
		else
		{
			for (it = _references.begin(); it != _references.end(); ++it) // scan all references in the value collection
			{
				if (( minimumStr.compare( (*it).first ) <= 0 ) && ( maximumStr.compare( (*it).first  ) >= 0 ))
					references.addSectors(it->second);
			}
		}
		//DEBUG("%s","LEaving DocumentPrintout::Values::copySectorsValueRange( ");
		return;
	}

	// list all values inside Values
	void DocumentPrintout::Values::getValues( std::set<string>& values ) 
	{
		//DEBUG("%s","Entering DocumentPrintout::Values::getValues( std::set<string>& values )");
		ValueCollection::iterator it;

		for ( it = _references.begin(); it != _references.end(); ++it )
		{
			values.insert( it->first );
		}
		//DEBUG("%s","Leaving DocumentPrintout::Values::getValues( std::set<string>& values )");
	}



	//================================================================================
	// Class DocumentPrintout::References
	//================================================================================


	// class References::Iterator exports the iterator of sectors.
	DocumentPrintout::References::Iterator::Iterator()
	{
	}

	DocumentPrintout::References::Iterator::Iterator(DocumentPrintout::Sectors::const_iterator& it) : it(it)
	{
	}
        DocumentPrintout::References::Iterator::Iterator(const DocumentPrintout::Sectors::const_iterator& it) : it(it)
        {
        }


	DocumentPrintout::References::Iterator::~Iterator()
	{
	}

	int DocumentPrintout::References::Iterator::operator*() const
	{
		//DEBUG("%s","Leaving  DocumentPrintout::References::Iterator::operator*() ");
		return *(this->it);
	}

	bool DocumentPrintout::References::Iterator::operator==(const Iterator& right) const
	{
		//DEBUG("%s","Leaving  DocumentPrintout::References::Iterator::operator*() ");
		return (this->it == right.it);
	}

	bool DocumentPrintout::References::Iterator::operator!=(const Iterator& right) const
	{
		//DEBUG("%s","LEaving  DocumentPrintout::References::Iterator::operator==");
		return (!(*this == right));
	}

	DocumentPrintout::References::Iterator DocumentPrintout::References::begin() const
	{
		//DEBUG("%s","Leaving   DocumentPrintout::References::Iterator DocumentPrintout::References::begin()");
		return this->_sectors.begin();
	}

	DocumentPrintout::References::Iterator DocumentPrintout::References::end() const
	{
		//DEBUG("%s","Leaving DocumentPrintout::References::Iterator& DocumentPrintout::References::Iterator::operator++(");
		return this->_sectors.end();
	}

	DocumentPrintout::References::Iterator& DocumentPrintout::References::Iterator::operator++()
	{
		//DEBUG("%s","Entering DocumentPrintout::References::Iterator& DocumentPrintout::References::Iterator::operator++(");
		++(this->it);
		//DEBUG("%s","Leaving DocumentPrintout::References::Iterator& DocumentPrintout::References::Iterator::operator++(");
		return *this;
	}


	// add a sector to References
	void DocumentPrintout::References::addSector( int sector )
	{
		//DEBUG("%s","Entering DocumentPrintout::References::addSector(");
		_sectors.insert(sector);
		//DEBUG("%s","Leaving DocumentPrintout::References::addSector(");
	}

	// print all sectors to cout, for diagnostics, only
	void DocumentPrintout::References::printAll() const
	{
		//DEBUG("%s","Entering  DocumentPrintout::References::clear()");
		cout << "     sectors:";

		for ( set<int>::const_iterator it = _sectors.begin(); it!=_sectors.end(); it++)
		{
			cout  << " value " << *it ;
		}

		cout << endl;
		//DEBUG("%s","Leaving DocumentPrintout::References::printAll(");
	}

	// clear the internal set of references
	void DocumentPrintout::References::clear()
	{
		//DEBUG("%s","Entering  DocumentPrintout::References::clear()");
		_sectors.clear();
		//DEBUG("%s","LEaving  DocumentPrintout::References::clear()");
	}

	// add all sectors from another references object
	void DocumentPrintout::References::addSectors( const DocumentPrintout::References& references )
	{
		//DEBUG("%s","Entering DocumentPrintout::References::addSectors( ");
		for ( Sectors::const_iterator it = references._sectors.begin(); it != references._sectors.end(); it++ )
		{
			this->addSector( *it );
		}
		//DEBUG("%s","LEaving DocumentPrintout::References::addSectors(");
	}

	// add all sectors from another references object (same as unite)
	void DocumentPrintout::References::unite( const DocumentPrintout::References& references )
	{
		//DEBUG("%s","Entering   DocumentPrintout::References::unite");
		this->addSectors( references );
		//DEBUG("%s","Leaving  DocumentPrintout::References::unite");
	}

	// check whether a specified integer is in the sectors
	bool DocumentPrintout::References::hasReference( const int i ) const
	{
		//DEBUG("%s","Leaving  DocumentPrintout::References::hasReference(");
		return (_sectors.find( i ) != _sectors.end() );	
	}

	// build intersection of own sectors with sectors of an other references, Store result in this
	void DocumentPrintout::References::intersect( const DocumentPrintout::References& references ) 
	{
		//DEBUG("%s","Entering  DocumentPrintout::References::intersect(");
		Sectors::iterator ahead = _sectors.begin();
		Sectors::iterator it;

		while ( ahead != _sectors.end())	// scan all objects in this sector
		{	
			it = ahead;
			ahead++;
			Sectors::const_iterator jt = references._sectors.find( *it ); // search found object in other references

			if ( jt == references._sectors.end() )		// if not found in other, then delete in this
			{
				_sectors.erase(*it);
			}
		}
		//DEBUG("%s","Leaving  DocumentPrintout::References::intersect(");
		return;
	}

	// build the set operation complement (references - this)
	// e.g. take out all objects from this which are objects of other.
	void DocumentPrintout::References::complement( const DocumentPrintout::References& references )
	{
		//DEBUG("%s","iEntering DocumentPrintout::References::complement(");
		Sectors::const_iterator it; 

		for ( it = references._sectors.begin(); it != references._sectors.end(); it++ ) // scan other
		{
			if (this->hasReference( *it ))				// found in other
			{
				_sectors.erase( _sectors.find(*it));	// take out from this
			}
		}
		//DEBUG("%s","Leaving DocumentPrintout::References::complement(");
	}

	// check if two references have the same sectors
	bool DocumentPrintout::References::isEqual( const DocumentPrintout::References& references ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::References::isEqual(");
		if ( _sectors.size() != references._sectors.size() )
		{
			//DEBUG("%s","Leaving DocumentPrintout::References::isEqual(");
			return false;  // different size ==> not equal
		}
		// sets are ordered, so we can scan in parallel
		Sectors::const_iterator my = _sectors.begin();						
		Sectors::const_iterator other = references._sectors.begin();

		for ( int i = 0; i < (int) _sectors.size(); i++ )
		{
			if ( *my != *other )
			{
				//DEBUG("%s","Leaving DocumentPrintout::References::isEqual(");
				return false;			// different values ==> not equal
			}
			my++;
			other++;
		}
		//DEBUG("%s","LEaving DocumentPrintout::References::isEqual(");
		return true;									// all were the same
	}

	// check is sector of references is empty
	bool DocumentPrintout::References::isEmpty() const
	{
		//DEBUG("%s","Entering DocumentPrintout::References::isEmpty(");
		return _sectors.empty() ;
	}

	// check if sector of this is subset of secton in references
	bool DocumentPrintout::References::isSubset( const DocumentPrintout::References& references ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::References::isSubset(");
		if ( _sectors.size() > references._sectors.size() )
		{
			//DEBUG("%s","Leaving DocumentPrintout::References::isSubset(");
			return false; // too many elements==> not subset
		}
		Sectors::const_iterator other = references._sectors.begin();

		for ( Sectors::const_iterator my = _sectors.begin(); my != _sectors.end(); my++ ) // scan this sector
		{
			if ( !references.hasReference( *my ))
			{
				//DEBUG("%s","LEaving DocumentPrintout::References::isSubset(");
				return false;	
			}		 // one of this is not in references==> not subset
		}
		//DEBUG("%s","Leaving DocumentPrintout::References::isSubset(");
		return true;			// no missing element found ==> subset
	}

	//================================================================================
	// Class DocumentPrintout::Filter
	//================================================================================

	// print all references of a filter
	void DocumentPrintout::Filter::printAll() const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::printAll()");
		_references.printAll();
		//DEBUG("%s","Leaving DocumentPrintout::Filter::printAll()");
	}

	DocumentPrintout::Filter::~Filter()
	{
	}

	// Filter constructor: take a documentPrintout, and a filternode in the rule set file
	// implicitly evaluate the complete filter
	DocumentPrintout::Filter::Filter(const DocumentPrintout& docPrintout, const DocumentXml::Node& ruleCheckFilter)
		: _docPrintout(docPrintout)
	{
		//DEBUG("%s","Entering  DocumentPrintout::Filter::Filter");
		// filter is empty, take the printout and copy the references from the printout to the filter
		if (ruleCheckFilter == 0)
		{
			for ( int i = 0; i <= _docPrintout._maxSector; i++  )
			{
				_references.addSector( i );
			}
		}
		else
		{
			// evaluate the filter
			_docPrintout.evaluateNode( _references, ruleCheckFilter );
		}
		//DEBUG("%s","Leaving  DocumentPrintout::Filter::Filter");
	}

	// check if references of two filters are equal
	bool DocumentPrintout::Filter::isEqual(const Filter& filter) const
	{
		//DEBUG("%s","Leaving DocumentPrintout::Filter::isEqual(");
		return ( _references.isEqual( filter._references ));
	}
	
	// check if references of filters is equal to references
	bool DocumentPrintout::Filter::isEqual(const References& references) const
	{
		//DEBUG("%s","LEaving DocumentPrintout::Filter::isEqual");
		return ( _references.isEqual( references ));
	}

	// check if this filter is a subset of other filter
	bool DocumentPrintout::Filter::isSubset(const Filter& filter) const
	{
		//DEBUG("%s","Leaving DocumentPrintout::Filter::isSubset");
		return ( _references.isSubset( filter._references ));
	}

	// check if filter has no references
	bool DocumentPrintout::Filter::isEmpty() const
	{
		//DEBUG("%s","LEaving DocumentPrintout::Filter::hasReference");
		return ( _references.isEmpty());
	}

	// check if filter has a specified reference
	bool DocumentPrintout::Filter::hasReference( int i ) const
	{
		//DEBUG("%s","Leaving DocumentPrintout::Filter::hasReference");
		return ( _references.hasReference( i ));
	}

	// check if all times are in a specified period
	// if filter is empty, then the method returns true
	// the keys for data and time can be set by the caller
	// if they are not set then the method checks for keys DATA and TIME or OUTPUTTIME
	// if none of these are found, the method returns false
	bool DocumentPrintout::Filter::isInPeriod
		( 
		time_t pLength, 
		const Filter& actTime, 
		string& dateKey , 
		string& timeKey ,
		bool checkAll
		) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::isInPeriod");
		if (isEmpty())
		{
			//DEBUG("%s","Leaving DocumentPrintout::Filter::isInPeriod");
			return true; // if no element in the filter the result is defined as true
		}
		bool result = false; 

		if ( dateKey.empty() )
			result = getDateTimeKeys( dateKey, timeKey );	// set keys for DATE and TIME

		if ( result )
			result = checkTimes( pLength, actTime, dateKey, timeKey, checkAll );
		//DEBUG("%s","Leaving DocumentPrintout::Filter::isInPeriod");
		return result;
	}



	bool DocumentPrintout::Filter::getDateTimeKeys( string& dateKey, string& timeKey) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::Filter::getDateTimeKeys(");
		// check if the printout has keys DATE and TIME
		// in case it has both ==> OK
		// in case it has only DATA ==> error
		if ( _docPrintout.hasKey( "DATE" ))
		{
			dateKey = "DATE";
			if ( _docPrintout.hasKey( "TIME" )) timeKey = "TIME";
			else 
			{
				//DEBUG("%s","Leaving  DocumentPrintout::Filter::getDateTimeKeys(");
				return false;
			}
			//DEBUG("%s","Leaving  DocumentPrintout::Filter::getDateTimeKeys(");
			return true;
		}
		// in case printout did not find DATE and TIME,
		// it must find OUTPUTTIME, else error
		if ( _docPrintout.hasKey( "OUTPUTTIME" )) 
		{
			dateKey = "OUTPUTTIME";
			timeKey = "";
			//DEBUG("%s","");
			return true;
		}
		// nothing found ==> error
		//DEBUG("%s","Leaving DocumentPrintout::Filter::getDateTimeKeys(");
		return false;
	}

	bool DocumentPrintout::Filter::checkTimes
		( 
		time_t pLength, 
		const Filter& actTime, 
		const string& dateKey, 
		const string& timeKey,
		bool checkAll
		) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::Filter::checkTimes");

		time_t actualTime = actTime.getActTime();

		Values dates = _docPrintout._collection.find( dateKey )->second; 
		Values times;
		if ( !timeKey.empty() )
		{
			times = _docPrintout._collection.find( timeKey )->second;
		}

		// iteration for the references inside filter
                References::Iterator refBegin;
		refBegin = _references.begin();
		for ( References::Iterator& refs = refBegin; refs!= _references.end(); ++refs ) 
		{
			string myDate = dates.getValueForSector( *refs ); // get the value with specified reference inside DATE
			string myTime = "";
			if ( !timeKey.empty())
			{
				myTime = times.getValueForSector( *refs );    // get the value with specified reference inside TIME
			}

			time_t compareTime = calcTime( myDate, myTime );

			if ( checkAll ) // if one time is found outside, then not all times are in the period, return false
			{
				if ( (compareTime < actualTime - pLength) || (compareTime > actualTime) ) // found time not inside the period
				{
					//DEBUG("%s","Leaving  DocumentPrintout::Filter::checkTimes");
					return false;
				}
			}
			else // if one time is found inside, then return true
			{
				if ( (compareTime >= actualTime - pLength) && (compareTime <= actualTime) ) // found time inside the period
				{
					//DEBUG("%s","Leaving  DocumentPrintout::Filter::checkTimes");
					return true;
				}
			}
		}
		//DEBUG("%s","LEaving  DocumentPrintout::Filter::checkTimes");
		return checkAll; // if checkAll is true, then no time was found outside, resulting in true
						 // if checkAll is false, then no time was found inside, resulting in false

	}

	time_t DocumentPrintout::Filter::getActTime() const
	{ 
		//DEBUG("%s","Entering  DocumentPrintout::Filter::getActTime()");
		string dateKey = "";
		string timeKey = "";

		string myTime = "";
		string dateBegin = "";
		bool result = getDateTimeKeys( dateKey, timeKey );

		set<string> dates;
		this->getValues( dates, dateKey );

		if ( dates.size() != 1 )
		{
			//DEBUG("%s","Leaving  DocumentPrintout::Filter::getActTime()");
			return 0; // TODO: error handling
		}
		if ( !timeKey.empty() )
		{
			set<string> times;
			this->getValues( times, timeKey );
			if ( times.size() == 1 ) myTime = *times.begin();
		}
		dateBegin = *dates.begin();
		//DEBUG("%s","Leaving  DocumentPrintout::Filter::getActTime()");
		return calcTime( dateBegin, myTime );
	}

	time_t DocumentPrintout::Filter::calcTime( string& date, string& time ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::calcTime(");
		string myDate = date + time.substr(0,6); // omit milliseconds

		// eliminate blanks
		myDate.erase(myDate.find_last_not_of(" \t\r\n") + 1).erase(0, myDate.find_first_not_of(" \t\r\n"));

		size_t pos = string::npos;

		while ((pos = myDate.rfind(" ", pos)) != string::npos) // delete space for OUTPUTFILE where format is yymmdd hhmmss
			myDate.erase(pos, 1 );
		//DEBUG("%s","Leaving DocumentPrintout::Filter::calcTime(");
		return toTime( myDate );

	}

	time_t DocumentPrintout::Filter::toTime( string& date ) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::Filter::toTime( ");
		struct tm when;

		when.tm_wday = 0;
		when.tm_isdst = 0;
		if ( date.size() == 12 )
		{
			sscanf( date.c_str(), "%02d%02d%02d%02d%02d%02d", &when.tm_year, &when.tm_mon, &when.tm_mday, &when.tm_hour, &when.tm_min, &when.tm_sec );
		}

		if ( date.size() == 10 )
		{
			sscanf( date.c_str(), "%02d%02d%02d%02d%02d", &when.tm_year, &when.tm_mon, &when.tm_mday, &when.tm_hour, &when.tm_min );
			when.tm_sec = 0;
		}
		when.tm_year += 100;
		//DEBUG("%s","Leaving  DocumentPrintout::Filter::toTime( ");
		return mktime( &when );
	}

	// list all values that fit to a key and are references in the filter
	void DocumentPrintout::Filter::getValues( std::set<string>& values, const string& key ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::getValues(");
		Values value = _docPrintout._collection.find( key )->second; 

		DocumentPrintout::References::Iterator refs;
		for ( refs = _references.begin(); refs != _references.end(); ++refs )
		{
			values.insert(value.getValueForSector( *refs ));
		}
		//DEBUG("%s","Leaving DocumentPrintout::Filter::getValues(");
	}
		
	// evaluate all objects where the value in two columns are equal
	void DocumentPrintout::Filter::compareColumns   (DocumentPrintout::References& references, const std::string& columnLeft, 
		const std::string& columnRight, DocumentPrintout::Operator op, const std::string& dataType, const string& percentage ) const 
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::compareColumns   (");
		Values valuesLeft = _docPrintout._collection.find( columnLeft )->second; 
		Values valuesRight = _docPrintout._collection.find( columnRight )->second; 

		for ( DocumentPrintout::References::Iterator it = this->_references.begin(); it != this->_references.end(); ++it )
		{

			if (compareValuesOp(valuesLeft.getValueForSector( *it ), valuesRight.getValueForSector( *it ), op, dataType, percentage ))
			{
				references.addSector( *it );
			}
		}
		//DEBUG("%s","Leaving DocumentPrintout::Filter::compareColumns   (");

	}

	bool DocumentPrintout::Filter::compareValuesOp( const string& leftStr, const string& rightStr, DocumentPrintout::Operator op, 
		const string& dataType, const string& percentage) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::range ( ");
		switch( op )
		{
		case DocumentPrintout::EQUAL:
			//DEBUG("%s","Leaving ocumentPrintout::Filter::compareValuesOp(");
			return	( dataType.compare("number")	== 0 )									?
					( /*ACE_OS::*/atof(leftStr.c_str()) * /*ACE_OS::*/atof(percentage.c_str()) / 100.0 == /*ACE_OS::*/atof(rightStr.c_str()))	:
					( leftStr.compare(rightStr)		== 0 );			
			break;
		case DocumentPrintout::GREATER:
			//DEBUG("%s","Leaving ocumentPrintout::Filter::compareValuesOp(");
			return	( dataType.compare("number")	== 0 )									?
					( /*ACE_OS::*/atof(leftStr.c_str()) > /*ACE_OS::*/atof(rightStr.c_str()) * /*ACE_OS::*/atof(percentage.c_str()) / 100.0 )	:
					( leftStr.compare(rightStr)		> 0 );			
			break;

		case DocumentPrintout::LESS:
			//DEBUG("%s","Leaving ocumentPrintout::Filter::compareValuesOp(");
			return	( dataType.compare("number")	== 0 )									?
					( /*ACE_OS::*/atof(leftStr.c_str()) < /*ACE_OS::*/atof(rightStr.c_str())* /*ACE_OS::*/atof(percentage.c_str()) / 100.0)	:
					( leftStr.compare(rightStr)		< 0 );			

			break;

		default:
			ACS_HCS_THROW(DocumentPrintout::ExceptionFilterSyntax, "compareValuesOp()", "Wrong operation '" << op << "'.", "");
			break;
		}
		//DEBUG("%s","Leaving ocumentPrintout::Filter::compareValuesOp(");
	}

	void DocumentPrintout::Filter::range ( DocumentPrintout::References& references, const std::string& column, const std::string& minNumber, const std::string& maxNumber,
			const std::string& dataType ) const
	{
		//DEBUG("%s","Entering DocumentPrintout::Filter::range ( ");
		Values values = _docPrintout._collection.find( column )->second; 

		for ( DocumentPrintout::References::Iterator it = this->_references.begin(); it != this->_references.end(); ++it )
		{

			if (rangeFits(values.getValueForSector( *it ), minNumber, maxNumber, dataType))
			{
				references.addSector( *it );
			}
		}
		//DEBUG("%s","Leaving DocumentPrintout::Filter::range (  ");
	}


	bool DocumentPrintout::Filter::rangeFits( const std::string& value, const std::string& minNumber, const std::string& maxNumber, 
		const std::string& dataType ) const
	{
		//DEBUG("%s","Entering  DocumentPrintout::Filter::rangeFits(");
		if ( dataType.compare("number")  == 0 )
		{
			double valueDouble = /*ACE_OS::*/atof(value.c_str());
			//DEBUG("%s","Leaving  DocumentPrintout::Filter::rangeFits(");
			return ( (/*ACE_OS::*/atof(minNumber.c_str()) <= valueDouble) && ( valueDouble <=  /*ACE_OS::i*/atoi(maxNumber.c_str())));
		}
		else
		{
			//DEBUG("%s","Leaving  DocumentPrintout::Filter::rangeFits(");
			return (( value.compare(minNumber) >= 0) && ( value.compare(maxNumber) <= 0 ));
		}
	}

}
