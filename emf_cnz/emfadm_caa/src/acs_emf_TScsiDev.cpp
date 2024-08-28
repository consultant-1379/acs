#include "acs_emf_TScsiDev.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <mntent.h>
// For debug only.
#include "acs_emf_tra.h"

using namespace ItsScsiUtilityNamespace;

TScsiDev::TScsiDev()
{
	// Initialize internal enumerate error
	m_enmError = errUndefined;
	// Initialize internal generic error string
	m_strErrorMsg = STR_TSCSIDEV_UNDEFINED;
	// Initialize internale extra error string
	m_strExtra.clear();
	// Clear the list
	clear();
}

TScsiDev::~TScsiDev()
{
	// Clear the list for release memory
	clear();
}

void TScsiDev::clear()
{
	std::list<TScsiItem*>::iterator it;
	// Set begin of the list
	it = m_lstScsiItem.begin();
	// For all element in the list
	while(it != m_lstScsiItem.end()){
		// Remove object from memory
		delete (*it);
		// Set pointer to null
		(*it) = NULL;
		// Next element
		++it;
	}
	// Then clear the list
	m_lstScsiItem.clear();
}

bool TScsiDev::findSysFsRoot(char *pszDev, char *pszSysFsRoot, char *pszFsType)
{
	int iScanRes;
	bool bRet;
	bool bError;
	FILE * fFile;
	char *pChar;
	char pcBuffer[INT_NAME_LEN_MAX];
	char szSysFsRoot[INT_NAME_LEN_MAX];
	char szDev[INT_DEVICE_LEN_MAX];
	char szFsType[INT_FSTYP_LEN_MAX];
	// Initialization
	iScanRes = -1;
	bRet = false;
	bError = false;
	fFile = NULL;
	pChar = NULL;
	// Clear pcBuffer
	memset(pcBuffer, 0, sizeof(pcBuffer));
	// Clear szDev
	memset(szDev, 0, sizeof(szDev));
	// Clear szFsType
	memset(szFsType, 0, sizeof(szFsType));
	// Check pointer
	if((pszDev != NULL) && (pszSysFsRoot != NULL) && (pszFsType != NULL)){
		// Open mount file
		fFile = fopen(STR_PROC_MOUNTS, STR_OPENFILE_READONLY);
		// Check if file is opened
		if(fFile != NULL){
			// File is open. Search device and sys root
			pChar = fgets(pcBuffer, sizeof(pcBuffer) - 2, fFile);
			// For all row in proc mounts file
			while((pChar != NULL) && (bRet == false) && (bError == false)){
				// Line parsing
				iScanRes = sscanf(pcBuffer, STR_MOUNTS_PARSING, szDev, szSysFsRoot, szFsType);
				// Check for sscanf error
				if(iScanRes == INT_MOUNTS_FIELDS){
					// Check if "sysfs" has been found
					if (strcmp(szFsType, STR_SYSFS_NAME) == 0){
						// copy szDev in output args
						if(pszDev != NULL){
							// Copy dev
							memcpy(pszDev, szDev, sizeof(szDev));
						}
						// copy szSysFsRoot in output args
						if(pszSysFsRoot != NULL){
							// Copy dev
							memcpy(pszSysFsRoot, szSysFsRoot, sizeof(szSysFsRoot));
						}
						// copy szFsType in output args
						if(pszFsType != NULL){
							// Copy dev
							memcpy(pszFsType, szFsType, sizeof(szFsType));
						}
						// Found: Set exit flag
						bRet = true;
					}
				}else{
					// sscanf error: invalid param
					setInternalError(errInvalidArguments, STR_TSCSIDEV_INVALIDARGS, STR_PROC_MOUNTS);
					// Set error
					bError = true;
				}
				// Next line
				pChar = fgets(pcBuffer, sizeof(pcBuffer) - 2, fFile);
			}
			// Close file
			fclose(fFile);
		}else{
			// Error: Unable to open file
			setInternalError(errUnableOpenFile, STR_TSCSIDEV_OPEN_RFILE, STR_PROC_MOUNTS);
		}
	}
	// Exit from method
	return bRet;
}

bool TScsiDev::isSdevAdd(const dirent *pstrctDirent)
{
	bool bFlag;
	bool bRet;
	// Initialization
	bRet = false;
	bFlag = false;
	/* Following no longer needed but leave for early lk 2.6 series */
	bFlag = ((strstr(pstrctDirent->d_name, "mt") == NULL) && 
					(strstr(pstrctDirent->d_name, "ot") == NULL) &&
					(strstr(pstrctDirent->d_name, "gen") == NULL));
	/* Above no longer needed but leave for early lk 2.6 series */
	bFlag = bFlag && 
					((strncmp(pstrctDirent->d_name, "host", 4) != 0) && 
					(strncmp(pstrctDirent->d_name, "target", 6)!= 0));
	// Check flag
	if(bFlag == true){
		// search ":"
		if (strchr(pstrctDirent->d_name, ':')){
			// Item is a scsi entry
			bRet = true;
		}
	}
	// Exit from method
	return (bRet);
}

bool TScsiDev::get_value(const std::string &strDirName, const char * base_name, char * value, int max_value_len)
{
	char buff[INT_NAME_LEN_MAX];
	FILE * f;
	int len;

	strcpy(buff, strDirName.c_str());
	strcat(buff, "/");
	strcat(buff, base_name);
	if (NULL == (f = fopen(buff, "r"))) {
	return false;
	}
	if (NULL == fgets(value, max_value_len, f)) {
	/* assume empty */
	value[0] = '\0';
	fclose(f);
	return true;
	}
	len = strlen(value);
	if ((len > 0) && (value[len - 1] == '\n'))
	value[len - 1] = '\0';
	fclose(f);
	return true;
}

bool TScsiDev::scanScsiItem(const char *pScanPath, char *pDevName, int iType, bool bKernelMode, TScsiItem* pItem, bool *pbAdd)
{
	bool bRet;
	std::string strRefPath;
	std::string strMountPath;
	char szBuff[INT_NAME_LEN_MAX];
	char szValue[INT_NAME_LEN_MAX];
	int iLocType;
	// Initialization
	bRet = false;
	iLocType = SCSI_TYP_INVALID;
	memset(szBuff,0,sizeof(szBuff));
	memset(szValue,0,sizeof(szBuff));
	// Check pointer
	if((pScanPath != NULL) && (pDevName != NULL) && (pItem != NULL) && (pbAdd != NULL)){
		// Initialize add flag argument
		(*pbAdd) = false;
		// Create scsi devices path
		strRefPath = pScanPath;
		strRefPath.append(STR_DIR_SEPARATOR);
		strRefPath.append(pDevName);
		// Create hctl name
		snprintf(szValue, INT_NAME_LEN_MAX, STR_TSCSIDEV_SLOTFMT, pDevName);
		// Set item name
		pItem->setHctl(std::string(szValue));
		// Get device type
		if (get_value(strRefPath, STR_TSCSIDEV_VTYPE, szValue, INT_NAME_LEN_MAX) == true){
			// Get device type from string
			if (sscanf(szValue, STR_TYPE_PARSING, &iLocType) == INT_TYPE_FIELDS){
				// Check type
				if((iLocType > SCSI_TYP_INVALID) && ((iType <= SCSI_TYP_INVALID) || (iType == iLocType))){
					// Insert device type in item
					pItem->setType(iLocType);
					// Get mount path
					bRet = scanScsiDevice(strRefPath, bKernelMode, &strMountPath);
					// Check error
					if(bRet == true){
						// Insert mount path in item
						pItem->setMount(strMountPath);
						// Set add flag to true: Add this item
						(*pbAdd) = true;
					}
				}else{
					// This itype is not required.
					bRet = true;
				}
			}else{
				// sscanf error: invalid param
				setInternalError(errInvalidArguments, STR_TSCSIDEV_INVALIDARGS, STR_TSCSIDEV_VTYPE);
			}
		}else{
			// Error to get scsi device type
				setInternalError(errArgumentsNotFound, STR_TSCSIDEV_TYPENOTFOUND, STR_TSCSIDEV_VTYPE);
		}
	}
	// Exit from method
	return(bRet);
}

bool TScsiDev::isNonSg(const dirent *pstrctDirEnt, std::string *pstrName, int *piType)
{
	int iLen;
	bool bRet;
	// Initialization
	iLen = -1;
	bRet = false;
	// Skip if not is a directory or not is a link or is a "." or "..".
	if ((DT_LNK == pstrctDirEnt->d_type) || ((DT_DIR == pstrctDirEnt->d_type) && ('.' != pstrctDirEnt->d_name[0]))){
		// Check on folder
		if (0 == strncmp("scsi_changer", pstrctDirEnt->d_name, 12)){  // scan scsi_changer dir
			// Assign output arg
			pstrName->assign(pstrctDirEnt->d_name);
			// Set type
			*piType = pstrctDirEnt->d_type;
			// Set exit flag
			bRet = true;
		} else if (0 == strncmp("block", pstrctDirEnt->d_name, 5)) {  // scan block dir
			// Assign output arg
			pstrName->assign(pstrctDirEnt->d_name);
			// Set type
			*piType = pstrctDirEnt->d_type;
			// Set exit flag
			bRet = true;
		} else if (0 == strcmp("tape", pstrctDirEnt->d_name)) { // scan tape dir
			// Assign output arg
			pstrName->assign(pstrctDirEnt->d_name);
			// Set type
			*piType = pstrctDirEnt->d_type;
			// Set exit flag
			bRet = true;
		} else if (0 == strncmp("scsi_tape:st", pstrctDirEnt->d_name, 12)) {  // scan scsi_tape:st dir
			// Get a len of the name
			iLen = strlen(pstrctDirEnt->d_name);
			// Check if last char is a digit
			if (isdigit(pstrctDirEnt->d_name[iLen - 1])) {
				/* want 'st<num>' symlink only */
				// Assign output arg
				pstrName->assign(pstrctDirEnt->d_name);
				// Set exit flag
				*piType = pstrctDirEnt->d_type;
				// Set exit flag
				bRet = true;
			} 
		} else if (0 == strncmp("onstream_tape:os", pstrctDirEnt->d_name, 16)) {  // onstream_tape:os dir
			// Assign output arg
			pstrName->assign(pstrctDirEnt->d_name);
			// Set exit flag
			*piType = pstrctDirEnt->d_type;
			// Set exit flag
			bRet = true;
		}
	}
	// Exit from method
	return (bRet);
}


bool TScsiDev::isValidFile(const dirent *pstrctDirent)
{
	bool bRet;
	// Initialization
	bRet = true;
	// Check file type
	if((pstrctDirent->d_type != DT_LNK) && ((pstrctDirent->d_type != DT_DIR) || (pstrctDirent->d_name[0] == '.'))){
		// This item should not be included.
		bRet = false;
	}
	// Exit from method
	return (bRet);
}

bool TScsiDev::scanAndSelectFirst(const std::string &strScanPath, std::string *pStrDev)
{
	bool bRet;
	bool bExit;
	DIR *pDir;
	TScsiItem* pItem;
	dirent *pstrctEnt;
	// Initialization
	bRet = false;
	bExit = false;
	pDir = NULL;
	pItem = NULL;
	pstrctEnt = NULL;
	// Check pointer
	if(pStrDev != NULL){
		// Clear output arg
		pStrDev->clear();
		// Open dir
		pDir = opendir(strScanPath.c_str());
		// Check if Dir exist
		if(pDir != NULL){
			// Set return flag
			bRet = true;
			// Get first element in directori
			pstrctEnt = readdir(pDir);
			// For all elements in dir
			while((pstrctEnt != NULL) && (bExit == false)){
				// Check if the entry is a scsi device
				if(isValidFile(pstrctEnt)==true){
					// Copy name of the tevice
					pStrDev->assign(pstrctEnt->d_name);
					// Set exit flag
					bExit = true;
				}
				// Next element
				pstrctEnt = readdir(pDir);
			}
			// Close dir
			closedir(pDir);
		}else{
			 // Error to open dir
			 setInternalError(errUnableOpenDir, STR_TSCSIDEV_OPEN_DIR, strScanPath);
		}
	}
	// Exit from method
	return(bRet);
}

bool TScsiDev::getScsiDevFolder(const std::string &strPath, const std::string &strName, int iType, bool bKernelMode, std::string *pstrMountPath)
{
	int iRes;
	bool bRet;
	char *pChar;
	char szWorkDir[INT_NAME_LEN_MAX];
	char szOldWorkDir[INT_NAME_LEN_MAX];
	std::string strDevPath;
	std::string strLocDevName;
	std::string strLnkName;
	struct stat strctStat;
	// Initialization
	iRes = -1;
	bRet = false;
	pChar = NULL;
	szWorkDir[0] = CHR_SCSI_NULLCHAR;
	szOldWorkDir[0] = CHR_SCSI_NULLCHAR;
	strLocDevName = strName;
	// Check pointer
	if(pstrMountPath != NULL){
		// Clear output args
		pstrMountPath->clear();
		// Costruct a device path
		strDevPath.assign(strPath);
		strDevPath.append(STR_DIR_SEPARATOR);
		strDevPath.append(strLocDevName);
		// Check if type is directory
		if(iType == DT_DIR){
			// Get first file in the folder (device)
			bRet = scanAndSelectFirst(strDevPath, &strLocDevName);
			// Create a dev folder
			strDevPath.append(STR_DIR_SEPARATOR);
			strDevPath.append(strLocDevName);
		}
		// Check for error
		if((bRet == true) || ((bRet ==false) && (iType != DT_DIR))){
			// Preset error
			bRet = false;
			// Check if a path is a valid directory
			iRes = stat(strDevPath.c_str(), &strctStat);
			// Check error
			if(iRes > -1){
				if(S_ISDIR(strctStat.st_mode) == true){
					// Get actual work dir
					pChar = getcwd(szOldWorkDir, INT_NAME_LEN_MAX);
					// Check error
					if(pChar != NULL){
						// Change dir: pos to scsi device dir
						iRes = chdir(strDevPath.c_str());
						// Check error
						if(iRes > -1){
							// Get actual workdir resolved by previews chdir
							pChar = getcwd(szWorkDir, INT_NAME_LEN_MAX);
							// Check error
							if(pChar != NULL){
								// Set return flag
								bRet = true;
								// Create device dir; (ex. szWorkDir=".../sr0)
								pstrMountPath->assign(STR_DEV_NAME);
								pstrMountPath->append(STR_DIR_SEPARATOR);
								// Check kernel mode
								if(bKernelMode == false){
									// Link mode
									getLinkModeDev(szWorkDir, &strLnkName);
									pstrMountPath->append(strLnkName);
								}else{
									// Kernel mode
									pstrMountPath->append(basename(szWorkDir));
								}
							}else{
								// Unable to get workdir
								setInternalError(errGetWd, STR_TSCSIDEV_GETWD, STR_NULLSTRING);        
							}
							// Restore old work dir
							iRes = chdir(szOldWorkDir);
						}else{
							// Unable to change dir
							setInternalError(errChangeWd, STR_TSCSIDEV_CHWD, strDevPath);            
						}
					}else{
						// Unable to get workdir
						setInternalError(errGetWd, STR_TSCSIDEV_GETWD, STR_NULLSTRING);            
					}
				}
			}else{
				// iRes <= -1: error in stat --> Invalid path
				setInternalError(errInvalidPath, STR_TSCSIDEV_INVALIDPATH, strDevPath);
			}
		} // if((iType == DT_DIR) && (bRet == true))
	} // if(pstrMountPath != NULL)
	// Exit from method
	return (bRet);
}

bool TScsiDev::isBlockFile(const std::string &strFilePath , unsigned int *piMaj, unsigned int *piMin)
{
	bool bRet;
	struct stat strctStat;
	// Inizializzazioni
	bRet = false;
	// Get information on device path link
	if(lstat(strFilePath.c_str(), &strctStat) == 0){
		// Skip non-block and char file
		if((S_ISBLK(strctStat.st_mode) == true) || (S_ISCHR(strctStat.st_mode) == true)){
			// Get maj and min
			*piMaj = major(strctStat.st_rdev);
			*piMin = minor(strctStat.st_rdev);
			// Set flag to true
			bRet = true;
		}
	}
	// Exit from method
	return (bRet);
}

void TScsiDev::getLinkModeDev(const std::string &strScsiPath, std::string *pstrLnkName)
{
	bool bExit;
	bool bFlag;
	int iRes;
	unsigned int iScsiMaj;
	unsigned int iFileMaj;
	unsigned int iScsiMin;
	unsigned int iFileMin;
	DIR *pDir;
	dirent *pstrctEnt;
	std::string strDevPath;
	std::string strDevFile;
	char szValue[INT_NAME_LEN_MAX];
	// Initialization
	bExit = false;
	bFlag = false;
	iRes = -1;
	iScsiMaj = -1;
	iFileMaj = -1;
	iScsiMin = -1;
	iFileMin = -1;
	pDir = NULL;
	pstrctEnt = NULL;
	szValue[0] = CHR_SCSI_NULLCHAR;
	// Create /dev string
	strDevPath.assign(STR_DIR_SEPARATOR);
	strDevPath.append(STR_DEV_NAME);
	// Check pointer
	if(pstrLnkName != NULL){
		// Clear output arg
		pstrLnkName->clear();
		// Open dir
		pDir = opendir(strDevPath.c_str());
		// Check if Dir exist
		if(pDir != NULL){
			// Get a dev major and nimor
			bFlag = get_value(strScsiPath.c_str(), STR_DEV_NAME, szValue, INT_NAME_LEN_MAX);
			// Check for error
			if(bFlag == true){
				// Get major an minor version
				iRes = sscanf(szValue, STR_MAJMIN_PARSING, &iScsiMaj, &iScsiMin);
			}
			// Check for sscanf error
			if(iRes == INT_MAJMIN_PARSING){
				// Get first element in directori
				pstrctEnt = readdir(pDir);
				// For all elements in dir
				while((pstrctEnt != NULL) && (bExit == false)){
					// Create file path
					strDevFile.assign(strDevPath);
					strDevFile.append(STR_DIR_SEPARATOR);
					strDevFile.append(pstrctEnt->d_name);
					// Check if the entry is a scsi device
					if(isBlockFile(strDevFile, &iFileMaj, &iFileMin)==true){
						// Verify match
						if((iFileMaj == iScsiMaj) && (iFileMin == iScsiMin)){
							// Match found
							pstrLnkName->assign(pstrctEnt->d_name);
							// Set exit flag to true
							bExit = true;
						}
					}
					// Next element
					pstrctEnt = readdir(pDir);
				} // while
			}
			// Close dir
			closedir(pDir);
		}
	}
}

bool TScsiDev::scanScsiDevice(const std::string &strPath, bool bKernelMode, std::string *pStrMount)
{
	bool bRet;
	bool bExit;
	int iItemType;
	std::string strMountPath;
	std::string strDevName;
	DIR *pDir;
	struct dirent *pstrctEnt;
	// Initialization
	bRet = false;
	bExit = false;
	iItemType = SCSI_TYP_INVALID;
	pDir = NULL;
	pstrctEnt = NULL;
	// Check pointer
	if(pStrMount != NULL){
		// Clear param
		pStrMount->clear();
		// Open dir
		pDir = opendir(strPath.c_str());
		// Check if Dir exist
		if(pDir != NULL){
			// Set ok flag to true
			bRet = true;
			// Get first element in directori
			pstrctEnt = readdir(pDir);
			// For all elements in dir
			while((pstrctEnt != NULL) && (bExit == false)){
				// Check element
				bExit = isNonSg(pstrctEnt, &strDevName, &iItemType);
				if(bExit == true){
					// Retrive device attrib
					bRet = getScsiDevFolder(strPath, strDevName, iItemType, bKernelMode, &strMountPath);
					// Check if is in kernel mode
					pStrMount->assign(strMountPath);
				}
				// Next element
				pstrctEnt = readdir(pDir);
			}
			// Close dir
			closedir(pDir);
		}else{
			// Error to open dir
			setInternalError(errUnableOpenDir, STR_TSCSIDEV_OPEN_DIR, strPath);
		}
	} // if(pStrMount != NULL)
	// Exit from metod
	return(bRet);
}

bool TScsiDev::enumDeviceS(const std::string &strScanPath, int iType, bool bKernelMode)
{
	bool bRet;
	bool bAdd;
	bool bExit;
	DIR *pDir;
	TScsiItem* pItem;
	dirent *pstrctEnt;
	// Initialization
	bRet = false;
	bAdd = false;
	bExit = false;
	pDir = NULL;
	pItem = NULL;
	pstrctEnt = NULL;
	// Open dir
	pDir = opendir(strScanPath.c_str());
	// Check if Dir exist
	if(pDir != NULL){
		// Get first element in directori
		pstrctEnt = readdir(pDir);
		// For all elements in dir
		while((pstrctEnt != NULL) && (bExit == false)){
			// Check if the entry is a scsi device
			if(isSdevAdd(pstrctEnt)==true){
				// Create item
				pItem = new TScsiItem;
				// Scan the device folder
				bRet = scanScsiItem(strScanPath.c_str(), pstrctEnt->d_name, iType, bKernelMode, pItem, &bAdd);
				// Check if this item to be inserted
				if(bRet == true){
					if(bAdd == true){
						// Insert into the list
						m_lstScsiItem.push_back(pItem);
					}else{
						// Item has not been inserted. Delete it.
						delete (pItem);
						pItem = NULL;
					}
				}else{
					// scanScsiItem return error.
					bExit = true;
				}
			}
			// Next element
			pstrctEnt = readdir(pDir);
		}
		// Close dir
		closedir(pDir);
	}else{
		// Error to open dir
		setInternalError(errUnableOpenDir, STR_TSCSIDEV_OPEN_DIR, strScanPath);
	}
	// Exit from method
	return(bRet);
}

void TScsiDev::setInternalError(enmTScsiError enmError, const std::string &strErrorMsg, const std::string &strExtra)
{
	// Set error type
	m_enmError = enmError;
	// Set Error message
	m_strErrorMsg = strErrorMsg;
	// Set extra error
	m_strExtra = strExtra;
}

// Public method implementation
bool TScsiDev::getDevices(std::list<const TScsiItem*> **plstDev, bool bKernelMode/* = false*/, int iType/* = SCSI_TYP_INVALID*/)
{
	bool bRet;
	char szSysFsRoot[INT_NAME_LEN_MAX];
	char szDev[INT_DEVICE_LEN_MAX];
	char szFsType[INT_FSTYP_LEN_MAX];
	std::string strDevPath;
	// Initialization
	bRet = false;
	szSysFsRoot[0] = CHR_SCSI_NULLCHAR;
	szDev[0] = CHR_SCSI_NULLCHAR;
	szFsType[0] = CHR_SCSI_NULLCHAR;
	// Clear old list
	clear();
	// Check pointer list
	if(plstDev != NULL){
		// Clear list pointer
		(*plstDev) = NULL;
		// Set internal error
		setInternalError(errUndefined, STR_TSCSIDEV_UNDEFINED, STR_NULLSTRING);
		// get sys root
		bRet = findSysFsRoot(szDev, szSysFsRoot, szFsType);
		// Check for error
		if(bRet == true){
			// Create main folder
			strDevPath = szSysFsRoot;
			strDevPath.append(STR_BUS_SCSI_DEVS);
			// scan scsi devices present in main folder
			bRet = enumDeviceS(strDevPath, iType, bKernelMode);
			// Check for error
			if(bRet == true){
				// Then share a list
				//*plstDev = &m_lstScsiItem;
				*plstDev = reinterpret_cast<std::list<const TScsiItem*>*>(&m_lstScsiItem);
				// Set error to success
				setInternalError(errSuccess, STR_TSCSIDEV_SUCCESS, STR_NULLSTRING);
			}
		}
	}
	// Exit from method
	return (bRet);
}

bool TScsiDev::getMountedDevOn(const std::string &strDevice, std::string *pStrOut)
{
	bool bRet;
	bool bExit;
	FILE *pfileMtab;
	struct mntent *pstrctPart;
	std::string strLink;
	// Initialization
	bRet = false;
	bExit = false;
	pfileMtab = NULL;
	pstrctPart = NULL;
	// Initialize internal
	setInternalError(errUndefined, STR_TSCSIDEV_UNDEFINED, STR_NULLSTRING);
	// Check out args
	if(pStrOut != NULL){
		// Clear output string
		pStrOut->clear();
		// Check if exist a initial "/" char
		if(strDevice.at(0) != '/'){
			// Insert "/" at begin of the string
			strLink.assign(STR_DIR_SEPARATOR);
		}
		// Append device
		strLink.append(strDevice);
		// Open mtab
		pfileMtab = setmntent (STR_MTAB_PATH, STR_OPENFILE_READONLY);
		// Check for error
		if (pfileMtab != NULL){
			// Point to first element
			pstrctPart = getmntent(pfileMtab);
			// For all mounted element
			while ((pstrctPart != NULL) && (bExit == false)){
				// Get a valid "fsname"
				if ((pstrctPart->mnt_fsname != NULL)	&& (strcmp(pstrctPart->mnt_fsname, strLink.c_str())) == 0){
					// Found device in mtab
					bExit = true;
					// Set output string
					pStrOut->assign(pstrctPart->mnt_dir);
				}
				// Next element
				pstrctPart = getmntent(pfileMtab);
			}
			// Close mntent (return always 1)
			endmntent(pfileMtab);
			//  Set return error flag to true
			bRet = true;
			// Set error to OK
			setInternalError(errSuccess, STR_TSCSIDEV_SUCCESS, STR_NULLSTRING);
		}else{
			// Can't open Mtab
			setInternalError(errMtab, STR_TSCSIDEV_OPEN_MTAB, STR_NULLSTRING);
		}
	}else{
		// Set internal error: Output parameter not valid (NULL pointer)
		setInternalError(errNullParam, STR_TSCSIDEV_INVALIDPARAM, STR_NULLSTRING);
	}
	// Exit from method
	return(bRet);
}

bool TScsiDev::isMounted(const std::string &strDevice)
{
	bool bRet;
	bool bFlag;
	std::string strFolder;
	// Initialization
	bRet = false;
	bFlag = false;
	// Get mount folder of the device "strDevice"
	bFlag = getMountedDevOn(strDevice, &strFolder);
	// Check if the folder is mounted
	if((bFlag == true) && (strFolder.empty() == false)){
		// The method getMountedDevOn return true (no error) and the device is mounted on "strFolder": return OK
		bRet = true;
	}
	// Exit from method
	return(bRet);
}

bool TScsiDev::isDirMounted(const std::string &strPath)
{
	bool bRet;
	FILE *pfileMtab;
	struct mntent *pstrctPart;
	// Initialization
	bRet = false;
	pfileMtab = NULL;
	pstrctPart = NULL;
	// Open mtab
	pfileMtab = setmntent (STR_MTAB_PATH, STR_OPENFILE_READONLY);
	// Check for error
	if (pfileMtab != NULL){
		// Point to first element
		pstrctPart = getmntent(pfileMtab);
		// For all mounted element
		while ((pstrctPart != NULL) && (bRet == false)){
			// Get a valid "fsname"
			if ((pstrctPart->mnt_fsname != NULL)	&& (strcmp(pstrctPart->mnt_dir, strPath.c_str())) == 0){
				// Found device in mtab
				bRet = true;
			}
			// Next element
			pstrctPart = getmntent(pfileMtab);
		}
		// Close mntent
		endmntent (pfileMtab);
	}
	// Exit from method
	return(bRet);
}

bool TScsiDev::isFbnDvdConfigured(uint16_t uiSysId, bool *pbPresent)
{
	return(isFbnConfigured(ACS_CS_API_HWC_NS::FBN_DVD, uiSysId, pbPresent));
}

bool TScsiDev::isFbnConfigured(uint16_t uiFbnIn, uint16_t uiSysId, bool *pbPresent)
{
	bool bExit;
	ACS_CS_API_HWC * pHwc;
	// Initialization
	bExit = false;
	pHwc = NULL;
	// Check out parameter
	if(pbPresent != NULL){
		// Initialize out parameters
		*pbPresent = false;
		// Create HWC instance
		pHwc = ACS_CS_API::createHWCInstance();
		// Chech for error
		if(pHwc != NULL){
			// Get FBN board on sysId
			bExit = searchFbnBoard(pHwc, uiFbnIn, uiSysId, pbPresent);
			// Release instance
			ACS_CS_API::deleteHWCInstance(pHwc);
			// Set pointer to null
			pHwc = NULL;
		}// if(pHwc != NULL)
	}
	// Exit from method
	return bExit;
}

bool TScsiDev::searchFbnBoard(ACS_CS_API_HWC *pHwc, uint16_t uiFbn, uint16_t uiSysId, bool *pbPresent)
{
	bool bExit;
	ACS_CS_API_NS::CS_API_Result apiResult;
	ACS_CS_API_BoardSearch *pBs;
	ACS_CS_API_IdList lstBoards;
	// Initialization
	bExit = false;
	pBs = NULL;
	apiResult = ACS_CS_API_NS::Result_Failure;
	// Check out parameter
	if(pbPresent != NULL){
		// Initialize out parameter
		*pbPresent = false;
		// Create a search instance
		pBs = ACS_CS_API_HWC::createBoardSearchInstance();
		// check if no error
		if(pBs != NULL){
			// search criteria
			pBs->reset();
			// Set criteria: search FBN specified only
			pBs->setFBN(uiFbn);
			// Set criteria: search on sysId specified only
			pBs->setSysId(uiSysId);
			// Get boards from CS
			apiResult = pHwc->getBoardIds(lstBoards, pBs);
			// Check for error
			if(apiResult == ACS_CS_API_NS::Result_Success){
				// Check the number of the list
				if(lstBoards.size() == 0){
					// Set exit flag to true (no error) and bPresent remain false (no error and FBN not found)
					bExit = true;
				}else	if(lstBoards.size() == 1){
					// Set present flag to true
					*pbPresent = true;
					// Set exit flag to true (no error) and bPresent to true (no error and FBN found)
					bExit = true;
				}
			}
			// Release instance
			ACS_CS_API_HWC::deleteBoardSearchInstance(pBs);
			pBs = NULL;
		}else{
			// Error in create instance
		}
	}
	// Exit from method
	return (bExit);
}

enmTScsiError TScsiDev::getInternaleError(std::string *pStrErrorMsg, std::string *pStrExtra)
{
	// Check pointer
	if(pStrErrorMsg != NULL){
		// Copy value
		pStrErrorMsg->assign(m_strErrorMsg);
	}
	// Check pointer
	if(pStrExtra != NULL){
		// Copy value
		pStrExtra->assign(m_strExtra);
	}
	// Exit from method
	return(m_enmError);
}

// ******************* class TScsiItem
TScsiItem::TScsiItem()
:m_iType(SCSI_TYP_INVALID)
{
}

TScsiItem::~TScsiItem()
{
}

void TScsiItem::getHctl(std::string *pStrHctl) const
{
	// Check pointer. In no null, return a HCTL info
	if(pStrHctl != NULL){
		pStrHctl->assign(m_strHctl);
	}
}

void TScsiItem::setHctl(const std::string &strHctl)
{
	m_strHctl = strHctl;
}

void TScsiItem::getMount(std::string *pStrMount) const
{
	// Check pointer. In no null, return a mount info
	if(pStrMount != NULL){
		pStrMount->assign(m_strMount);
	}
}

void TScsiItem::setMount(const std::string &strMount)
{
	// Set mount string
	m_strMount = strMount;
}

int TScsiItem::getType() const
{
	// Return Type of scsi media
	return(m_iType);
}

void TScsiItem::setType(int iType)
{
	m_iType = iType;
}
