
/*=================================================================== */
   /**
   @file acs_chb_cp_object.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_cp_objectlist.h module

   @version 1.0.0

   */
   /*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/01/2011   XNADNAR   Initial Release
   */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_chb_cp_object.h"
#include "acs_chb_cp_object_implementer.h"

static const char* tempDebug="ACS_CHB_CPObject_DebugTrace";
static const char* tempError="ACS_CHB_CPObject_ErrorTrace";
static const char* C512="C512";
ACS_TRA_trace ACS_CHB_CPObject_DebugTrace(tempDebug, C512);
ACS_TRA_trace ACS_CHB_CPObject_ErrorTrace(tempError, C512);
/*===================================================================
   ROUTINE: ACS_CHB_CP_object
=================================================================== */
ACS_CHB_CP_object::ACS_CHB_CP_object(CP_object Name)
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object constructor");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object constructor");
  myCHB_cp_Object_Implementation = new ACS_CHB_CP_object_Implementer(Name);
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object constructor");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Leaving ACS_CHB_CP_object constructor");
}//End of ACS_CHB_CP_object


/*===================================================================
   ROUTINE: ~ACS_CHB_CP_object
=================================================================== */
ACS_CHB_CP_object::~ACS_CHB_CP_object()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object Destructor");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object Destructor");
  delete myCHB_cp_Object_Implementation;
  myCHB_cp_Object_Implementation = 0;
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object Destructor");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Leaving ACS_CHB_CP_object Destructor");
}//End of ~ACS_CHB_CP_object


/*===================================================================
   ROUTINE: connect
=================================================================== */
ACS_CHB_returnType ACS_CHB_CP_object::connect(int ReTries,int TimeToWait)
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::connect()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::connect()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->connect()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->connect()");
  return myCHB_cp_Object_Implementation->connect(ReTries, TimeToWait);
}//End of connect


/*===================================================================
   ROUTINE: disconnect
=================================================================== */
void ACS_CHB_CP_object::disconnect()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::disconnect()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::disconnect()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->disconnect()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->disconnect()");
  myCHB_cp_Object_Implementation->disconnect();
}//End of disconnect


/*===================================================================
   ROUTINE: get_status
=================================================================== */
State ACS_CHB_CP_object::get_status()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::get_status()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::get_status()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->get_status()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->get_status()");
  return myCHB_cp_Object_Implementation->get_status();
}//End of get_status


/*===================================================================
   ROUTINE: get_fileDescriptor
=================================================================== */
int ACS_CHB_CP_object::get_fileDescriptor()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::get_fileDescriptor()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::get_fileDescriptor()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->get_fileDescriptor()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->get_fileDescriptor()");
  return myCHB_cp_Object_Implementation->get_fileDescriptor();
}//End of get_fileDescriptor


/*===================================================================
   ROUTINE: get_name
=================================================================== */
char *ACS_CHB_CP_object::get_name()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::get_name()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::get_name()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->get_name()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->get_name()");
  return myCHB_cp_Object_Implementation->get_name();
}//End of get_name


/*===================================================================
   ROUTINE: get_value
=================================================================== */
char *ACS_CHB_CP_object::get_value()
{
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object::get_value()");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Entering ACS_CHB_CP_object::get_value()");
  if( ACS_CHB_CPObject_DebugTrace.ACS_TRA_ON())
  {
      char traceBuffer[256];
      memset(&traceBuffer, 0, sizeof(traceBuffer));
      sprintf(traceBuffer, "%s", "Calling myCHB_cp_Object_Implementation->get_value");
      ACS_CHB_CPObject_DebugTrace.ACS_TRA_event(1, traceBuffer);
  }
  //DEBUG(0,"%s","Calling myCHB_cp_Object_Implementation->get_value");
  return myCHB_cp_Object_Implementation->get_value();
}//End of get_value





