/*
 * ACS_CS_CpClusterManager.h
 *
 *  Created on: Jan 25, 2013
 *      Author: eanform
 */

#ifndef ACS_CS_CPCLUSTERMANAGER_H_
#define ACS_CS_CPCLUSTERMANAGER_H_

#include <ace/Semaphore.h>
#include <ace/RW_Mutex.h>


#include "ACS_CS_Event.h"
#include "ACS_CS_Thread.h"
#include "ACS_CS_ReaderWriterLock.h"
#include "ACS_CS_ImUtils.h"

#include <ace/Task_T.h>
#include <ace/Synch.h>
#include <ace/Message_Block.h>


class ACS_CS_NEHandler;

//Class used to exchange information about "IMM Actions"

class ACS_CS_OperationInfo : public ACE_Message_Block {

public:

	enum
	{
		MT_THREAD_SHUTDOWN = 0,
		MT_OP_MODE_CHANGE,
		MT_OM_PROFILE_CHANGE,
		MT_OM_REMOVE,
		MT_OM_ACTIVE,
		MT_CCF_IMPORT,
		MT_CCF_EXPORT,
		MT_CCF_ROLLBACK,
		MT_OM_PROFILE_NOTIFICATION_SUCCESS,
		MT_OM_PROFILE_NOTIFICATION_FAILURE,
		MT_OM_SET_PROFILES,
		MT_UNDEFINED = 0xFFFFFFFF
	};

	//Constructor
	inline ACS_CS_OperationInfo()
	: ACE_Message_Block(), OperationId(ACS_CS_ImmMapper::CP_CLUSTER_OP_NO_OPERATION),
	  ObjectType(-1),
	  clusterOpModeRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_CLUSTER_OP_MODE_REQUESTED),
	  omProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED),
	  filename(""),
	  profilePhase(ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_PHASE),
	  profileReason(ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_REASON),
	  apzProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED),
	  aptProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED)
	{
		msg_type(MT_UNDEFINED);
	}


	inline ACS_CS_OperationInfo(int msgType)
	: ACE_Message_Block(), OperationId(ACS_CS_ImmMapper::CP_CLUSTER_OP_NO_OPERATION),
	  ObjectType(-1),
	  clusterOpModeRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_CLUSTER_OP_MODE_REQUESTED),
	  omProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED),
	  filename(""),
	  profilePhase(ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_PHASE),
	  profileReason(ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_REASON),
	  apzProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED),
	  aptProfileRequested(ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED)
	{
		msg_type(msgType);
	}

	//Destructor
	inline virtual ~ACS_CS_OperationInfo() {}

	inline void setMsgType(int msgType) { msg_type(msgType); } ;

	//---Internal Operation Parameters---//
public:
	int OperationId;
	int ObjectType;

	/* ---------------------------- */
	/* Cp Cluster Action Parameters */
	/* ---------------------------- */

	int clusterOpModeRequested;  //"changeClusterOpMode" operation

	int omProfileRequested;    	 //"changeOmProfile" operation

	std::string filename; 				 //"importCcFile"

	/* ---------------------------------------- */
	/* Advanced Configuration Action Parameters */
	/* ---------------------------------------- */

	int profilePhase;

	int profileReason;

	int apzProfileRequested;

	int aptProfileRequested;

private:
	// Disallow copying and assignment.
	//ACS_CS_OperationInfo(const ACS_CS_OperationInfo&);
	//void operator= (const ACS_CS_OperationInfo&);

};

class ACS_CS_CpClusterManager: public ACE_Task<ACE_MT_SYNCH> {

public:
	ACS_CS_CpClusterManager(ACS_CS_NEHandler*);

	virtual ~ACS_CS_CpClusterManager();

	/**
	 * 	@brief  Run by a daemon thread
	 */
	virtual int svc(void);

	/**
	 * 	@brief	This method initializes a task and prepare it for execution
	 */
	virtual int open(void *args = 0);

	/**
			@brief  This method is called by ACE_Thread_Exit, as hook, on svc termination
	 */
	virtual int close(u_long flags = 0);

	bool wakeUpEvent(ACS_CS_OperationInfo * opInfo);

	void shutDown(bool isServiceShutDown);

	inline bool isRunningThread(){ return m_thread_running;}

	bool restoreCpClusterFromDisk();

	inline bool isClusterAligned(){ return m_cluster_aligned_after_restore;}

	void storeCpClusterOnDisk(const ACS_CS_ImModel* model);


protected:

	ACS_CS_NEHandler* m_neHandler;

private:

	void handleRequest(ACS_CS_OperationInfo * const actionInfo);

	bool checkParameters(ACS_CS_OperationInfo * const actionInfo);

	bool createCpClusterEntry();

	bool setClusterAlignedAfterRestore(bool aligned);

	ACS_CS_ImModel* findDifferences(ACS_CS_ImModel* newModel, ACS_CS_ImModel* currModel) const;

	ACS_CS_ImModel * getCandidateCcFileModel(const ACS_CS_ImModel* model);

	bool m_thread_running;

	bool m_cluster_aligned_after_restore;
};

#endif /* ACS_CS_CPCLUSTERMANAGER_H_ */
