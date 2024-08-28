/**
 * @file Cute_ACS_NSF_Server.h
 * @brief header file for Cute_ACS_NSF_Server
 * It consists declaration of the cute test class for ACS_NSF_Server
 * @version 1.1.1
 */
#ifndef _CUTE_ACS_NSF_Server_H_
#define _CUTE_ACS_NSF_Server_H_
/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"
//#include "ACS_NSF_Server.h"



/* Forward declaration */
class ACS_NSF_Server;
class ACS_NSF_Common;
/**
 * @brief Cute_ACS_NSF_Server
 * Cute test class having test functions to test ACS_NSF_Server_
 */
//
//enum NODE_NAME
//{
//	Node_A=1,    //!< NODE_A
//	Node_B=2, 	 //!< NODE_B
//	Undefined=3  //!< UNDEFINED
//};
class Cute_ACS_NSF_Server
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_ACS_NSF_Server();
	~Cute_ACS_NSF_Server();
	static void vsetNodeSpecificParamsTest();
	static void vGetPhaParameters();
	static void vSetDefaultFailoverCount();
	static void vSetDefaultLastFailoverTime();
	static void vSetDefaultAlarmState();
	static void vStartPingServer();
	static void vSaveSystemTime();
	static void vSetDefaultNetworkSurveillanceValue();
	static void vSetDefaultIMMParamsSurveillanceflag1();
	static void vSetDefaultIMMParamsSurveillanceflag0();
	static void vGetConfigParameters();
	static void vGetDefaultGateways();
	static void vGetDefaultGateways2();
	static void vCeaseAllAlarms();
	static void vCeaseAllAlarms2();
	static void vStopServer();
	static void vTerminateGatewaySup();
	static void vOkToFailover();
	static void vVerifyHeartBeat();
	static void vCountBlockedGw();
	static void vPingThread();
	static void vNetworkSurveillance();
	static void vCheckIfFailoverForPS();
	static void vReportEvent();
	static void init();

	static ACS_NSF_Server* getPoNsfServer(){
		return m_poNsfServer;
	}
	static ACS_NSF_Common  *getPoNsfCommonptr(){
		return m_poNsfCommonptr;
	}
	static void vNsfServerCleanup();

	static cute::suite make_suite_Cute_ACS_NSF_Server();

private:
	/**
	 * @brief
	 * its a static reference of type ACS_NSF_Server
	 */
	static ACS_NSF_Server* m_poNsfServer;
	static ACS_NSF_Common* m_poNsfCommonptr;
};

#endif
