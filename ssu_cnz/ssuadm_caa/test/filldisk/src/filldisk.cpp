#if 0

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define FILE_PREFIX  _T("Dummy")
#define FILE_SIZE    (unsigned long)0x6400000
#define BUFFER_SIZE  (unsigned long)0xFFFF


// Prototypes
static BOOL CheckFile(const _TCHAR* lpszFile, const BOOL bDirOrDrive = FALSE);
static BOOL FillFile(const _TCHAR* lpszDrive, const unsigned long nFileIndex, const DWORD dwSize);
static unsigned long GetStartFileIndex(const _TCHAR* lpszDrive);

// Implementation
BOOL CheckFile(const _TCHAR* lpszFile, const BOOL bDirOrDrive)
{
   DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
   if (bDirOrDrive)
      dwFlagsAndAttributes |= FILE_FLAG_BACKUP_SEMANTICS;

   HANDLE hFile = ::CreateFile(lpszFile,
                               0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               dwFlagsAndAttributes,
                               NULL);

   if (hFile == INVALID_HANDLE_VALUE)
      return FALSE;

   ::CloseHandle(hFile);
   return TRUE;
}

BOOL FillFile(const _TCHAR* lpszDrive, const unsigned long nFileIndex, const DWORD dwSize)
{
   _TCHAR szFile[MAX_PATH];
   _stprintf(szFile, _T("%s\\%s%03lu.tmp"), lpszDrive, FILE_PREFIX, nFileIndex);

   fprintf(stdout, "Creates file \"%s\"... ", szFile);
   fflush(stdout);

   HANDLE hFile = ::CreateFile(szFile,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

   if (hFile == INVALID_HANDLE_VALUE)
   {
      fprintf(stdout, "failed!\n");
      return FALSE;
   }

   DWORD dwBytesWritten;
   DWORD dwBytesTotal = 0;

   BYTE Buffer[BUFFER_SIZE];
   BOOL bSuccess, bContinue = TRUE;
   while (bContinue)
   {
      if (dwBytesTotal >= dwSize)
         bContinue = FALSE;
      {
         

         bSuccess = ::WriteFile(hFile,
                                Buffer, 
                                ((dwSize-dwBytesTotal) > BUFFER_SIZE ? BUFFER_SIZE : (dwSize-dwBytesTotal)),
                                &dwBytesWritten,
                                NULL);

         if (!bSuccess)
            bContinue = FALSE;
         else
            dwBytesTotal += dwBytesWritten;
      }
   }

   ::CloseHandle(hFile);

   if (!bSuccess)
      fprintf(stdout, "failed!\n");
   else
      fprintf(stdout, "done.\n");
   fflush(stdout);

   return bSuccess;
}

unsigned long GetStartFileIndex(const _TCHAR* lpszDrive)
{
   unsigned long nFileIndex = 0;
   _TCHAR szFile[MAX_PATH];

   BOOL bContinue = TRUE;
   while (bContinue)
   {
      _stprintf(szFile, _T("%s\\%s%03lu.tmp"), lpszDrive, FILE_PREFIX, ++nFileIndex);
      if (!CheckFile(szFile))
         bContinue = FALSE;
   }

   return nFileIndex;
}

int main(int argc, char* argv[])
{
   if (argc < 3)
   {
      fprintf(stderr, "Usage: filldisk <size in Mb to fill> <partition letter>\n\n");
      return 255;
   }

   long nSize = atol(argv[1]);
   if (nSize == 0)
   {
      fprintf(stderr, "Invalid size value: %s\n\n", argv[1]);
      return 255;
   }

   int nNumOfWholeFiles = (nSize / 100);
   unsigned long nBytesRest = (nSize % 100)*0xFFFFF;

   if (!CheckFile(argv[2], TRUE))
   {
      fprintf(stderr, "Invalid partition letter \"%s\"\n\n", argv[2]);
      return 255;
   }

   unsigned long nStartFileIndex = GetStartFileIndex(argv[2]);

   BOOL bSuccess;
   for (int nIndex = 0; nIndex < nNumOfWholeFiles; nIndex++)
   {
      if (!(bSuccess = FillFile(argv[2], nStartFileIndex++, FILE_SIZE)))
         break;
   }

   // Writes the remaining data
   if (bSuccess && nBytesRest > 0)
      bSuccess = FillFile(argv[2], nStartFileIndex++, nBytesRest);

   fprintf(stdout, "\n");
   fflush(stdout);

	return 0;
}
#endif
