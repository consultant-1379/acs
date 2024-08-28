//********************************************************************************
//
// NAME
// ACS_RTR_BlockToCommitStore.cpp
//
// COPYRIGHT Marconi S.p.A, Italy 2007.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Marconi S.p.A, Italy.
// The program(s) may be used and/or copied only with 
// the written permission from Marconi S.p.A or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied.
//
// AUTHOR 
// 2012-12-12 by GP/AP/A XSAMECH
//
// DESCRIPTION 
// This class implements a table containing the association among the block ID, of the
// latest transmitted block within a transaction, and  the number of ACA records that the
// block contains.
//
//********************************************************************************
#include "acs_rtr_blocktocommitstore.h"
#include "ACS_APGCC_CommonLib.h"

#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_BLOCK_STORE)

//------------
// Constructor
//------------
ACS_RTR_BlockToCommitStore::ACS_RTR_BlockToCommitStore(const char* msname, const char* siteName, unsigned cpSystemId)
: _idx(-1),
  _lpMapAddress(NULL)
{
	ACS_RTR_TRACE_FUNCTION;
	(void)siteName;
	char tmp[FILENAME_MAX] = {0};
	char dir[FILENAME_MAX] = {0};
	ACE_INT32 err = 0;

	memset(_fileNamePath, 0, sizeof(_fileNamePath));
	_tableSize = sizeof(aca_goh_bind_t) * MAX_BLOCK_TRANSACTION;

	if (fxGetBaseDir(tmp, cpSystemId) >= 0) 
	{
		ACE_OS::snprintf(dir, sizeof(dir) - 1, "%s/%s", tmp, msname);

		createDir(dir, err);

		ACE_OS::snprintf(_fileNamePath, sizeof(_fileNamePath) - 1, "%s/toCommit.dat", dir);
		ACS_RTR_TRACE_MESSAGE("Creating block to commit store file: %s",_fileNamePath);

		load();

		if (!_lpMapAddress)
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Cannot create file \"%s\". errno: %d", _fileNamePath, ACE_OS::last_error());
			ACS_RTR_TRACE_MESSAGE("Error: Cannot create file \"%s\". errno: %d", _fileNamePath, ACE_OS::last_error());
		}
	}
}

//-----------
// Destructor
//-----------
ACS_RTR_BlockToCommitStore::~ACS_RTR_BlockToCommitStore(void)
{
	ACS_RTR_TRACE_FUNCTION;
	if (_lpMapAddress && _lpMapAddress != MAP_FAILED)
	{
		if (munmap(_lpMapAddress, _tableSize))
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "munpap() Failed. errno: %d", errno);
			ACS_RTR_TRACE_MESSAGE("Error: munpap() Failed. errno: %d", errno);
		}
	}
}

//---------
// addEntry
//---------
void ACS_RTR_BlockToCommitStore::addEntry(unsigned int nBlock, unsigned int nAca)
{
	//-----------
	//Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		return;
	}

	if (exists(nBlock)) return;

	//----------------------------------------
	// Add the entry at the first free index
	_idx = (_idx + 1) % MAX_BLOCK_TRANSACTION;

	_table[_idx].idGohBlock = nBlock; 
	_table[_idx].numAcaMessages = nAca;

}

//---------------
// getAcaMessages
//---------------
unsigned int ACS_RTR_BlockToCommitStore::getAcaMessages(unsigned int nBlock, unsigned int &blocksInTheMap)
{
	ACS_RTR_TRACE_FUNCTION;
	blocksInTheMap = 0;

	//-----------
	//Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "FILE MAPPING ERROR!!! Cannot read Block Id: %u", nBlock);
		return 0;
	}

	//-------------------------------------------------------------------
	// Read the amount of ACA messages bound to the specified block IDs:
	// from the first block ID in the file to the block with id 'nBlock'
	unsigned int ret = 0;
	int idx = 0;
	bool elementFound = false;

	for (int i = 0; i < MAX_BLOCK_TRANSACTION; i++)
	{
		if (_table[i].idGohBlock == nBlock)
		{
			elementFound = true;
			idx = i;
			break;
		}
	}

	if (elementFound)
	{
		blocksInTheMap = idx;

		for( int k = idx; k >= 0; k--)
		{
			if (_table[k].idGohBlock != 0)
			{
				ret = ret + _table[k].numAcaMessages;
			}
		}

		ACS_RTR_LOG(LOG_LEVEL_INFO, "ELEMENT FOUND. Block Id: %u; Number of ACA messages: %u", nBlock, ret);
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "ELEMENT NOT FOUND!!! Cannot read Block Id: %u", nBlock);
	}
	return ret;
}

//--------
// exists
//--------
bool ACS_RTR_BlockToCommitStore::exists(unsigned int nBlock)
{
	ACS_RTR_TRACE_FUNCTION;
	//------------
	// Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		return false;
	}

	//--------------------------
	// Check if 'nBlock' exists
	for (int i = 0; i < MAX_BLOCK_TRANSACTION; i++)
	{
		if (_table[i].idGohBlock == nBlock) return true;
	}
	return false;
}

//---------------
// deleteEntries
//---------------
bool ACS_RTR_BlockToCommitStore::deleteEntries(unsigned int nBlock)
{
	ACS_RTR_TRACE_FUNCTION;

	//-----------
	//Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		return false;
	}

	//------------------------------------------------
	// Delete all entries prior the block id 'nBlock'
	int idx = 0;
	bool elementFound = false;

	for (int i = 0; i < MAX_BLOCK_TRANSACTION; i++)
	{
		if (_table[i].idGohBlock == nBlock)
		{
			elementFound = true;
			idx = i;
			break;
		}
	}

	if (elementFound && (idx == MAX_BLOCK_TRANSACTION - 1))
	{
		//if the entry is at the latest one, remove the whole table
		deleteAllEntries();
	}
	else if (elementFound && (idx != (MAX_BLOCK_TRANSACTION - 1)))
	{
		//if the entry is at the latest one, remove the whole table
		for( int k = idx; k >= 0; k--)
		{
			if (_table[k].idGohBlock != 0)
			{
				_table[k].idGohBlock = 0;
				_table[k].numAcaMessages = 0;
			}
		}

		int j = idx + 1;
		for(int i = 0 ; j < MAX_BLOCK_TRANSACTION; i++, j++)
		{
			_table[i].idGohBlock = _table[j].idGohBlock;
			_table[i].numAcaMessages = _table[j].numAcaMessages;

			_table[j].idGohBlock = 0;
			_table[j].numAcaMessages = 0;
		}

		resetTableIndex();
	}

	return elementFound;
}

//-----
// load
//-----
bool ACS_RTR_BlockToCommitStore::load()
{
	ACS_RTR_TRACE_FUNCTION;

	//------------------------------------
	//if it is already loaded, unmap it
	if (_lpMapAddress && _lpMapAddress != MAP_FAILED)
	{
		if (munmap(_lpMapAddress, _tableSize))
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "munmap() failed. errno: %d", errno);
			ACS_RTR_TRACE_MESSAGE("Error: munmap() failed. errno: %d", errno);
		}
		_lpMapAddress = NULL;
	}

	//---------------------------------------
	// load the file and create the mapping

	//open/create the file on the disk
	ACE_HANDLE fileHandle = ACE_OS::open(_fileNamePath, O_CREAT |O_RDWR, ACE_DEFAULT_OPEN_PERMS);

	if (ACE_INVALID_HANDLE == fileHandle)
	{
		// We failed to create/open the block table file
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "File create/open failed. errno: %d", ACE_OS::last_error());
		ACS_RTR_TRACE_MESSAGE("Error: File create/open failed. errno: %d", ACE_OS::last_error());
		return false;
	}

	ACS_RTR_TRACE_MESSAGE("Successful in opening %s", _fileNamePath);

	ACE_INT32 fSize = ACE_OS::filesize(fileHandle);
	ACE_INT32 i_tableSize = static_cast<ACE_INT32>(_tableSize);
	if ((fSize == 0) || (fSize != i_tableSize) )
	{
		// If the file size is 0, it's created for the first time.
		// If the file size is other than the expected one, the file is corrupted.

		ACE_INT32 nobw = 0;
		char * dummyBuf = new char[_tableSize];
		memset(dummyBuf,0,_tableSize);
		nobw = ACE_OS::write(fileHandle, dummyBuf, _tableSize);
		delete [] dummyBuf;
		if ((size_t)nobw < _tableSize)
		{
			// We failed to write to file
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Filed to write to file. errno: %d", errno);
			ACS_RTR_TRACE_MESSAGE("Error: Filed to write to file. errno: %d", errno);
			ACE_OS::close(fileHandle);
			return false;
		}
	}

	fSize = ACE_OS::filesize(fileHandle);
	if (fSize == i_tableSize)
	{
		_lpMapAddress = ACE_OS::mmap(0,fSize,PROT_READ|PROT_WRITE,MAP_SHARED ,fileHandle,0);
		ACE_OS::close(fileHandle);
		if ((_lpMapAddress == MAP_FAILED) || (_lpMapAddress == NULL))
		{
			ACS_RTR_LOG(LOG_LEVEL_ERROR, "Filed to map file. errno: %d", ACE_OS::last_error());
			ACS_RTR_TRACE_MESSAGE("Error: Filed to map file. errno: %d", ACE_OS::last_error());
			return false;
		}
	}
	else
	{
		ACS_RTR_LOG(LOG_LEVEL_ERROR, "Probably the disk is full. File size: %d, Expected size: %zu, Last Error: %d", fSize, _tableSize, ACE_OS::last_error());
		ACS_RTR_TRACE_MESSAGE("Error: Probably the disk is full. File size: %d, Expected size: %zu, Last Error: %d", fSize, _tableSize, ACE_OS::last_error());
		ACE_OS::close(fileHandle);
		return false;
	}

	_table = (aca_goh_bind_t *) _lpMapAddress;

	resetTableIndex();
	return true;
}

//-----------------
// deleteAllEntries
//-----------------
void ACS_RTR_BlockToCommitStore::deleteAllEntries()
{
	ACS_RTR_TRACE_FUNCTION;

	//Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		return;
	}

	for (int i = 0; i < MAX_BLOCK_TRANSACTION; i++)
	{
		_table[i].idGohBlock = 0;
		_table[i].numAcaMessages = 0;
	}
	_idx = -1;

}

//-----------
// dataExists
//-----------
bool ACS_RTR_BlockToCommitStore::dataExists()
{
	ACS_RTR_TRACE_FUNCTION;
	if (_idx == -1) return false;
	else return true;
}

//----------------
// resetTableIndex
//----------------
void ACS_RTR_BlockToCommitStore::resetTableIndex()
{
	ACS_RTR_TRACE_FUNCTION;
	_idx = -1;

	//-----------
	//Safe check
	if (_lpMapAddress == NULL || _lpMapAddress == MAP_FAILED)
	{
		//Trace error
		return;
	}

	//------------
	//Reset Index
	for (int i = 0; (i < MAX_BLOCK_TRANSACTION); i++)
	{
		if ((_table[i].idGohBlock > 0) && (_table[i].numAcaMessages > 0)) _idx++;
		else break;
	}
}

//--------------
// dbgPrintTable
//--------------
void ACS_RTR_BlockToCommitStore::dbgPrintTable()
{
	ACS_RTR_TRACE_FUNCTION;
	printf("\n----------------------------\n");
	if (_idx == -1)
	{
		printf("BlockToCommit table is empty\n");
	} else
	{
		printf("current index: %d [num of entries: %d]\n----------------------------\n", _idx, (_idx + 1));
		printf("idx\tidBlock\tnAca\n");
		for (int i = 0; i < MAX_BLOCK_TRANSACTION; i++)
		{
			if (_table[i].idGohBlock != 0) printf("%d\t%u\t%u\n", i, _table[i].idGohBlock, _table[i].numAcaMessages);
		}
	}
	printf("----------------------------\n");
}

//-------------
// fxGetBaseDir
//-------------
int ACS_RTR_BlockToCommitStore::fxGetBaseDir(char* RTRdataHome, unsigned cpSystemId)
{
	ACS_RTR_TRACE_FUNCTION;

	char szPath[FILENAME_MAX]				= {0};
	ACE_INT32 dwLen							= FILENAME_MAX;
	int returnCode							= 0;
	ACS_APGCC_DNFPath_ReturnTypeT retCode	= ACS_APGCC_DNFPATH_SUCCESS;
	std::string rtr_data("ACS_RTR_DATA");

	ACS_APGCC_CommonLib oComLib;

	if (cpSystemId == ~0U)
	{
		retCode = oComLib.GetDataDiskPath(rtr_data.c_str(),szPath,dwLen);
	}
	else
	{
		retCode = oComLib.GetDataDiskPathForCp(rtr_data.c_str(), cpSystemId, szPath, dwLen);
	}

	if (ACS_APGCC_DNFPATH_SUCCESS == retCode)
	{
		strncpy(RTRdataHome, szPath, FILENAME_MAX - 1);
	}
	else
	{
		returnCode = -1;
	}

	return returnCode;
}
