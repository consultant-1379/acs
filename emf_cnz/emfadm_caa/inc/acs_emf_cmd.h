#ifndef ACS_EMF_COMMAND_H
#define ACS_EMF_COMMAND_H

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_emf_common.h"
#include <ace/ACE.h>

// Return code definition
#define CMD_OK							0
#define CMD_ERROR						1

namespace acs_emf_cmd_ns {
enum emf_cmdCode {
	 EMF_CMD_CopyToMedia = 3,
	 EMF_CMD_EraseAndCopyToMedia = 4,
	 EMF_CMD_CheckForDVDPresence = 10,
	 EMF_CMD_CheckForDVDMediaPresence = 11,
	 EMF_CMD_FetchMediaInfo = 12,
	 EMF_CMD_MountMedia = 13,
	 EMF_CMD_UnmountMedia = 14,
	 EMF_CMD_SyncMedia =16,
	 EMF_CMD_RemovePassiveShare =17,
	 EMF_CMD_PassiveShare = 18,
	 EMF_CMD_FormatMedia = 19,
	 EMF_CMD_SyncMediaOnPassive = 20,
	 EMF_CMD_UnmountActiveMedia = 21,
	 EMF_CMD_CheckAndMountMedia = 22
};

}


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_EMF_Cmd
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
					@brief       Parameterised constructor for ACS_EMF_Cmd

					@param       argc				:ACE_INT16

					@param       cmdCode            :ACE_INT16

					@param       argv               :ACE_TCHAR

					@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_Cmd(ACE_INT16 cmdCode);
	/*=================================================================== */
	/**
					@brief       Default constructor for ACS_EMF_Cmd

					@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_Cmd(void);
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_EMF_Cmd

					@exception   None
	 */
	/*=================================================================== */
	~ACS_EMF_Cmd(void);

        //*=================================================================== */
        /**
                        @brief      addArguments

                        @param       alist            :list<string>
			
                        @return         none
         */
        /*=================================================================== */

	void addArguments(std::list<string> alist);

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

        /*=================================================================== */
        /**
        @brief   cmdCode
         */
        /*=================================================================== */

	ACE_INT16 cmdCode;
        /*=================================================================== */
        /**
        @brief   cmdSize
         */
        /*=================================================================== */

	ACE_INT16 cmdSize;
        /*=================================================================== */
        /**
        @brief   cmdArgs
         */
        /*=================================================================== */

	std::list<std::string> cmdArgs;
        /*===================================================================
                                                         PRIVATE METHOD
         =================================================================== */

	//*=================================================================== */
	/**
			@brief      copyBuffer

			@post        None

			@param  	destBuf                     :ACE_TCHAR

			@param  	destPos            		    :ACE_UINT16

		    @param  	srcBuf     					:ACE_TCHAR

			@param  	srcPos             			:ACE_UINT16

			@param  	srcLen                      :ACE_UINT16

			@return 	 void
	 */
	/*=================================================================== */

	static void copyBuffer(ACE_TCHAR * destBuf, const ACE_UINT16 destPos, const ACE_TCHAR* srcBuf, const ACE_UINT16 srcPos, const ACE_UINT16 srcLen);

public:
	/*===================================================================
   						  PUBLIC ATTRIBUTE
   	=================================================================== */

	/*===================================================================
   							   PUBLIC METHOD
   	=================================================================== */
	//*=================================================================== */
	/**
			@brief      commandCode

			@return 	ACE_INT16
	 */
	/*=================================================================== */
	inline ACE_INT16 commandCode() const
	{
		return cmdCode;
	}
	//*=================================================================== */
	/**
			@brief      commandArguments

			@return 	list
	 */
	/*=================================================================== */

	inline std::list<std::string> commandArguments() const
			{
		return cmdArgs;
			}
	//*=================================================================== */
	/**
			@brief      size

			@return 	ACE_INT16
	 */
	/*=================================================================== */

	inline ACE_INT16 size() const
	{
		return cmdSize;
	}
	//*=================================================================== */
	/**
			@brief      toBytes

			@return 	ACE_TCHAR
	 */
	/*=================================================================== */

	ACE_TCHAR* toBytes() const;
	//*=================================================================== */
	/**
			@brief      fromBytes

			@return 	ACE_TCHAR
	 */
	/*=================================================================== */
	bool fromBytes(ACE_TCHAR* data);
	//*=================================================================== */
	/**
				@brief      decode16

				@param  	buf 						:ACE_TCHAR

				@param 		pos					        :ACE_UINT32

				@return 	ACE_UINT32
	 */
	/*=================================================================== */

	static ACE_INT32 decode16(ACE_TCHAR* buf, const ACE_UINT32 pos);
	//*=================================================================== */
	/**
					@brief      encode16

					@param  	buf				     :ACE_TCHAR

					@param	 	pos					 :ACE_UINT16

					@param	 	value				 :ACE_UINT16

					@return    void
	 */
	/*=================================================================== */

	static void encode16(ACE_TCHAR* buf, const ACE_UINT16 pos, const ACE_UINT16 value);
};

#endif
