//======================================================================
//
// NAME
//      Properties.h
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
//      Implementation of Properties class for config file of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDSTL
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_Properties_h
#define AcsHcs_Properties_h

#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <string>

#include "acs_hcs_tra.h"
#include "acs_hcs_exception.h"

namespace AcsHcs
{
	/**
	* Properties - Representation of a properties file.
	* A properties file always has got the following format:
	* <pre>
	* # Comment describing a property.
	* property.name=property value
	*</pre>
	* This class reads the file and stored the properties in a map.
	* Use method get(const string& propName) to retrieve a property.
	*/
	class Properties
	{
		friend std::ostream& operator<<(std::ostream& os, const Properties& props);

	public:
		/**
		* Properties::ExceptionFileNotFound - Exception indicating non-existing property file.
		* Properties::ExceptionFileNotFound is thrown if the property file does not exist.
		*/
		class ExceptionFileNotFound : public Exception
		{
		public:
			ExceptionFileNotFound(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionFileNotFound() throw() {}
		};

		/**
		* Properties::ExceptionPropertyUndefined - Exception indicating non-existing property.
		* Properties::ExceptionPropertyUndefined is thrown if the property does not exist.
		*/
		class ExceptionPropertyUndefined : public Exception
		{
		public:
			ExceptionPropertyUndefined(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionPropertyUndefined() throw() {}
		};

		/**
		* Constructor.
		* @param[in] path     The path to the properties file, ';' separated.
		* @param[in] fileName The name of the properties file without path.
		* @throws AcsHcs::Properties::ExceptionFileNotFound
		*/
		Properties(const std::string& path, const std::string& fileName);

		/**
		* Destructor.
		*/
		~Properties();

		/**
		* Gets a property of the name passed in 'propName'.
		* @param[in] propName The name of the property to be retrieved.
		* @return The value of the property to be retrieved.
		* @throws AcsHcs::Properties::ExceptionPropertyUndefined
		*/
		std::string get(const std::string& propName) const;

		/**
		* Reads all properties from the properties file.
		* The operation is performed only in the case the file
		* has been updated since it was read last.
		* All properties stored previously are cleared before reading.
		* @throws AcsHcs::Properties::ExceptionFileNotFound
		*/
		void read();

	private:
		static void split(std::list<std::string>& items, std::string list, char sep);

		Properties(const Properties&);
		Properties& operator=(const Properties&);

		std::stringstream& toString(std::stringstream& s) const;

		std::string                        fileName;
		std::map<std::string, std::string> props;
		time_t                             modificationTime;
	};
}

#endif // AcsHcs_Properties_h
