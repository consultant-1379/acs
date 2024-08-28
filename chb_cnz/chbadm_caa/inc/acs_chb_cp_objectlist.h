
/*=================================================================== */
/**
   @file   acs_chb_cp_objectlist.h

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
   N/A       19/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_OBJECTLIST_H 
#define ACS_CHB_OBJECTLIST_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_chb_heartbeat_def.h"
#include "acs_chb_error.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_CHB_CPObjectList

*/
/*=================================================================== */

class ACS_CHB_CPObjectList : public virtual ACS_CHB_Error
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
      @brief     Constructor for ACS_CHB_CPObjectList class.

	             Default constructor, used to initialize variables.


*/
/*=================================================================== */
  ACS_CHB_CPObjectList();

  /*=====================================================================
  	                        CLASS DESTRUCTOR
  ==================================================================== */
  /*=================================================================== */
  /**
        @brief     Destructor for ACS_CHB_CPObjectList class.

  				   The destructor of the class. The object is removed from the
			       list of objects and resources are deleted.

  */
  /*=================================================================== */
  virtual ~ACS_CHB_CPObjectList();

  /*=====================================================================
  	                        PROTECTED DECLARATION SECTION
  ==================================================================== */
protected:
  /*=================================================================== */
  /**
        @brief     Create list of supported CP objects.

        @param     headOfCPObjectList
				   Ptr to first object in list of objects.

        @return    void

   */
  /*=================================================================== */
  void initCPObjectList(ACS_CHB_CPObjectList *headOfCPObjectList);


  /*=================================================================== */
    /**
          @brief     Find the CP object name.
					 The method will search the list of CP objects (headCPObjectList)
					 and return a pointer to the CPObjectValue int the object where
					 CPObjectItem corresponds to the parameter cpObjectValue.

          @param     cpObjectValue
  				     CP object item value.

          @return    char* ,A pointer to the CPObjectValue

     */
    /*=================================================================== */
  char *getCPObjectValueFromCPObjectList(char cpObjectValue[]);

  /*===================================================================
                          PROTECTED ATTRIBUTE
  =================================================================== */
  /*=================================================================== */
  /**
        @brief   pointer to internal object

                 Head of the CP object list.
   */
  /*=================================================================== */


  ACS_CHB_CPObjectList *headCPObjectList;

  /*=====================================================================
    	                        PUBLIC DECLARATION SECTION
    ==================================================================== */
public:

  /*=================================================================== */
      /**
            @brief     Returns a pointer to the next object in the list of objects.

            @return    ACS_CHB_CPObjectList
					   0-No more objects
					   *-Pointer to next object

       */
      /*=================================================================== */
  ACS_CHB_CPObjectList* get_next();

  /*=================================================================== */
        /**
              @brief     Appends a new object to the end of current list.

              @param     newElm
						 The new object to be added to the
                         end of the list

              @return    void

         */
        /*=================================================================== */
  void appendToList(ACS_CHB_CPObjectList* newElm);


  /*=================================================================== */
        /**
              @brief     Returns a pointer to the variable CPObjectItem.

              @return    char*
                         A pointer to the CPObjectItem

         */
        /*=================================================================== */
  const char* get_CPObjectItemPtr();

  /*=================================================================== */
        /**
              @brief     Returns a pointer to the variable CPObjectValue.

              @return    char*
                         A pointer to the CPObjectValue

         */
        /*=================================================================== */
  const char* get_CPObjectValuePtr();


  /*=================================================================== */
        /**
              @brief     Set a new value in the variable CPObjectValue

              @param     v
						 The new value

              @return    void

         */
        /*=================================================================== */
  void set_CPObjectValue(const char* v);

  /*=================================================================== */
        /**
              @brief     Returns the CPObjectValueChanged status.

              @return    ACS_CHB_FAIL/ACS_CHB_OK

         */
        /*=================================================================== */
  int get_CPObjectValueChanged();

  /*=================================================================== */
        /**
              @brief     Set the CPObjectValueChanged status.

              @param     v
						 ACS_CHB_FAIL: Value not changed
                         ACS_CHB_OK  : Value changed

              @return   void

         */
        /*=================================================================== */
  void set_CPObjectValueChanged(int v);

  /*===================================================================
                            PRIVATE DECLARATION SECTION
     =================================================================== */
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
  char	CPObjectItem[ACS_CHB_SMALL_BUFSIZE];

  /*=================================================================== */
    /**
          @brief   CPObjectValue
                   Save CP object item value
     */
    /*=================================================================== */
  char	CPObjectValue[ACS_CHB_SMALL_BUFSIZE];

  /*=================================================================== */
      /**
            @brief   CPObjectValueChanged
                     Holds status whether value of
		     		 CP object has changed since
					 last update of subsc.event list.

       */
      /*=================================================================== */
  int	CPObjectValueChanged;
  
  /*=================================================================== */
        /**
              @brief   Next
                       Pointer to next element.

         */
        /*=================================================================== */
  ACS_CHB_CPObjectList *Next;

  /*=================================================================== */
          /**
                @brief   Pre
                         Pointer to previous elements

           */
          /*=================================================================== */
  ACS_CHB_CPObjectList *Pre;

  /*=================================================================== */
            /**
                  @brief   CPObjectListCreated
                           TRUE if List is created from
						   this object and still exists.

             */
            /*=================================================================== */
  bool CPObjectListCreated;
};

#endif

