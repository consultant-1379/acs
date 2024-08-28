
/*=================================================================== */
   /**
   @file acs_chb_eventlist.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_eventlist.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       23/01/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <acs_chb_eventlist.h>

/*===================================================================
   ROUTINE: ACS_CHB_EventList
=================================================================== */
 
ACS_CHB_EventList::ACS_CHB_EventList()
{
	Next = 0;
	Pre = 0;
	CPObjectItem[0] = 0;
	CPObjectValue[0] = 0;
	CPObjectValueChanged = 0;
} // End of constructor
/*===================================================================
   ROUTINE: ~ACS_CHB_EventList
=================================================================== */
ACS_CHB_EventList::~ACS_CHB_EventList()
{
	if (Next != 0)
	{
		Next->Pre = Pre;
	}
	if (Pre != 0)
	{
		Pre->Next = Next;
	}
} // End of destructor
/*===================================================================
   ROUTINE: get_next
=================================================================== */
ACS_CHB_EventList* ACS_CHB_EventList::get_next()
{
	return Next;
} // End of get_next
/*===================================================================
   ROUTINE: get_last
=================================================================== */
ACS_CHB_EventList* ACS_CHB_EventList::get_last()
{
	ACS_CHB_EventList* tmp = this;
	while (tmp->Next)
	{
		tmp = tmp->Next;
	}
	return tmp;
} // End of get_last
/*===================================================================
   ROUTINE: appendToList
=================================================================== */
void ACS_CHB_EventList::appendToList(ACS_CHB_EventList* newElm)
{
	ACS_CHB_EventList* tmp = get_last();
	tmp->Next = newElm;
	newElm->Pre = tmp;
} // End of appendToList
/*===================================================================
   ROUTINE: get_CPObjectItemPtr
=================================================================== */
char* ACS_CHB_EventList::get_CPObjectItemPtr()
{
	return CPObjectItem;
} // End of get_CPObjectItemPtr
/*===================================================================
   ROUTINE: get_CPObjectValuePtr
=================================================================== */
char* ACS_CHB_EventList::get_CPObjectValuePtr()
{
	return CPObjectValue;
} // End of CPObjectValuePtr
/*===================================================================
   ROUTINE: get_CPObjectValueChanged
=================================================================== */
int ACS_CHB_EventList::get_CPObjectValueChanged() const
{
	return CPObjectValueChanged;
} // End of get_CPObjectValueChanged
/*===================================================================
   ROUTINE: set_CPObjectValueChanged
=================================================================== */
void ACS_CHB_EventList::set_CPObjectValueChanged(int s)
{
	CPObjectValueChanged = s;
} // End of set_CPObjectValueChanged
