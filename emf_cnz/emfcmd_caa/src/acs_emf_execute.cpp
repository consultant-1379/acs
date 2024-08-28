//******************************************************************************
//
//  NAME
//     ACS_EMF_Execute.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2005. All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
//  DESCRIPTION 
//     -
// 	 
//  DOCUMENT NO
//	    190 89-CAA 109 0260
//
//  AUTHOR 
//     2005-04-01 by EAB/UZ/DH UABCHSN
//
//  SEE ALSO 
//     -
//
//******************************************************************************
#include <stdio.h>
#include <acs_emf_execute.h>
#include <ace/ACE.h>


//============================================================================
// Constructor
//============================================================================
ACS_EMF_Execute::ACS_EMF_Execute( ) : m_dwLastError(0)
{

}
//============================================================================
// Destructor
//============================================================================
ACS_EMF_Execute::~ACS_EMF_Execute()
{

}
//============================================================================
// GetErrorText
// Returns the error text from a error Code
//============================================================================
const ACE_TCHAR* ACS_EMF_Execute::GetErrorText(const ACE_INT32 errorCode)
{
	static ACE_TCHAR szText[128] = ACE_TEXT("");
	switch (errorCode)
	{
	case 2:
		ACE_OS::strcpy(szText, ACE_TEXT("Incorrect usage\nUsage:emfcopy -m TODVD [-l label] [-o [-f]] [-n] source...\nemfcopy -m FROMDVD -a directory \n      emfcopy -m FROMDVD [-o [-f]] source... destination...\n\n"));
		break;
	case 3:
		ACE_OS::strcpy(szText, ACE_TEXT("File not found"));
		break;
	case 4:
		ACE_OS::strcpy(szText, ACE_TEXT("Folder does not exist"));
		break;
	case 8:
		ACE_OS::strcpy(szText, ACE_TEXT("DVD empty, copy is not possible"));
		break;
	case 9:
		ACE_OS::strcpy(szText, ACE_TEXT("Invalid source name"));
		break;
	case 14:
		ACE_OS::strcpy(szText, ACE_TEXT("Operation is already in progress"));
		break;
	case 19:
		ACE_OS::strcpy(szText, ACE_TEXT("Operand source too long"));
		break;
	case 21:
		ACE_OS::strcpy(szText, ACE_TEXT("No external media board available"));
		break;
	case 22:
		ACE_OS::strcpy(szText, ACE_TEXT("No Media in DVD Drive"));
		break;
	case 23:
		ACE_OS::strcpy(szText, ACE_TEXT("DVD drive is busy"));
		break;
	case 25:
		ACE_OS::strcpy(szText, ACE_TEXT("Appending data to this media is not supported"));
		break;
	case 29:
		ACE_OS::strcpy(szText, ACE_TEXT("Assigning DVD drive is not supported on this platform"));
		break;
	case 31:
		ACE_OS::strcpy(szText, ACE_TEXT("The option is not valid for this platform"));
		break;
	case 34:
		ACE_OS::strcpy(szText, ACE_TEXT("Overwriting data to this media is not supported"));
		break;
	case 35:
		ACE_OS::strcpy(szText, ACE_TEXT("Too many Destinations specified"));
		break;
	case 36:
		ACE_OS::strcpy(szText, ACE_TEXT("Folder is not empty"));
		break;
	case 56:
		ACE_OS::strcpy(szText, ACE_TEXT("Disc Media error"));
		break;
	case 117:
		ACE_OS::strcpy(szText, ACE_TEXT("Unable to connect to server"));
		break;
	default:
		ACE_OS::strcpy(szText, ACE_TEXT("Other error"));
	}
	return szText;
}
//============================================================================
// GetResultCodeText
// Returns the text from a result code
//============================================================================
const ACE_TCHAR* ACS_EMF_Execute::GetResultCodeText(const ACE_INT32 dwResultCode,
													ACE_INT32* pExitCode)
{
   static ACE_TCHAR szText[128] = ACE_TEXT("");
   int nExitCode = 0;

   switch (dwResultCode)
   {
   case EMF_RC_UNKNOWNCMDTYPE:
      ACE_OS::strcpy(szText, ACE_TEXT("Unknown command type"));
      break;

   case EMF_RC_OPERATIONINPROGRESS:
      ACE_OS::strcpy(szText, ACE_TEXT("Operation already in progress"));
      nExitCode = 14;
      break;

   case EMF_RC_SERVERNOTRESPONDING:
      ACE_OS::strcpy(szText, ACE_TEXT("ACS_EMF_Server not responding"));
      nExitCode = 5;
      break;

   case EMF_RC_FILENOTFOUND:
      ACE_OS::strcpy(szText, ACE_TEXT("File not found"));
      nExitCode = 3;
      break;

   case EMF_RC_PATHNOTFOUND:
      ACE_OS::strcpy(szText, ACE_TEXT("Folder does not exist"));
      nExitCode = 4;
      break;

   case EMF_RC_FILEALREADYEXIST:
      ACE_OS::strcpy(szText, ACE_TEXT("File already exist"));
      nExitCode = 16;
      break;

   case EMF_RC_FILECREATEERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to create file"));
      break;

   case EMF_RC_FILEREADERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to read file"));
      break;

   case EMF_RC_FILEWRITEERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to write to file"));
      break;

   case EMF_RC_CRYPTHASHERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to create hash object for verification"));
      break;

   case EMF_RC_HASHCHECKSUMDIFFER:
      ACE_OS::strcpy(szText, ACE_TEXT("Hash checksum differ"));
      break;
/*
   case EMF_RC_TAPEDRIVENOTFOUND:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape device not found"));
      break;

   case EMF_RC_DVDDRIVEBUSY:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape drive is busy"));
      nExitCode = 11;
      break;

   case EMF_RC_TAPEDRIVEERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape drive error"));
      break;

   case EMF_RC_TAPEMEDIAERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Error on the tape media"));
      break;

   case EMF_RC_TAPEMEDIAWRITEPROTECTED:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape medium is write protected"));
      nExitCode = 12;
      break;

   case EMF_RC_TAPEMEDIAFORMATTINGERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape media formatting error"));
      break;

   case EMF_RC_TAPEMEDIAINVALIDFORMAT:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape medium not in valid TAR format"));
      nExitCode = 13;
      break;

//   case EMF_RC_NOTAPEINDRIVE:
//      ACE_OS::strcpy(szText, ACE_TEXT("No tape in drive"));
//      nExitCode = 10;
//      break;

   case EMF_RC_TAPEMEDIAEOF:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape position has reached end-of-file"));
      break;

   case EMF_RC_TAPEMEDIAINVALIDBLOCKSIZE:
      ACE_OS::strcpy(szText, ACE_TEXT("Tape media has invalid block size"));
      break;

   case EMF_RC_TAPEMEDIAREADERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to read from tape media"));
      nExitCode = 28;
      break;

   case EMF_RC_TAPEMEDIAWRITEERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to write to tape media"));
      break;
      */

   case EMF_RC_INVALIDFILENAME:
      ACE_OS::strcpy(szText, ACE_TEXT("Invalid file name"));
      nExitCode = 9;
      break;

   case EMF_RC_INVALIDPATHNAME:
      ACE_OS::strcpy(szText, ACE_TEXT("Invalid path name"));
      nExitCode = 9;
      break;

   case EMF_RC_INVALIDLABELNAME:
      ACE_OS::strcpy(szText, ACE_TEXT("Invalid label name"));
      nExitCode = 10;
      break;

   case EMF_RC_FILENAMETOOLONG:
      ACE_OS::strcpy(szText, ACE_TEXT("Operand file too long"));
      nExitCode = 6;
      break;

   case EMF_RC_FILEPATHTOOLONG:
      ACE_OS::strcpy(szText, ACE_TEXT("Operand path too long"));
      nExitCode = 7;
      break;

   case EMF_RC_SOURCETOOLONG:
      ACE_OS::strcpy(szText, ACE_TEXT("Operand source too long"));
      nExitCode = 17;
      break;

   case EMF_RC_LABELNAMETOOLONG:
      ACE_OS::strcpy(szText, ACE_TEXT("Argument label too long"));
      nExitCode = 26;
      break;

   case EMF_RC_FILEEMPTY:
      ACE_OS::strcpy(szText, ACE_TEXT("File empty, copy not possible"));
      nExitCode = 8;
      break;

   case EMF_RC_INVALIDARGUMENT:
      ACE_OS::strcpy(szText, ACE_TEXT("Invalid argument"));
      break;

   case EMF_RC_PERMISSIONSDENIED:
      ACE_OS::strcpy(szText, ACE_TEXT("Permission denied"));
      nExitCode = 15;
      break;

   case EMF_RC_UNKNOWNNODENAME:
      ACE_OS::strcpy(szText, ACE_TEXT("Unknown node name"));
      nExitCode = 6;
      break;

   case EMF_RC_SERVERONACTIVENOTRESPONDING:
      ACE_OS::strcpy(szText, ACE_TEXT("ACS_EMF_Server on the active node not responding"));
      nExitCode = 17;
      break;

   case EMF_RC_FILETOLARGE:
      ACE_OS::strcpy(szText, ACE_TEXT("File to large for TAR file format"));
      nExitCode = 18;
      break;

   case EMF_RC_HWCOMPNOTSUPPORTED:
      ACE_OS::strcpy(szText, ACE_TEXT("Hardware data compression not supported"));
      break;

   case EMF_RC_CHANGEHWCOMPNOTSUPPORTED:
      ACE_OS::strcpy(szText, ACE_TEXT("Changing hardware data compression not supported"));
      nExitCode = 19;
      break;

   case EMF_RC_CHANGEHWCOMPBOPONLY:
      ACE_OS::strcpy(szText, ACE_TEXT("Changing hardware data compression not allowed in append mode"));
      nExitCode = 20;
      break;

   case EMF_RC_DVDDEVICENOTFOUND:
      ACE_OS::strcpy(szText, ACE_TEXT("No external media board available"));
      nExitCode = 21;
      break;

   case EMF_RC_DVDNOTASSIGNEDONTHISNODE:
      ACE_OS::strcpy(szText, ACE_TEXT("DVD device not assigned to this node"));
      nExitCode = 26;
      break;

   case EMF_RC_NOMEDIAINDRIVE:
      ACE_OS::strcpy(szText, ACE_TEXT("No media in DVD drive"));
      nExitCode = 22;
      break;

   case EMF_RC_DVDDEVICEBUSY:
      ACE_OS::strcpy(szText, ACE_TEXT("DVD drive is busy"));
      nExitCode = 23;
      break;

   case EMF_RC_DISCNOTAPPENDABLE:
      ACE_OS::strcpy(szText, ACE_TEXT("Appending data to this media is not supported"));
      nExitCode = 25;
      break;

   case EMF_RC_MEDIANOTOVERWRITABLE:
      ACE_OS::strcpy(szText, ACE_TEXT("Overwriting data to this media is not supported"));
      nExitCode = 34;
      break;
   
   case EMF_RC_DISCNOTERASABLE:
      ACE_OS::strcpy(szText, ACE_TEXT("Media is not writable"));
      nExitCode = 24;
      break;

   case EMF_RC_DISCMEDIAERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Corrupted disc media"));
      nExitCode = 33;
      break;

   case EMF_RC_COMMANDNOTVALIDONTHISPLATFORM:
      ACE_OS::strcpy(szText, ACE_TEXT("The command is not valid for this platform"));
      nExitCode = 30;
      break;

   case EMF_RC_OPTIONNOTVALIDONTHISPLATFORM:
      ACE_OS::strcpy(szText, ACE_TEXT("The option is not valid for this platform"));
      nExitCode = 31;
      break;

   case EMF_RC_IMAGEFILEALREADYEXIST:
      ACE_OS::strcpy(szText, ACE_TEXT("Image file already exist"));
      nExitCode = 32;
      break;

   case EMF_RC_GETDVDDEVICEFAILED:
      ACE_OS::strcpy(szText, ACE_TEXT("Failed to assign DVD drive"));
      nExitCode = 27;
      break;

   case EMF_RC_OPERATIONCANCELLEDBYUSER:
      ACE_OS::strcpy(szText, ACE_TEXT("Operation cancelled by user"));
      nExitCode = 0;
      break;

   case EMF_RC_ENDEVENTSIGNALED:
      ACE_OS::strcpy(szText, ACE_TEXT("Operation cancelled by ACS_EMF_Server"));
      nExitCode = 21;
      break;

   case EMF_RC_OTHERERROR:
      ACE_OS::strcpy(szText, ACE_TEXT("Other error"));
      break;

   case EMF_RC_NOTACTIVENODE:
	   ACE_OS::strcpy(szText, ACE_TEXT("The command can not be executed on the passive node"));
	   nExitCode = 41;
	   break;

   default:
      ACE_OS::strcpy(szText, ACE_TEXT("Other error"));
   }

   if (pExitCode)
      *pExitCode = nExitCode;

   return szText;
}



