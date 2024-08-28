
/*=================================================================== */
/**
   @file   acs_chb_eventlist.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       23/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_EVENTLIST_H 
#define ACS_CHB_EVENTLIST_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_heartbeat_def.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_EventList

*/
/*=================================================================== */

class ACS_CHB_EventList
{
	/*=====================================================================
		                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		                        CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Constructor for ACS_CHB_EventList class.

		             Default constructor, used to initialize variables.


	*/
	/*=================================================================== */
  ACS_CHB_EventList();
  /*=====================================================================
  	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
        @brief     Destructor for ACS_CHB_EventList class.

				   The destructor of the class. The object is removed from the
				   list of objects.

  */
  /*=================================================================== */
  ~ACS_CHB_EventList();
  /*=================================================================== */
  /**
        @brief     Returns a pointer to the next object in the list of objects.

        @return    ACS_CHB_EventList
    			   0-No more objects
				   *-Pointer to next object

   */
  /*=================================================================== */

  ACS_CHB_EventList* get_next();
  
  /*=================================================================== */
    /**
          @brief     Returns a pointer to the last object in the list of objects.

          @return    ACS_CHB_EventList
      			     *     Pointer to last object

     */
    /*=================================================================== */
  ACS_CHB_EventList* get_last();

  /*=================================================================== */
    /**
          @brief     Appends a new object to the end of current list.

          @param     newElm
					 The new object to be added to the
                     end of the list
          @return    void

     */
    /*=================================================================== */
  void appendToList(ACS_CHB_EventList* newElm);

  /*=================================================================== */
    /**
          @brief     Returns a pointer to the variable CPObjectItem.

          @return    char*
                     A pointer to the CPObjectItem

     */
    /*=================================================================== */
  char* get_CPObjectItemPtr();

  /*=================================================================== */
    /**
          @brief     Returns a pointer to the variable CPObjectValue.

          @return    char*
                     A pointer to the CPObjectItem

     */
    /*=================================================================== */
  char* get_CPObjectValuePtr();

  /*=================================================================== */
    /**
          @brief     Set the CPObjectValueChanged status.

          @return    ACS_CHB_FAIL            Value not changed
					 ACS_CHB_OK              Value changed

     */
    /*=================================================================== */
  int get_CPObjectValueChanged() const;


  /*=================================================================== */
    /**
          @brief     Set the CPObjectValueChanged status.

          @param     s
   					 ACS_CHB_FAIL: Value not changed
                     ACS_CHB_OK  : Value changed

          @return    void

     */
    /*=================================================================== */
  void set_CPObjectValueChanged(int s);
  
  /*=====================================================================
                         PRIVATE DECLARATION SECTION
  ==================================================================== */

private:
  /*===================================================================
                          PRIVATE ATTRIBUTE
   =================================================================== */
  /*=================================================================== */
  /**
        @brief   CPObjectItem
				 Save CP object item name.
   */
  /*=================================================================== */
	char CPObjectItem[ACS_CHB_SMALL_BUFSIZE]; // Save CP object item name.
	/*=================================================================== */
	/**
	      @brief   CPObjectValue
				   Save CP object item value.
	 */
	/*=================================================================== */
    char CPObjectValue[ACS_CHB_SMALL_BUFSIZE];// Save CP object item value.

    /*=================================================================== */
    /**
          @brief   CPObjectValueChanged
				   Holds status whether value of
			       CP object has changed since
				   last read of value.
     */
    /*=================================================================== */

	int CPObjectValueChanged;

	/*=================================================================== */
	/**
	      @brief   pointer to next elements
	 */
	/*=================================================================== */
    ACS_CHB_EventList *Next;

    /*=================================================================== */
    /**
          @brief   pointer to previous element
     */
    /*=================================================================== */
    ACS_CHB_EventList *Pre;
};

#endif

