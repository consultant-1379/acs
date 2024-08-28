/*
 * ACS_CS_CpClusterManager.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: eanform
 */

#include "ACS_CS_CpClusterManager.h"

#include "ACS_CS_ImCpCluster.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImRepository.h"

#include "ACS_CS_NEHandler.h"

#include "ACS_CS_Util.h"
#include "ACS_CS_EventReporter.h"

#include "ACS_CS_Registry.h"

#include "ACS_CS_Trace.h"
#include "ACS_CS_API_Util_Internal.h"

#include <vector>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "ACS_CS_ImSerialization.h"

ACS_CS_Trace_TDEF(ACS_CS_CpClusterManager_TRACE);

using std::string;
using std::ostringstream;
using namespace ACS_CS_NS;

ACS_CS_CpClusterManager::ACS_CS_CpClusterManager(ACS_CS_NEHandler* neHandler) : m_neHandler(neHandler), m_thread_running(false)
{
	createCpClusterEntry();

	//create cs folder on data disk
	if (ACS_APGCC::create_directories(ACS_DATA_PATH.c_str(), ACCESSPERMS) == -1)
	{
		ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_ERROR, "(%t) [%s@%d] Failed to create the folder %s.\n", __FUNCTION__, __LINE__, ACS_DATA_PATH.c_str()));
	}

	//fetch cluster aligned status from advancedConfiguration object
	m_cluster_aligned_after_restore = ACS_CS_API_Util_Internal::isClusterAlignedAfterRestore();
}

ACS_CS_CpClusterManager::~ACS_CS_CpClusterManager()
{
}

int ACS_CS_CpClusterManager::open(void * /*args*/)
{
	int result;
	// start event loop by svc thread
	//result = activate(THR_NEW_LWP| THR_DETACHED | THR_INHERIT_SCHED); //THR_DETACHED means that you do not have to call wait to synchronize at the termination

	result = activate(THR_NEW_LWP| THR_JOINABLE | THR_INHERIT_SCHED); // THR_JOINABLE means that you have to call wait
	return result;
}

int ACS_CS_CpClusterManager::close(u_long /*flags*/)
{
	// dequeue new messages if any
	msg_queue_->flush();

	m_thread_running = false;

	return 0;
}

void ACS_CS_CpClusterManager::shutDown(bool isServiceShutDown)
{

	if (isServiceShutDown)
	{
		// Deactive the queue as forced closure
		msg_queue_->deactivate();
	}
	else
	{
		ACS_CS_OperationInfo *stopMsg = new (std::nothrow) ACS_CS_OperationInfo(ACS_CS_OperationInfo::MT_THREAD_SHUTDOWN);
		if( NULL != stopMsg)
		{
			int putResult = putq(stopMsg);

			if(putResult < 0)
			{
				stopMsg->release();
				// Deactive the queue as forced closure
				msg_queue_->deactivate();
			}
		}
		else
		{
			// Deactive the queue as forced closure
			msg_queue_->deactivate();
		}
	}

	//Block until there are no more threads running in this task
	if (-1 == wait())
	{
		ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_ERROR,
					"(%t) ACS_CS_CpClusterManager::shutDown()\n Wait error\n"));;
	}
}

int ACS_CS_CpClusterManager::svc ()
{
	ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_INFO, "(%t) [%s@%d] Thread starting\n", __FUNCTION__, __LINE__));
	m_thread_running = true;

	bool _exit = false;



	while ( ! _exit )	// Loop as long as exit_ is false (set to false by the deactivate function)
	{
		ACE_Message_Block* base_msg = 0;
		ACS_CS_OperationInfo *actionInfo = 0;
		int returnValue = getq(base_msg);

		if (-1 == returnValue)		// Queue deactivated
		{
			ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_INFO, "(%t) [%s@%d] Queue deactivated.\n", __FUNCTION__, __LINE__));
			_exit = true;					// Leave this thread function
		}
		else
		{
			actionInfo = reinterpret_cast<ACS_CS_OperationInfo*> (base_msg);

			if (actionInfo->msg_type() == ACS_CS_OperationInfo::MT_THREAD_SHUTDOWN)
			{
				ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_INFO, "(%t) [%s@%d] Shutdown event signaled.\n", __FUNCTION__, __LINE__));
				_exit = true;					// Leave this thread function
			}
			else
			{
				//ACS_OS::
				handleRequest(actionInfo);
			}

			actionInfo->release();
		}

	}
	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] Thread is stopping", __FUNCTION__, __LINE__));
	m_thread_running = false;

	return 0;
}


bool ACS_CS_CpClusterManager::wakeUpEvent(ACS_CS_OperationInfo *opInfo)
{
	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - In", __FUNCTION__, __LINE__));

	bool result = true;
	if (opInfo && checkParameters(opInfo))
	{
		if (opInfo && (putq(opInfo) < 0))
		{
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Error putq", __FUNCTION__, __LINE__));
			result = false;
		}
	}
	else
	{
		ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Error checkParameters", __FUNCTION__, __LINE__));
		result = false;
	}

	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Out", __FUNCTION__, __LINE__));
	return result;
}

void ACS_CS_CpClusterManager::handleRequest(ACS_CS_OperationInfo * const actionInfo)
{
	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process Request", __FUNCTION__, __LINE__));

		if(!actionInfo)
			return;

		switch (actionInfo->msg_type())
		{
		case ACS_CS_OperationInfo::MT_OP_MODE_CHANGE:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_OP_MODE_CHANGE", __FUNCTION__, __LINE__));
			m_neHandler->setClusterOpModeChange(static_cast<ACS_CS_API_ClusterOpMode::Value>(actionInfo->clusterOpModeRequested));
			break;
		case ACS_CS_OperationInfo::MT_OM_PROFILE_CHANGE:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_OM_PROFILE_CHANGE", __FUNCTION__, __LINE__));
			m_neHandler->setClusterOmProfile(actionInfo->OperationId, actionInfo->omProfileRequested, actionInfo->apzProfileRequested, actionInfo->aptProfileRequested);
			break;
		case ACS_CS_OperationInfo::MT_OM_REMOVE:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_OM_REMOVE", __FUNCTION__, __LINE__));
			m_neHandler->removeSupportedOmProfiles();
			break;
		case ACS_CS_OperationInfo::MT_OM_ACTIVE:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_OM_ACTIVE", __FUNCTION__, __LINE__));
			m_neHandler->setClusterOmProfile(actionInfo->OperationId);
			break;
		case ACS_CS_OperationInfo::MT_CCF_IMPORT:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_CCF_IMPORT", __FUNCTION__, __LINE__));
			m_neHandler->importCommandClassificationFile(actionInfo->filename);
			break;
		case ACS_CS_OperationInfo::MT_CCF_EXPORT:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_CCF_EXPORT", __FUNCTION__, __LINE__));
			m_neHandler->exportCommandClassificationFile();
			break;
		case ACS_CS_OperationInfo::MT_CCF_ROLLBACK:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Process MT_CCF_ROLLBACK", __FUNCTION__, __LINE__));
			m_neHandler->setClusterOmProfile(actionInfo->OperationId);
			break;
		case ACS_CS_OperationInfo::MT_OM_PROFILE_NOTIFICATION_SUCCESS:
			if (m_neHandler->handleSetOmProfileNotificationStatus(true, static_cast <ACS_CS_API_OmProfilePhase::PhaseValue>(actionInfo->profilePhase), static_cast <ACS_CS_API_Set::ReasonType>(actionInfo->profileReason)) != 0)
			{
				ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Error: ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_SUCCESS", __FUNCTION__, __LINE__));
			}
			break;
		case ACS_CS_OperationInfo::MT_OM_PROFILE_NOTIFICATION_FAILURE:
			if (m_neHandler->handleSetOmProfileNotificationStatus(false, static_cast <ACS_CS_API_OmProfilePhase::PhaseValue>(actionInfo->profilePhase), static_cast <ACS_CS_API_Set::ReasonType>(actionInfo->profileReason)) != 0)
			{
				ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Error: ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_FAILURE", __FUNCTION__, __LINE__));
			}
			break;
		default:
			ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_DEBUG, "[%s@%d] Action not recognized!",
					__FUNCTION__, __LINE__));
			break;
		}

	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Out", __FUNCTION__, __LINE__));
}

bool ACS_CS_CpClusterManager::checkParameters(ACS_CS_OperationInfo *actionInfo)
{
	bool result = false;

	if(!actionInfo)
		return false;

	if (actionInfo->ObjectType == CPCLUSTER_T)
	{
		switch(actionInfo->OperationId)
		{
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_CLUSTER_OP_MODE_ID):
				if (actionInfo->clusterOpModeRequested != ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_CLUSTER_OP_MODE_REQUESTED)
				{
					//check if change OmProfile ongoing
					ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
					m_neHandler->getOmProfilePhase(omProfilePhase);
					if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
					{
						//compare with current ClusterOpMode
						ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
						m_neHandler->getClusterOpMode(currentClusterOpMode);

						if ((currentClusterOpMode != ACS_CS_API_ClusterOpMode::SwitchingToExpert)
							&& (currentClusterOpMode != ACS_CS_API_ClusterOpMode::SwitchingToNormal))
						{
							result = true;
						}
					}
				}
				break;
			default:
				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - EVENT NOT RECOGNIZED: %d", __FUNCTION__, __LINE__, actionInfo->OperationId));
				break;
		}
	}
	else if (actionInfo->ObjectType == OMPROFILEMANAGER_T)
	{

		switch(actionInfo->OperationId)
		{
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_CHANGE_OM_PROFILE_ID):
				if(actionInfo->omProfileRequested != ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED)
				{
					//check if change OmProfile ongoing
					ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
					m_neHandler->getOmProfilePhase(omProfilePhase);
					if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
					{
						//check if change ClusterOpMode ongoing
						ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
						m_neHandler->getClusterOpMode(currentClusterOpMode);
						if ((currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToExpert)
								&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToNormal)){
							result = true;
						}
					}
				}
				break;

			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_REMOVE_OM_PROFILE_ID):
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_ACTIVE_OM_PROFILE_ID):
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_EXPORT_CCF_ID):
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_ROLLBACK_FILE_ID):
				//check if change OmProfile ongoing
				ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
				m_neHandler->getOmProfilePhase(omProfilePhase);
				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - CURRENT PHASE: %d", __FUNCTION__, __LINE__, omProfilePhase));
				if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
				{
					//check if change ClusterOpMode ongoing
					ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
					m_neHandler->getClusterOpMode(currentClusterOpMode);
					ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - CURRENT OP MODE: %d", __FUNCTION__, __LINE__, currentClusterOpMode));
					if ((currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToExpert)
							&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToNormal)){
						result = true;
					}
				}
				break;


			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_IMPORT_CCF_ID):
				if(::strcmp(actionInfo->filename.c_str(),"") != 0)
				{
					//check if change OmProfile ongoing
					ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
					m_neHandler->getOmProfilePhase(omProfilePhase);
					if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
					{
						//check if change ClusterOpMode ongoing
						ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
						m_neHandler->getClusterOpMode(currentClusterOpMode);
						if ((currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToExpert)
								&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToNormal)){
							result = true;
						}
					}
				}
				break;
			default:
				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - EVENT NOT RECOGNIZED: %d", __FUNCTION__, __LINE__, actionInfo->OperationId));
				break;
		}


	}
	else if (actionInfo->ObjectType == CANDIDATECCFILE_T)
	{
		switch(actionInfo->OperationId)
		{
			case (ACS_CS_ImmMapper::CP_CLUSTER_OP_IMPORT_CCF_ID):
				if(::strcmp(actionInfo->filename.c_str(),"") != 0)
				{
					//check if change OmProfile ongoing
					ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
					m_neHandler->getOmProfilePhase(omProfilePhase);
					if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
					{
						//check if change ClusterOpMode ongoing
						ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
						m_neHandler->getClusterOpMode(currentClusterOpMode);
						if ((currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToExpert)
								&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToNormal)){
							result = true;
						}
					}
				}
				break;

			default:
				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - EVENT NOT RECOGNIZED: %d", __FUNCTION__, __LINE__, actionInfo->OperationId));
				break;

		}
	}
	else if (actionInfo->ObjectType == ADVANCEDCONFIGURATION_T)
	{
		switch(actionInfo->OperationId)
		{
		case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_SUCCESS):
		{
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_SUCCESS", __FUNCTION__, __LINE__));
			if(actionInfo->profilePhase != ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_PHASE)
			{
				result = true;
			}
			else ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ERROR", __FUNCTION__, __LINE__));
		}
		break;

		case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_FAILURE):
		{
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ADVANCED_CONFIGURATION_OP_OM_PROFILE_NOTIFICATION_FAILURE", __FUNCTION__, __LINE__));
			if(actionInfo->profilePhase != ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_PHASE &&
					actionInfo->profileReason != ACS_CS_ImmMapper::DEFAULT_OP_OM_PROFILE_NOTIFICATION_REASON)
			{
				result = true;
			}
			else ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ERROR", __FUNCTION__, __LINE__));
		}
		break;

		case (ACS_CS_ImmMapper::ADVANCED_CONFIGURATION_OP_OM_PROFILE_APA_NOTIFICATION):
		{
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ADVANCED_CONFIGURATION_OP_OM_PROFILE_APA_NOTIFICATION", __FUNCTION__, __LINE__));
			if(actionInfo->apzProfileRequested != ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED &&
					actionInfo->aptProfileRequested != ACS_CS_ImmMapper::DEFAULT_OP_CHANGE_OM_PROFILE_REQUESTED)
			{
				result = true;
			}
			else ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - ERROR", __FUNCTION__, __LINE__));
		}
		break;

		default:
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - EVENT NOT RECOGNIZED: %d", __FUNCTION__, __LINE__, actionInfo->OperationId));
			break;
		}
	}


//	if (actionId == CHANGE_CLUSTER_OP_MODE_ID)
//	{
//		if (i_parameter != -1)
//		{
//			//check if change OmProfile ongoing
//			ACS_CS_API_OmProfilePhase::PhaseValue omProfilePhase;
//			m_neHandler->getOmProfilePhase(omProfilePhase);
//			if( omProfilePhase == ACS_CS_API_OmProfilePhase::Idle)
//			{
//				//compare with current ClusterOpMode
//				ACS_CS_API_ClusterOpMode::Value currentClusterOpMode;
//				m_neHandler->getClusterOpMode(currentClusterOpMode);
//
//				if ((currentClusterOpMode!=i_parameter)
//						&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToExpert)
//						&& (currentClusterOpMode!=ACS_CS_API_ClusterOpMode::SwitchingToNormal)){
//
//					result = true;
//				}
//			}
//		}
//	}
//
//	if (actionId == ACS_CS_ImClusterCp::OP_CHANGE_OM_PROFILE)
//	{
//		if (i_parameter != -1)
//		{
//			result = true;
//		}
//
//	}


	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "(%t) [%s@%d] - Check Parameter return value: %d", __FUNCTION__, __LINE__, result));

	return result;
}


bool ACS_CS_CpClusterManager::createCpClusterEntry()
{
	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"(%t) ACS_CS_CpClusterManager::createCpClusterEntry()"));

	ACS_CS_ImModel* tempModel = new ACS_CS_ImModel();
	const ACS_CS_ImModel *readModel = ACS_CS_ImRepository::instance()->getModelCopy();

	string cpClusterRdn = ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER;
	string cpClusterClass = ACS_CS_ImmMapper::CLASS_CP_CLUSTER;
	string cpClusterStructRdn = ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER_STRUCT;
	string cpClusterStructClass = ACS_CS_ImmMapper::CLASS_CP_CLUSTER_STRUCT;

	ACS_CS_ImCpCluster *cpCluster = new ACS_CS_ImCpCluster();

	cpCluster->rdn = cpClusterRdn;
	cpCluster->axeCpClusterId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_ID + "=1";
	cpCluster->allBcGroup.clear();

	//Set default values
	cpCluster->type = CPCLUSTER_T;
	cpCluster->alarmMaster = 0;
	cpCluster->clockMaster = 0;
	cpCluster->clusterOpMode = 	NORMAL;
	cpCluster->clusterOpModeType = UNDEF_CLUSTEROPMODETYPE;
	cpCluster->frontAp = AP1;
	cpCluster->ogClearCode = UNDEF_CLEARCODE;
	cpCluster->reportProgress = cpClusterStructRdn;

	cpCluster->action = ACS_CS_ImBase::CREATE;


	ACS_CS_ImCpClusterStruct* cpClusterStruct = new ACS_CS_ImCpClusterStruct();

	cpClusterStruct->rdn = cpClusterStructRdn;
	cpClusterStruct->type = CPCLUSTERSTRUCT_T;
	cpClusterStruct->axeCpClusterStructId = ACS_CS_ImmMapper::ATTR_CP_CLUSTER_STRUCT_ID + "=cpCluster";
	cpClusterStruct->state = UNDEFINED_STATE;
	cpClusterStruct->result = NOT_AVAILABLE;
	cpClusterStruct->actionId = UNDEFINED_TYPE;
	cpClusterStruct->reason.clear();
	cpClusterStruct->timeOfLastAction.clear();

	cpClusterStruct->action = ACS_CS_ImBase::CREATE;

	ACS_CS_ImBase *baseCpCluster = 0;
	ACS_CS_ImBase *baseCpClusterStruct = 0;

	ACS_CS_ImCpCluster *theCluster = 0;
	ACS_CS_ImCpClusterStruct *theClusterStruct = 0;

	baseCpCluster = tempModel->getObject(cpClusterRdn.c_str());
	baseCpClusterStruct = tempModel->getObject(cpClusterStructRdn.c_str());
	if (baseCpCluster){
		theCluster = dynamic_cast<ACS_CS_ImCpCluster*>(baseCpCluster);
		theClusterStruct = dynamic_cast<ACS_CS_ImCpClusterStruct*>(baseCpClusterStruct);
	}
	else {
		baseCpCluster = readModel->getObject(cpClusterRdn.c_str());
		baseCpClusterStruct = readModel->getObject(cpClusterStructRdn.c_str());

		if (baseCpCluster)
			theCluster = dynamic_cast<ACS_CS_ImCpCluster*>(baseCpCluster);

		if (baseCpClusterStruct)
			theClusterStruct = dynamic_cast<ACS_CS_ImCpClusterStruct*>(baseCpClusterStruct);

	}

	if(NULL != theCluster)//  the Cluster already exists, we do not need to add it
	{
		delete cpCluster;
		delete cpClusterStruct;
		delete readModel;
		delete tempModel;
		ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,
				"(%t) ACS_CS_CpClusterManager::createCpClusterEntry, the Cluster already exists!"));

		return true;
	}

	tempModel->addObject(cpCluster);
	tempModel->addObject(cpClusterStruct);


	ACS_CS_ImModelSaver saver(tempModel);
	saver.save("createCpClusterEntry");


	ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"(%t) ACS_CS_CpClusterManager::createCpClusterEntry temp object!"));


	delete readModel;
	return true;
}


bool ACS_CS_CpClusterManager::restoreCpClusterFromDisk()
{
	bool result = false;
	ACS_CS_ImModel * model = new ACS_CS_ImModel();

	//Load from QuorumData file
	try
	{
		std::string QuorumDataFile = PATH_QUORUM_DATA_FILE;
		std::ifstream file(QuorumDataFile.c_str());
		boost::archive::binary_iarchive ia(file);

		//check achive version compatibility
		if (ia.get_library_version() < BOOST_ARCHIVE_VERSION)
		{
			ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE, "restoreCpClusterFromDisk() Boost archive version NOT SUPPORTED!!!."));

			//remove data file
			std::remove(PATH_QUORUM_DATA_FILE.c_str());

			delete model;
			return result;
		}

		ia >> *model;
	}
	catch (boost::archive::archive_exception& e)
	{
		ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"restoreCpClusterFromDisk() Serialization exception occurred: %s.", e.what()));
	}
	catch (boost::exception& )
	{
		ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"restoreCpClusterFromDisk() Serialization exception occurred."));
	}

	// ---------------------------------------------------------------------------------------------------------------------------------------
	//  - Replace IMM QUORUM data after the restore procedure -
	//  In case there are no differences between the ALLBC group fetched from the data file and the current ALLBC stored in IMM,
	//  QUORUM data file content is restored on IMM.
	//  Otherwise the clusterAligned attribute of the AdvancedConfiguration MOC is set to false waiting for the operator to align the HW Table.
	//	 ---------------------------------------------------------------------------------------------------------------------------------------
	if (model)
	{
		ACS_CS_ImModel *currentModel = ACS_CS_ImRepository::instance()->getModelCopy();

		ACS_CS_ImBase * currentCpCluster = currentModel->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());
		ACS_CS_ImBase * CpCluster = model->getObject(ACS_CS_ImmMapper::RDN_AXE_CP_CLUSTER.c_str());

		if (currentCpCluster && CpCluster)
		{
			ACS_CS_ImCpCluster *cluster = dynamic_cast<ACS_CS_ImCpCluster *> (CpCluster);
			ACS_CS_ImCpCluster *currentCluster = dynamic_cast<ACS_CS_ImCpCluster *> (currentCpCluster);

			if (currentCluster->allBcGroup == cluster->allBcGroup)
			{
				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"restoreCpClusterFromDisk() No differences between current and stored ALLBC group !!!"));

//				ACS_CS_ImModel *currentCandidateCcFileModel = getCandidateCcFileModel(currentModel);
//				if (currentCandidateCcFileModel)
//				{
//					ACS_CS_ImModel *diffCandidateCcFileModel = currentCandidateCcFileModel->findDifferences(model);
//				}

				ACS_CS_ImModel *diffModel = findDifferences(model, currentModel);
				if (diffModel && diffModel->size() > 0)
				{

					bool addToModel = false;

					//get AdvancedConfiguration object from the differences found
					ACS_CS_ImBase *advBase = diffModel->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());

					if (!advBase)
					{
						//get AdvancedConfiguration object from the current IMM model instance
						advBase= currentModel->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
						addToModel = true;
					}

					ACS_CS_ImAdvancedConfiguration* advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

					if (advancedConfiguration)
					{
						//set clusterAligned attribute to true in order to complete the QUORUM alignment
						advancedConfiguration->clusterAligned = true;
						advancedConfiguration->action = ACS_CS_ImBase::MODIFY;

						if (addToModel)
						{
							ACS_CS_ImAdvancedConfiguration *advancedConfigurationClonedObj = dynamic_cast<ACS_CS_ImAdvancedConfiguration *> (advancedConfiguration->clone());
							advancedConfigurationClonedObj->action = ACS_CS_ImBase::MODIFY;
							diffModel->addObject(advancedConfigurationClonedObj);
						}
					}

					ACS_CS_ImModelSaver saver(diffModel);
					ACS_CC_ReturnType saved = saver.save("Restore-Quorum");
					if(ACS_CC_FAILURE == saved)
					{
						ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"restoreCpClusterFromDisk() Could not restore Quorum Data"));
					}
					else
					{
						m_cluster_aligned_after_restore = true;
						result = true;
					}
					delete diffModel;
				}
				else
				{
					if (setClusterAlignedAfterRestore(true)){
						result = true;
					}
				}
			}
			else
			{
				if (setClusterAlignedAfterRestore(false)){
					result = true;
				}

				ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"restoreCpClusterFromDisk() Manually restore of Quorum Data NEEDED !!!"));
				ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_INFO,"restoreCpClusterFromDisk() Manually restore of Quorum Data NEEDED !!!"));
			}

			delete currentModel;
		}
		delete model;
	}

	return result;
}


bool ACS_CS_CpClusterManager::setClusterAlignedAfterRestore(bool aligned)
{
	bool result = false;
	ACS_CS_ImModel *model = ACS_CS_ImRepository::instance()->getModelCopy();
	if (model)
	{
		ACS_CS_ImBase *advBase= model->getObject(ACS_CS_ImmMapper::RDN_ADVANCEDCONFIGURATION.c_str());
		ACS_CS_ImAdvancedConfiguration* advancedConfiguration = dynamic_cast<ACS_CS_ImAdvancedConfiguration*>(advBase);

		if (advancedConfiguration)
		{
			if (aligned != (bool) advancedConfiguration->clusterAligned)
			{
				ACS_CS_ImAdvancedConfiguration *advancedConfigurationClonedObj = dynamic_cast<ACS_CS_ImAdvancedConfiguration *> (advancedConfiguration->clone());
				advancedConfigurationClonedObj->clusterAligned = aligned;
				advancedConfigurationClonedObj->action = ACS_CS_ImBase::MODIFY;

				ACS_CS_ImModel tempModel;
				tempModel.addObject(advancedConfigurationClonedObj);

				//Save
				ACS_CS_ImModelSaver saver(&tempModel);
				if (saver.save("setCurrentClusterAlignedAfterRestore") == ACS_CC_FAILURE)
				{
					ACS_CS_FTRACE((ACS_CS_CpClusterManager_TRACE, LOG_LEVEL_ERROR,"setCurrentClusterAlignedAfterRestore() Could not save ClusterAligned"));
				}
				else
				{
					m_cluster_aligned_after_restore = aligned;
					result = true;
				}
			}
			else
			{
				m_cluster_aligned_after_restore = aligned;
				result = true;
			}
		}

		delete model;
	}

	return result;
}

ACS_CS_ImModel* ACS_CS_CpClusterManager::findDifferences(ACS_CS_ImModel* newModel, ACS_CS_ImModel* currModel) const
{
	ACS_CS_ImModel* diffModel = new ACS_CS_ImModel();

	int numObj = 4;
	std::set<const ACS_CS_ImBase *> objects[numObj];
	currModel->getObjects(objects[0], ADVANCEDCONFIGURATION_T);
	currModel->getObjects(objects[1], CPCLUSTER_T);
	currModel->getObjects(objects[2], CLUSTERCP_T);
	currModel->getObjects(objects[3], CANDIDATECCFILE_T);

	for (int i = 0; i < numObj; i++)
	{
		//Look for objects that need to be deleted or modified
		for (std::set<const ACS_CS_ImBase *>::const_iterator it_obj = objects[i].begin(); it_obj != objects[i].end(); it_obj++)
		{
			ACS_CS_ImBase *newObj = newModel->getObject((*it_obj)->rdn.c_str());

			if (CANDIDATECCFILE_T != (*it_obj)->type)
			{
				if (*newObj != *(*it_obj))
				{
					//The object is changed in the new Model. Then it should be modified.
					newObj->action = ACS_CS_ImBase::MODIFY;
					ACS_CS_ImBase* modObj = newObj->clone();
					if (modObj)
					{
						diffModel->addObject(modObj);
						ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,
								"ACS_CS_CpClusterManager::findDifferences()\n"
								"The object with RDN: %s has to be modified", modObj->rdn.c_str()));
					}
				}
				else{
					newObj->action = ACS_CS_ImBase::MODIFY;
				}
			}
			else
			{
				//Removal of CandidateCcFile MOs

				if (!newObj)
				{
					//The object doesn't exist in the new Model. Then it should be deleted.
					ACS_CS_ImBase* delObj = (*it_obj)->clone();
					if (delObj)
					{
						delObj->action = ACS_CS_ImBase::DELETE;
						diffModel->addObject(delObj);
						ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,
								"ACS_CS_CpClusterManager_TRACE::findDifferences()\n"
								"The object with RDN: %s has to be deleted", delObj->rdn.c_str()));
					}
				}
			}
		}
	}

	//Adding of CandidateCcFile MOs
	std::set<const ACS_CS_ImBase *> object;
	newModel->getObjects(object, CANDIDATECCFILE_T);
	for (std::set<const ACS_CS_ImBase *>::const_iterator it_obj = object.begin(); it_obj != object.end(); it_obj++)
	{
		if (CANDIDATECCFILE_T == (*it_obj)->type)
		{
			const ACS_CS_ImBase *oldObj = currModel->getObject((*it_obj)->rdn.c_str());

			if (!oldObj)
			{
				//The object doesn't exist in the old Model. Then it should be created.
				ACS_CS_ImBase* addObj = (*it_obj)->clone();
				if (addObj)
				{
					addObj->action = ACS_CS_ImBase::CREATE;
					diffModel->addObject(addObj);
					ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,
							"ACS_CS_CpClusterManager_TRACE::findDifferences()\n"
							"The object with RDN: %s has to be added", addObj->rdn.c_str()));
				}
			}
		}
	}

	if (diffModel->size() == 0) {
		ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,
				"ACS_CS_CpClusterManager::findDifferences()\n"
				" No differences between the two models"));
		delete diffModel;
		return 0;
	}


	return diffModel;

}


void ACS_CS_CpClusterManager::storeCpClusterOnDisk(const ACS_CS_ImModel* model)
{
	if (!model)
		return;

	bool found = false;
	std::set<const ACS_CS_ImBase *> objects;

	//check if the CpCluster object is present in the subset
	model->getObjects(objects, CPCLUSTER_T);
	found = (objects.size() > 0 ) ? true : false;

	if (!found)
	{
		//check if the ClusterCp objects are present in the subset
		model->getObjects(objects, CLUSTERCP_T);
		found = (objects.size() > 0 ) ? true : false;
	}

	if (!found)
	{
		//Note: Need to align /sw_package/CP/CCF NBI folder content with CandidateCcFile MOC after AP restore
		//check if the CandidateCcFile objects are present in the subset
		model->getObjects(objects, CANDIDATECCFILE_T);
		found = (objects.size() > 0 ) ? true : false;
	}

	if (found)
	{
		ACS_CS_ImModel *tmpModel = new ACS_CS_ImModel();

		if (tmpModel)
		{
			ACS_CS_ImModel *currentModel = ACS_CS_ImRepository::instance()->getModelCopy();

			if (currentModel)
			{
				int numObj = 4;
				std::set<const ACS_CS_ImBase *> objects[numObj];
				currentModel->getObjects(objects[0], ADVANCEDCONFIGURATION_T);
				currentModel->getObjects(objects[1], CPCLUSTER_T);
				currentModel->getObjects(objects[2], CLUSTERCP_T);
				currentModel->getObjects(objects[3], CANDIDATECCFILE_T);

				for (int i = 0; i < numObj; i++)
				{
					//Look for objects that need to be deleted or modified
					for (std::set<const ACS_CS_ImBase *>::const_iterator it_obj = objects[i].begin(); it_obj != objects[i].end(); it_obj++)
					{
						ACS_CS_ImBase* addObj = (*it_obj)->clone();
						if(addObj)
						{
							tmpModel->addObject(addObj);
						}
					}
				}

				//AdvancedConfiguration,CpCluster and ClusterCp MOCs are stored on data disk in /data/acs/data/cs/QuorumData file.
				try
				{
					std::string fileName = PATH_QUORUM_DATA_FILE;
					std::ofstream ofs(fileName.c_str());
					boost::archive::binary_oarchive ar(ofs);
					ar << * tmpModel;
				}
				catch (boost::archive::archive_exception& e)
				{
					ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"storeCpClusterOnDisk() Serialization exception occurred: %s.", e.what()));
				}
				catch (boost::exception& )
				{
					ACS_CS_TRACE((ACS_CS_CpClusterManager_TRACE,"storeCpClusterOnDisk() Serialization exception occurred."));
				}

				delete currentModel;
			}
			delete tmpModel;
		}
	}
}

ACS_CS_ImModel * ACS_CS_CpClusterManager::getCandidateCcFileModel(const ACS_CS_ImModel* model)
{
	ACS_CS_ImModel *currentCandidateCcFileModel = new ACS_CS_ImModel();

	if (currentCandidateCcFileModel)
	{
		std::set <const ACS_CS_ImBase *> outputObjects;
		model->getObjects(outputObjects, COMPUTERESOURCE_T);

		for (std::set <const ACS_CS_ImBase *>::iterator it = outputObjects.begin(); it != outputObjects.end(); ++it)
		{
			ACS_CS_ImBase* addObj = (*it)->clone();
			if(addObj)
			{
				currentCandidateCcFileModel->addObject(addObj);
			}
		}

		return currentCandidateCcFileModel;
	}

	return 0;

}
