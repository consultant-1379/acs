/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/
/**
*
* @file ACS_XBRM_MMLMessageHandler.cpp
*
* @brief
* ACS_XBRM_MMLMessageHandler Class for MML session event handler
*
* @details
* Implementation of ACS_XBRM_MMLMessageHandler class to handle events for MML commands
*
* @author XHARBAV
*
-------------------------------------------------------------------------*/
/*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-07-11  XHARBAV  Created First Revision
 *
 ****************************************************************************/
#include "ACS_XBRM_MMLMessageHandler.h"
#include "ACS_XBRM_MMLEventHandler.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include <string>

using namespace std;
ACS_XBRM_TRACE_DEFINE(ACS_XBRM_MMLMessageHandler);
const char *mmlSybueCommand = "sybue;";
const char *mmlSybupCommand = "sybup:file;";
const char *mmlSybuiCommand = "sybui;";
const char *mmlioexpCommand = "ioexp;";
//----------------------------------------------------------------------------
ACS_XBRM_MMLMessageHandler::ACS_XBRM_MMLMessageHandler() : m_mmlEventHandler(0)
{
   // Empty
   backupSwLevel = "";
   backupErrorCode = "";
   sigTermReceived = false;
   ACS_XBRM_TRACE_FUNCTION;
}

std::string ACS_XBRM_MMLMessageHandler::getBackupSwLevel()
{
   return backupSwLevel;
}

std::string ACS_XBRM_MMLMessageHandler::getBackupErrorCode()
{
   return backupErrorCode;
}

std::string ACS_XBRM_MMLMessageHandler::getBackupRelVolume()
{
   return backupRelVolume;
}

//----------------------------------------------------------------------------
ACS_XBRM_MMLMessageHandler::~ACS_XBRM_MMLMessageHandler()
{
   // Empty
   ACS_XBRM_TRACE_FUNCTION;
}

//----------------------------------------------------------------------------
int ACS_XBRM_MMLMessageHandler::open(const char *cpName)
{

   // Save a reference to the options
   ACS_XBRM_TRACE_FUNCTION;
   int res = 0;

   char *userName = (char *)"ADMINISTRATOR";

   // Create MML Handler
   ACE_NEW_RETURN(m_mmlEventHandler, ACS_XBRM_MMLEventHandler(this), -1);
   res = m_mmlEventHandler->open(cpName, MO_NONE, userName, "", "", MS_CPEX, false, "", "", false, false, "", "");

   // Start message handler
   if (res < 0)
   {
      ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to open MML Session");
      return -1;
   }
   this->activate(THR_NEW_LWP | THR_JOINABLE, 1);
   this->wait();
   if(m_mmlEventHandler){
      delete m_mmlEventHandler;
      m_mmlEventHandler = NULL;
   }

   return 0;
}

int ACS_XBRM_MMLMessageHandler::svc()
{
   ACS_XBRM_TRACE_FUNCTION;

   bool autoDump = false;
   int restartCount = 0;
   ACE_Message_Block *mb;

   // Initial step, execute sybue
   ACE_OS::sleep(1);
   std::string mmlExecCmd = (string)mmlSybueCommand;

   m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

   bool done = false, isConnectionLost = false, isCpRestarted = false;
   int failureRetryFlag = 0, funcBusyRetryFlag = 0, sessionLocked = false;
   // ACE_Time_Value timeout;
   ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
   timeout = this->gettimeofday();
   timeout += ACE_Time_Value(900, 0);
   bool commandToBeExec = false;
   while (!done)
   {

      mb = 0;
      if (this->getq(mb, &timeout) < 0)
      {

         break;
      }
      ACS_XBRM_LOG(LOG_LEVEL_INFO, "msg type : %d", mb->msg_type());
      ACS_XBRM_TRACE_MESSAGE("msg type : %d", mb->msg_type());
      switch (mb->msg_type())
      {
      case MML_PROMPT:

         if (mb)
         {
            MCS_MML_READORDER type = (MCS_MML_READORDER)mb->rd_ptr()[0];
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "MML_Prompt, type: %d", type);
            ACS_XBRM_TRACE_MESSAGE("MML_Prompt, type: %d", type);
            switch (type)
            {

            case MR_RDDATA: // Request for command in dialogue mode
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Request for command in dialogue mode");
               ACS_XBRM_TRACE_MESSAGE("Request for command in dialogue mode");
               m_mmlEventHandler->execCmd(";");
               break;

            case MR_RDCONFIRM: // Request for confirmation order
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Request for confirmation order");
               ACS_XBRM_TRACE_MESSAGE("Request for confirmation order");
               m_mmlEventHandler->execCmd(";");
               break;

            case MR_RDCMD: // Ready to Read command
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Ready to Read command");
               ACS_XBRM_TRACE_MESSAGE("Ready to Read command");
               if (isConnectionLost && isCpRestarted)
               {
                  isConnectionLost = false;
                  isCpRestarted = false;
                  
                     ACS_XBRM_LOG(LOG_LEVEL_WARN, "Connection regained after CP restart and ready to receive command");
                     ACS_XBRM_LOG(LOG_LEVEL_WARN, "Retry execution of command : %s", mmlExecCmd.c_str());
                     ACE_OS::sleep(2);
                     int ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);
                     if (ret < 0)
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                     }
                  
               }

               break;

            case MR_UNDEF:
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "MR_UNDEF");
               ACS_XBRM_TRACE_MESSAGE("MR_UNDEF");

               break;

            case MR_RDUSER:
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "MR_RDUSER");
               ACS_XBRM_TRACE_MESSAGE("MR_RDUSER");

               break;

            case MR_RDPASSWD:
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "MR_RDPASSWD");
               ACS_XBRM_TRACE_MESSAGE("MR_RDPASSWD");

               break;
            }
            (void)mb->release();
            mb = 0;
         }
         break;

      // Handle service shutdown
      case MML_EXIT:

         ACS_XBRM_LOG(LOG_LEVEL_INFO, "Recieved shutdown event");
         ACS_XBRM_TRACE_MESSAGE("Recieved shutdown event");
         if (autoDump)
         {
            ACS_XBRM_LOG(LOG_LEVEL_WARN, "Initiate automatic dump before shutdown");
            mmlExecCmd = string(mmlSybuiCommand);

            m_mmlEventHandler->tryReconnect();
            ACE_OS::sleep(1);
            if (!sessionLocked)
            {
               int ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

               if (ret < 0)
               {
                  ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
               }
            }
            else
            {
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
               commandToBeExec = true;
            }
         }
         else
            done = true;

         if (mb)
         {
            (void)mb->release();
            mb = 0;
         }
         break;

      case MML_EVENT:
         if (mb)
         {

            MCS_MML_EVENTTYPE type = (MCS_MML_EVENTTYPE)mb->rd_ptr()[0];
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "MML Event Type : %d", type);
            ACS_XBRM_TRACE_MESSAGE("MML Event Type : %d", type);
            switch (type)
            {

            case ME_LOSTCONNECTION: // Connection to the CP has been lost

               ACS_XBRM_LOG(LOG_LEVEL_WARN, "Connection to the CP has been lost");
               isConnectionLost = true;
               m_mmlEventHandler->tryReconnect();
               break;

            case ME_CONNECTED: // CP connection has been established/regained
               ACS_XBRM_LOG(LOG_LEVEL_INFO, " CP connection has been established/regained");
               ACS_XBRM_TRACE_MESSAGE("CP connection has been established/regained");
               break;

            case ME_CP_RESTARTED: // CP has restarted
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "CP has restarted");
               isCpRestarted = true;
               m_mmlEventHandler->tryReconnect(); // TODO to be removed
               break;

            case ME_READBREAK: // Reply on read order is wanted by the CP
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Reply on read order is wanted by the CP");
               ACS_XBRM_TRACE_MESSAGE("Reply on read order is wanted by the CP");
               break;

            case ME_RELDEVICE: // The CP has released the session
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "The CP has released the session");
               ACS_XBRM_TRACE_MESSAGE("The CP has released the session");

               m_mmlEventHandler->tryReconnect();

               break;

            case ME_SHUTDOWN: // Session has been closed by the MML server, and cannot be used anymore.
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session has been closed by the MML server, and cannot be used anymore");
               backupErrorCode = "Session has been closed by the MML server";
               done = true;
               break;

            case ME_LOCKED: // 'Session Locked' Event
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "'Session locked' Event");
               sessionLocked = true;
               break;

            case ME_UNLOCKED: // 'Session Unlocked' Event
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "'Session Unlocked' Event");
               sessionLocked = false;

               m_mmlEventHandler->tryReconnect();
               ACE_OS::sleep(1);
               if (commandToBeExec)
               {

                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "Retry execution of command : %s", mmlExecCmd.c_str());

                  int ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);
                  if (ret < 0)
                  {
                     ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                  }
               }
               commandToBeExec = false;
               break;

            case ME_CLOSING: // ADH-initiated graceful session close
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "ADH-initiated graceful session close");
               backupErrorCode = "ADH-initiated graceful session close";
               done = true;
               break;

            case ME_UNAVAILABLE: // 'Session Unavailable' Event
               ACS_XBRM_LOG(LOG_LEVEL_WARN, "'Session Unavailable' Event");
               backupErrorCode = "'Session Unavailable' Event";
               done = true;
               break;

            case ME_AVAILABLE: // 'Session Available' Event
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "'Session Available' Event");
               ACS_XBRM_TRACE_MESSAGE("'Session Available' Event");
               break;
            }
            (void)mb->release();
            mb = 0;
         }
         break;

      case MML_PRINTOUT:
         MCS_MML_Printout printout;
         MCS_MML_Printoutbuf printoutbuf;
         int ret = -1;
         if (0 == m_mmlEventHandler->getDirectPrtBuffer(printoutbuf))
         {
            string PrintoutBuf = printoutbuf.getPrintoutBuf();
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "PrintoutBuf : %s", PrintoutBuf.c_str());
            ACS_XBRM_TRACE_MESSAGE("PrintoutBuf : %s", PrintoutBuf.c_str());
            if (PrintoutBuf.find("FUNCTION BUSY") != std::string::npos)
            {
               if (funcBusyRetryFlag < 3 && !sigTermReceived)
               {
                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "another backup operation ongoing, sleep for 3minutes before retry");
                  // sleep for 3 minutes
                  ACE_OS::sleep(180);

                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "Retry execution of command : %s", mmlExecCmd.c_str());
                  m_mmlEventHandler->tryReconnect();
                  ACE_OS::sleep(1);
                  if (!sessionLocked)
                  {
                     ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);
                     funcBusyRetryFlag++;
                     if (ret < 0)
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                     }
                  }
                  else
                  {
                     ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                     commandToBeExec = true;
                  }
               }
               else
               {
                  ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute %s even after %d retries", mmlExecCmd.c_str(), funcBusyRetryFlag);
                  funcBusyRetryFlag = 0;
                  done = true;
                  backupErrorCode = mmlExecCmd + ":" + PrintoutBuf;
                  break;
               }
               // done = true;
            }
            else if ((mmlExecCmd == mmlSybupCommand) && (PrintoutBuf.find("OUTPUT TO FILE") != std::string::npos))
            {

               ACS_XBRM_LOG(LOG_LEVEL_INFO, "PrintoutBuf in sybup : %s", PrintoutBuf.c_str());
               ACS_XBRM_TRACE_MESSAGE("PrintoutBuf in sybup : %s", PrintoutBuf.c_str());
               string substring = "OUTPUT TO FILE";
               size_t pos = PrintoutBuf.find(substring);
               pos = pos + substring.length() + 1;

               backupRelVolume = PrintoutBuf.substr(pos, PrintoutBuf.length());
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "CP Backup loaded in : %s", backupRelVolume.c_str());
               ACS_XBRM_TRACE_MESSAGE("CP Backup loaded in : %s", backupRelVolume.c_str());
            }
            else if ((mmlExecCmd == mmlSybueCommand) && ((PrintoutBuf.find("AUTOMATIC OUTPUT ALREADY PASSIVE") != std::string::npos) || (PrintoutBuf.find("EXECUTED") != std::string::npos)))
            {
               if (PrintoutBuf.find("AUTOMATIC OUTPUT ALREADY PASSIVE") == std::string::npos)
               {
                  autoDump = true;
               }
               failureRetryFlag = 0;
               funcBusyRetryFlag = 0;

               mmlExecCmd = string(mmlSybupCommand);
               ACE_OS::sleep(1);

               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Execution of command : %s", mmlExecCmd.c_str());
               ACS_XBRM_TRACE_MESSAGE("Execution of command : %s", mmlExecCmd.c_str());
               m_mmlEventHandler->tryReconnect();
               ACE_OS::sleep(1);
               if (!sessionLocked)
               {
                  ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                  if (ret < 0)
                  {
                     ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                  }
               }
               else
               {
                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                  commandToBeExec = true;
               }
            }
            else if ((mmlExecCmd == mmlSybuiCommand) && ((PrintoutBuf.find("NOT ACCEPTED") != std::string::npos) || (PrintoutBuf.find("EXECUTED") != std::string::npos)))
            {
               failureRetryFlag = 0;
               funcBusyRetryFlag = 0;
               autoDump = false;
               done = true;
            }
            else if ((mmlExecCmd == mmlioexpCommand) && ((PrintoutBuf.find("EXCHANGE IDENTITY DATA") != std::string::npos)))
            {
               failureRetryFlag = 0;
               funcBusyRetryFlag = 0;
               string substring = "IDENTITY", substring1 = "END";
               size_t pos = PrintoutBuf.find(substring);

               pos = pos + substring.length() + 1;

               pos = PrintoutBuf.find(substring.c_str(), pos, substring.length());
               pos = pos + substring.length() + 1;

               size_t pos1 = PrintoutBuf.find(substring1.c_str(), substring1.length());

               backupSwLevel = PrintoutBuf.substr(pos, (pos1 - pos));
               size_t i = 0;
               while (i < backupSwLevel.length())
               {
                  i = backupSwLevel.find('\n');
                  if (i != std::string::npos)
                  {
                     backupSwLevel.erase(i);
                  }
                  else
                     break;
               }
	       //Start of TR IA72790
	       std::replace_if(backupSwLevel.begin(), backupSwLevel.end(), ::ispunct, '_');
	       std::replace(backupSwLevel.begin(), backupSwLevel.end(), ' ', '_');
	       //End of TR IA72790
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "Fetching CP SwLevel  : %s", backupSwLevel.c_str());
               ACS_XBRM_TRACE_MESSAGE("Fetching CP SwLevel  : %s", backupSwLevel.c_str());

               if (autoDump)
               {
                  mmlExecCmd = string(mmlSybuiCommand);

                  ACS_XBRM_LOG(LOG_LEVEL_INFO, "Execution of command : %s", mmlExecCmd.c_str());
                  ACS_XBRM_TRACE_MESSAGE("Execution of command : %s", mmlExecCmd.c_str());
                  m_mmlEventHandler->tryReconnect();
                  ACE_OS::sleep(1);
                  if (!sessionLocked)
                  {
                     ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                     if (ret < 0)
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                     }
                  }
                  else
                  {
                     ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                     commandToBeExec = true;
                  }
               }
               else
                  done = true;
            }
            else if ((PrintoutBuf.find("NOT ACCEPTED") != std::string::npos))
            {
               if (failureRetryFlag < 3)
               {

                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "Failure while executing, retry after sleep for 3minutes");
                  // sleep for 3 minutes
                  ACE_OS::sleep(180);

                  ACS_XBRM_LOG(LOG_LEVEL_WARN, "Retry execution of command : %s", mmlExecCmd.c_str());
                  m_mmlEventHandler->tryReconnect();
                  ACE_OS::sleep(1);
                  if (!sessionLocked)
                  {
                     ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                     if (ret < 0)
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                     }
                  }
                  else
                  {
                     ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                     commandToBeExec = true;
                  }
                  failureRetryFlag++;
               }
               else
               {
                  ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute %s even after %d retries", mmlExecCmd.c_str(), failureRetryFlag);
                  failureRetryFlag = 0;
                  backupErrorCode = mmlExecCmd + ":" + PrintoutBuf;
                  if (autoDump)
                  {
                     mmlExecCmd = string(mmlSybuiCommand);

                     ACS_XBRM_LOG(LOG_LEVEL_INFO, "Execution of command : %s", mmlExecCmd.c_str());
                     ACS_XBRM_TRACE_MESSAGE("Execution of command : %s", mmlExecCmd.c_str());
                     m_mmlEventHandler->tryReconnect();
                     ACE_OS::sleep(1);
                     if (!sessionLocked)
                     {
                        ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                        if (ret < 0)
                        {
                           ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                        }
                     }
                     else
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                        commandToBeExec = true;
                     }
                  }
                  else
                     done = true;
                  break;
               }
            }
         }

         m_mmlEventHandler->getPrintout(printout, MP_ALL);

         if (printout.lineCount() > 0)
         {
            for (int i = 0; i < printout.lineCount(); i++)
            {
               ACS_XBRM_LOG(LOG_LEVEL_INFO, "PrintOut : %s",printout.line[i]);
               if ((strcmp(printout.line[i], "BACKUP INFORMATION OUTPUT") == 0) || (strcmp(printout.line[i], "CP CLUSTER BACKUP INFORMATION OUTPUT") == 0))
               {
                  bool sybupExec = false;
                  for (int j = 0; j < printout.lineCount(); j++)
                  {

                     if (strcmp(printout.line[j], "EXECUTED") == 0)
                     {
                        sybupExec = true;
                        break;
                     }
                     else if (strstr(printout.line[j], "FAULT CODE") != NULL)
                     {

                        if (failureRetryFlag < 2)
                        {

                           ACS_XBRM_LOG(LOG_LEVEL_WARN, "Failure while executing, retry after sleep for 3minutes");
                           ACE_OS::sleep(180);

                           ACS_XBRM_LOG(LOG_LEVEL_WARN, "Retry execution of command : %s", mmlExecCmd.c_str());
                           m_mmlEventHandler->tryReconnect();
                           ACE_OS::sleep(1);
                           if (!sessionLocked)
                           {
                              ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                              if (ret < 0)
                              {
                                 ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                              }
                           }
                           else
                           {
                              ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                              commandToBeExec = true;
                           }
                           failureRetryFlag++;
                        }
                        else
                        {
                           ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute %s even after %d retries", mmlExecCmd.c_str(), failureRetryFlag);
                           failureRetryFlag = 0;

                           ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Reason for failure : %s , %s", printout.line[j], printout.line[j + 1]);

                           char errorMessage[200] = "";
                           strcpy(errorMessage, mmlExecCmd.c_str());
                           strcat(errorMessage, printout.line[j]);
                           strcat(errorMessage, " ");
                           strcat(errorMessage, printout.line[j + 1]);
                           backupErrorCode = errorMessage;
                           ACS_XBRM_LOG(LOG_LEVEL_WARN, "Error : %s", backupErrorCode.c_str());
                           if (autoDump)
                           {
                              mmlExecCmd = string(mmlSybuiCommand);

                              ACS_XBRM_LOG(LOG_LEVEL_INFO, "Execution of command : %s", mmlExecCmd.c_str());
                              ACS_XBRM_TRACE_MESSAGE("Execution of command : %s", mmlExecCmd.c_str());
                              m_mmlEventHandler->tryReconnect();
                              ACE_OS::sleep(1);
                              if (!sessionLocked)
                              {
                                 ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                                 if (ret < 0)
                                 {
                                    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                                 }
                              }
                              else
                              {
                                 ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                                 commandToBeExec = true;
                              }
                           }
                           else
                              done = true;
                           break;
                        }
                     }
                  }
                  if (sybupExec)
                  {
                     mmlExecCmd = string(mmlioexpCommand);

                     ACS_XBRM_LOG(LOG_LEVEL_INFO, "Execution of command : %s", mmlExecCmd.c_str());
                     ACS_XBRM_TRACE_MESSAGE("Execution of command : %s", mmlExecCmd.c_str());
                     m_mmlEventHandler->tryReconnect();
                     ACE_OS::sleep(1);
                     if (!sessionLocked)
                     {
                        ret = m_mmlEventHandler->execCmd(mmlExecCmd.c_str(), true);

                        if (ret < 0)
                        {
                           ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to execute MML command : %s", mmlExecCmd.c_str());
                        }
                     }
                     else
                     {
                        ACS_XBRM_LOG(LOG_LEVEL_WARN, "Session is in locked state, cannot execute MML command. Wait for sometime before retry");
                        commandToBeExec = true;
                     }
                  }
                  break;
               }
            }
         }

         if (mb)
         {
            (void)mb->release();
            mb = 0;
         }
         break;
      }
   }
   if (!done)
   {
      ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Timeout occurred while wiating for MML input, aborting");
      backupErrorCode = "Internal Program Fault";
   }
   // close the API
   m_mmlEventHandler->close();

   return 0;
}

//----------------------------------------------------------------------------
// Always called after termination of svc()
//----------------------------------------------------------------------------
int ACS_XBRM_MMLMessageHandler::close(u_long)
{
   ACS_XBRM_TRACE_FUNCTION;

   // No action at the moment
   return 0;
}

void ACS_XBRM_MMLMessageHandler::stop()
{
   ACS_XBRM_TRACE_FUNCTION;

   sigTermReceived = true;
   ACE_Message_Block *mb = new ACE_Message_Block();

   mb->msg_type(MML_EXIT);

   if (putq(mb) < 0)
   {
      // Failed to forward event
      ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to forward shutdown Event");
      (void)mb->release();
   }

   if (!mb)
   {
      delete mb;
      mb = NULL;
   }
}
