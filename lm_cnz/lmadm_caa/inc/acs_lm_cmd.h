#ifndef ACS_LM_COMMAND_H
#define ACS_LM_COMMAND_H

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include <ace/ACE.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_Cmd
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
					@brief       Parameterised constructor for ACS_LM_Cmd

					@param       argc				:ACE_INT16

					@param       cmdCode            :ACE_INT16

					@param       argv               :ACE_TCHAR

					@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Cmd(ACE_INT16 cmdCode, ACE_INT16 argc, ACE_TCHAR** argv);
	/*=================================================================== */
	/**
					@brief       Default constructor for ACS_LM_Cmd

					@exception   None
	 */
	/*=================================================================== */
	ACS_LM_Cmd(void);
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_LM_Cmd

					@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_Cmd(void);

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	ACE_INT16 cmdCode;
	ACE_INT16 cmdSize;
	std::list<std::string> cmdArgs;
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
