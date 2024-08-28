/*
 * MktrConfig.h
 *
 *  Created on: Sep 6, 2010
 *      Author: xludesi
 */

#ifndef MKTRCONFIG_H_
#define MKTRCONFIG_H_

#include <stddef.h>
#include <assert.h>
#include <MktrCommonDefs.h>
#include <ace/ACE.h>


class MktrConfig
{
public:

	MktrConfig();
	virtual ~MktrConfig();

	// get config parameters from a file
	int buildFromFile(const char *configFile = NULL) {ACE_UNUSED_ARG (configFile); assert(false); return -1; };

	// set config parameters using defaults
	int buildWithDefaults();

	const MktrCommonDefs::File * commonFileTable() const  { return _commonFileTable; }
	const MktrCommonDefs::File * memFileTable() const { return _memFileTable; }
	const MktrCommonDefs::Command* alogFileTable() const { return _alogFileTable; }
	const MktrCommonDefs::File * aposFileTable() const { return _aposFileTable; }
        const MktrCommonDefs::File * plogFileTable() { return _plogFileTable; }
	//const MktrCommonDefs::File * masFileTable() const { return _masFileTable; }
	const MktrCommonDefs::File * scxFileTable() const { return _scxFileTable; }
	const MktrCommonDefs::File * cmxFileTable() const { return _cmxFileTable; }
	const MktrCommonDefs::File * smxFileTable() const { return _smxFileTable; }

	const MktrCommonDefs::Command *commonCmdTable() const { return _commonCmdTable; }
	const MktrCommonDefs::Command *singleCpCmdTable() const { return _cmdTable_SingleCp; }
	const MktrCommonDefs::Command *multiCpCmdTable() const { return _cmdTable_MultiCp; }
	const MktrCommonDefs::Command *aposCmdTable() const { return _aposCmdTable; }
	const MktrCommonDefs::Command *phaCmdTable() const { return _phaCmdTable; }
	const MktrCommonDefs::Command *scxCmdTable() const { return _scxCmdTable; }
	const MktrCommonDefs::Command *smxCmdTable() const { return _smxCmdTable; }
	const MktrCommonDefs::Collector *lbbCltTable() const { return _lbbCltTable;}
	const MktrCommonDefs::Cliss_command_aux_info *clissCmdAuxInfoTable() const { return _clissCmdAuxInfoTable; }

private:

	MktrConfig(const MktrConfig &obj) {ACE_UNUSED_ARG (obj);};
	MktrConfig & operator=(const MktrConfig &obj) {ACE_UNUSED_ARG (obj); return *this;};

	MktrCommonDefs::File *_commonFileTable;
	MktrCommonDefs::Command *_commonCmdTable;
	MktrCommonDefs::Command *_cmdTable_SingleCp;
	MktrCommonDefs::Command *_cmdTable_MultiCp;
	MktrCommonDefs::File *_memFileTable;
	MktrCommonDefs::Command *_alogFileTable;
	MktrCommonDefs::File *_aposFileTable;
        MktrCommonDefs::File *_plogFileTable;
	//MktrCommonDefs::File *_masFileTable;
	MktrCommonDefs::Command *_aposCmdTable;
	MktrCommonDefs::Collector *_lbbCltTable;
	MktrCommonDefs::Command *_phaCmdTable;
	MktrCommonDefs::File *_scxFileTable;
	MktrCommonDefs::File *_cmxFileTable;
	MktrCommonDefs::File *_smxFileTable;
	MktrCommonDefs::Command *_scxCmdTable;
	MktrCommonDefs::Command *_smxCmdTable;
	MktrCommonDefs::Cliss_command_aux_info *_clissCmdAuxInfoTable;

	// default definitions
	static MktrCommonDefs::File _default_commonFileTable[];
	static MktrCommonDefs::Command _default_commonCmdTable[];
	static MktrCommonDefs::Command _default_cmdTable_SingleCp[];
	static MktrCommonDefs::Command _default_cmdTable_MultiCp [];
	static MktrCommonDefs::File _default_memFileTable[];
	static MktrCommonDefs::Command _default_alogFileTable[];
	static MktrCommonDefs::File _default_aposFileTable[];
        static MktrCommonDefs::File _default_plogFileTable[];//PLOG
	//static MktrCommonDefs::File _default_masFileTable[];
	static MktrCommonDefs::Command _default_aposCmdTable[];
	static MktrCommonDefs::Collector _default_lbbCltTable[];
	static MktrCommonDefs::Command _default_phaCmdTable[];
	static MktrCommonDefs::File _default_scxFileTable[];
	static MktrCommonDefs::File _default_cmxFileTable[];
	static MktrCommonDefs::File _default_smxFileTable[];
	static MktrCommonDefs::Command _default_scxCmdTable[];
	static MktrCommonDefs::Command _default_smxCmdTable[];
	static MktrCommonDefs::Cliss_command_aux_info _default_clissCmdAuxInfoTable[];
};

#endif /* MKTRCONFIG_H_ */
