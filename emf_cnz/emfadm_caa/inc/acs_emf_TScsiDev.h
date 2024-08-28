#pragma once

#include <string>
#include <dirent.h>
#include <list>
#include <ACS_CS_API.h>

// ******************************************** INT section ********************************************
// Max len of filename
#define INT_NAME_LEN_MAX					260
// Max len of device name
#define INT_DEVICE_LEN_MAX				34
// Max len of fs type
#define INT_FSTYP_LEN_MAX					34
// Fields in sscanf mounts string
#define INT_MOUNTS_FIELDS					3
// Fields in scanf type string
#define INT_TYPE_FIELDS						1
// Fields in sscanf Major and Minour string
#define INT_MAJMIN_PARSING				2
// ***************************************** SCSI Devices type *****************************************
#define SCSI_TYP_INVALID					-1
#define SCSI_TYP_DISK							0
#define SCSI_TYP_TAPE							1
#define SCSI_TYP_PRINTER					2
#define SCSI_TYP_PROCESS					3
#define SCSI_TYP_WORM							4
#define SCSI_TYP_CDDVD						5
#define SCSI_TYP_SCANNER					6
#define SCSI_TYP_OPTICAL					7
#define SCSI_TYP_MEDIUMX					8
#define SCSI_TYP_COMMS						9
#define SCSI_TYP_STORAGE					12
#define SCSI_TYP_ENCLOSU					13
#define SCSI_TYP_SIMDSK						14
#define SCSI_TYP_OPTIRD						15
#define SCSI_TYP_BRIDGE						16
#define SCSI_TYP_OSD							17
// ****************************************** String section *******************************************
// NullString
#define STR_NULLSTRING						""
// Directory separator
#define STR_DIR_SEPARATOR					"/"
// SCSI Device slot format
#define STR_TSCSIDEV_SLOTFMT			"[%s]"
// Open file Read flag
#define STR_OPENFILE_READONLY			"r"
// sysfs name
#define STR_SYSFS_NAME						"sysfs"
// dev folder
#define STR_DEV_NAME							"dev"
// Mounts path
#define STR_PROC_MOUNTS						"/proc/mounts"
// Devices part folder
#define STR_BUS_SCSI_DEVS					"/bus/scsi/devices"
// mtab path
#define STR_MTAB_PATH							"/etc/mtab"
// sscanf string for parsing mounts file
#define STR_MOUNTS_PARSING				"%32s %256s %32s"
// sscanf string for parsing "type" attribute
#define STR_TYPE_PARSING					"%d"
// Major and Minour parsing
#define STR_MAJMIN_PARSING				"%u:%u"
// *** SCSI attributes value ***
#define STR_TSCSIDEV_VTYPE				"type"
// *** Generic char ***
#define CHR_SCSI_NULLCHAR					'\0'
// ******************************************* Error string ********************************************
// Undefined error
#define STR_TSCSIDEV_UNDEFINED			"Undefined error"
// Success
#define STR_TSCSIDEV_SUCCESS				"Success"
// Invalid arguments
#define STR_TSCSIDEV_INVALIDARGS		"Unexpected number of arguments in file"
// Unable to open file for read
#define STR_TSCSIDEV_OPEN_RFILE			"Unable to open file for reading"
// Unable to open dir
#define STR_TSCSIDEV_OPEN_DIR				"Error opening directory"
// Scsi "type" value not found
#define STR_TSCSIDEV_TYPENOTFOUND		"Error to get scsi device type"
// Error to locate path
#define STR_TSCSIDEV_INVALIDPATH		"Invalid path error"
// Unable to get workdir
#define STR_TSCSIDEV_GETWD					"Unable to get work dir"
// Unable to change workdir
#define STR_TSCSIDEV_CHWD						"Unable to change work dir"
// Invalid parameter
#define STR_TSCSIDEV_INVALIDPARAM		"Invalid parameter"
// Can't open mtab file
#define STR_TSCSIDEV_OPEN_MTAB			"Can't open mtab file"

namespace ItsScsiUtilityNamespace{
	// Return code enumerator
	enum enmTScsiError{
		errUndefined = -1,			// Undefined error
		errSuccess = 0,					// Success
		errInvalidArguments,		// Invalid arguments in some scanf
		errArgumentsNotFound,		// Arguments not found in folder
		errUnableOpenFile,			// Unable to open file
		errUnableOpenDir,				// Unable to open directory
		errInvalidPath,					// Invalid path
		errGetWd,								// Unable to get work dir
		errChangeWd,						// Unable to change work dir
		errNullParam,						// Output parameter is a null pointer
		errMtab									// Unable to open mtab file
	};
	// Scsi item class
	class TScsiItem{
		// Costructor-distructor
		public:
			TScsiItem();
			virtual ~TScsiItem();
		// Public method
		public:
			// Get hctl
			void getHctl(std::string *pStrHctl) const;
			// Set hctl
			void setHctl(const std::string &strHctl);
			// Get Mount path for mounting the device
			void getMount(std::string *pStrMount) const;
			// Set mounting device path
			void setMount(const std::string &strMount);
			// Get device type (see the 'SCSI Devices type')
			int getType() const;
			// Set device type
			void setType(int iType);
		// private attrib
		private:
			std::string m_strHctl;
			std::string m_strMount;
			int m_iType;
	};
	// Main class
	class TScsiDev
	{
		// Costructor - distructor
		public:
			TScsiDev();
			virtual ~TScsiDev();
		// Public method
		public:
			// 'plstDev' point to internal list that contains the devices type 'iType'. If 'iType' is SCSI_TYP_INVALID
			// all the device are returned. Return 'true' if Ok else return 'false'.
			//bool getDevices(std::list<TScsiItem*> **plstDev, bool bKernelMode = false, int iType = SCSI_TYP_INVALID);  
			bool getDevices(std::list<const TScsiItem*> **plstDev, bool bKernelMode = false, int iType = SCSI_TYP_INVALID);
			// Return true if "strDevice" (e.g /dev/sr0) is mounted; otherwise false.
			bool isMounted(const std::string &strDevice);
			// Return true if "strPath" is a directory mount on.
			bool isDirMounted(const std::string &strPath);
			// Get mount point folder of the "strDevice" device. The output argument "pStrOut" return mount folder.
			// The method return true if no error occur; else return false.
			bool getMountedDevOn(const std::string &strDevice, std::string *pStrOut);
			// Chek if DVD is configurated in CS. "uiSysId" is sysId of the ap to check the DVD (eg. 2001,2002,...);
			//  pbPresent is true if DVD is configured on specified AP. Return true if no error occur else false.
			bool isFbnDvdConfigured(uint16_t uiSysId, bool *pbPresent);
			// Get last error. The string "pStrErrorMsg" is filled with generic error message. The "pStrExtra" is
			// filled with extra message (e.g. path of file). Return a enmTScsiError Error code.
			enmTScsiError getInternaleError(std::string *pStrErrorMsg, std::string *pStrExtra);
		// Private method
		private:
			void clear();
			bool isSdevAdd(const dirent *pstrctDirent);
			bool scanScsiItem(const char *pScanPath, char *pDevName, int iType, bool bKernelMode, TScsiItem* pItem, bool *pbAdd);
			bool get_value(const std::string &strDirName, const char * base_name, char * value, int max_value_len);
			bool enumDeviceS(const std::string &strScanPath, int iType, bool bKernelMode);
			bool findSysFsRoot(char *pszDev, char *pszSysFsRoot, char *pszFsType);
			bool scanScsiDevice(const std::string &strPath, bool bKernelMode, std::string *pStrMount);
			bool isNonSg(const dirent *pstrctDirEnt, std::string *pstrName, int *piType);
			bool getScsiDevFolder(const std::string &strPath, const std::string &strName, int iType, bool bKernelMode, std::string *pstrMountPath);
			bool isValidFile(const dirent *pstrctDirent);
			bool scanAndSelectFirst(const std::string &strScanPath, std::string *pStrDev);
			void getLinkModeDev(const std::string &strScsiPath, std::string *pstrLnkName);
			bool isBlockFile(const std::string &strFilePath, unsigned int *piMaj, unsigned int *piMin);
			bool isFbnConfigured(uint16_t uiFbnIn, uint16_t uiSysId, bool *pbPresent);
			bool searchFbnBoard(ACS_CS_API_HWC *pHwc, uint16_t uiFbn, uint16_t uiSysId, bool *pbPresent);
			void setInternalError(enmTScsiError enmError, const std::string &strErrorMsg, const std::string &strExtra);
		// Private attrib
		private:
			// Internal error generic string
			std::string m_strErrorMsg;
			// Internale error extra string
			std::string m_strExtra;
			// Enumerate error
			enmTScsiError m_enmError;
			// contains scsi found item
			std::list<TScsiItem*> m_lstScsiItem;
	};
} // namespace
