/*=================================================================== */
/**
   @file  acs_rtr_blockreport.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       22/01/2013   XHARBAV   Initial Release
==================================================================== */
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_blockReport_h
#define ACS_RTR_blockReport_h
/*=====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "aes_ohi_blockhandler2.h"
#include <string>
#include "ace/ACE.h"

#include "acs_rtr_blocktocommitstore.h"
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define MIN_BLOCK_NUMBER 1
#define MAX_BLOCK_NUMBER 4000000000

//#define MAX_BLOCKS_FOR_TRANSACTION 25
/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
//const unsigned int GOH_BUFFERFULL=0x1234;
//const unsigned int GOH_NOTINSYNC=0x5678;
/*=================================================================== */
/**
   @brief      bufferFullWait
 */
/*=================================================================== */
const int BUFFER_FULL_WAIT = 1;// 1 Second
/*=================================================================== */
/**
   @brief      transferFailedWait
 */
/*=================================================================== */
const int transferFailedWait=2000;//10000;
/*=================================================================== */
/**
   @brief      attachFailedWait
 */
/*=================================================================== */
const int attachFailedWait=500;//3000;
/*=================================================================== */
/**
   @brief      noServerAccessWait
 */
/*=================================================================== */
const int noServerAccessWait=2000;//3000;
/*=================================================================== */
/**
   @brief      detachFailedWait
 */
/*=================================================================== */
const int detachFailedWait=500;
/*=================================================================== */
/**
   @brief      terminateFailedWait
 */
/*=================================================================== */
const int terminateFailedWait=500;
/*=================================================================== */
/**
   @brief      sendErrorWait
 */
/*=================================================================== */
const int sendErrorWait=500;
/*=================================================================== */
/**
   @brief      maxRetriesOnError
 */
/*=================================================================== */
const int maxRetriesOnError = 5;
/*=================================================================== */
/**
   @brief      maxRetriesOnBufferFull
 */
/*=================================================================== */
const int maxRetriesOnBufferFull = 3;
/*=====================================================================
                        ENUMERATION DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      state
 */
/*=================================================================== */
enum state
{
	detached,
    attached,
	sending
};
/*=================================================================== */
/**
   @brief      trReq
 */
/*=================================================================== */
enum trReq
{
	BEGIN,
    END,
    COMMIT
};
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief      RTRblockReporter
 */
/*=================================================================== */
class RTRblockReporter {
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Constructor for RTRblockReporter class.

   @param      subSys

   @param      appName

   @param      blockTQ

   @param      evtext

   @param      streamId
 */
/*=================================================================== */
	RTRblockReporter(const char * subSys,
						const char * appName,
						const char * blockTQ,
						const char * evtext,
						const char * streamId);
/*=====================================================================
                        CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief      Destructor for RTRblockReporter class.
 */
/*=================================================================== */
	virtual ~RTRblockReporter();
/*=================================================================== */
/**
   @brief      attachGOH

   @param      err

   @return     bool   
 */
/*=================================================================== */
	bool attachGOH(ACE_INT32& err);
/*=================================================================== */
/**
   @brief      getLastBlockNumber

   @param      lastCommBlock

   @param      err

   @return     bool
 */
/*=================================================================== */
	bool getLastBlockNumber(unsigned int& lastCommBlock, ACE_INT32& err);
/*=================================================================== */
/**
   @brief      transActionReq

   @param      ttype

   @param      blNo

   @param      err

   @return     bool
 */
/*=================================================================== */
	bool transActionReq(trReq ttype, unsigned int& blNo, ACE_INT32& err);
/*=================================================================== */
/**
   @brief      sendGOH

   @param      buf

   @param      bufln

   @param      blockNo

   @param      errstr

   @return     bool
 */
/*=================================================================== */
	bool sendGOH(const char* buf, const int bufln, unsigned int& blockNo, ACE_INT32& errstr);
/*=================================================================== */
/**
   @brief      detach

   @return     unsigned int
 */
/*=================================================================== */
	unsigned int detach();
/*=================================================================== */
/**
   @brief      transactionTerminate

   @return     unsigned int
 */
/*=================================================================== */
	unsigned int transactionTerminate();
/*=================================================================== */
/**
   @brief      HandleGOHevent

   @return     void
 */
/*=================================================================== */
	void HandleGOHevent(void);
/*=================================================================== */
/**
   @brief      handleTransferFailed

   @return     bool
 */
/*=================================================================== */
	bool handleTransferFailed();
/*=================================================================== */
/**
   @brief      isEndTransaction

   @return     bool
 */
/*=================================================================== */
	bool isEndTransaction();
/*=================================================================== */
/**
   @brief      getLastCommittedBlockNumber

   @return     unsigned int
 */
/*=================================================================== */
	inline unsigned int getLastCommittedBlockNumber() { return lastCommittedBlock; }
/*=================================================================== */
/**
   @brief      getCurrentBlockNumber

   @return     unsigned int
 */
/*=================================================================== */
	inline unsigned int getCurrentBlockNumber() { return currentBlock; }
/*=================================================================== */
/**
   @brief      getFirstBlockNumber

   @return     unsigned int
 */
/*=================================================================== */
	inline unsigned int getFirstBlockNumber() { return firstBlockNumber; }
/*=================================================================== */
/**
   @brief      setStore

   @param      store

   @return     void
 */
/*=================================================================== */
	inline void setStore(ACS_RTR_BlockToCommitStore* store) { _store = store; }
/*=================================================================== */
/**
   @brief      getStore

   @return     ACS_RTR_BlockToCommitStore
 */
/*=================================================================== */
	inline ACS_RTR_BlockToCommitStore* getStore() { return _store; }
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=================================================================== */
/**
   @brief      blockState
 */
/*=================================================================== */
	state blockState;
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief      operator=
 */
/*=================================================================== */
	RTRblockReporter& operator=(const RTRblockReporter&);
/*=================================================================== */
/**
   @brief      lastCommittedBlock
                 block number of the last committed block
 */
/*=================================================================== */
	unsigned int lastCommittedBlock; 
/*=================================================================== */
/**
   @brief      currentBlock
                 current block number [1, 4000000000]
 */
/*=================================================================== */
	unsigned int currentBlock;		 
/*=================================================================== */
/**
   @brief      firstBlockNumber
 */
/*=================================================================== */
	unsigned int firstBlockNumber;
/*=================================================================== */
/**
   @brief      maxNumOfBlocksInTransaction
 */
/*=================================================================== */
	unsigned int maxNumOfBlocksInTransaction;
/*=================================================================== */
/**
   @brief      _store
 */
/*=================================================================== */
	ACS_RTR_BlockToCommitStore* _store;
/*=================================================================== */
/**
   @brief      _blockHandler
 */
/*=================================================================== */
	AES_OHI_BlockHandler2 * _blockHandler;
};
#endif
