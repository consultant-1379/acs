////////////////////////////////////////////////////////////////////////////
// NAME - 
//
// COPYRIGHT Ericsson AB, Sweden 2004
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden. 
// The program(s) may be used and/or copied only with the written 
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//		File class written to replace RWFile of Rougewave Tools.h++
//
//
// DOCUMENT NO
//	----
//
// AUTHOR
//20040429		QVINKAL		First Release
//
// CHANGES
// 20040429		QVINKAL		First Release
//
////////////////////////////////////////////////////////////////////////////
#include <string>
#include <stdio.h>
using namespace std;

class File
{
public:
	/*Constructor */
	File(string filename,string mode = "rb+");

	/*destructor*/
	~File();

	/*Check validity of file pointer*/
	bool isValid();
	
	/*Check if file is empty*/
	bool isEmpty();

	/*Read string from file*/
	void operator>> (string& opstr);
	
	/*write string into file*/
	void operator<< (string ipstr);
	
	/*Read long from file*/
	bool Read(long &a);

	/*Read integer from file*/
	bool Read(int &a);

	/*Read unsigned int from file*/
	bool Read(unsigned &a);
	
	/*Check for error in file operations*/
	bool Error();

	/*write long into file*/
	bool Write(long a);
private:
	string filenm,filemode;
	bool valid, empty;
	FILE *fptr;
};