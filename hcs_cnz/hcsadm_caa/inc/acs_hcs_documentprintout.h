//======================================================================
//
// NAME
//      DocumentPrintout.h
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
//      Implementation of the MML Printout evaluation of HCS.
//
// DOCUMENT NO
//      190 89-CAA 109 0949
//
// AUTHOR
//      2011-06-10 by EEDKBR
// CHANGES
//     
//======================================================================

#ifndef AcsHcs_DocumentPrintout_h
#define AcsHcs_DocumentPrintout_h

#include <string>
#include <set>
#include <map>

#include "acs_hcs_documentxml.h"

namespace AcsHcs
{
	/** This class stores a representation of a printout in the RAM.
	 *  A printout is regarded as a set of objects, where one object consists
	 *  of a number of <type, value> pairs (so-called "parameters") that belong
	 *  together. For instance, if the printout consists of a table then the
	 *  headline of the table defines the types and the following rows define
	 *  the values. Each object consists of the types in the headline plus the
	 *  values of one line.
	 *  The internal structure is as follows:
	 *  map<type, map<value, set<references>>>
	 *  where type and value are the parameters and the references define the
	 *  objects they belong to.
	 *  Filters can be applied on this structure by class Filter. Set operations 
	 *  like unite, intersect, etc. are used to filter the objects.
	 *  
	 */ 

	class DocumentPrintout
	{
	public:
		typedef std::set<int> Sectors;

		enum Operator
		{
			EQUAL,
			GREATER,
			LESS,
			MATCH,
			NOTEQUAL,
			RANGE
		};

		/**
		* DocumentPrintout::ExceptionFilterSyntax - Exception during processing a filter.
		* DocumentPrintout::ExceptionFilterSyntax is thrown in the case of an invalid filter passed.
		*/
		class ExceptionFilterSyntax : public Exception
		{
		public:
			ExceptionFilterSyntax(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionFilterSyntax() throw() {}
		};

		/**
		* DocumentPrintout::ExceptionArgumentInvalid - Exception during filtering values.
		* DocumentPrintout::ExceptionArgumentInvalid is thrown in the case of an invalid argument passed.
		*/
		class ExceptionArgumentInvalid : public Exception
		{
		public:
			ExceptionArgumentInvalid(const std::string& where, const std::string& what, const std::string& cause) : Exception(where, what, cause) {}
			~ExceptionArgumentInvalid() throw() {}
		};

		/** @brief stores references for a value
		* The class belongs to the internal structure for data which are extracted from printouts by
		* the Printout Parser and evaluated by the Rule Checker.
		*/
		class References 
		{
		public:

			/**
			* DocumentPrintout::References::Iterator - Iterator for DocumentPrintout::References.
			*/
			class Iterator 
			{
			public:
				Iterator();
				Iterator(Sectors::const_iterator& it);
				Iterator(const Sectors::const_iterator& it);
				~Iterator();

				int operator*() const;
				bool operator==(const Iterator& right) const;
				bool operator!=(const Iterator& right) const;
				Iterator& operator++();

			private:
				Sectors::const_iterator it;
			};
			/**@brief add a sector to the sectorList
			* @param[in] sector the reference sector to be added
			*/
			void addSector( int sector );

			/**@brief add all sectors in 'references' to this object.
			* @param[in] references
			*/
			void addSectors( const References& references );
			
			/** @brief clear the sectors */
			void clear();
			
			void printAll() const;

			/** build the union of two Sectors 
			* Add all 'references' to this object's references.
			* @param[in] references The reference list to be united with the references of this object.
			*/
			void unite( const References& references);
			
			/** build the intersection of two Sectors 
			* Remove from the this object's references all references not in 'references'.
			* @param[in] references The reference list to be intersected with the references of this object
			*/
			void intersect( const References& references );

			/** checks if a special reference is included in Sectors 
			* @param[in] i the reference to be checked
			* @returns bool true, if the reference is included
			*/
			bool hasReference( const int i ) const;

			/** build the complement of the sector (A-B)
			 *  starting with the complete printout, delete all references which are in 'references'
			 */
			void complement( const References& references );

            /** check if the sectors of the references are equal
			*/

			bool isEqual( const References& references ) const;
			/** check if the sector of the reference is empty
			*/

			bool isEmpty() const;
			/** check if the sector is a subset of the sector of references 
			* @[in] references the references to be checked against
			* @returns true if references is superclass
			* @        false else
			*/
			bool isSubset( const References& references ) const;
			/** give the first reference
			* @returns the first reference
			*/
			int getFirstReference() { return *_sectors.begin(); };

			/** return the size 
			* @returns number of sectors in the references
			*/
			int size() { return _sectors.size(); };

			Iterator begin() const;
			Iterator end() const;


			private:
				Sectors _sectors;
		};

		typedef  std::map<std::string, References> ValueCollection;

		/** @brief used for the internal representation of printouts
		* The class belongs to the internal structure for data which are extracted from printouts by
		* the Printout Parser and evaluated by the Rule Checker.
		* The class stores a string values together
		* with a collection of all sectors where the value is used.
		*/
		class Values
		{
		public:
			/** @brief add a reference for the value, in case value is not yet referenced, 
			* then it is created.
			* @param[in] value the value to get a new sector reference
			* @param[in] the sector to be added 
			*/
			void addValue( std::string& value, int sector ); // search or add value, enter sector


			/** @brief recursively clear the complete ValueCollection */
			void clear();
			void printAll() const;
			
			void copySectors( References& references );
			void copySectorsValue( References& references, const std::string& value, DocumentPrintout::Operator op );
			void copySectorsValue( References& references, double value,             DocumentPrintout::Operator op );
			void copySectorsValueRange( References& references, const std::string& minimum, const std::string& maximum, bool numeric );

			bool isEqual( References& references ) const;
			std::string getValueForSector( int sector );
			void getValues( std::set<std::string>& values );

		private:
			// map of values to References
			ValueCollection _references;
		};

		typedef  std::map<std::string, Values> PrintoutCollection;

		/**
		 * Filters the data passed in 'docPrintout' according to the filter-specification passed in 'ruleCheckFilter'.
		 */
		class Filter
		{
		public:
			/**
			* Filters the data passed in 'docPrintout' according to the filter-specification passed in 'ruleCheckFilter'.
			* @param[in] docPrintout     The parsed printout data.
			* @param[in] ruleCheckFilter The filter specifying the prinout data to be filtered.
			* @throws DocumentPrintout::ExceptionFilterSyntax
			* @throws DocumentPrintout::ExceptionArgumentInvalid
			*/
			Filter( const DocumentPrintout& docPrintout, const DocumentXml::Node& ruleCheckFilter = 0);
			~Filter();

			/**
			 * check if the Filters are equal
			 * @param[in] filter The filter to be compared
			 */
			bool isEqual(const Filter& filter) const;

			/**
			 * check if filter is equal to references
			 * @param[in] references The references to be compared
			 */
			bool isEqual( const References& references) const;


			/**
			 * check if the filter is a subset
			 * @param filter The filter that is tested if it is a subset
			 */


			bool isSubset(const Filter& filter) const;
			/**
			 * check if the filter is empty
			 */
			bool isEmpty() const;

			/**
			 * check if elements in a filter belong to a specialized period. 
			 * In case the keys for DATE and TIME used in the printout are the standard, e.g. DATE and TIME or OUTPUTTIME
			 * then parameters dateKey and timeKey can be set empty
			 * @param[in] length of the period in seconds
			 * @param[in] actTime The filter including the actual Time, usually resulting form command CACLP
			 * @param[in] dateKey The key used to define the date
			 * @param[in] timeKey The key used to define the time
			 * @param[in] checkAll true means all are elements must be in the period
			 *                     false means at least one element must be in the period
			 */
			bool isInPeriod( time_t pLength, const Filter& actTime, std::string& dateKey, std::string& timeKey, bool checkAll ) const;


			/**
			 * list all values that are applicable for a defined key
			 *@[out] values the set of all values for key
			 *@[in]  key    the search key 
			 */
			void getValues( std::set<std::string>& values, const std::string& key ) const;

			/**
			 * copy all references from filter where two columns fullfil one of the operations EQUAL, GREATER, LESS
			 * additionally a percentage can be given, so that leftColumn <op> rightColumn * percentage / 100
			 *@[out] references the resulting references
			 *@[in] columnLeft		left column to compare
			 *@[in] columnRight		right column to compare
			 *@[in] op				the operation to be performed
			 *@[in] dataType		numeric or string operation
			 *@[in] percentage		percentage
			 */
			void compareColumns		( References& references, const std::string& columnLeft, const std::string& columnRight,
				DocumentPrintout::Operator op, const std::string& dataType, const std::string& percentage) const ;

			/**
			* filter all references from filter which are inside a specified range
			*@[out] references the resulting references
			*@[in] column the column to be checked
			*@[in] minNumber the minimum number for the range
			*@[in] maxNumber the maximum number for the range
			*@[in] dataType the type of the range, either number or string
			*/
			void range ( References& references, const std::string& column, const std::string& minNumber, const std::string& maxNumber,
				const std::string& dataType ) const;

			/** 
			* return the size of the filter
			* @returns the number of references in the filter
			*/
			int size() { return _references.size();};

			/**
			 * print the internal representation for diagnostics.
			 * This method is mainly used during rule design. It enables the rule writer
			 * to manually check if the rule extracts the right data from the printout.
			 */
			void printAll() const;

		private:
			Filter(const Filter&);
			Filter& operator=(const Filter&);

			/* used internally by compareColumns() */
			bool compareValuesOp	( const std::string& leftStr, const std::string& rightStr, DocumentPrintout::Operator op, 
				const std::string& dataType, const std::string& percentage) const;

			/* used internally by range() */
			bool rangeFits( const std::string& value, const std::string& minNumber, const std::string& maxNumber, 
				const std::string& dataType ) const;

			/**
			 * check if the filter has reference
			 * @i the reference to be tested
			 */
			bool hasReference( int i ) const;
			bool getDateTimeKeys( std::string& dateKey, std::string& timeKey ) const;	// set keys for DATE and TIME
			bool checkTimes( time_t pLength, const Filter& actTime, const std::string& dateKey, const std::string& timeKey, bool checkAll ) const;

			time_t		getActTime() const;
			time_t		toTime( std::string& ) const;
			time_t		calcTime( std::string& date, std::string& time ) const;
	
			const DocumentPrintout& _docPrintout;
			References _references;
		};


		DocumentPrintout();
		virtual ~DocumentPrintout();

		/** @brief close a sector, new sector starts with next addToCollection.
		* This method is used during the creation of the data collection
		*/
		void closeSector();

		/** @brief add key and value to the current collection, open a collection, if necessary 
		* This method is used during the creation of the data collection
		* @param[in] key the key of the parameter
		* @param[in] value the value of the parameter
		*/
		void add( std::string key, std::string value );

		//ValueCollection& findCollection( string key );
		/** @brief recursively clear the complete collection */
		void clear();
		
		void printAll() const;

//		void openSector(const std::string& key, const std::string& value);
		void closePrintout();
		void adjustSector( std::string key, std::string value );
		void restoreSector();
		bool hasKey( std::string key ) const;

		/* put all values found for a special key into a set
		 *@param[out] values
		 *@param[in]  key
		 */
		void getValues( std::set<std::string>& values, const std::string& key ) const;

		/** return the size 
		* @returns number of sectors in the printout
		*/
		int size() { return _maxSector+1; };

	private:
		DocumentPrintout(const DocumentPrintout&);
		DocumentPrintout& operator=(const DocumentPrintout&);

		/** @brief add all sectors containing a specified key 
		* The param sectors is enhanced by all sectors, it is not cleared before
		* @param[inout]  sectors the sectors found for the key
		* @param[in]     key the key for the search
		*/
		void copySectorsKey(References& references, const std::string& key) const;

		/** @brief add all sectors containing a specified key / value pair
		* The param sectors is enhanced by all sectors, it is not cleared before
		* @param[inout]  references the sectors found for the key
		* @param[in]     key the key for the search
		* @param[in]     value the value for the search
		*/
		void copySectorsKeyValue(References& references, const std::string& key, const std::string& value, Operator op) const;
		void copySectorsKeyValue(References& references, const std::string& key, double value,             Operator op) const;

		void copySectorsKeyValueRange( References& references, const std::string& key, const std::string& mininum, 
			const std::string& maximum, bool numeric ) const;


		void evaluateNode          (References& references, const DocumentXml::Node& filter    ) const;
		void evaluateNodeFilter    (References& references, const DocumentXml::Node& filter    ) const;
		void evaluateNodeIntersect (References& references, const DocumentXml::Node& intersect ) const;
		void evaluateNodeUnite     (References& references, const DocumentXml::Node& unite     ) const;
		void evaluateNodeEqual     (References& references, const DocumentXml::Node& filter    ) const;		
		void evaluateNodeNotEqual  (References& references, const DocumentXml::Node& filter    ) const;		
		void evaluateNodeGreater   (References& references, const DocumentXml::Node& filter    ) const;		
		void evaluateNodeLess      (References& references, const DocumentXml::Node& filter    ) const;		
		void evaluateNodeMatch     (References& references, const DocumentXml::Node& filter    ) const;
		void evaluateNodeRange     (References& references, const DocumentXml::Node& filter    ) const;


		PrintoutCollection _collection;      // map of keys to ValueCollections
		int                _currentSector;
		//int                _tmpSector;
		int                _maxSector;
		bool               _sectorOpen;
		int                _currentPrintout; // distinguish several printout in one file

		static const std::string _printoutNumber;
	};
}

#endif // AcsHcs_DocumentPrintout_h
