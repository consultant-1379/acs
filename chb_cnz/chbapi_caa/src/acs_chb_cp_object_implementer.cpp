
/*=================================================================== */
   /**
   @file acs_chb_cp_object_implementer.cpp

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
#include <acs_chb_cp_object_implementer.h>
#include <ACS_TRA_trace.h>

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_CPObjectAPISocketStartupFailedText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_CPObjectAPISocketStartupFailedText "Socket startup failed"
/*=================================================================== */
/**
   @brief ACS_CHB_socketCreationFailureText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_socketCreationFailureText "Creation of socket failed"
/*=================================================================== */
/**
   @brief ACS_CHB_connectFailureText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_connectFailureText "Connection request to server failed"
/*=================================================================== */
/**
   @brief ACS_CHB_registerClientFailureText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_registerClientFailureText "Registration of client failed"
/*=================================================================== */
/**
   @brief ACS_CHB_readCPObjectValueFromServerFailureText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_readCPObjectValueFromServerFailureText  "Failed to read CP object value from server"
/*=================================================================== */
/**
   @brief ACS_CHB_connectionClosedByServerFailureText
 */
/*=================================================================== */
//const char*  const

#define ACS_CHB_connectionClosedByServerFailureText "Connection closed by server"
/*=================================================================== */
/**
   @brief ACS_CHB_requestCPObjectValueFromServerFailureText
 */
/*=================================================================== */
//const char*  const
#define ACS_CHB_requestCPObjectValueFromServerFailureText "Request for CP object value from server failed"
static const char* tempDebug="ACS_CHB_CPObject_Implementer_DebugTrace";
static const char* tempError="ACS_CHB_CPObject_Implementer_ErrorTrace";
static const char* C512="C512";
static ACS_TRA_trace ACS_CHB_CPObject_Implementer_DebugTrace(tempDebug, C512);
static ACS_TRA_trace ACS_CHB_CPObject_Implementer_ErrorTrace(tempError, C512);
/*===================================================================
   ROUTINE: ACS_CHB_CP_object
=================================================================== */
ACS_CHB_CP_object_Implementer::ACS_CHB_CP_object_Implementer(CP_object Name) :
statusOfConnection(Disconnected),
validObjectName(Name)
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer Constructor");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer Constructor");
	Socket = ACE_INVALID_HANDLE;
	Port = 0;
	Server = 0;
	// Allocate memory and assign default name for CP object.
	
	CPObjectName = new char[ACS_CHB_BUFSIZE];
	getCPObjectNameAsString(Name);
	
	// Allocate memory and assign default value for CP object.
	
	CPObjectValue = new char[ACS_CHB_BUFSIZE];
	ACE_OS::strcpy(CPObjectValue,"");
	
	// Set (reset) values for "Error" and "Error text" to default
	// assuming syntax check successful in case of repeated requests.
	setError(ACS_CHB_noErrorType);
	setErrorText(ACS_CHB_noError);
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer Constructor");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer Constructor");
}// End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_CP_object
=================================================================== */
ACS_CHB_CP_object_Implementer::~ACS_CHB_CP_object_Implementer()
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer Destructor");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer Destructor");
        // Close session
	if (Socket != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket(Socket);
		Socket = ACE_INVALID_HANDLE;
	}
	
	// De-allocate memory for CP object name and CP object value.
	
	if( CPObjectName != 0)
	{
		delete [] CPObjectName;
		CPObjectName = 0;
	}
	if( CPObjectValue != 0 )
	{
		delete [] CPObjectValue;
		CPObjectValue = 0;
	}
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer Destructor");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer Destructor");
} // End of destructor

/*===================================================================
   ROUTINE: connect
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_CP_object_Implementer::connect(int ReTries,int TimeToWait)
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::connect()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::connect()");
	char error_text[512]; // Large enough

	struct sockaddr_in server_addr;
	ACE_OS::memset(&server_addr, 0, sizeof (server_addr));
        server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (ACS_CHB_serverPort);
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		
	// Connect to server
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Calling connectToServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Calling connectToServer()");
	if (connectToServer(&server_addr,ReTries,TimeToWait) == ACS_CHB_FAIL) 
	{
		setError(ACS_CHB_communicationFailure);
		ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_connectFailureText, ACE_OS::last_error());
		setErrorText(error_text);
		if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", error_text);
			ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}

		if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "connectToServer() failed");
			ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connect()");
			ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}

		return(ACS_CHB_FAIL);
	}
		// Register client to subscribe on all object changes.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Calling registerClient()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Calling registerClient()");
	if (registerClient(CPObjectName) == ACS_CHB_FAIL) 
	{
		ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_registerClientFailureText,ACE_OS::last_error());
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", error_text);
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s",error_text);
		setError(ACS_CHB_communicationFailure);
		setErrorText(error_text);
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "registerClient() failed");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "registerClient() failed");
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connect()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connect()");
		return (ACS_CHB_FAIL);
	}
	// Return success.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connect()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connect()");
	return(ACS_CHB_OK);
} // End of connect

/*===================================================================
   ROUTINE: disconnect
=================================================================== */
void ACS_CHB_CP_object_Implementer::disconnect()
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::disconnect()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::disconnect()");
	if (Socket != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket(Socket);
	}
	Socket = ACE_INVALID_HANDLE;
	statusOfConnection = Disconnected;
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::disconnect()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::disconnect()");
} // End of disconnect

/*===================================================================
   ROUTINE: get_status
=================================================================== */
State ACS_CHB_CP_object_Implementer::get_status()
{
	// Get status of connection (Disconnected or Connected)
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::get_status()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::get_status()");
	if(statusOfConnection != Disconnected)
	{
		struct pollfd clientConnection;
		clientConnection.fd  = Socket;
		clientConnection.events = POLLIN;
		
		// Check the connection with server. //UABBJIS
		ACE_Time_Value tv(ACS_CHB_pollTimeout,0);

		if(ACE_OS::poll(&clientConnection, 1, &tv) == 1)
		{
			char DumBuf[ACS_CHB_BUFSIZE];
			if(ACE_OS::recv(Socket, DumBuf, ACS_CHB_BUFSIZE, MSG_PEEK) == 0) 
			{
		                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
		                {
		                    char traceBuffer[512];
		                    memset(&traceBuffer, 0, sizeof(traceBuffer));
		                    sprintf(traceBuffer, "%s", "Failed in receiving Socket");
		                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		                }
                                //ERROR(0,"%s", "Failed in receiving Socket");
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Calling disconnect()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
                                //ERROR(0,"%s", "Calling disconnect()");
				disconnect();
			}
		}
	}
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_status()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_status()");
	return(statusOfConnection);
} // End of get_status

/*===================================================================
   ROUTINE: get_fileDescriptor
=================================================================== */
int ACS_CHB_CP_object_Implementer::get_fileDescriptor()
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::get_fileDescriptor()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::get_fileDescriptor()");
	// Return fildescriptor to current socket session.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_fileDescriptor()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_fileDescriptor()");
	return(Socket);
} // End of get_fileDescriptor

/*===================================================================
   ROUTINE: get_name
=================================================================== */
char *ACS_CHB_CP_object_Implementer::get_name()
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::get_name()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::get_name()");
	// Returns a pointer to current object-name
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_name()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_name()");
	return(CPObjectName);
} // End of get_name

/*===================================================================
   ROUTINE: get_value
=================================================================== */

char *ACS_CHB_CP_object_Implementer::get_value()
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::get_value()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::get_value()");
	struct pollfd clientConnection;
	int rc, result;
	
	result = ACE_OS::strcmp(CPObjectName,"EX_Exchange_identity"); //Exchange header value lagrad p k:\ disk. 041126
	
	// Init
	if (statusOfConnection == Disconnected)
	{
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "StatusOfConnection was Disconnected");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
	        //ERROR(0,"%s","StatusOfConnection was Disconnected");
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
	        //ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
		return ((char *)0);					// UABMAGN 040917
	}
	
	clientConnection.fd  = Socket;
	clientConnection.events = POLLIN;
	
	// Get data either locally or from server.
	ACE_Time_Value tv(ACS_CHB_pollTimeout, 0);

	rc = ACE_OS::poll(&clientConnection, 1, &tv);

	if (rc == 1) 
	{
		ACE_OS::memset(CPObjectValue,' ',ACS_CHB_BUFSIZE);
		CPObjectValue[ACS_CHB_BUFSIZE-1] = 0;
		// Read CP object data from server. 
	        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
	        {
	            char traceBuffer[512];
	            memset(&traceBuffer, 0, sizeof(traceBuffer));
	            sprintf(traceBuffer, "%s", "Calling readCPObjectValueFromServer()");
	            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
	        }
		//DEBUG(0,"%s","Calling readCPObjectValueFromServer()");
		if (readCPObjectValueFromServer(CPObjectValue) != ACS_CHB_OK) 
		{
	                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                {
	                    char traceBuffer[512];
	                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                    sprintf(traceBuffer, "%s", "readCPObjectValueFromServer is Failed");
	                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                }
                        //ERROR(0,"%s","readCPObjectValueFromServer is Failed");
                        if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                        {
                            char traceBuffer[512];
                            memset(&traceBuffer, 0, sizeof(traceBuffer));
                            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
                            ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                        }
		        //ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
			return ((char *)0);				// UABMAGN 040917
		}
	}
	else // Timeout or error
	{
		if (CPObjectValue[0] == 0) 
		{    
			ACE_OS::memset(CPObjectValue,' ',ACS_CHB_BUFSIZE);
			CPObjectValue[ACS_CHB_BUFSIZE-1] = 0;
			// Send request for latest CP object value from server,
		        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
		        {
		            char traceBuffer[512];
		            memset(&traceBuffer, 0, sizeof(traceBuffer));
		            sprintf(traceBuffer, "%s", "Calling requestCPObjectValueFromServer()");
		            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			//DEBUG(0,"%s","Calling requestCPObjectValueFromServer()");
			if (requestCPObjectValueFromServer(CPObjectValue) != ACS_CHB_OK) 
			{
	                        if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                        {
	                            char traceBuffer[512];
	                            memset(&traceBuffer, 0, sizeof(traceBuffer));
	                            sprintf(traceBuffer, "%s", "requestCPObjectValueFromServer is Failed");
	                            ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                        }
                                //ERROR(0,"%s","requestCPObjectValueFromServer is Failed");
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
			        //ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
				return ((char *)0);
			}
		        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
		        {
		            char traceBuffer[512];
		            memset(&traceBuffer, 0, sizeof(traceBuffer));
		            sprintf(traceBuffer, "%s", "Calling readCPObjectValueFromServer()");
		            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			//DEBUG(0,"%s","Calling readCPObjectValueFromServer()");
			// Read requested value.
			if (readCPObjectValueFromServer(CPObjectValue) != ACS_CHB_OK) 
			{
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "readCPObjectValueFromServer is Failed");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
                                //ERROR(0,"%s","readCPObjectValueFromServer is Failed");
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				return ((char *)0);
			}
		}
	}
	if (!result) // Exchange header
	{
		if (!(result = ACE_OS::strcmp(CPObjectValue, " "))) //If space then check exchange file on k:\disk
		{
		        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
		        {
		            char traceBuffer[512];
		            memset(&traceBuffer, 0, sizeof(traceBuffer));
		            sprintf(traceBuffer, "%s", "Calling checkExchangeFile()");
		            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
                        //DEBUG(0,"%s","Calling checkExchangeFile()");
			if (checkExchangeFile(CPObjectValue) != ACS_CHB_OK)
			{
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "CheckExchangeFile() Failed");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
                                //DEBUG(0,"%s","CheckExchangeFile() Failed");
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Error: Failed to open /data/ACS/CHB/exchangefile.txt file");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s","Error: Failed to open /data/ACS/CHB/exchangefile.txt file");
			}
		}
	}
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::get_value()");
	return CPObjectValue;
} // End of get_value

/*===================================================================
   ROUTINE: connectToServer
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_CP_object_Implementer::connectToServer(struct sockaddr_in *server_addr,
								   int ReTries, int TimeToWait)
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::connectToServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::connectToServer()");
	int len;
	int i, one = 1;
	char error_text[512]; // Large enough
	bool  bSock = false;
	struct  sockaddr_in Adr;

	memset(&Adr, 0, sizeof(Adr));

	Adr.sin_family = AF_INET;
	
	struct protoent *prot = getprotobyname("tcp");

	len = sizeof(struct sockaddr);

	for (i = 1; i <= ReTries; ++i) 
	{
		while ( bSock == false)
		{
		    Adr.sin_port = 0;
			Adr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			Socket = ACE_OS::socket(AF_INET, SOCK_STREAM, prot->p_proto);
			if (Socket == ACE_INVALID_HANDLE) 
			{
					
				ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_socketCreationFailureText,ACE_OS::last_error());
				setError(ACS_CHB_communicationFailure);
				setErrorText(error_text);

				// Trace point
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "Failed to create UNIX domain socket, errorText = %s",error_text);
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"Failed to create UNIX domain socket, errorText = %s",error_text);
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
				return (ACS_CHB_FAIL); // NO need to try again. Severe error
			}
			if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(one)) < 0)
			{
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "setsockopt error");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s","setsockopt error");
				ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_socketCreationFailureText,ACE_OS::last_error());
				setError(ACS_CHB_communicationFailure);
				setErrorText(error_text);
				// Trace point
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "Failed to create UNIX domain socket, errorText = %s",error_text);
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"Failed to create UNIX domain socket, errorText = %s",error_text);
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
				return (ACS_CHB_FAIL); // NO need to try again. Severe error
			}
			if (ACE_OS::bind(Socket, (struct sockaddr *)&Adr, sizeof(Adr)) < 0 )
			{
                          // Trace point
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "%s","bind error");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s","bind error");
				ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_socketCreationFailureText,ACE_OS::last_error());
				setError(ACS_CHB_communicationFailure);
				setErrorText(error_text);
				// Trace point
                                // Trace point
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer, "Failed to create UNIX domain socket, errorText = %s",error_text);
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"Failed to create UNIX domain socket, errorText = %s",error_text);
                                // Trace point
                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                                {
                                    char traceBuffer[512];
                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                                    sprintf(traceBuffer,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                                }
				//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
				return (ACS_CHB_FAIL); // NO need to try again. Severe error
			}
			else
			{
				int AdrLen = sizeof(Adr);
				if (ACE_OS::getsockname(Socket, (struct sockaddr *)&Adr, &AdrLen) < 0)
				{
	                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                                {
	                                    char traceBuffer[512];
	                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                                    sprintf(traceBuffer, "%s","getsockname error");
	                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                                }
					//ERROR(0,"%s","getsockname error");
					ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_socketCreationFailureText,ACE_OS::last_error());
					setError(ACS_CHB_communicationFailure);
					setErrorText(error_text);
					// Trace point
	                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                                {
	                                    char traceBuffer[512];
	                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                                    sprintf(traceBuffer, "Failed to create UNIX domain socket, errorText = %s",error_text);
	                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                                }
					//ERROR(0,"Failed to create UNIX domain socket, errorText = %s",error_text);
	                                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                                {
	                                    char traceBuffer[512];
	                                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                                    sprintf(traceBuffer, "%s","Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
	                                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                                }
					//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
					return (ACS_CHB_FAIL);
				}
				else
				{
					struct servent* cs = getservbyport(Adr.sin_port,"");
					if (cs)
					{
					        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
					        {
					            char traceBuffer[512];
					            memset(&traceBuffer, 0, sizeof(traceBuffer));
					            sprintf(traceBuffer, "port found in service file: %d", Adr.sin_port);
					            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
					        }
						//DEBUG(0,"port found in service file: %d", Adr.sin_port);
						// port number found in services file, can't take that one.
						ACE_OS::closesocket(Socket);
						Socket = ACE_INVALID_HANDLE;
					}
					else
					{
						bSock = true;
					}
				}
			}
		}// End of while loop
		if(Socket > 0)
		{
		if (ACE_OS::connect(Socket, (sockaddr*)server_addr,len) < 0)
		{
			ACE_OS::sleep(TimeToWait);           // Wait for next retry
			ACE_OS::closesocket(Socket);
			Socket = ACE_INVALID_HANDLE;
			statusOfConnection = Disconnected;
		}
		else 
		{
			statusOfConnection = Connected;
		        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
		        {
		            char traceBuffer[512];
		            memset(&traceBuffer, 0, sizeof(traceBuffer));
		            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
		            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
			return ACS_CHB_OK;
		}
		}
	
	} // End of for-loop
	// Return error result.
        if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
            ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
        }
	//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::connectToServer()");
	return ACS_CHB_FAIL;

} // End of connectToServer


/*===================================================================
   ROUTINE: registerClient
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_CP_object_Implementer::registerClient(char cpObjectName[])
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::registerClient()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::registerClient()");
	char buffer[2*ACS_CHB_BUFSIZE];
	
	// Setup client registration string with dummy value to force 
	// immidiate update.
	
	ACE_OS::sprintf(buffer,"[%s][Dummy]",cpObjectName);
	
	// Trace point
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "Subscribe req. on socket for CP object,%s",cpObjectName);
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"Subscribe req. on socket for CP object,%s",cpObjectName);
	
	// Send registration to server.

	if ((ACE_OS::send(Socket,buffer,strlen(buffer) + 1,0)) <= 0 ) 
	{
		ACE_OS::closesocket(Socket);
		Socket = ACE_INVALID_HANDLE;
	        if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	        {
	            char traceBuffer[512];
	            memset(&traceBuffer, 0, sizeof(traceBuffer));
	            sprintf(traceBuffer,"%s", "Sending registration to server is failed");
	            ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	        }
		//ERROR(0,"%s","Sending registration to server is failed");
	        if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	        {
	            char traceBuffer[512];
	            memset(&traceBuffer, 0, sizeof(traceBuffer));
	            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::registerClient()");
	            ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	        }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::registerClient()");
		return ACS_CHB_FAIL;			// Indicate failure.
	}
	
	// Indicate success.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::registerClient()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::registerClient()");
	return ACS_CHB_OK;
} // End of registerClient

/*===================================================================
   ROUTINE: getCPObjectNameAsString
=================================================================== */

char *ACS_CHB_CP_object_Implementer::getCPObjectNameAsString(CP_object Name)
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::getCPObjectNameAsString()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::getCPObjectNameAsString()");
	static char cp_object_name[40]; // Never used. But cannot be reomved
	ACE_OS::strcpy(cp_object_name," "); // Dummy to avoid compiler complaint
	
	// Convert incoming CP object into string format.
	switch(Name) 
	{
	case APZ_type :
		ACE_OS::strcpy(CPObjectName,"APZ_type");
		break;
	case EX_side :
		ACE_OS::strcpy(CPObjectName,"EX_side");
		break;
	case SB_status :
		ACE_OS::strcpy(CPObjectName,"SB_status");
		break;
	case CP_status :
		ACE_OS::strcpy(CPObjectName,"CP_status");
		break;
	case CP_connection :
		ACE_OS::strcpy(CPObjectName,"CP_connection");
		break;
	case EX_Exchange_identity :
		ACE_OS::strcpy(CPObjectName,"EX_Exchange_identity");
		break;
	case SB_Exchange_identity :
		ACE_OS::strcpy(CPObjectName,"SB_Exchange_identity");
		break;
	case Last_Restart_Value :
		ACE_OS::strcpy(CPObjectName,"Last_Restart_Value");
		break;
	case Time_Zone_Alarm :
		ACE_OS::strcpy(CPObjectName,"Time_Zone_Alarm");
		break;
	default:
		ACE_OS::strcpy(CPObjectName," ");
		break;
	} // End of switch
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::getCPObjectNameAsString()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::getCPObjectNameAsString()");
	return CPObjectName;
} // End of getCPObjectNameAsString

/*===================================================================
   ROUTINE: readCPObjectValueFromServer
=================================================================== */
ACS_CHB_returnType 
ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer(char cpObjectValue[])
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
	int rc;
	char error_text[256]; // Large enough

	//Hardcoded for testing
	ACE_OS::strcpy(cpObjectValue,"someCPObjectValue");

	// Read CP object value from server.
	rc = ACE_OS::recv(Socket,cpObjectValue,ACS_CHB_BUFSIZE,0);

	if (rc < 0) 
	{
		// Error.
		ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_readCPObjectValueFromServerFailureText,ACE_OS::last_error());
		setError(ACS_CHB_communicationFailure);
		setErrorText(error_text);
		// Trace point
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", error_text);
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
		return ACS_CHB_FAIL;
	}
	else 
	{
		// If zero Bytes are returned the server side is closed
		if (rc == 0) 
		{	//
			// Connection closed by server		//uabbjis
			//
			ACE_OS::sprintf(error_text,"%s - errno = %d", ACS_CHB_connectionClosedByServerFailureText,errno);
			setError(ACS_CHB_communicationFailure);
			setErrorText(error_text);
			// Trace point
	                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                {
	                    char traceBuffer[512];
	                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                    sprintf(traceBuffer, "%s", error_text);
	                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                }
			//ERROR(0,"%s",error_text);
	                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
	                {
	                    char traceBuffer[512];
	                    memset(&traceBuffer, 0, sizeof(traceBuffer));
	                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
	                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	                }
			//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
			return ACS_CHB_FAIL;
		} 
		else // rc > 0. Data received
		{
			// Make sure the received message is NULL terminated.
			cpObjectValue[rc] = '\0';
			// Trace point
		        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
		        {
		            char traceBuffer[512];
		            memset(&traceBuffer, 0, sizeof(traceBuffer));
		            sprintf(traceBuffer, "== Read %d bytes from server, cpObjectValue= %s",rc,cpObjectValue);
		            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			//DEBUG(0,"== Read %d bytes from server, cpObjectValue= %s",rc,cpObjectValue);
		}
	}
	// Indicate success.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::readCPObjectValueFromServer()");
	return ACS_CHB_OK;
} // End of readCPObjectValueFromServer


/*===================================================================
   ROUTINE: requestCPObjectValueFromServer
=================================================================== */
ACS_CHB_returnType
ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer(char cpObjectValue[])
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
	char buffer[2*ACS_CHB_BUFSIZE+5];
	char error_text[256]; // Large enough
	int rc;
	
	// Setup communication string to server
	ACE_OS::sprintf(buffer,"[%s][%s]",get_name(),cpObjectValue);
	
	// Send current value to host.
	rc = ACE_OS::send(Socket,buffer,strlen(buffer),0);
	// Check result of write.
	
	if (rc <= 0) 
	{
		// Error.
		ACE_OS::sprintf(error_text,"%s - errno = %d",
        ACS_CHB_requestCPObjectValueFromServerFailureText,ACE_OS::last_error());

		setError(ACS_CHB_communicationFailure);
		setErrorText(error_text);
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", error_text);
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s",error_text);
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
		return ACS_CHB_FAIL;
	} 
	else 
	{
		// Trace point
	        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
	        {
	            char traceBuffer[512];
	            memset(&traceBuffer, 0, sizeof(traceBuffer));
	            sprintf(traceBuffer, "== Wrote %d bytes from server, buffer= %s",rc,buffer);
	            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
	        }
		//DEBUG(0,"== Wrote %d bytes from server, buffer= %s",rc,buffer);
	}
	// Indicate success.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::requestCPObjectValueFromServer()");
	return ACS_CHB_OK;

} // End of requestCPObjectValueFromServer


/*===================================================================
   ROUTINE: checkExchangeFile
=================================================================== */
ACS_CHB_returnType
ACS_CHB_CP_object_Implementer::checkExchangeFile(char cpObjectValue[])
{
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_CP_object_Implementer::checkExchangeFile()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0,"%s", "Entering ACS_CHB_CP_object_Implementer::checkExchangeFile()");
	char dirbuf[ACS_CHB_BUFSIZE];
	ACS_APGCC_CommonLib oComLib;
	char* szPath = new char[ACS_CHB_BUFSIZE];

	int dwLen = ACS_CHB_BUFSIZE;
	const char * szLogName = "ACS_DATA";
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Calling GetDataDiskPath()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s","Calling GetDataDiskPath()");
	int retCode = oComLib.GetDataDiskPath(szLogName, szPath, dwLen);

	if (retCode!= ACS_APGCC_DNFPATH_SUCCESS)
	{
		delete[] szPath;
		szPath = 0;
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "GetDataDiskPath() is Failed");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "GetDataDiskPath() is Failed");
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
		return ACS_CHB_FAIL;
	}

	char lpExchangeFile[ACS_CHB_BUFSIZE] = "exchangefile.txt";
	ACE_OS::strcat( szPath, "/" );
	ACE_OS::strcat( szPath, "chb");
	ACE_OS::strcat( szPath, "/" );
	ACE_OS::strcat( szPath, lpExchangeFile );

	FILE *chbExchangeFile = 0;


	chbExchangeFile = ACE_OS::fopen(szPath , "r");

	if(chbExchangeFile == NULL)
	{
		delete[] szPath;
		szPath  = 0;
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "chbExchangeFile is NULL");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "chbExchangeFile is NULL");
                if( ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_ON())
                {
                    char traceBuffer[512];
                    memset(&traceBuffer, 0, sizeof(traceBuffer));
                    sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
                    ACS_CHB_CPObject_Implementer_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
		//ERROR(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
		return ACS_CHB_FAIL;
	}
	else
	{
		int numread;
		numread = ACE_OS::fread(dirbuf, 1, sizeof(dirbuf), chbExchangeFile);
		ACE_OS::strcpy(cpObjectValue, dirbuf);
		cpObjectValue[numread] = '\0';
	}

	delete[] szPath;
	szPath = 0;

	ACE_OS::fclose(chbExchangeFile);
	// Indicate success.
        if( ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[512];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
            ACS_CHB_CPObject_Implementer_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0,"%s", "Leaving ACS_CHB_CP_object_Implementer::checkExchangeFile()");
	return ACS_CHB_OK;

} // End of checkExchangeFile
