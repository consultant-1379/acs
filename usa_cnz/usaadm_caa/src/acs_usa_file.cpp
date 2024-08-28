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
#include <stdlib.h>
#include "acs_usa_file.h"
using namespace std;
/******************* Class implementation : Begin ************/
//========================================================================================
//	Constructors
//========================================================================================
File::File(string filename,string mode )
{
	filenm = filename;
	filemode = mode;
	if ((fptr=fopen(filenm.c_str(),mode.c_str())) == NULL)
    		valid = false;        
    	else
		valid = true;
	
	if(valid == false)
	{
		if ((fptr=fopen(filenm.c_str(),"wb+")) == NULL)
    		valid = false;        
		else
			valid = true;
	}

	if(valid)
	{
		fseek (fptr , 0L , SEEK_END);
		long lsize = ftell (fptr);
		rewind (fptr);
		if(lsize > 0)
			empty = false;
		else 
			empty = true;
	}
	else
		empty = true;

}

//========================================================================================
//	Destructors
//========================================================================================
File::~File()
{
	if (valid)
		fclose(fptr);
}


//========================================================================================
//	Member Functions
//========================================================================================
bool File::isValid()
{
	return valid;
}

bool File::isEmpty()
{
	return empty;
}
void File::operator<< (string ipstr)
{
	fputs(ipstr.c_str(),fptr);
	fputc('\0',fptr);
}

void File::operator>> (string &opstr)
{
	char * p ;
	char c;
	int a = 1;

	while ((c = (char)fgetc(fptr)) != '\0' && c != EOF)
	{
		a++;
	}
	int sz = a;
	a-=2*a;
	p = (char *) malloc(sz + 1);
	fseek(fptr,a,1);
	fread(p,1,sz,fptr);
	opstr = p;

	free(p);
}


bool File::Read(long &a)
{
	int ret;
	ret = (fread(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}

bool File::Read(int &a)
{
	int ret;
	ret = (fread(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}

bool File::Read(unsigned &a)
{
	int ret;
	ret = (fread(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}
bool File::Write(long a)
{
	int ret;	
	ret= (fwrite(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}

bool File::Write(int a)
{
	int ret;	
	ret= (fwrite(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}
bool File::Write(unsigned a)
{
	int ret;	
	ret= (fwrite(&a,sizeof(a),1,fptr));
	if (ret < 0 || ret > 1)
		return false;
	else
		return true;	
}

bool File::Error()
{
	int ret;	
	if( valid )
	{
		ret= ferror(fptr);
		if (ret != 0 )
			return true;	
	}
	return false;
}
