//  *********************************************************
//   COPYRIGHT Ericsson 2010.
//   All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson 2010.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson 2010 or in
//  accordance with the terms and conditions stipulated in
//  the agreement/contract under which the program(s) have
//  been supplied.
//
//  *********************************************************

#ifndef ACS_CS_Master_h
#define ACS_CS_Master_h 1

#include <vector>
#include <set>
#include <list>
#include <map>

#include <ace/Singleton.h>
#include "ace/RW_Mutex.h"
#include <ace/Recursive_Thread_Mutex.h>

#include "ACS_CS_Thread.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_ImModel.h"


class ACS_CS_TCPServer;
class ACS_CS_TableOperationEntry;
class ACS_CS_API_TableLoader;


/** @class ACS_CS_Master
 *	@brief Replication class for master CS
 *	@author xminaon
 *
 *	A class used by the master CS (the CS instance on AP1) to replicate data to the slaves and receive
 *	table operations from the slaves. This class is a singleton.
 */
class ACS_CS_Master: public ACS_CS_Thread
{
	public:

		//typedef std::map<ACS_CS_TableOperationEntry *, ACS_CS_TCPClient *> entryMap;
		typedef std::map<ACS_CS_ImModel *, ACS_CS_TCPClient *> entryMap;

		virtual int exec ();

		/** @brief Start the thread
		 */
		static void start ();

		/** @brief Stop the thread
		 */
		static void stop ();

		/** @brief Sends an operation to all the slaves
		 *  param[in] tableOperation     The table operation entry that should be sent
		 */
		static void sendOperation(const ACS_CS_TableOperationEntry * tableOperation);

		static void sendOperation(const ACS_CS_ImModel * tableOperation);

		static bool isIncomingEntry(const ACS_CS_ImBase* entry);

		static void finalize();

		friend class ACE_Singleton<ACS_CS_Master, ACE_Recursive_Thread_Mutex>;

	protected:

		ACS_CS_Master();

		~ACS_CS_Master();

		static ACS_CS_Master* instance();

	private:

		ACS_CS_Master(const ACS_CS_Master &right);

		ACS_CS_Master & operator=(const ACS_CS_Master &right);

		void startListener();

		void openNewConnection(int tcpServerHandle);

		void sendTables(ACS_CS_TCPClient * tcpClient);

		void checkOutQueue();

		//void sendToAllSlaves(const ACS_CS_TableOperationEntry * entry);

		void sendToAllSlaves(const ACS_CS_ImModel * entry);

		ACS_CS_TCPClient::LinkStatus_t handleClientRequest(ACS_CS_TCPClient * tcpClient);

		typedef ACE_Singleton<ACS_CS_Master, ACE_Recursive_Thread_Mutex> instance_;

		ACS_CS_TCPServer *tcpServer;

		std::vector<ACS_CS_TCPClient *> connectedSlaves;

		std::list<ACS_CS_TableOperationEntry *> outQueue;

		std::list<ACS_CS_ImModel *> outModelQueue;

		ACS_CS_EventHandle shutdownEvent;

		ACS_CS_EventHandle sendOperationEvent;

		ACS_CS_API_TableLoader *tableLoader;

		ACE_RW_Mutex outModelQueueLock;

		ACE_RW_Mutex incomingEntryLock;

		entryMap incomingEntries;
};

#endif // ACS_CS_Master_h
