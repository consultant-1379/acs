/*=================================================================== */
/**
   @file  acs_rtr_blocktocommitstore.h 

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
#ifndef __ACS_RTR_BLOCKTOCOMMITSTORE_H___
#define __ACS_RTR_BLOCKTOCOMMITSTORE_H___
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_rtr_global.h"
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     ACS_RTR_BlockToCommitStore

 */
/*=================================================================== */
class ACS_RTR_BlockToCommitStore {
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                          STRUCTURE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     aca_goh_bind_t
 */
/*=================================================================== */
	struct aca_goh_bind_t {
		unsigned int idGohBlock;
		unsigned int numAcaMessages;
	};
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR 
==================================================================== */
/*=================================================================== */
/**
   @brief     Constructor for ACS_RTR_BlockToCommitStore class.

   @param     msname

   @param     siteName

   @param     cpSystemId
 */
/*=================================================================== */
	ACS_RTR_BlockToCommitStore(const char* msname, const char* siteName, unsigned cpSystemId = ~0U);
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Destructor for ACS_RTR_BlockToCommitStore class
 */
/*=================================================================== */
	virtual ~ACS_RTR_BlockToCommitStore();
/*=================================================================== */
/**
   @brief     addEntry

   @param     nBlock

   @param     nAca

   @return    void
 */
/*=================================================================== */
	void addEntry(unsigned int nBlock, unsigned int nAca);
/*=================================================================== */
/**
   @brief     deleteEntries

   @param     nBlock

   @return    bool
 */
/*=================================================================== */
	bool deleteEntries(unsigned int nBlock);
/*=================================================================== */
/**
   @brief     deleteEntries

   @return    void
 */
/*=================================================================== */
	void deleteAllEntries();
/*=================================================================== */
/**
   @brief     getAcaMessages

   @param     nBlock

   @return    unsigned int
 */
/*=================================================================== */
	unsigned int getAcaMessages(unsigned int nBlock, unsigned int &blocksInTheMap);
/*=================================================================== */
/**
   @brief     exists

   @param     nBlock

   @return    bool
 */
/*=================================================================== */
	bool exists(unsigned int nBlock);
/*=================================================================== */
/**
   @brief     dataExists

   @return    bool
 */
/*=================================================================== */
	bool dataExists();
/*=================================================================== */
/**
   @brief     dbgPrintTable

   @return    void
 */
/*=================================================================== */
	void dbgPrintTable();
/*====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief     load

   @return    bool
 */
/*=================================================================== */
	bool load();
/*=================================================================== */
/**
   @brief     resetTableIndex

   @return    void
 */
/*=================================================================== */
	void resetTableIndex();
/*=================================================================== */
/**
   @brief     fxGetBaseDir

   @param     RTRdataHome

   @param     cpSystemId

   @return    int
 */
/*=================================================================== */
	int fxGetBaseDir(char* RTRdataHome, unsigned cpSystemId);
/*=================================================================== */

/*=====================================================================
                        FIELDS DECLARATION SECTION
==================================================================== */
/*====================================================================
                        PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
/**
   @brief     _table
 */
/*=================================================================== */
	aca_goh_bind_t *_table;
/*=================================================================== */
/**
   @brief     _fileNamePath
 */
/*=================================================================== */
	char _fileNamePath[FILENAME_MAX];
/*=================================================================== */
/**
   @brief     _idx
 */
/*=================================================================== */
	int _idx;
/*=================================================================== */
/**
   @brief     _tableSize
 */
/*=================================================================== */
	size_t _tableSize;
/*=================================================================== */

/*=================================================================== */
/**
   @brief     _lpMapAddress
                pointer to the base address of the memory-mapped region
 */
/*=================================================================== */
	void* _lpMapAddress; 

};
#endif
