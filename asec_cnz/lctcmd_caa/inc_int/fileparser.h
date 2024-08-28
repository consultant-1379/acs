#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <list>
using namespace std;


#define MAXROW 2048
#define READ "r"
#define WRITE "w"
#define APPEND "a"
#define TAG_ENABLE "Enable"
#define TAG_ENABLE2 "Enable2"
#define TAG_DISABLE "Disable"
#define TAG_DISABLE2 "Disable2"
#define TAG_REAPPLY "Reapply"
#define TAG_REAPPLY2 "Reapply2"
#define TAG_SHOWPOLICY "ShowPolicy"
#define TAG_SHOWASSIGNED "ShowAssignedPolicy"
#define TAG_SHOWRULE "ShowRule"
#define TAG_SHOWRULE2 "ShowRule2"					// Physical Separation impact
#define TAG_SHOWFILTERACTION "ShowFilteraction"
#define TAG_SHOWFILTERACTION2 "ShowFilteraction2"	// Physical Separation impact
#define TAG_SHOWFILTERLIST "ShowFilterlist"
#define TAG_SHOWFILTERLIST2 "ShowFilterlist2"		// Physical Separation impact
#define TAG_ASSIGNYES "AssignYes"
#define TAG_ASSIGNNO "AssignNo"
#define TAG_ASSIGNPOLICY "AssignPolicy"
#define TAG_FILTERS "Filters"
#define TAG_FILTERS2 "Filters2"
#define TAG_FILTERPRINT "FilterPrint"
#define TAG_UDPFILTERPRINT "UDPFilterPrint" 		//CNI 997: New Tag added to the template file.
#define TAG_DSTPORTS "DstPorts"
#define TAG_PUBLICNET "<%PublicNet%>"
#define TAG_PUBLICIP "<%publicIP%>"
#define TAG_PUBLICMASK "<%publicMask%>"
#define TAG_PORT "<%dstPort%>"
#define TAG_ACTIVE "<%Activate%>"
#define TAG_ACTIVE2 "<%Activate2%>"					// Physical Separation impact
#define TAG_FILTERADD "<%FilterAdd%>"
#define TAG_FILTERADD2 "<%FilterAdd2%>"				// Physical Separation impact
#define POLICY_NAME "APG40Policy"
#define RULE_NAME "CAA1090590_Rule"
#define FILTERLIST_NAME "CAA1090590_FilterList"
#define FILTERLIST2_NAME "Public_2_FilterList"
#define FILTERACTION_NAME "CAA1090590_FilterAction"
#define FILTERACTION2_NAME "Public_2_FilterAction"
#define STATUS_BLOCK "BLOCK"

/*
* Check if row contains key.
*
* @param row the row to check.
* @param key the key to use.
* @returns true if row contains key otherwise false
*/
bool rowContainsKey(const string& row, const string& key);

/*
* Check if row starts with key.
*
* @param row the row to check.
* @param strTag the key to use.
* @returns true if row starts with key otherwise false
*/
bool isStartOfRow(const string& row, const string& strTag);

/*
* Extract netsh script from template file.
*
* @param tag The tag to use in order to find correct script in template file.
* @param strlist string list that keeps the script (each row in the script is one string in list).
* @throws CsadmException if failing to open or parse file.
*/
void extractScriptTemplate(const string& templateFilePath, const string& tag, list<string>& strList);

/*
* Extract netsh script from template file.
*
* @param tag The tag to use in order to find correct script in template file.
* @param str string that keeps the script.
* @throws CsadmException if failing to open or parse file.
*/
void extractScriptTemplate(const string& templateFilePath, const string& tag, string& str);

/*
* Create netsh script to use for security configuration commands. Extracts script
* and then writes it to file.
*
* @param tag The tag to use in order to find correct script in template file.
*/
void createScript(const string& templateFilePath, const string& tempScriptFilePath, const string& tag);

/*
* Format tags to use when extracting netsh script.
*
* @param tag The tag to use in order to find correct script in template file.
* @param iStart indicates whether it is a startag or end tag.
* @returns the formatted string.
*/
string formatTag(const string& tag, bool isStart);

/*
* Searches a row for a tag and replaces all occurances of the tag in the row with a string.
*
* @param row The row to search in.
* @param tag The tag to be replaced.
* @param data The string that the tag8s) shall be replaced by.
* @returns the Row with replaced tags.
*/
string searchAndReplace(const char* row, const char* tag, const char* data);

/*
* Checks whether a file contains just empty rows.
*
* @param fileName The name of file to check.
* @returns true if empty rows in file, otherwise false
* @throws CsadmException if failing to open file.
*/
bool isEmptyLines(list<string> * rows);

/*
* Gets the first occurance of a row in a file that matches a key. Removes line feed in the
* returned row as well.
*
* @param key The key to match.
* @returns First occurance of matching row.
* @throws CsadmException if failing to open file.
*/
string& getRow(list<string> * rows, const string& key);

/*
* Gets the first occurance of a parameter value in a file that matches a parameter, according to the format:
* <param>:	<paramValue>
* Searches for the first occurance of <param> and returns <paramValue>.
*
* @param searchParam The parameter to search for.
* @returns The parameter value.
* @throws CsadmException if failing to open file.
*/
string getParamValue(list<string> * rows, const string& searchParam);

/*
* Write data to a temporary script file (used by netsh to execute ipsec commands).
*
* @param strList List with containg the rows to write to the file.
* @throws CsadmException if failing to open file, or writing to file.
*/
void writeTempFile(const string& tempScriptFilePath, const list<string>& strList);
void dumpTempFile(const string& templateFilePath);
/*
* Replace the <%Activate%> tags in the temporary script file with either "yes"
* or "no".
*
* @param setActive If true set "yes" otherwise set "no".
* @throws CsadmException if failing to open file.
*/
void setActiveStatus(const string& tempScriptFilePath, bool setActive);

/*
* Replace the passed tag in the temporary script file with either "yes"
* or "no".
*
* @param setActive If true set "yes" otherwise set "no".
* @throws CsadmException if failing to open file.
*/

void setTagStatus(const string& tempScriptFilePath, bool setActive, const string& tag );

/**
 * function ::setFilters
 * Replaces TAG_FILTERADD in temporary script file with the specified list of lines.
 */
void setFilters(const string& tempScriptFilePath, const list<string>& filterAdd);

/**
 * function ::setTag
 * Replaces a specified tag in temporary script file with the specified list of lines.
 */
void setTag(const string& tempScriptFilePath, const list<string>& filterAdd, char * tag);


/*
* Find string in row. Exact match is required.
*
* @param row The string to check
* @param str The string that shall be matched.
* @returns true if match, otherwise false.
*/
bool findStr(const string& row, const string& str);

/*
* Extract parameter value in string. The string is expected to be in any format
* including a delimiter.
*
* @param row The string.
* @param delimiter The delimiter to use.
* @returns The parameter value.
*/
string extractParam(const string& row, const char delimiter = ':');


#endif