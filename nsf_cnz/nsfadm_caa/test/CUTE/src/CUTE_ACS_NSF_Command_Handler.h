/**
 * @file CUTE_ACS_NSF_Command_Handler.h
 * @brief header file for CUTE_ACS_NSF_Command_Handler.cpp
 * It consists declaration of the cute test class for CUTE_ACS_NSF_Command_Handler
 * @version 1.1.1
 */
#ifndef _CUTE_NSF_Command_Handler_H_
#define _CUTE_NSF_Command_Handler_H_
/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"





/* Forward declaration */
class ACS_NSF_CommandHandler;

/**
 * @brief Cute_NSF_Command_Handler
 * Cute test class having test functions to test Cute_NSF_Command_Handler
 */

class Cute_NSF_Command_Handler
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_NSF_Command_Handler();
	~Cute_NSF_Command_Handler();
	/**
					 * @brief
					 *static test function to test Cute_NSF_Command_Handler::bCheckForTimestampFile()
					 * No arguments
					 * @return bool
					 *
					 */
	static void vvalidateChange();
	static void vvalidateChangeNegative();
	static void vModifySurvellienceflag();
	static void vModifyNumberFailover();
	static void vModifyRouterResponse();
	static void vModifyPingPeriod();
	static void vModifyResetTime();
	static void vModifyFailovers();
	static void vModifyActiveFlag();
	static void vModifyFailoverPriority();
	static void vvalidateNSValue();
//	static void vtestModify();

	static void init();

	static ACS_NSF_CommandHandler* getCommandhandler(){
		return objCommandHandler;
	}

	static void vCommandHandlerCleanup();

	/**
											 * @brief
											 * static function to make the test suite for NSF_Biostime_Recovery 
											 * No arguments
											 * @return void
											 *
											 */
	static cute::suite make_suite_Cute_NSF_Command_Handler();
//private members declaration
private:
	/**
	 * @brief
	 * its a static reference of type ACS_NSF_CommandHandler
	 */
	static ACS_NSF_CommandHandler *objCommandHandler;

};

#endif
