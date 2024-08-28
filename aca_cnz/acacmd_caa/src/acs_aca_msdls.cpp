//****************************************************************
//
// NAME
//   msdls.C
//
// COPYRIGHT
//   COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//   All rights reserved.
//
//   The Copyright to the computer program(s) herein 
//   is the property of Ericsson Utvecklings AB, Sweden.
//   The program(s) may be used and/or copied only with 
//   the written permission from Ericsson Utvecklings AB or in 
//   accordance with the terms and conditions stipulated in the 
//   agreement/contract under which the program(s) have been 
//   supplied.

// DOCUMENT NO
//   CAA 109 0601

// AUTHOR
//   EAB/UZ/DG Carl Johannesson
 
//  DESCRIPTION
//    This program implements the 'msdls' command.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO  DATE        NAME        DESCRIPTION
//    A       2006-06-07  qnarrao     product release	  

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO  DATE        NAME        DESCRIPTION
//    A       2000-05-10  uabcajn     product release
//    B       2004-03-12  uabcajn     rogue wave replaced by STL.
//****************************************************************
//#pragma warning (disable: 4786 4800)


#include <cstring>
#include<iostream>
#include<cstdlib>
#include <ace/ACE.h>
//#include <acams_parameters.h>
//#include <acs_pha_parameter.h>
#include <acs_aca_ms_constants.H>
#include <acs_aca_message_number.h>
#include "acs_aca_ms_commit_file.h"
#include"acs_aca_ms_statistics_file.h"
//#include <acams_commitfile.h>
//#include <acams_statisticsfile.h>
//#include <acs_prc_process.h>
#include <functional>
#include <algorithm>
//#include <acs_exceptionhandler.h>
//#include <clusapi.h>
#include <vector>
#include <list>
#include <acs_prc_api.h>
#include <ACS_TRA_trace.h>
//#include <commondll.h>
using namespace std;

#define FILESIZE (1000)

// Command list option definitions:
//
static const ACE_INT32   NoOfOptions = 7;
static const ACE_INT32   NoOfCombinations = 6;

static const ACE_TCHAR* Options[NoOfOptions] = { "-m", 
				            "-s", 
				            "-d", 
				            "-t", 
				            "-n", 
				            "-a", 
				            "-S"};				     
// Index into Options array.
enum Option { 	Option_None = -1,
		Option_MessageStore = 0,
		Option_Site = 1,
		Option_Data = 2,
		Option_DataTrunc = 3,
		Option_DataNumberOnly = 4,
		Option_ReadEntriesOnly = 5,
		Option_Statistics = 6,
		Option_Struct = 7 }; // Default option 
		

static const ACE_INT32 ValidOptionCombinations[NoOfCombinations][NoOfOptions] = 
                                                    {{1,1,1,0,0,0,0},
						     {1,1,1,1,0,0,0},
						     {1,1,1,0,1,0,0},
						     {1,1,0,0,0,0,0},
						     {1,1,0,0,0,1,0},
						     {1,1,0,0,0,0,1}};


static const ACE_INT32 ValidNoOfParams[NoOfCombinations] = { 7, 8, 8, 5, 6, 6 };                                                     

//struct string64 {char str[64];};

static ACE_INT32 MaxNoOfRecParameter;
static ACE_INT32 MaxRecSizeParameter;
static ACE_INT32 MaxNoOfFilesParameter;

static string MessageStoreHome;  // Complete path to the message store

// Some globals on data input file.
//
static ACE_HANDLE dfile_hand = ACE_INVALID_HANDLE;
static ACE_HANDLE MapDataFile = ACE_INVALID_HANDLE;
static void* file_view = NULL;
static ACE_INT32  SizeDataFileLow32 = 0;
static ACE_INT32  SizeDataFileHigh32 = 0;

union LongLong
{
    unsigned char   octets[8];
    unsigned long   halves[2];
};



static ACS_TRA_trace ACS_ACA_CMD_MSDLS_DebugTrace("ACS_ACA_CMD_MSDLS_DebugTrace", "C512");
static ACS_TRA_trace ACS_ACA_CMD_MSDLS_ErrorTrace("ACS_ACA_CMD_MSDLS_ErrorTrace", "C512");

class MsgFile
{
    public:
		// constructor.
        MsgFile( const char* s ): name(s), firstMsgNum(s), lastMsgNum(0),
                                  fileNum(0), noOfMsgs(0), 
                                  eofFound(0), eomFound(0)              
        {
            const char* ri = strchr( s, '_' );
            if( ri ) 
			{
				fileNum = atoi(ri+1);
			}
        }

		// destructor.
        ~MsgFile()
        { 
			if(lastMsgNum) 
			{
				delete lastMsgNum;
			}
        }
        int operator<(const MsgFile& f) const 
        { 
			return firstMsgNum < f.firstMsgNum;
		}
        
        int operator==( const MsgFile& f) const 
        { 
			return firstMsgNum == f.firstMsgNum;
		}
        bool operator()(const MsgFile& x, const MsgFile& y)
		{
			return x.firstMsgNum < y.firstMsgNum;
		}

        int readLastMsg();
        const char* getFullPathName();
                
        string                  name;
        ACS_ACA_MessageNumber	firstMsgNum;
        ACS_ACA_MessageNumber*	lastMsgNum;
        ACE_INT32                fileNum;
        ACE_INT32                     noOfMsgs;
        bool                    eofFound;
        bool                    eomFound;
};

//****************************************************************
// Try to read the last message in the file.
// An unlinked file has an EndOfFile mark at the very end.
// A non-corrupted still in use file has an EndOfMessage mark at 
// the end.
// Returns: -1 if file no longer exists; 
//           0 if corrupted; 
//           1 if OK.
//****************************************************************
int MsgFile::readLastMsg()
{
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering MsgFile::readLastMsg()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    unsigned char          buffer[ACAMS_Eof_Mark_Length];
    unsigned int           offset = 0;
    
#if 0
	file_hand=CreateFile(getFullPathName(),
		  		         GENERIC_READ,
				         FILE_SHARE_READ | FILE_SHARE_WRITE,
					     NULL,
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_READONLY,
						 NULL);
	if (file_hand==INVALID_HANDLE_VALUE)
	{
		return -1;
	}
            
    // Read the tail of the file into a buffer.
	SetFilePointer(file_hand,
	    		   -ACAMS_Eof_Mark_Length,
				   NULL,
				   FILE_END);

	DWORD nobr;
	if (!ReadFile(file_hand,
				 (char*)buffer,
				 ACAMS_Eof_Mark_Length,
				 &nobr,
				 NULL))
	{
		CloseHandle(file_hand);
		return 0;
	}
#endif

	ACE_INT32 file_hand,file_read;
	int result=0;(void)result;

	file_hand = ACE_OS::open(getFullPathName(), O_RDONLY);	
	result = lseek(file_hand, -ACAMS_Eof_Mark_Length, SEEK_END);
	
	file_read = read(file_hand, &buffer, ACAMS_Eof_Mark_Length);
	if(file_read == -1)
	{
		close(file_hand);
		close(file_read);
		return 0;
	}
	
	
            
    if(ACE_OS::memcmp((void*)buffer,(void*)ACAMS_Eof_Mark,ACAMS_Eof_Mark_Length) 
                                                                  == 0 )
    {
        // It was a good EOF.
        eofFound = true;
                 
        // Position file pointer to offset to start of last message.
#if 0
		SetFilePointer(file_hand,
	    			   -(ACAMS_Eof_Mark_Length+8),
					   NULL,
					   FILE_CURRENT);
#endif

	 lseek(file_hand, -(ACAMS_Eof_Mark_Length+8), SEEK_CUR);
    }
    else if(ACE_OS::memcmp((void*)(buffer+8),(void*)ACAMS_Msg_End_Mark,4)==0)
    {
        // It was an end-of-message mark.
        eomFound = true;  
		
        // Position file pointer to offset to start of last message.
#if 0
		SetFilePointer(file_hand,
	    			   -8,
					   NULL,
					   FILE_CURRENT);
#endif
	lseek(file_hand, -8, SEEK_CUR);
    }
    else
    {
        // Can't recognize were we are in the file. Possibly corrupted.
        close(file_hand);
        return 0; // Failed.
    }
                
    // Read offset to beginning of last message and position to that point.              
#if 0
	if (!ReadFile(file_hand,
				 &offset,
				 sizeof(int),
				 &nobr,
				 NULL))
	{
		CloseHandle(file_hand);
		return 0;
	}
#endif
        if( read(file_hand, &offset, sizeof(int)) == -1)
	{
                close(file_hand);
                return 0;
        }

    if (offset>0xFFFF) //version 3
	{
		int leastsigbyte = (offset & 0xFF000000)>>24;
		int middlebyte = (offset & 0x00FF0000)>>8;
		offset = leastsigbyte + middlebyte;	
	}
    offset = (offset & 0x0000FFFF);
#if 0
	SetFilePointer(file_hand,
	    			   -(offset+4),
					   NULL,
					   FILE_CURRENT);
#endif
	lseek(file_hand, -(offset+4), SEEK_CUR);        
    // Read message number.
#if 0
	if (!ReadFile(file_hand,
				 (char*)buffer,
				 8,
				 &nobr,
				 NULL))
	{
		CloseHandle(file_hand);
		return 0;
	}
#endif
        if( read(file_hand, &buffer, 8) == -1)
        {
                close(file_hand);
                return 0;
        }
        
	buffer[8]='\0';
    // Create a message number.
    lastMsgNum = new ACS_ACA_MessageNumber(buffer);
                
    // Compute difference between first and last.	
    noOfMsgs = ((int)((unsigned long)((*lastMsgNum) - firstMsgNum)))+1;
    close(file_hand); 
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving MsgFile::readLastMsg()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    return 1;
}

//******************************************************************************
// Get full path name
//******************************************************************************
const char* MsgFile::getFullPathName()
{
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering MsgFile::getFullPathName()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    static char	buffer[1024];
    
    sprintf(buffer,"%s%s\\%s", MessageStoreHome.c_str(),
                                ACAMS_NameOfDataFileDir.c_str(),
                                name.c_str() );
    if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving MsgFile::getFullPathName()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    return buffer;
}

//****************************************************************
//      readParameters()
//****************************************************************
int readParameters(const char* /*site*/,
				   const char* /*store*/)
{
    if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering readParameters()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

   char msdHome[256]="";

//   ACE_TCHAR* szLogicName = const_cast<char *>("ACS_ACA_DATA$");
   char* szPath = NULL;
   unsigned long dwLen = 100;
//   int retCode = -1;

   memset((void*)msdHome,'\0',256);
   szPath = (char *)malloc(sizeof(char)*dwLen);
   memset((void*)szPath,'\0',100);
  
//TODO IMM related operations
#if 0 
   ACS_PHA_NamedTable msdTables("ACS/CXC1371131", "ACS_ACABIN_MessageStores");
   ACS_PHA_Parameter<ACAMS_Parameters> tableValue;

   while( tableValue.get(msdTables) == ACS_PHA_PARAM_RC_OK)
   {
      if (strcmp(tableValue.data().name, store) != 0)
	  {
		  continue;
	  }
      if (strcmp(tableValue.data().site, site) != 0)
      {
          continue;
      }
      // We have found the table entry.
      // Copy parameters of interest to global variables.
      
      MaxNoOfRecParameter = tableValue.data().noOfRecords;
      MaxRecSizeParameter= tableValue.data().recordSize;
      MaxNoOfFilesParameter = tableValue.data().noOfFiles;
		  while (-1 == retCode) {
			  if (NULL != szPath)
				  free(szPath);
			  szPath = (char *) malloc(dwLen*sizeof(char));
			  if(szPath)
				{
				  retCode = GetDataDiskPath(szLogicName,szPath,&dwLen);
				}
			  else
				{
				  printf("unable to create memory: Malloc failed");
				  exit(-1);
				}
		  }

		  if (retCode  <= -2) // failed to open registry / cluster
		  {
			printf("Wrong Logical Name or Failed to open registry key /cluster\n");
			exit(-1);
		  }

	  if ((!retCode) && (szPath != NULL))
			strcpy(msdHome,szPath);

		  break;
	  }

   MessageStoreHome = msdHome;
   MessageStoreHome += "\\";
   MessageStoreHome += store;
   MessageStoreHome += "\\";
   MessageStoreHome += site;
   MessageStoreHome += "\\";

   if (NULL != szPath)
	  free(szPath);
#endif

   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving readParameters()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	   if (NULL != szPath)
          free(szPath);

   return (0);
}

//****************************************************************
//  displayData()
//  This method is executed as a result of "-d" option, i.e it
//  displays data related to a file.
//****************************************************************
int displayData(int subOpt, const string& fileName)
{
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering displayData()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    enum State          {Empty,Filled,Lost,Skipped };
    static const char*	stateText[4] = {"Empty","Filled","Lost","Skipped" };
    const int			MessageTail = 8;
  
    unsigned char*		dataPtr = 0; 
    unsigned char*		msgData = 0; 
    LongLong			msgNumber;
    ACE_INT32		msgSize = 0;
                       
    ACS_ACA_MessageNumber*	thisNumber = 0;	
    ACS_ACA_MessageNumber*	previousNumber = 0;
    ACS_ACA_MessageNumber	one( (unsigned int)1 );
    State			state = Empty;
    int				msgInFile = 0;
    ACE_INT32	bytesDone = 0;
    bool			zeroMessage = false;
    unsigned char   tempv[4];
    int             additionalData;
    int             versionNumber;

    do
    {
		// Must check if we have enough data for message number and size.
		if (SizeDataFileLow32 - bytesDone < ACAMS_MsgHeaderSize)
		{
			printf( "Unexpected EOF in data file, %s\n", fileName.data() );
			return (-1);
		}          

		// Move to current message.
        if(dataPtr==0)
        {
            // First time.       
            dataPtr = (unsigned char*)file_view;
        }
        else
        {
            dataPtr += ACAMS_FileOverhead+msgSize;
        }

		// Get message number and size and position pointer into message.
        memcpy((void*)&msgNumber,(const void*)dataPtr,8);
        memcpy((void*)&msgSize,(const void*)(dataPtr+8),sizeof(long));
        msgSize -= MessageTail;
        msgData = dataPtr+ACAMS_MsgHeaderSize;
        bytesDone += ACAMS_MsgHeaderSize;
        
		// See if all message is in file.
        if( SizeDataFileLow32 - bytesDone < msgSize )
        {
			printf("Truncated message in data file, %s\n", fileName.data());
			return (-1);
		}
        memcpy(tempv,(void*)(dataPtr+12+msgSize),4);
        additionalData = tempv[0];
        versionNumber = tempv[1];
        versionNumber = versionNumber >> 1;
		if (tempv[3]==0)
		{//version 2
			additionalData=0;
			versionNumber=0;
		}
		bytesDone += msgSize+MessageTail;
	
		// Swap message numbers.
    	unsigned long halves[2];
		if ( previousNumber == 0 )
		{
			size_t      pos = fileName.find_last_of( '\\' );
			const char* ptr = fileName.c_str() +pos;
	    
			// First time. Must fake a previous number.
			thisNumber = new ACS_ACA_MessageNumber( ptr );
			thisNumber->getNumber( halves[0], halves[1] );
			if ((halves[0] > 0) || (halves[1] > 0)) (*thisNumber) -= 1;
			previousNumber = thisNumber;
		}

		thisNumber->getNumber( halves[0], halves[1] );
		if ((halves[0] > 0) || (halves[1] > 0))
		{
			if (previousNumber != 0) 
			{
				delete previousNumber;
			}
			previousNumber = thisNumber;
		}

		thisNumber = new ACS_ACA_MessageNumber(msgNumber.octets );     
		previousNumber->getNumber( halves[0], halves[1] );
		zeroMessage = false;
		if ((halves[0] == 0) && (halves[1] == 0))
		{
			zeroMessage = true;
		}
		else
		{
			thisNumber->getNumber( halves[0], halves[1] );
			if ((halves[0] == 0) && (halves[1] == 0)) 
			{
				zeroMessage = true;
			}
		}

		// Find out state of current message.
		while ((zeroMessage == false) &&
	       ((*thisNumber) - one > (*previousNumber)))
		{
    	    // There is a hole in the sequence! Messages are lost!
    	    // These lost messages are not stored at all. 
    	    // Must fake and print them!
    	    ++(*previousNumber);
    	    previousNumber->getNumber( halves[0], halves[1] );

     	    // Print basic info about lost message:
            printf("%05u: MessNo: {%010lu, %010lu}, "
                   "MessSize: 0000000000, messState: Lost\n",
                   msgInFile++, halves[0], halves[1] );
		}

		if( msgSize == 0 )
		{
    		  state = Skipped;
		}
		else
		{
			  state = Filled;
		}
    	    
        // Print basic info about message:
		printf( 
        "%05u: MessNo: {%010lu, %010lu}, MessSize: %d, messState: %s\n",
                 msgInFile++,
                 msgNumber.halves[0],
                 msgNumber.halves[1],
                 msgSize,
                 stateText[state] );
		printf("       Version:%d, AdditionalData:%d\n",
                versionNumber,
                additionalData);
           
		// Print some of its contents.    
		if( subOpt != Option_DataNumberOnly )
		{	    
			for( int j = 0; j < msgSize; j+= 16 )
			{
				long k = j;
		
				printf("       %05X: ", j);
		
				for (k = j; k < msgSize && (k - j) < 16; k++)
				{
					printf("%02X ", *(msgData + k));
				}
				for (; (k - j) < 16; k++)
				{
					printf("   ");
				}
				for (k = j; k < msgSize && (k - j) < 16; k++)
				{
					printf("%c", 
						(isprint(*(msgData + k)) ? *(msgData + k) : '.'));
				}
				printf("\n");
		
				if ((j > (1024 * 1024)) || 
					(msgSize > 16 && subOpt == Option_DataTrunc))
				{
					 printf("       Printout truncated\n" );
					break;
				}
			}
		}
     }while( (bytesDone+ACAMS_Eof_Mark_Length) < SizeDataFileLow32 );
     delete thisNumber;
     delete previousNumber;
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving displayData()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

     return 0;
    
} // End of displayData

//****************************************************************
//  displayStructInfo()
//  This method is executed as a result of "msdls -m -s (-a)" and
//  it displays a summary of all files in the message store.
//****************************************************************
void displayStructInfo(int subOpt,
                  ACAMS_CommitFile*   commitFile,
                  vector<MsgFile>& allDataFiles )
{    
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering displayStructInfo()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    int       noOfDataFiles = allDataFiles.size();
    LongLong  firstMessNo;
    LongLong  lastMessNo;
    int       lastMessIncorrect = 0;
    
    firstMessNo.halves[0] = (long)0;
    firstMessNo.halves[1] = (long)0;
    lastMessNo.halves[0] = (long)0;
    lastMessNo.halves[1] = (long)0;
 
    if( noOfDataFiles > 0 )
    {
        allDataFiles[0].firstMsgNum.getNumberIntoBuffer(firstMessNo.octets);
        // The sequence searches for the last non-empty file. 
        
        int lastFileIndex = noOfDataFiles;
//TODO modify this WINAPI
#if 0
        while (lastFileIndex > 0)
        {   
          lastFileIndex--;
		  ACE_HANDLE file_hand=CreateFile(allDataFiles[lastFileIndex].getFullPathName(),
		  					   GENERIC_READ,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_READONLY,
							   NULL);
            
		  if (file_hand != INVALID_HANDLE_VALUE)
		  {
			if (GetFileSize(file_hand,NULL)>0)
			{
				CloseHandle(file_hand);
				break;
            }
            CloseHandle(file_hand);
		  }
		} // End of while	
#endif
        if (allDataFiles[lastFileIndex].readLastMsg() > 0 )
        {
            allDataFiles[lastFileIndex].
                           lastMsgNum->getNumberIntoBuffer(lastMessNo.octets);
        }
        else
        {
            lastMessIncorrect = 1;
        }
    }
    else
    {
        printf( "Common part:\nNo data files available\n" );
        return;
    }

    printf( "Common part:\nfirstMessNo: {%010lu, %010lu}\n", 
             firstMessNo.halves[0], firstMessNo.halves[1] );
    
    if( lastMessIncorrect == 0 )
    {
        printf( "lastMessNo:  {%010lu, %010lu}\n", 
        lastMessNo.halves[0], lastMessNo.halves[1] );
    }
    else
    {
        printf( "lastMessNo:  { Not available }\n" );
    }

    LongLong lastAck;

    if (commitFile)
    {
        lastAck.halves[0] = (long)0;
        lastAck.halves[1] = (long)0;	    
//TODO Ask sridhar for this method
#if 0
		(commitFile->getMsgNumber()).getNumberIntoBuffer(lastAck.octets);

		if( lastAck.halves[0] > 0 || lastAck.halves[1] > 0 )
		{
			printf( "lastAckMess: {%010lu, %010lu}\nin file:     %s%s\\%s\n",
					lastAck.halves[0], lastAck.halves[1],
					MessageStoreHome.c_str(),
					ACAMS_NameOfDataFileDir.data(),
					commitFile->getFileName().data() );
		}
		else
		{
			  printf("lastAckMess: No message acknowledged yet\n");
		}
#endif
    }

    printf( "firstFile:   %05u, lastFile:    %05u, noOfFiles:    %05u\n", 
            allDataFiles[0].fileNum, 
            allDataFiles[noOfDataFiles-1].fileNum, 
            noOfDataFiles );
            
    printf( "maxNoOfMess: %05u, maxMessSize: %05u, maxNoOfFiles: %05u\n",
            MaxNoOfRecParameter, 
            MaxRecSizeParameter,
            MaxNoOfFilesParameter );
	
    if( commitFile &&
	subOpt == Option_ReadEntriesOnly &&
	lastAck.halves[0] == lastMessNo.halves[0] && 
	lastAck.halves[1] == lastMessNo.halves[1] )
    {
       printf( "\nFile part:\n" );
       printf( "Message store empty\n" );

    }
    else
    {
       printf( "\nFile part:\n" );
       for( int i = 0; i < noOfDataFiles; i++ )
       {	                                     
			LongLong firstInFile;
			firstInFile.halves[0] = (long)0;
			firstInFile.halves[1] = (long)0;	    
			allDataFiles[i].firstMsgNum.getNumberIntoBuffer( firstInFile.octets );

			if(allDataFiles[i].readLastMsg() == -1)	// The file is read
			{
				 printf( "       permission denied\n" );	// It could not be read
				 continue;
			}

			if (subOpt == Option_ReadEntriesOnly)		// Check if this file
			{						// should be displayed.
				int numbMsg = (allDataFiles[i].noOfMsgs) - 1;
				LongLong nextStartMsg;
				nextStartMsg.halves[0] = firstInFile.halves[0];
				nextStartMsg.halves[1] = firstInFile.halves[1];
				if (nextStartMsg.halves[1] > (0xFFFFFFFF - numbMsg))
				{
					nextStartMsg.halves[0] += 1;
				}
				nextStartMsg.halves[1] += numbMsg;

				if (lastAck.halves[0] == nextStartMsg.halves[0])
				{
					if (lastAck.halves[1] >= nextStartMsg.halves[1]) 
					{
						continue;
					}
				}
				else
				{
					if (lastAck.halves[0] > nextStartMsg.halves[0]) 
					{
						continue;
					}
				}
			}

			printf( "%05u: fileName: %s\n", i, allDataFiles[i].getFullPathName() );

			int eof_found = allDataFiles[i].eofFound;

			if( eof_found || allDataFiles[i].eomFound )
			{
				printf("       firstMessInFile: {%010lu, %010lu}, noOfMess: %05u\n", 
						firstInFile.halves[0], firstInFile.halves[1],
		        allDataFiles[i].noOfMsgs );
			}
			else
			{	// File might not be complete.
				printf("       firstMessInFile: {%010lu, %010lu}, "
                     "noOfMess: Unknown. File %s.\n",
                     firstInFile.halves[0], firstInFile.halves[1],
				eof_found? "corrupt" : "not complete" );
			}
            
			printf("       Unlinked: %s, MTAPFileNo: %03u\n",
                 eof_found? "Yes" : " No",
                 allDataFiles[i].fileNum );
       }
    }
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving displayStructInfo()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }


}

//*******************************************************************************
//      displayStatistics()
//*******************************************************************************
void
displayStatistics( ACAMS_StatisticsFile* statisticsFile,
                   const char*           ms,
                   const char*           site )
{
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering displayStatistics()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    ULONGLONG aLongLong=0;(void)aLongLong;
//	LongLong aLongLong;
    struct tm			createdAt = statisticsFile->getCreationTime();
      
    printf( "Statistics of message store '%s' \n", ms );
    printf( "Created: %d-%02d-%02d, %02d:%02d\n", 
	createdAt.tm_year,createdAt.tm_mon,createdAt.tm_mday,createdAt.tm_hour,createdAt.tm_min );
    
    printf( "lostMsg:    %05u\n", statisticsFile->getNumberOfLostMsg() );
    printf( "skippedMsg: %05u\n", statisticsFile->getNumberOfSkippedMsg() );
    
    aLongLong = statisticsFile->getNumberOfMsg();
   // printf( "RecMsg:    {%010lu, %010lu}\n", aLongLong.halves[0],
     //                                        aLongLong.halves[1] );
    
    aLongLong = statisticsFile->getNumberOfBytes();
    //printf( "RecBytes:  {%010lu, %010lu}\n", aLongLong.halves[0],
      //                                       aLongLong.halves[1] );
            
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving displayStatistics()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

//****************************************************************
//      loadDataFile()
//****************************************************************
bool loadDataFile(const string& fileName)
{
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering loadDataFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    // Open the file.
    char errstr[100];
#if 0	
	dfile_hand=CreateFile(fileName.c_str(),
	 					  GENERIC_READ,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_READONLY,
						  NULL);
            
	if (dfile_hand != INVALID_HANDLE_VALUE)
    {
		SizeDataFileLow32 = GetFileSize(dfile_hand,&SizeDataFileHigh32);
		if (SizeDataFileLow32<0 || SizeDataFileLow32 == 0xFFFFFFFF)
		{	
			sprintf(errstr, "Failed to read file size%s\n", fileName.c_str());
			perror(errstr);	    
			return false;
		}
    }
    else
    {	
		sprintf(errstr, "Failed to open data file %s\n", fileName.c_str());
		perror(errstr);	    
		return false;
    }
    
    // Map file in memory.
	if ((MapDataFile = CreateFileMapping(dfile_hand,
										 NULL,
										 PAGE_READONLY,
										 0,//SizeDataFileHigh32,
										 0,//SizeDataFileLow32,
										 NULL))==INVALID_HANDLE_VALUE)
	{
		// failed to map the data file in memory.
		perror("Failed to map the data file in memory");	    	
		return false;
    }
    
	// map a view of the file
	if ((file_view = MapViewOfFile(MapDataFile,FILE_MAP_READ,0,0,0))==NULL)
	{
		// failed to make a view of the data file in memory.
		perror("Failed to map a view of the data file in memory");	
		return false;
	}
#endif
	dfile_hand = ACE_OS::open(fileName.c_str(), O_RDONLY);

	if (dfile_hand == -1) 
	{	
		sprintf(errstr, "Failed to open data file %s\n", fileName.c_str());
                //perror(errstr);
	if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "Leaving loadDataFile() with error Failed to open data file %s\n", fileName.c_str());
                ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }

                return false;		
	}
		
	//void *map;

	file_view = (mmap(0, FILESIZE, PROT_READ , MAP_SHARED, dfile_hand, 0));
	if (file_view == MAP_FAILED) 
	{
		file_view = 0;
		//perror("Error mmapping the file");
        if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s","Leaving loadDataFile() with error in mmapping the file");
                ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }

		return false;
	}
    

   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving loadDataFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }



    return true;
}

//*******************************************************************************
//      closeDataFile()
//*******************************************************************************
void closeDataFile()
{
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering closeDataFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    if (dfile_hand != ACE_INVALID_HANDLE)
    {
		if (MapDataFile != ACE_INVALID_HANDLE)
		{
#if 0
			if (!UnmapViewOfFile(file_view))
			{
				perror("Failed to unmap data file");
			}
			CloseHandle(MapDataFile);      
			MapDataFile = ACE_INVALID_HANDLE;
#endif
		munmap((file_view), FILESIZE);
		close(dfile_hand);	
		}
	
		/*if (!CloseHandle(dfile_hand))
		{
			perror("Failed to close data file");
		}*/
	
		dfile_hand = ACE_INVALID_HANDLE;
    }
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving closeDataFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

//*******************************************************************************
//      loadStructInfo()
//*******************************************************************************
bool loadStructInfo(ACAMS_CommitFile*& commitFile,
                   vector<MsgFile>& allDataFiles )
{
   if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering loadStructInfo()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	char errstr[100];

    string commitFileName(MessageStoreHome);
    string dataDirectoryName(MessageStoreHome); 
	dataDirectoryName += ACAMS_NameOfDataFileDir;
	commitFileName += ACAMS_NameOfCommitFile;
 
#if 0 
	HANDLE dir =CreateFile(dataDirectoryName.c_str(),
	 		 			   GENERIC_READ,
						   FILE_SHARE_READ | FILE_SHARE_WRITE,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_READONLY | FILE_FLAG_BACKUP_SEMANTICS,
						   NULL);
            
    if (dir == INVALID_HANDLE_VALUE)
    { 
        sprintf(errstr,"Failed to read directory %s\n",dataDirectoryName.c_str()); 
        perror(errstr); 
        return false;
    }
#endif
ACE_stat checking;
//  int result = 0;

  if (ACE_OS::stat (dataDirectoryName.c_str(), &checking) == -1 )
    {
	sprintf(errstr,"Failed to read directory %s\n",dataDirectoryName.c_str());
        //perror(errstr);
	if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "Leaving loadStructInfo() with Failed to read directory %s\n",dataDirectoryName.c_str());
                ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }

        return false;
    }

 
	// See if there really exists a commit file. If so, create
	// such an object.
#if 0
	HANDLE temphand =CreateFile(commitFileName.c_str(),
	 		 					GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_READONLY,
								NULL);
          
    if (temphand != INVALID_HANDLE_VALUE)
    {
        CloseHandle(temphand);
        commitFile = new ACAMS_CommitFile(0, commitFileName);
    }
    else
    {
        sprintf(errstr, "Failed to read commit file %s\n", commitFileName.data());
        perror(errstr);
		return false;
    }
#endif
//ACE_stat checking;
  //int result = 0;

  if (ACE_OS::stat (commitFileName.c_str(), &checking) == -1 )
    {
        sprintf(errstr,"Failed to read commit file %s\n", commitFileName.data());
        //perror(errstr);
        if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "Leaving loadStructInfo() with Failed to read commit file %s\n", commitFileName.data());
                ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }

        return false;
    }
    else
	{
	commitFile = new ACAMS_CommitFile(0, commitFileName);
	}
  
 
#if 0
	WIN32_FIND_DATA currentEntry;
	string dotfile=dataDirectoryName + "\\*";
	HANDLE search_hand = FindFirstFile(dotfile.c_str(),&currentEntry);
	if (search_hand == INVALID_HANDLE_VALUE)
	{
        sprintf(errstr, "Failed to read directory %s\n", dataDirectoryName.data()); 
        perror(errstr); 
        return false;
    }

	// Scan through the directory.
    do
    {
        if( strcmp( currentEntry.cFileName, "." ) == 0 ||
            strcmp( currentEntry.cFileName, ".." ) == 0)
        {
            continue;
        }
		// We have a good filename. Create a MsgFile instance and
		// put it in the vector.
		allDataFiles.push_back(MsgFile(currentEntry.cFileName));
    } while( FindNextFile(search_hand,&currentEntry ));
#endif

	DIR *dir;
	class dirent *ent;
	class stat st;
	dir = opendir(dataDirectoryName.c_str()); 
	while ((ent = readdir(dir)) != NULL) 
	{
		const string file_name = ent->d_name;
		const string full_file_name = (const string)dataDirectoryName.c_str() + "/" + file_name;
		if (file_name[0] == '.') 
		{
			continue;
		}
		if (stat(full_file_name.c_str(), &st) == -1) 
		{
			continue;
		}
		const bool is_directory = (st.st_mode & S_IFDIR) != 0;
		if (!is_directory)
		{
			continue;
		}
//TODO check push_back()
//		allDataFiles.push_back(MsgFile(file_name); 
 	}
	closedir(dir); 


	sort(allDataFiles.begin(),allDataFiles.end(),less<MsgFile>());
	
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving loadStructInfo()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
 
    return true;
}

//*******************************************************************************
//      loadStatisticsFile()
//*******************************************************************************
bool loadStatisticsFile(ACAMS_StatisticsFile*& /*statisticsFile */)
{
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering loadStatisticsFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    string              statisticsFileName(MessageStoreHome);
//    ACE_HANDLE              temphand = ACE_INVALID_HANDLE;

    statisticsFileName+=ACAMS_NameOfStatisticsFile;
 
    // See if there really exists a statistics file. If so, create
    // such an object.
    //
#if 0
    if ((temphand = CreateFile(statisticsFileName.data(),
	 		 					GENERIC_READ,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_READONLY,
								NULL)) != INVALID_HANDLE_VALUE)
    {
        CloseHandle(temphand);
        statisticsFile = new ACAMS_StatisticsFile(  0, statisticsFileName );
    }
    else
    {
        char errstr[100];
        sprintf(errstr, "Failed to read statistics file %s\n",
			statisticsFileName.data() );
        perror(errstr);
        return false;
    }
#endif

ACE_stat checking;
  //int result = 0;

  if (ACE_OS::stat (statisticsFileName.data(), &checking) == -1 )
    {
	char errstr[100];
        sprintf(errstr, "Failed to read statistics file %s\n",
                        statisticsFileName.data() );
        //perror(errstr);
        if( ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "Leaving loadStatisticsFile()with error Failed to read statistics file %s\n", statisticsFileName.data() );
                ACS_ACA_CMD_MSDLS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }

        return false;

    }
        if( ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving loadStatisticsFile()");
                ACS_ACA_CMD_MSDLS_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

    return true;
}


//*******************************************************************************
//      usage()
//*******************************************************************************
void usage()
{
    printf("Usage:\n");
    printf("msdls -m name -s site -d[-t|-n] file\n");
    printf("                    Show the contents of a data file\n");
    printf("                    -t: Only the first 16 bytes are printed\n");
    printf("                    -n: Only the record number of stored records\n");
    printf("                        are printed\n");
    printf("msdls -m name -s site [-a]\n");
    printf("                    Show the contents of a message store\n");
    printf("                    -a: Show all files in message store also the read ones\n");
    printf("msdls -m name -s site -S\n");
    printf("                    Show statistics about the message store\n");
    exit(2);
}

bool isOption(char* par)
{
    for (int i = 0; i < NoOfOptions; i++)
    {
        if (strcmp(par, Options[i]) == 0)
		{
			return true;
		}
    }

    return false;
}
#if 0
//*******************************************************************************
//      checkNodeState(void)
//*******************************************************************************
unsigned short checkNodeState(void)
{
	HCLUSTER	clusterH = 0;
	HGROUP		groupH = 0;
	
    clusterH=OpenCluster(NULL);
	if (clusterH==0)  // unable to open cluster
	{
		if(clusterH) CloseCluster(clusterH);
		if(groupH) CloseClusterGroup(groupH);
		return 1; //We regard ourself as active.
	}

	groupH=OpenClusterGroup(clusterH, L"Cluster Group");
	if (groupH==0)   // unable to open clustergroup
	{ 
		if(clusterH) CloseCluster(clusterH);
		if(groupH) CloseClusterGroup(groupH);
		return 1; //We regard ourself as active.
	}
    
	unsigned int nodeState;
	DWORD	namelen=64;
	LPWSTR	currentNode=new WCHAR[namelen];
	GetComputerNameW(currentNode,&namelen);
	
	namelen=64;
	LPWSTR	activeNode=new WCHAR[namelen];
	DWORD	status=GetClusterGroupState(groupH,activeNode,&namelen);
	if (status==ClusterGroupStateUnknown)   // Unable to get clustergroup status
	{	
		if(clusterH) CloseCluster(clusterH);
		if(groupH) CloseClusterGroup(groupH);
		delete[] activeNode;
		delete[] currentNode;
		return 1; //We regard ourself as active.
	}
	
	if (wcscmp(activeNode,currentNode)==0)
	{
		// This node is active
		nodeState = 1;
	}
	else
	{
		// This node is Not active
		nodeState = 0;
	}

	if(clusterH) CloseCluster(clusterH);
	if(groupH) CloseClusterGroup(groupH);
	delete[] activeNode;
	delete[] currentNode;
	
	return nodeState;
}
#endif
//*******************************************************************************
//      main()
//*******************************************************************************
ACE_INT32 main(ACE_INT32 argc, ACE_TCHAR* argv[])
{
	ACE_TCHAR   messageStore[32];
    ACE_TCHAR   site[32];

    ACE_TCHAR   inputFileName[128]="noFile";

    ACE_INT32    collectedOptions[NoOfOptions]={0,0,0,0,0,0,0};

    Option option=Option_None;
    Option subOption = Option_None;
/*
	AP_InitProcess("msdls",AP_COMMAND);
	AP_SetCleanupAndCrashRoutine("msdls",NULL);
*/
        //Create a PRC object.
        ACS_PRC_API prcObj;

        //Check if node state is active or not.
        int nState = prcObj.askForNodeState();

        if( nState != 1)        //Node is not active. 1-->active
        {
		cout<<"Only allowed from active AP node\n\n"<<endl;
		return 0;                

        }
#if 0
	if (checkNodeState()==0)
	{ 
        printf("Only allowed from active AP node\n\n");
        return 0;
    }
#endif
    // Check for 'msdls' only. Fill in -m and -s parameters
    if (argc == 1)
    {
/*
       ACS_PHA_NamedTable msdTables("ACS/CXC1371131", "ACS_ACABIN_MessageStores");
       ACS_PHA_Parameter<ACAMS_Parameters> tableValue;
	   list<string> msList;
	   char msStr[128];
       while(tableValue.get(msdTables) == ACS_PHA_PARAM_RC_OK)
       { 
          strcpy(site, tableValue.data().site);
          strcpy(messageStore, tableValue.data().name);
          sprintf(msStr,"%-15s  %s\n", site, messageStore);
		  msList.push_front(msStr);

       } 
       if (msList.size() > 1)
       { 
		  printf("Existing Message Stores\n\n");
          printf("Site:            MS:\n");
		  while (msList.size())
		  {
             printf("%s",msList.front().c_str());
			 msList.pop_front();
		  }
          printf("\n\n");
          usage();
          return 0;
       }
	   else	if (msList.size() == 1)
	   {
		  option = Option_Struct;
		  subOption = Option_ReadEntriesOnly;
	   }
	   else
	   {
		  printf("Could not find any entries in parameter file\n");
          exit(1);
	   }
*/
	cout<<"Executing msdls command"<<endl;
    }
	else
	{ // there are options specified.

  	   // Collect all options.
	   for (ACE_INT32 par = 1; par < argc; par++)
	   {
			for (int i = 0; i < NoOfOptions; i++)
			{
				if (strcmp(argv[par], Options[i]) == 0)
				{
					// Store position of this option.
					collectedOptions[i] = par;
				}
			}
		}

		// Check if valid combination.
		bool foundComb=false;
		int validComb=-1;
		for (int i = 0; i < NoOfCombinations; i++)
		{
			foundComb=true;
			for (int j = 0; j < NoOfOptions; j++)
			{
				if ((ValidOptionCombinations[i][j] && !collectedOptions[j])
				|| (!ValidOptionCombinations[i][j] && collectedOptions[j]))
				{
					foundComb=false;
					break;
				}
			}
			if (foundComb)
			{
				validComb=i;
				break;
			}
		}
  
		if (!foundComb)	// No matching valid combination was found
		{
			usage();
		}

		if (argc != ValidNoOfParams[validComb])	// Check if the correct number of
		{					                    // arguments was provided.
			usage();
		}
  
		// now collect argument for options needing one;
		// First, we must have message store name.
		if (!isOption(argv[collectedOptions[Option_MessageStore] + 1]))
		{
			strcpy(messageStore,argv[collectedOptions[Option_MessageStore] + 1]);
		}
		else
		{
			usage();
		}
 
		// We also must have name of site.
		if (!isOption(argv[collectedOptions[Option_Site] + 1]))
		{
			strcpy(site,argv[collectedOptions[Option_Site] + 1]);
		}
		else
		{
			usage();
		}

		// Check if "-d" was given;
		// If so, check "-t" and "-n" and find data file name.
		if (collectedOptions[Option_Data] > 0)
		{	
			option = Option_Data;
			subOption = Option_None;

			// Check if "-t" was given
			if (collectedOptions[Option_DataTrunc] > 0)
			{
				subOption = Option_DataTrunc;
			}    		
			else if (collectedOptions[Option_DataNumberOnly] > 0)
			{  // Check if "-n" was given
				subOption = Option_DataNumberOnly;
			}    

			for (int i= collectedOptions[Option_Data] + 1;i<argc;i++)
			{
				if ((i!=collectedOptions[Option_DataNumberOnly]) &&
					(i!=collectedOptions[Option_DataTrunc]))
				{
					strcpy(inputFileName,argv[i]);
					break;
				}
			}
			if (strcmp(inputFileName,"noFile")==0)
			{
				usage();
			}
		}
		else
		{
			// Check if "-S" was given.
			if (collectedOptions[Option_Statistics] > 0)
			{
				option = Option_Statistics;
			}
			else // Structure info is going to be displayed
			{
				option = Option_Struct;

				// Check if "-a" was given
				if (collectedOptions[Option_ReadEntriesOnly] > 0)
				{
					subOption = Option_None;
				}
				else
				{
					subOption = Option_ReadEntriesOnly;
				}
			}
		}
	}

	// Get parameters for the actual message store.
	// 'MessageStoreHome' will be filled in now...
    if( readParameters(site, messageStore) != 0 )
    {
        exit( 1 );
    }

	// Must check if the Message Store really exists.
#if 0
	HANDLE dir = INVALID_HANDLE_VALUE;
	if ((dir = CreateFile(MessageStoreHome.c_str(),
	 		 		      GENERIC_READ,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_READONLY | FILE_FLAG_BACKUP_SEMANTICS,
						  NULL)) == INVALID_HANDLE_VALUE)
    {
		printf("Could not find or access message store '%s' on site '%s'\nSearch path was %s\n Fault %d\n",
	       messageStore,
	       site,
	       MessageStoreHome.c_str(),
		   GetLastError());
        exit(1);
    }
#endif

	ACE_stat checking;
  //int result = 0;
//TODO remove below two lines and keep MessageStoreHome.c_str() in future.
	ACE_TCHAR new_file[MAXPATHLEN];
ACE_OS::strcpy (new_file, ACE_TEXT ("rename_test_new"));


  if (ACE_OS::stat (new_file, &checking) == -1)
    {
      cout<<"Could not find or access message store"<<messageStore<<"  on site"<<site<<" \nSearch path was "<<MessageStoreHome.c_str()<<" \n Fault "<<" \n"<<endl;//GetLastError()<<" \n"<<endl;
      exit(1);
    }

    // Now it's time to do the job...
    switch (option)
    {
    case Option_Data:
    {
        string dataFileName(MessageStoreHome);

        dataFileName += ACAMS_NameOfDataFileDir;
        dataFileName += "\\";
        dataFileName += inputFileName;
 
		if (loadDataFile(dataFileName))
		{
			displayData(subOption, dataFileName);
		}
		closeDataFile();
		break;

    }
    case Option_Struct:
    {
        ACAMS_CommitFile* commitFile = 0;
        vector<MsgFile> allDataFiles;
        
		if (loadStructInfo(commitFile, allDataFiles))
		{
			displayStructInfo(subOption, commitFile, allDataFiles);
		}
		if (commitFile) 
		{
			delete commitFile;
		}
		break;

    }
    case Option_Statistics:
    {
        ACAMS_StatisticsFile* statisticsFile = 0;

		if (loadStatisticsFile(statisticsFile))
		{
			displayStatistics(statisticsFile, messageStore, site);
		}
		if (statisticsFile) 
		{
			delete statisticsFile;
		}
		break;

    }
    default:;
    }

    return 0;
}

