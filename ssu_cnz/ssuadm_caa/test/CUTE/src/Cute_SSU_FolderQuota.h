
#ifndef _CUTE_ACS_FOLDERQUOTA_H
#define _CUTE_ACS_FOLDERQUOTA_H

#include "cute_suite.h"

class ACS_SSU_FolderQuota;


class Cute_SSU_FolderQuota
{
public:

  // Cute_SSU_FolderQuota();
  static cute::suite make_suite_Cute_SSU_FolderQuota();
  static void vFolderQuotaCleanup();

  static void vTestCheckConfigSet();
  static void vTestGetQuotaValue();
  static void vTestIsActive();
  static void vTestIsEqual();
  static void vTestStop();
  static void vTestRemoteStop();
  static void vTestAddSubDirObj();
  static void vTestvRemoveSubDirObjs();
  static void vTestCheckConfig();
  static void vTestFolderAvailable();
  static void vTestget_Path();
  static void vTestget_ConfigType();

  static void vTestCalculateFolderSize();
  static void vTestenCheckAndRaiseAlarm();
  static void vTestszGetBlockDevice();

  static void vTestAddFolderQuota();
  static void vTestModifyFolderQuota();
  static void VTestRemoveFolderQuota();

  static void vTestBytesToSizeString();

  static void vTestCalculateFolderQuota();

private:
  static ACS_SSU_FolderQuota *m_poFolderQuota;
  static ACE_Recursive_Thread_Mutex  m_srctQuotaCS;
};

#endif
