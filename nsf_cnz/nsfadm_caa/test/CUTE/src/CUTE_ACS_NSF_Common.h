/**
 * @file Cute_ACS_NSF_Common.h
 * @brief header file for Cute_ACS_NSF_Common
 * It consists declaration of the cute test class for ACS_NSF_Common
 * @version 1.1.1
 */
#ifndef _CUTE_ACS_NSF_Common_H_
#define _CUTE_ACS_NSF_Common_H_
/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"



/* Forward declaration */
class ACS_NSF_Common;

/**
 * @brief Cute_ACS_NSF_Common
 * Cute test class having test functions to test ACS_NSF_Common_
 */
class Cute_ACS_NSF_Common
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_ACS_NSF_Common();
	~Cute_ACS_NSF_Common();
	static void vsetRegTime();
	static void vgetRegTime();
	static void vgetNode();
	static void vgetActiveNodeFlag();
	static void vgetRegTimeNegative();
	static void vsetRegTimeNegative();
	static void vcalculateStartStopValueactive();
	static void vcalculateStartStopValuepassive();
	static void vsetAndgetRegWordForfailover();
	static void vsetAndgetRegWordForAlarmstate2B();
	static void vsetAndgetRegWordForAlarmstateB();
	static void vsetAndgetRegWordForAlarmstate2A();
	static void vsetAndgetRegWordForAlarmstateA();
	static void vsetAndgetRegWordForSurvellianceActFlag();
	static void vsetAndgetRegWordForNsfDummyFlag();
	static void vsetNodeSpecificParamsTest();


	static void init();

	static ACS_NSF_Common *getPoNsfCommon(){
		return m_poNsfCommon;
	}

	static void vNsfCommonCleanup();

	static cute::suite make_suite_Cute_ACS_NSF_Common();

private:
	/**
	 * @brief
	 * its a static reference of type ACS_NSF_Common
	 */
	static ACS_NSF_Common* m_poNsfCommon;
};

#endif
