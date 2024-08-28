
#if 0

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      cerr << "Usage: Handles <number of handles>\n" << endl;
      return 255;
   }

   long nNumOfHandles = atol(argv[1]);
   if (nNumOfHandles <= 0)
   {
      cerr << "Invalid argument \"" << argv[1] << "\".\n" << endl;
      return 255;
   }

   HANDLE* lpvHandles = (HANDLE*)malloc(nNumOfHandles*sizeof(HANDLE));
   if (!lpvHandles)
   {
      cerr << "Failed to allocate " << argv[1] << " number of handles.\n" << endl;
      return 255;
   }

   TCHAR szEventName[64+1];
   long nCount = 0;

   cout << "Creating " << nNumOfHandles << " event handles... " << flush;

   BOOL bContinue = TRUE;
   while (bContinue && nCount < nNumOfHandles)
   {
      _stprintf(szEventName, _T("Handles%d"), nCount);
      (HANDLE)lpvHandles[nCount] = ::CreateEvent(NULL, TRUE, FALSE, szEventName);
      if (!(HANDLE)lpvHandles[nCount])
      {
         cerr << "\nFailed to create event \"" << szEventName << "\".\n" << endl;
         bContinue = FALSE;
      }
      else
      {
         if (++nCount % 200 == 0)
            ::Sleep(50);
      }
   }

   cout << "Done.\n" << nCount << " event handles created.\n";
   cout << "Press <ENTER> to continue. " << flush;
   cin.get(szEventName, 64);

   cout << "\nClosing handles (this might take several minutes)... " << flush;

   for (long nIdx = 0; nIdx < nCount; nIdx++)
   {
      ::CloseHandle((HANDLE)lpvHandles[nIdx]);
      if (nIdx > 0 && (nIdx % 200) == 0)
         ::Sleep(50);
   }

   free(lpvHandles);
   cout << "Done.\nHave a nice day!\n" << endl;

   return 0;
}

#endif
