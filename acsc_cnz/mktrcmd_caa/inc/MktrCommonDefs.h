/*
 * MktrCommonDefs.h
 *
 *  Created on: Sep 7, 2010
 *      Author: xludesi
 */

#ifndef MKTRCOMMONDEFS_H_
#define MKTRCOMMONDEFS_H_

#define MAX_CLISS_COMMAND_SIZE 2048
#define APG_NODE_NAME_MAX_SIZE  256

namespace MktrCommonDefs
{

	enum NodeType
	{
		NODE_NONE    = 0x00000000,
		NODE_ACTIVE  = 0x00000001,
		NODE_PASSIVE = 0x00000002,
		NODE_BOTH    = 0xFFFFFFFF

	};

	enum ApType
	{
		AP_NONE     = 0x00000000,
		AP_1        = 0x00000001,
		AP_2        = 0x00000002,
		AP_ALL      = 0xFFFFFFFF
	};

	enum ApgType
	{
		APG_NONE    = 0x00000000,
		APG_APG40C2 = 0x00000001,
		APG_APG40C4 = 0x00000002,
		APG_APG43   = 0x00000004,
		APG_APG43L	= 0x00000008,
		APG_ALL     = 0xFFFFFFFF
	};

	enum ApzType
	{
		APZ_NONE	= 0x00000000,
		APZ_212_3X_SHDC = 0x00000001,
		APZ_212_33 = 0x00000002,
		APZ_212_40 = 0x00000004,
		APZ_212_50 = 0x00000008,
		APZ_212_55_OR_LATER = 0x00000010,
		APZ_UNKNOWN = 0x00000020,
		APZ_ALL		= 0xFFFFFFFF
	};

	enum ApgShelfArchitectureType
	{
		APG_SHELF_ARCHITECTURE_NONE = 0x00000000,
		APG_SHELF_ARCHITECTURE_SCB = 0x00000001,
		APG_SHELF_ARCHITECTURE_SCX = 0x00000002,
		APG_SHELF_ARCHITECTURE_DMX = 0x00000004,
		APG_SHELF_ARCHITECTURE_UNKNOWN = 0x00000008,
		APG_SHELF_ARCHITECTURE_VIRTUALIZED = 0x00000010,
		APG_SHELF_ARCHITECTURE_SMX = 0x00000020,
		APG_SHELF_ARCHITECTURE_ALL = 0xFFFFFFFF
	};

	enum CmdFillerType
	{
		CMD_FILLER_NONE = 0x00000000,
		CMD_FILLER_CS = 0x00000001
	};

	enum ApgHwGepType
	{
		APG_HW_GEP_NONE    =  0x00000000,
		APG_HW_GEP_1_2 	   =  0x00000001,
		APG_HW_GEP_4_5     =  0x00000002,
		APG_HW_GEP_UNKNOWN =  0x00000004,
		APG_HW_GEP_7_8     =  0x00000008,
		APG_HW_GEP_ALL     =  0XFFFFFFFF
	};
		

	class File
	{
	public:
		unsigned int apType;
		unsigned int apgType;
		unsigned int apgShelfArchitectureType;
		unsigned int apzType;
		NodeType nodeType;
		const char* title;
		const char* srcFile;
		const char* dstFile;
		bool apply_strong_match;
	};

	class Command
	{
	public:
		unsigned int apType;
		unsigned int apgType;
		unsigned int apgShelfArchitectureType;
		unsigned int apzType;
		unsigned int apgHwGepType;
		NodeType nodeType;
		CmdFillerType fType;
		const char* title;
		const char* cmdLine;
		const char* logFile;
		unsigned int timeout;
		const char * userName;
		const char * grpName;
		bool apply_strong_match;
	};

	class Collector
	{
	public:
		unsigned int apType;
		unsigned int apgType;
		unsigned int apgShelfArchitectureType;
		unsigned int apzType;
		NodeType nodeType;
		CmdFillerType fType;
		const char* title;
		const char* directory;
		const char* cmdLine;
		const char* logFile;
		unsigned int timeout;
		bool apply_strong_match;
	};

	class Cliss_command_aux_info
	{
	public:
		const char * cliss_cmd_template;
		const char * cliss_cmd_input_file_path;
	};
}

#endif /* MKTRCOMMONDEFS_H_ */
