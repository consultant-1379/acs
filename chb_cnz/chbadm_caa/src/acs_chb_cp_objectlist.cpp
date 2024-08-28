/*=================================================================== */
   /**
   @file acs_chb_cp_objectlist.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_cp_objectlist.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       19/01/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <ace/ACE.h>
#include <unistd.h>  //uablowe
#include <string.h>

#include <acs_chb_request_handler.h>
#include <acs_chb_cp_objectlist.h>
#include <acs_chb_heartbeat_def.h>

/*=====================================================================
                        GLOBAL METHOD
==================================================================== */
/*=================================================================== */
     /**
        @brief      HeadCPObjectList

      */
  /*=================================================================== */
extern ACS_CHB_CPObjectList *HeadCPObjectList; //uabnyb trying to use the global

/*===================================================================
   ROUTINE: ACS_CHB_CPObjectList
=================================================================== */
ACS_CHB_CPObjectList::ACS_CHB_CPObjectList() 
{
	CPObjectItem[0] = 0;
	CPObjectValue[0] = 0;
	CPObjectValueChanged = 0;
	headCPObjectList = 0;
	CPObjectListCreated = false;
	Next = 0;
	Pre = 0;
} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_CPObjectList
=================================================================== */
ACS_CHB_CPObjectList::~ACS_CHB_CPObjectList()
{
	if (Next != 0)
	{
		Next->Pre = Pre;
	}
	if (Pre != 0)
	{
		Pre->Next = Next;
	}
	//	
	//	If there is a list created from this object, delete it
	//
	if (CPObjectListCreated == true)
	{
		ACS_CHB_CPObjectList* ptr = headCPObjectList->get_next();
		while (ptr)
		{
			delete ptr;
			ptr = headCPObjectList->get_next();
		} // End of while
		delete headCPObjectList;
		headCPObjectList = 0;
		CPObjectListCreated = false;
	}
} // End of destructor

/*===================================================================
   ROUTINE: initCPObjectList
=================================================================== */

void 
ACS_CHB_CPObjectList::initCPObjectList(ACS_CHB_CPObjectList *headOfCPObjectList)
{
	ACS_CHB_CPObjectList *ptr = headOfCPObjectList;

	//
	// List of supported CP Objects.
	//

	// CP Object APZ type.
	ACE_OS::strcpy(ptr -> CPObjectItem,"APZ_type");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object EX side
	ACS_CHB_CPObjectList* newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm; 
	ACE_OS::strcpy(ptr -> CPObjectItem,"EX_side");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object SB status
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"SB_status");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object CP status
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"CP_status");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object CP connection
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"CP_connection");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object EX exchange identity
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"EX_Exchange_identity");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object SB exchange identity
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"SB_Exchange_identity");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object Last restart value
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;			
	ACE_OS::strcpy(ptr -> CPObjectItem,"Last_Restart_Value");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	// CP Object Time zone alarm string
	newElm = new ACS_CHB_CPObjectList();
	ptr->appendToList(newElm);
	ptr = newElm;
	ACE_OS::strcpy(ptr -> CPObjectItem,"Time_Zone_Alarm");
	ACE_OS::strcpy(ptr -> CPObjectValue," ");
	ptr -> CPObjectValueChanged = 0;

	CPObjectListCreated = true;

} // End of initCPObjectList

/*===================================================================
   ROUTINE: getCPObjectValueFromCPObjectList
=================================================================== */

char *
ACS_CHB_CPObjectList::getCPObjectValueFromCPObjectList(char cpObjectValue[])
{
	ACS_CHB_CPObjectList *ptrCPObjectList = headCPObjectList;

	// Search through list to find matchin CP object name.
	while (ptrCPObjectList != (ACS_CHB_CPObjectList *)0)
	{
		if (!ACE_OS::strcmp(cpObjectValue,ptrCPObjectList->CPObjectItem))
		{
			// CP object name found, get value
			return ptrCPObjectList->CPObjectValue;
		}
		ptrCPObjectList = ptrCPObjectList->get_next();
	} // End of while

	return headCPObjectList->CPObjectValue; // Should never get here
} // End of getCPObjectValueFromCPObjectList

/*===================================================================
   ROUTINE: get_next
=================================================================== */
ACS_CHB_CPObjectList* ACS_CHB_CPObjectList::get_next()
{
	return Next;
} // End of get_next

/*===================================================================
   ROUTINE: appendToList
=================================================================== */
void ACS_CHB_CPObjectList::appendToList(ACS_CHB_CPObjectList* newElm)
{
	ACS_CHB_CPObjectList* tmp = this;
	while (tmp->Next) 
	{
		tmp = tmp->Next;
	}
	tmp->Next = newElm;
	newElm->Pre = tmp;      
} // End of appendToList

/*===================================================================
   ROUTINE: get_CPObjectItemPtr
=================================================================== */
const char* ACS_CHB_CPObjectList::get_CPObjectItemPtr()
{
	return CPObjectItem;
} // End of get_CPObjectItemPtr

/*===================================================================
   ROUTINE: get_CPObjectValuePtr
=================================================================== */
const char* ACS_CHB_CPObjectList::get_CPObjectValuePtr()
{
	return CPObjectValue;
} // End of get_CPObjectValuePtr

/*===================================================================
   ROUTINE: set_CPObjectValue
=================================================================== */
void ACS_CHB_CPObjectList::set_CPObjectValue(const char* v)
{
	ACE_OS::strcpy(CPObjectValue,v);
} // End of set_CPObjectValue

/*===================================================================
   ROUTINE: get_CPObjectValueChanged
=================================================================== */
int ACS_CHB_CPObjectList::get_CPObjectValueChanged()
{
	return CPObjectValueChanged;
} // End of get_CPObjectValueChanged

/*===================================================================
   ROUTINE: set_CPObjectValueChanged
=================================================================== */
void ACS_CHB_CPObjectList::set_CPObjectValueChanged(int v)
{
	CPObjectValueChanged = v;
} // End of set_CPObjectValueChanged
