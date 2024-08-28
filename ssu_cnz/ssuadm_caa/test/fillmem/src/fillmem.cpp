#if 0

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <list>

#define BLOCK_SIZE (ULONG)0x100000

typedef struct _MEMITEM
{
   ULONG  Size;
   LPBYTE Buffer;
} MEMITEM, *LPMEMITEM;


static BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType);
static void AllocateMemory(void);
static void AllocateMemoryPerHour(void);
static void ReleaseMemory(BOOL bWantsResponce);

static std::list<LPMEMITEM> g_listMemItems;
static HANDLE g_hEvent = NULL;

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
   if (dwCtrlType == CTRL_C_EVENT && g_hEvent)
      ::SetEvent(g_hEvent);

   return TRUE;
}

void AllocateMemory()
{
   long nSize;
   printf("Amount of Memory to allocate in Mb (0 cancels the operation): ");
   scanf("%d", &nSize);

   if (nSize <= 0)
   {
      if (nSize < 0)
         printf("Invalid value!\n\n");
   }
   else
   {
      ULONG nTotal = (ULONG)(nSize*BLOCK_SIZE);
      ULONG nRemain = nTotal;
      ULONG nAllocSize;
      BOOL bContinue = TRUE;

      printf("Allocating Memory... ");

      while (nRemain > 0 && bContinue)
      {
         if (nRemain >= BLOCK_SIZE)
            nAllocSize = BLOCK_SIZE;
         else
            nAllocSize = nRemain;

         LPMEMITEM lpMemItem = (LPMEMITEM)malloc(sizeof(MEMITEM));
         if (!lpMemItem)
         {
            printf("Failed.\n\nEnter a key to return ");
            _getche();
            printf("\n\n");
            return;
         }
         else
         {
            lpMemItem->Buffer = (LPBYTE)::VirtualAlloc(NULL, nAllocSize, MEM_COMMIT, PAGE_READWRITE);
            if (!lpMemItem->Buffer)
            {
               printf("Failed.\n\nEnter a key to return ");
               _getche();
               printf("\n\n");

               free(lpMemItem);
               lpMemItem = NULL;
               return;
            }
            else
            {
               RtlZeroMemory(lpMemItem->Buffer, nAllocSize);
               //memset(lpMemItem->Buffer, 0, nAllocSize);
               lpMemItem->Size = nAllocSize;
               nRemain -= nAllocSize;

               g_listMemItems.push_back(lpMemItem);
               ::Sleep(50);
            }
         }
      }

      printf("Done.\n\nEnter a key to return ");
      _getche();
      printf("\n\n");
   }
}

void AllocateMemoryPerHour()
{
   long nSize;
   printf("Amount of Memory to allocate per hour in Mb (0 cancels the operation): ");
   scanf("%d", &nSize);

   if (nSize <= 0)
   {
      if (nSize < 0)
         printf("Invalid value!\n\n");
   }
   else
   {
      ULONG nTotalAllocated = 0;
      ULONG nAllocPerTimeunit = ((nSize*BLOCK_SIZE)/100);
      ULONG nRemain;
      ULONG nAllocSize;
      BOOL bContinue = TRUE;

      g_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
      printf("Allocating Memory (press CTRL+C to end the operation)... ");

      while (bContinue)
      {
         if (::WaitForSingleObject(g_hEvent, (1000*36)-50) != WAIT_TIMEOUT)
            bContinue = FALSE;
         else
         {
            nRemain = nAllocPerTimeunit;

            while (nRemain > 0 && bContinue)
            {
               if (nRemain >= BLOCK_SIZE)
                  nAllocSize = BLOCK_SIZE;
               else
                  nAllocSize = nRemain;

               LPMEMITEM lpMemItem = (LPMEMITEM)malloc(sizeof(MEMITEM));
               if (!lpMemItem)
               {
                  printf("Memory allocation failed.\n\nEnter a key to return ");
                  _getche();
                  printf("\n\n");
                  return;
               }
               else
               {
                  lpMemItem->Buffer = (LPBYTE)::VirtualAlloc(NULL, nAllocSize, MEM_COMMIT, PAGE_READWRITE);
                  if (!lpMemItem->Buffer)
                  {
                     printf("Memory allocation failed.\n\nEnter a key to return ");
                     _getche();
                     printf("\n\n");

                     free(lpMemItem);
                     lpMemItem = NULL;
                     return;
                  }
                  else
                  {
                     RtlZeroMemory(lpMemItem->Buffer, nAllocSize);
                     lpMemItem->Size = nAllocSize;
                     nRemain -= nAllocSize;
                     nTotalAllocated += nAllocSize;

                     g_listMemItems.push_back(lpMemItem);
                     ::Sleep(50);
                  }
               }
            }
         }
      }

      ::CloseHandle(g_hEvent);
      g_hEvent = NULL;

      printf("Done.\n%lu Mb totally allocated.\n\nEnter a key to return ",
             (ULONG)(nTotalAllocated/BLOCK_SIZE));
      _getche();
      printf("\n\n");
   }
}

void ReleaseMemory(BOOL bWantsResponce)
{
   if (!g_listMemItems.empty())
   {
      printf("Releasing Memory... ");

      std::list<LPMEMITEM>::iterator iter;
      while (!g_listMemItems.empty())
      {
         LPMEMITEM lpMemItem = *g_listMemItems.begin();
         if (lpMemItem->Buffer && lpMemItem->Size > 0)
            ::VirtualFree(lpMemItem->Buffer, 0, MEM_RELEASE);

         g_listMemItems.erase(g_listMemItems.begin());
         free(lpMemItem);
         ::Sleep(50);
      }

      if (bWantsResponce)
      {
         printf("Done.\n\nEnter a key to return ");
         _getche();
         printf("\n\n");
      }
      else
         printf("Done.\n\n");
   }
}

int main(int argc, char* argv[])
{
   // Register the control handler routine
   ::SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

   char sAction[8];
   BOOL bQuit = FALSE;
   while (!bQuit)
   {
      printf("1. Allocate specified amount of Memory\n");
      printf("2. Allocate specified amount of Memory per hour\n");
      printf("3. Release Memory\n");
      printf("4. Exit\n\nEnter number and <ENTER>: ");
      scanf("%s", sAction);

      if (strcmp(sAction, "1") != 0 &&
         strcmp(sAction, "2") != 0 &&
         strcmp(sAction, "3") != 0 &&
         strcmp(sAction, "4") != 0)
      {
         fprintf(stderr, "Invalid option specified!\n\n");
      }
      else
      {
         if (strcmp(sAction, "1") == 0)
         {
            AllocateMemory();
         }
         else if (strcmp(sAction, "2") == 0)
         {
            AllocateMemoryPerHour();
         }
         else if (strcmp(sAction, "3") == 0)
         {
            ReleaseMemory(TRUE);
         }
         else
         {
            ReleaseMemory(FALSE);
            bQuit = TRUE;
         }
      }
   }

   // Unregister the control handler routine
   ::SetConsoleCtrlHandler(ConsoleHandlerRoutine, FALSE);

   return 0;
}

#endif
