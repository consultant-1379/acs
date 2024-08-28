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

#ifndef ACS_CS_Slave_h
#define ACS_CS_Slave_h 1

#include <sstream>
#include <csignal>

#include <ace/Singleton.h>
#include "ace/Mutex.h"
#include <ace/Recursive_Thread_Mutex.h>

#include "ACS_CS_Thread.h"
#include "ACS_CS_Event.h"
#include "ACS_CS_TCPClient.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_ImModel.h"

//class ACS_CS_TableOperationEntry;

/** @class ACS_CS_Master
 *	@brief Replication class for slave CS
 *	@author xminaon
 *
 *	A class used by slave CS (CS instances on APs other than AP1) to replicate data to the master CS
 *	and receive table operations from the master CS. This class is a singleton.
 */
class ACS_CS_Slave: public ACS_CS_Thread
{
	public:

		virtual int exec ();

		/** @brief Start the thread
				 */
		static void start ();

		/** @brief Stop the thread
		 */
		static void stop ();

		/** @brief Sends an operation to the master for verification
		 *  param[in] tableOperation     The table operation entry that should be sent
		 *  @return true if successful, otherwise false
		 */
		static bool sendToMaster(ACS_CS_ImModel * model);

		/** @brief Used to check if the slave class is updating the tables (i.e. writing to IMM)
		 *  @return true if yes, otherwise false
		 */
		static bool isUpdatingTables(const ACS_CS_ImBase * entry);

		static bool isUpdatingTables(const ACS_CS_ImModel * model);

		/** @brief Used to check if an entry is oroginating from an update
		 *  @return true if yes, otherwise false
		 */
		static bool isUpdatingEntry(const ACS_CS_ImModel * model);

		static bool isUpdatingEntry(const ACS_CS_ImBase * entry);

		friend class ACE_Singleton<ACS_CS_Slave, ACE_Recursive_Thread_Mutex>;

//		static ACS_CS_ImModel *getModel(){return ACS_CS_Slave::instance()->copyOfRequest;};

		static void finalize();

		static bool isReadyForRequest();

	protected:

		ACS_CS_Slave();

		~ACS_CS_Slave();

		static ACS_CS_Slave* instance();

	private:

		ACS_CS_Slave(const ACS_CS_Slave &right);

		ACS_CS_Slave & operator=(const ACS_CS_Slave &right);

		void connect();

		void close();

		ACS_CS_TCPClient::LinkStatus_t read();

		ACS_CS_TCPClient::LinkStatus_t send();

		bool readTables(int size);

		bool readNewOperation(int size);

		void resetStream();

		std::string getVersionString(int version);

		//ACS_CS_TCPClient::LinkStatus_t readMasterReply(int size);

		typedef ACE_Singleton<ACS_CS_Slave, ACE_Recursive_Thread_Mutex> instance_;

		ACS_CS_TCPClient * tcpClient;

		ACS_CS_EventHandle shutdownEvent;

		ACS_CS_EventHandle masterReplyEvent;

		ACS_CS_EventHandle masterRequestEvent;

		std::stringstream * inputStream;

		ACS_CS_ImModel * masterRequest;

//		ACS_CS_TableOperationEntry * updateRequest;

		ACS_CS_ImModel * updateRequest;

		ACS_CS_ImModel * masterTables;

		ACS_CS_NS::ACS_CS_ReplicationData * replicationData;

		bool masterResult;

		bool updatingTables;

		ACE_RW_Mutex lock;

		bool readyForRequest;

};

#endif // ACS_CS_Slave_h
