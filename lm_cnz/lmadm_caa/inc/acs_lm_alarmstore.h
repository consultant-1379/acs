//******************************************************************************
//
//  NAME
//     ACS_LM_AlarmStore.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR 
//     2008-12-08 by XCSVEMU PA1
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#ifndef _ACS_LM_ALARMSTORE_H_ 
#define _ACS_LM_ALARMSTORE_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */

#include "acs_lm_common.h"
#include <ace/ACE.h>
//#define ALARM_HOME	"/data/acs/data/lm"
#define ALARM_HOME	"/data/acs/data/ACS-LM"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_AlarmStore
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
   						  PUBLIC ATTRIBUTE
   	=================================================================== */

	/*===================================================================
   							   PUBLIC METHOD
   	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_AlarmStore

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_AlarmStore();
	//*=================================================================== */
	/**
				@brief      insertIntoA2List		:Method used to insert the LK
													 into a list to raise the A2 alarm

				@param      lkId					:string

				@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool insertIntoA2List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      insertIntoA3List		:Method used to insert the LK
														 into a list to raise the A3 alarm

					@param      lkId					:string

					@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool insertIntoA3List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      removeFromA2List		:Method used to remove the file
															 from the list

					@param      lkId					:string

					@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool removeFromA2List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      removeFromA3List		:Method used to remove the file
															 from the list

					@param      lkId					:string

					@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool removeFromA3List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      isLkPresentInA2List		:Method used to check the
														 presence of LK in the A2 list

					@param      lkId					:string

					@return 	true/false              :bool
	 */
	/*=================================================================== */

	bool isLkPresentInA2List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      isLkPresentInA3List		:Method used to check the
														 presence of LK in the A3 list

					@param      lkId					:string

					@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool isLkPresentInA3List(const std::string& lkId);
	//*=================================================================== */
	/**
					@brief      clearA2List				:Method used to clear the A2 list

					@return 	void
	 */
	/*=================================================================== */
	void clearA2List();
	//*=================================================================== */
	/**
					@brief      clearA3List				:Method used to clear the A3 list

					@return 	void
	 */
	/*=================================================================== */
	void clearA3List();
	//*=================================================================== */
	/**
					@brief      getLkfMissingAlarmState		:Method to get the status of
															 missing LK file

					@return 	bool						:true/false
	 */
	/*=================================================================== */
	bool getLkfMissingAlarmState();	
	//*=================================================================== */
	/**
					@brief      setLkfMissingAlarmState		:Method to set the status of
															 missing LK file
					@param 		state 						:bool

					@return 	void
	 */
	/*=================================================================== */
	void setLkfMissingAlarmState(bool state);	
	//*=================================================================== */
	/**
					@brief      getEmergencyAlarmState		:Method to get the status of
																 emergency alarm mode

					@return		bool
	 */
	/*=================================================================== */
	bool getEmergencyAlarmState();
	//*=================================================================== */
	/**
					@brief      setEmergencyAlarmState		:Method to set the status of
															 emergency alarm
					@param 		state 						:bool

					@return 	void
	 */
	/*=================================================================== */
	void setEmergencyAlarmState(bool state);
	//*=================================================================== */
	/* LM Maintenance Mode Start*/
	/**
                                        @brief      getMaintenanceAlarmState              :Method to get the status of
                                                                                                                                 maintenance mode alarm 

                                        @return         bool
         */
        /*=================================================================== */
        bool getMaintenanceAlarmState();
        //*=================================================================== */
        /**
                                        @brief      setMaintenanceAlarmState              :Method to set the status of
                                                                                                                         maintenance mode alarm
                                        @param          state                                           :bool

                                        @return         void
         */
        /*=================================================================== */
        void setMaintenanceAlarmState(bool state);
        //*=================================================================== */
	/* LM Maintenance Mode End */
	/**
					@brief      getOwnerOfFile		:Method to get the owner of the file


					@return 	string
	 */
	/*=================================================================== */
	std::string getOwnerOfFile();
	//*=================================================================== */
	/**
					@brief      setOwnerOfFile		:Method to set the owner of the file


					@return 	string
	 */
	/*=================================================================== */
	void setOwnerOfFile(std::string);
	//*=================================================================== */
	/**
					@brief      sizeInBytes

					@return 	ACE_INT64
	 */
	/*=================================================================== */
	ACE_INT64 sizeInBytes();
	//*=================================================================== */
	/**
					@brief      sizeInBytes

					@param 		 destBuff        :ACE_TCHAR

					@param 		 destPos        :ACE_UINT64

					@return 	void
	 */
	/*=================================================================== */

	void store(ACE_TCHAR* destBuff, ACE_UINT64& destPos);
	//*=================================================================== */
	/**
					@brief      ACS_LM_AlarmStore

					@param 	    alarmFile        :string

					@return 	static
	 */
	/*=================================================================== */
	static ACS_LM_AlarmStore* load(const std::string& alarmFile);
	//*=================================================================== */
	/**
					@brief      validate

					@param 		lkList            :list

					@return 	void
	 */
	/*=================================================================== */
	void validate(const std::list<LkData*>& lkList);
	//*=================================================================== */
	/**
					@brief      commit

					@return 	bool : true/false
	 */
	/*=================================================================== */

	bool commit();
	//*=================================================================== */
	/**
					@brief      isA3ListEmpty		:Method used to check whether
													 A2 list is empty


					@return 	bool					:true/false
	 */
	/*=================================================================== */
	bool isA3ListEmpty();
	//*=================================================================== */
	/**
					@brief      isA3ListEmpty		:Method used to check whether
													 A3 list is empty


					@return 	bool					:true/false
	 */
	/*=================================================================== */
	bool isA2ListEmpty();

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	std::string alarmFile;
	std::list<std::string> alarm3LkList;
	std::list<std::string> alarm2LkList;
	ACE_TCHAR emergencyAlarmState;
	ACE_TCHAR maintenanceAlarmState; /* LM Maintenance Mode */ 
	ACE_TCHAR lkfMissingAlarmState;
	std::string ownerOfFile;
	static ACS_LM_AlarmStore* load(const ACE_TCHAR * srcBuff, ACE_UINT64& srcPos);
	ACS_LM_AlarmStore();	

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	//Disable the copy.
	ACS_LM_AlarmStore(const ACS_LM_AlarmStore&);
	ACS_LM_AlarmStore operator =(const ACS_LM_AlarmStore&);
};

#endif
