//******************************************************************************
//
// NAME
//      ACS_USA_ObjectManager.cpp
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR 
// 	1995-06-20 by ETX/TX/T XKKHEIN

// REVISION
//	PA2	 	990915

// CHANGES

//
//	REV NO		DATE		NAME 		DESCRIPTION
//	PA1			950829		XKKHEIN		First Revision
//  PA2			990915		UABDMT		Ported to Windows NT4.0
//	PA3			040429		QVINKAL	  Removal of RougeWave Tools.h++
//	PA4			080311		EKAMSBA		PRC alarms handling.
// SEE ALSO 
//
//******************************************************************************

#pragma warning (disable : 4786)
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_ObjectManager.h"


//******************************************************************************
//	ACS_USA_ObjectManager()
//******************************************************************************
ACS_USA_ObjectManager::ACS_USA_ObjectManager(const String& src) :
vec (initialSize),
index(initialIndex),
size(initialSize),
last(initialIndex),
eventSource(src)
{
}  



//******************************************************************************
//	~ACS_USA_ObjectManager()
//******************************************************************************
ACS_USA_ObjectManager::~ACS_USA_ObjectManager()
{

	vec.clear();
	vector<ACS_USA_AnalysisObject *>::iterator it = ptrVec.begin() ;
	try
	{
		for(;it != ptrVec.end();it++ )
			if(*it != NULL)
			{
				(*it)->DestroyPtrList();
				delete *it;
			}
		ptrVec.clear();
	}
	catch(...)
	{
	}
}  



//******************************************************************************
//	append()
//******************************************************************************
void
ACS_USA_ObjectManager::append(ACS_USA_AnalysisObject* object)
{
    // If vector is full then make it bigger
    if (last == size) {
    	size += resizeValue;
		vec.resize(size);
		ptrVec.resize(size);
    }
	vec[last] = *object;
	ptrVec.push_back(object);
    last++;
}  



//******************************************************************************
//	getFirst()
//******************************************************************************
ACS_USA_AnalysisObject*
ACS_USA_ObjectManager::getFirst()
{
    index = initialIndex;
	return &vec[index++];
}  



//******************************************************************************
//	getNext()
//******************************************************************************
ACS_USA_AnalysisObject*
ACS_USA_ObjectManager::getNext()
{
    if (index == last) {
	return 0;
    }

	return &vec[index++];
 }  


//******************************************************************************
//	selectAnalysisObject()
//******************************************************************************
ACS_USA_AnalysisObject*
ACS_USA_ObjectManager::selectAnalysisObject(const String& fileName)
{
	for(int index=0; index<vec.size(); index++)
	{
		if( _stricmp( vec[index].getLogFile().c_str(), fileName.c_str() ) == 0 )
		{
			return &vec[index];
		}
	}

	// no match !
	return NULL;
}


//******************************************************************************
