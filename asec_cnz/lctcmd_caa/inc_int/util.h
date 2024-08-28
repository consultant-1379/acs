#ifndef UTIL_H
#define UTIL_H

//#include <stdafx.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <cstring>
#include <list>
#include <map>

/**
 * namespace ::util
 * Contains utility functions and structures for general purposes.
 *
 */
namespace util
{	
	/** constant util::s::CSTR_WHITESPACE is a c-string containing all whitespace characters */
	const char CSTR_WHITESPACE[3] = {32,'\t',0};

	/** constant util::s::CSTR_CRLF is a c-string containing carriage return line feed */
	const char CSTR_CRLF[3] = {13,10,0};

	/** constant util::s::CSTR_CR is a c-string containing the carriage return character */
	const char CSTR_CR[2] = {13,0};

	/** constant util::s::CSTR_CRLF is a c-string containing the line feed character */
	const char CSTR_LF[2] = {10,0};

	/** constant util::s::CSTR_EMPTY is an empty c-string */
	const char CSTR_EMPTY[1] = {0};
	
	/** constant util::s::CSTR_NUMBERS is a c-string containing all number characters */
	const char CSTR_NUMBERS[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0};

	/** constant util::s::STR_CRLF is a string containing carriage return line feed */
	const std::string STR_CRLF(CSTR_CRLF);

	/** constant util::s::STR_CR is a string containing the carriage return character */
	const std::string STR_CR(CSTR_CR);
	
	/** constant util::s::STR_LF is a string containing the line feed character */	
	const std::string STR_LF(CSTR_LF);
	
	/** constant util::s::CSTR_EMPTY is an empty string */
	const std::string STR_EMPTY;
	
	/** struct TableIndex is a helper struct storing 2D-positions (row, column) for tables */
	struct TableIndex
	{
		size_t absIndex; // absolute index
		size_t rowIndex; // row index
		size_t colIndex; // column index
		TableIndex() : absIndex((size_t)-1), rowIndex((size_t)-1), colIndex((size_t)-1) {}
		bool operator==(const TableIndex& compareTo);
	};
	
	/** constant util::s::TI_NPOS is an instance of TableIndex signaling an invalid position */
	const TableIndex TI_NPOS;
		
	/**
	 * function ::util::tokenize
	 * Breaks down the specified string s in its substrings between the specified delimiter.
	 * Returns a pointer to a new list of string containing the substrings.
	 * 
	 * param s:
	 *   The string to tokenize.
	 * param delim:
	 *   The delimiter.
	 * returns:
	 *   A pointer to a new list that contains the substrings extracted from s.
	 * 
	 */
	std::list<std::string> * tokenize(const std::string& s, const std::string& delim = ",");
	
	/**
	 * function ::util::trim
	 * Removes whitespaces at the front and tail of the specified string s. Removes characters in 
	 * CSTR_WHITESPACE by default, but can also be used to remove other characters if used with 
	 * two arguments.
	 *
	 * param s:
	 *   The string to be trimmed.
	 * param wipeOut:
	 *   The characters to be erased at the start and the end of s.
	 */
	void trim(std::string& s, const std::string& wipeOut = std::string(CSTR_WHITESPACE));

	/**
	 * function ::util::GetPSState
	 * Return 2 if PSState Windows Register key is CONFIGURED  
	 *
	 * input param :
	 *   
	 *
	 * param wipeOut:
	 *   -1 Error 
	 *   1 PSState NOT COFIGURED.
	 *   2 PSState CONFIGURED.
	 *
	 */
	 int GetPSState(void);
	
	/**
	 * function ::util::split
	 * Attempts to split the specified string s into two strings. Splits it at the first index of 
	 * the specified delimiter. The substrings are trimmed first and then stored in parameters key 
	 * and value. Returns true if the operation succeeded. Returns false if the specified delimiter 
	 * was not found in s.
	 * 
	 * param s:
	 *   The string to split.
	 * param key:
	 *   The trimmed first part of string s will be stored here.
	 * param value:
	 *   The trimmed second part of string s will be stored here.
	 * param delim:
	 *   The delimiter.
	 * returns:
	 *   true if the operation succeeded, otherwise false.
	 */
	bool split(std::string& key, std::string& value, const std::string& s, const std::string& delim = ":");
	
	/**
	 * function ::util::findLabelIndex
	 * Searches a header row which contains all specified labels in string s. Tries to find the table index 
	 * of the specified label within this header row. Returns the table index of the specified label, 
	 * or TI_NPOS if the header row or the label was not found.
	 *
	 * param s:
	 *   The string to search in. 
	 * param label:
	 *   The label to search for.
	 * param allLabels:
	 *   A coma-separated sequence containing the labels expected in the header row. 
	 * returns:
	 *   The TableIndex that represents the table index of the specified label in string s.
	 */
	TableIndex findLabelIndex(const std::string& s, const std::string& label, const std::string& allLabels);
	
	/**
	 * function ::util::count
	 * Counts the occurances of the specified string search in string s. Starts counting at the specified 
	 * offset, and stops after num characters.
	 *
	 * param s:
	 *   The string to search in.
	 * param search:
	 *   The string to search for.
	 * param offset:
	 *   The index to start counting at.
	 * param num:
	 *   The numbers of characters in string s to search in.
	 * returns:
	 *   The number of occurances of specified string search in string s within the area specified by params 
	 *   offset and num.
	 */
	size_t count(const std::string& s, const std::string& search, const size_t offset = 0, const size_t num = -1);
	
	/**
	 * function ::util::indexOfRow
	 * Returns the start index of the specified row within string s. Returns -1 if string s contains less rows. 
	 * 
	 * param s:
	 *   The string to search in.
	 * param row:
	 *   The row number of the row to search for. 
	 * returns:
	 *   The start index of the specified row in s, or -1 if string s contains less rows.
	 */
	size_t indexOfRow(const std::string& s, const size_t row);
	
	/**
	 * function ::util::getTableContent
	 * Returns a substring of s at the specified table index. 
	 * 
	 * param s:
	 *   The string to search in.
	 * param index:
	 *   The table index to search at. Only row index and column index are used by this function.
	 * param delim:
	 *   The delimiter used in the table.
	 * returns:
	 *   The content of the cell in string s at the specified table index. The result is trimmed.
	 */
	std::string getTableContent(const std::string& s, const TableIndex& index, const std::string& delim = ":");
	
	/**
	 * function ::util::tableSearch
	 * This function handles primitive tables, containing two columns separated by a delimiter. It returns 
	 * the value of the next key-value pair after index in string s that key is equal to the one specified. 
	 * The result is trimmed. The absolute position of the next row is stored in param index. If the key 
	 * was not found, the function stores -1 in index.
	 *
	 * param index
	 *   The index at which to start searching for the key. After successful operation, the index of the 
	 *   following row is stored here. If the operation fails, -1 is stored.
	 * param s:
	 *   The string to search in.
	 * param key:
	 *   The key to search for.
	 * param delim:
	 *   The delimiter between keys and values.
	 * returns:
	 *   The trimmed value behind key and delimiter.
	 */
	std::string tableSearch(size_t * index, const std::string& s, const std::string& key, 
									const std::string& delim = ":");

	/**
	 * function ::util::serviceIsRunning
	 * Returns true if the specified service is running on the specified host, otherwise false. Also 
	 * returns false if the services' status cannot be determined. 
	 * 
	 * param serviceName:
	 *   The name of the service to check.
	 * param host:
	 *   The name or IP address of the host to check. 
	 * returns:
	 *   true if the specified service is definitely running on the specified host, otherwise false.
	 */
	bool serviceIsRunning(const std::string& serviceName, const std::string& host = "localhost");
	
	/**
	 * 
	 */
	class Command
	{
	public:
		Command(const std::string& name);
		Command(const char * name = NULL);
		Command::~Command();
		std::string& getName();
		std::string& getOutput();
		std::list<std::string> * getOutputLines();
		DWORD getExitCode();
		void setName(const std::string& name);
		void setOutput(const std::string& output);
		void setExitCode(DWORD exitCode);
		operator const std::string& () const;
	private:
		std::string name;
		std::string output;
		std::list<std::string> * outputLines;
		DWORD exitCode;
	};
	
	class CommandSender
	{
	protected:
		std::string errorMessage;
	public:
		virtual ~CommandSender() = 0;
		virtual bool send(Command& command) = 0;
		const std::string& getErrorMessage();
		static const int BUFFER_SIZE;
	};
	
	class CLCommandSender : public CommandSender
	{
	public:
		CLCommandSender();
		~CLCommandSender();
		bool send(Command& command);
	private:
		char * readBuffer;
	};
	
}

#endif