//******************************************************************************
//
// NAME
//      ACS_USA_ObjectManagerCollection.cpp
//
// COPYRIGHT Ericsson Utvecklings AB, Sweden 2008.
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
//	.

// AUTHOR 
// 	2008-03-17 by DR/SD/M EKAMSBA

// REVISION
//	.

// CHANGES

//
//	REV NO		DATE		NAME 		DESCRIPTION
//	A			080317		EKAMSBA		First Revision
// SEE ALSO 
//
//******************************************************************************

#include "ACS_USA_ObjectManagerCollection.h"
#include "ACS_USA_ObjectManager.h"


//******************************************************************************
//	ACS_USA_ObjectManagerCollection()
//******************************************************************************
ACS_USA_ObjectManagerCollection::ACS_USA_ObjectManagerCollection()
{
}

//******************************************************************************
//	~ACS_USA_ObjectManagerCollection()
//******************************************************************************
ACS_USA_ObjectManagerCollection::~ACS_USA_ObjectManagerCollection()
{
	std::vector<ACS_USA_ObjectManager *>::iterator iElem = m_aList.begin();
	for( ; iElem != m_aList.end(); iElem++)
	{
		try
		{	
			if( NULL != *iElem )
			{
				delete *iElem;
				*iElem = NULL;
			}
		}
		catch(...)
		{
		}
	}

	m_aList.clear();
}

//******************************************************************************
//	addManager()
//******************************************************************************
ACS_USA_ObjectManager* 
ACS_USA_ObjectManagerCollection::addManager(const String& source)
{
	ACS_USA_ObjectManager* ptr = new ACS_USA_ObjectManager( source );
	if( NULL != ptr )
	{
		m_aList.push_back( ptr );
	}

	return ptr;
}

//******************************************************************************
//	selectManager()
//******************************************************************************
ACS_USA_ObjectManager* 
ACS_USA_ObjectManagerCollection::selectManager(const String& source) const
{
	std::vector<ACS_USA_ObjectManager *>::const_iterator iElem = m_aList.begin();
	for( ; iElem != m_aList.end(); iElem++)
	{
		if( _stricmp((*iElem)->getSource().c_str(), source.c_str()) == 0 )
		{
			return (*iElem);
		}
	}

	// no match !
	return NULL;
}

//******************************************************************************



















