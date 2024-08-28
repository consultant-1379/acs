/*
 * acs_prc_runlevel.h
 *
 *  Created on: Mar 14, 2012
 *      Author: xlucpet
 */

#ifndef ACS_PRC_RUNLEVEL_H_
#define ACS_PRC_RUNLEVEL_H_

#include "acs_prc_types.h"
#include "ACS_PRC_ispapi.h"
#include "ACS_PRC_suInfo.h"
#include "ACS_TRA_Logging.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Connector.h>

class acs_prc_runlevel {
public:

	acs_prc_runlevel();
	virtual ~acs_prc_runlevel();

	void setlocalNode( const char* localNode ) { local_Node = localNode; };
	void setremoteNode( const char* remoteNode ) { remote_Node = remoteNode; };

	std::string getlocalNode() { return local_Node; };
	std::string getremoteNode() { return remote_Node; };

	void updateSUState ( string SuName, int SuState );

	bool calculateRunLevel ();
	void refreshSuList ( );

	int open();
	int close();

	int getRunLevel(std::string node);
private :

	bool sendIspEvent ( string Node, runLevelType NodeLevel, reasonType reason );
	bool sendPrcAlarm ( string Node, runLevelType NodeLevel, runLevelType previousRunLevel_remote );
	bool sendIspEvent_APState ( runLevelType Local_NodeLevel, runLevelType Remote_NodeLevel );

	bool isNodeShutDownOngoing();

	static runLevelType currentRunLevel_local;
	static runLevelType currentRunLevel_remote;

	static std::map<std::string,int> suStateSC1Map;
	static std::map<std::string,int> suStateSC2Map;

	void printMaps();
	int computeRunLevelSC1();
	int computeRunLevelSC2();

	Ispapi* prc_isp_Event;
	ACS_PRC_suInfo* data;

	std::string local_Node;
	std::string remote_Node;
	int counter;
	bool flag;

};

#endif /* ACS_PRC_RUNLEVEL_H_ */
