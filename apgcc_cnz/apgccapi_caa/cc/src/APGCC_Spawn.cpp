#include "APGCC_Spawn.H"


//@@ Required lib at link time
#pragma comment(lib, "userenv.lib")


using namespace std;


// Static variables
ACE_HANDLE APGCC_SpawnPgm::hStdIn     = NULL;
BOOL   APGCC_SpawnPgm::bRunThread = TRUE;
string APGCC_SpawnPgm::errmsg_;
DWORD  MCS_SpawnPgm::lastError_ = 0;


//-----------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------
APGCC_SpawnPgm::APGCC_SpawnPgm()
:hChildProcess_(NULL)
 //,hStdIn(NULL) //,bRunThread(TRUE)
{
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
APGCC_SpawnPgm::~APGCC_SpawnPgm()
{
}


//-----------------------------------------------------------------------------
// Returns last error code
//-----------------------------------------------------------------------------
DWORD APGCC_SpawnPgm::GetError() const
{
   return this->lastError_;
}

//-----------------------------------------------------------------------------
// Return assembled error message.
//-----------------------------------------------------------------------------
const char* APGCC_SpawnPgm::GetErrorMsg() const
{
   return this->errmsg_.c_str();
}


// Session based methods
//=======================

//-----------------------------------------------------------------------------
// Logon the specified user and loads the user's profile
//-----------------------------------------------------------------------------
bool APGCC_SpawnPgm::LogonUser(const std::string& domain,
                             const std::string& user,
                             const std::string& psw,
                             DWORD              logonType)
{
   char tmpbuf[256];

   // Clear error message
   lastError_ = 0;
   errmsg_ = "";

   // Save local variables
   this->user_ = user;

   // Logon as given user
   BOOL logon_ok = ::LogonUser(  (char*)user.c_str(),
                                 (char*)domain.c_str(),
                                 (char*)psw.c_str(),
                                 logonType,
                                 LOGON32_PROVIDER_DEFAULT,
                                 &hUserSession_);

   if ( !logon_ok ) {
      ::sprintf(tmpbuf,"LogonUser(); domain=%s,user=%s;",domain.c_str(),user.c_str());
      AssembleError(tmpbuf);
      return false;
   }

   // Load user profile
   ::memset(&userProfile_,0,sizeof(userProfile_));

   userProfile_.dwSize     = sizeof userProfile_;
   userProfile_.lpUserName = (char*)user.c_str();

   if ( !::LoadUserProfile(hUserSession_,&userProfile_) ) {
      AssembleError("LoadUserProfile");

      // Close login session handle
      ::CloseHandle(hUserSession_);

      return false;
   }

   // Create the user's own environment
   pEnvironment_  = NULL;
   creationFlags_ = CREATE_NO_WINDOW | DETACHED_PROCESS;

   if ( !::CreateEnvironmentBlock(&pEnvironment_, hUserSession_, FALSE) ) {
      if ( !::CreateEnvironmentBlock(&pEnvironment_, hUserSession_, TRUE) ) {
         pEnvironment_ = NULL;
      }
   }

   if ( pEnvironment_ )
      creationFlags_ |= CREATE_UNICODE_ENVIRONMENT;

   return true;
}


//-----------------------------------------------------------------------------
// Spawns and execute the indicated program
//-----------------------------------------------------------------------------
bool APGCC_SpawnPgm::Spawn(const std::string& program,
                         std::string&       output,
                         const std::string& work_dir)
{
   HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
   HANDLE hOutputRead    = INVALID_HANDLE_VALUE;
   HANDLE hOutputWrite   = INVALID_HANDLE_VALUE;
   HANDLE hInputWriteTmp = INVALID_HANDLE_VALUE;
   HANDLE hInputRead     = INVALID_HANDLE_VALUE;
   HANDLE hInputWrite    = INVALID_HANDLE_VALUE;
   HANDLE hErrorWrite    = INVALID_HANDLE_VALUE;
   HANDLE hThread        = INVALID_HANDLE_VALUE;
   DWORD ThreadId;
   SECURITY_ATTRIBUTES sa;


   // Clear output variable
   output = "";

   // Clear error message
   errmsg_ = "";
   lastError_ = 0;


#if defined(IMPERSONATE_USER) && defined(LOGON_USER)
   char tmpbuf[40];

   // Impersonate user; This seems to be needed even if
   // when use the CreateProcessAsUser() call
   BOOL logon_ok = ::ImpersonateLoggedOnUser(hUserSession_);
   if ( !logon_ok ) {
      ::sprintf(tmpbuf,"ImpersonateLoggedOnUser(); user=%s;",user_.c_str());
      AssembleError(tmpbuf);
      goto Cleanup;
   }

#endif //IMPERSONATE_USER

   // Set up the security attributes struct.
   sa.nLength= sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;


   // Create the child output pipe.
   if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0)) {
      AssembleError("CreatePipe(child output pipe)");
      goto Cleanup;
   }


   // Create a duplicate of the output write handle for the std error
   // write handle. This is necessary in case the child application
   // closes one of its std output handles.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
                        GetCurrentProcess(),&hErrorWrite,0,
                        TRUE,DUPLICATE_SAME_ACCESS)) {
      AssembleError("DuplicateHandle(output write handle)");
      goto Cleanup;
   }


   // Create the child input pipe.
   if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0)) {
      AssembleError("CreatePipe(child input pipe)");
      goto Cleanup;
   }


   // Create new output read handle and the input write handles. Set
   // the Properties to FALSE. Otherwise, the child inherits the
   // properties and, as a result, non-closeable handles to the pipes
   // are created.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                        GetCurrentProcess(),
                        &hOutputRead, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(output read handle)");
      goto Cleanup;
   }

   if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
                        GetCurrentProcess(),
                        &hInputWrite, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(input write handle)");
      goto Cleanup;
   }


   // Close inheritable copies of the handles you do not want to be
   // inherited.
   CloseHandle(hOutputReadTmp);
   CloseHandle(hInputWriteTmp);


   // Get std input handle so you can close it and force the ReadFile to
   // fail when you want the input thread to exit.
   if ( (hStdIn = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE ) {
      AssembleError("GetStdHandle(hStdIn)");
      goto Cleanup;
   }

   // Expand working directory
   char startDir[512];

   if ( ExpandEnvironmentStrings(work_dir.c_str(),startDir,sizeof(startDir)) == 0 ) {
      strcpy(startDir,work_dir.c_str());
   }


   // Run program
   if ( !PrepAndLaunchRedirectedChild(startDir,program,hOutputWrite,hInputRead,hErrorWrite) )
      goto Cleanup;


   // Close pipe handles (do not continue to modify the parent).
   // You need to make sure that no handles to the write end of the
   // output pipe are maintained in this process or else the pipe will
   // not close when the child process exits and the ReadFile will hang.
   CloseHandle(hOutputWrite);
   CloseHandle(hInputRead );
   CloseHandle(hErrorWrite);


   // Launch the thread that gets the input and sends it to the child.
   hThread = CreateThread(NULL,0,GetAndSendInputThread,
                           (LPVOID)hInputWrite,0,&ThreadId);
   if (hThread == NULL) {
      AssembleError("CreateThread");
      goto Cleanup;
   }


   // Read the child's output.
   if ( !ReadAndHandleOutput(hOutputRead,output) )
      goto Cleanup;


   //** Redirection is complete and child has ended **


   // Force the read on the input to return by closing the stdin handle.
   CloseHandle(hStdIn);


   // Tell the thread to exit and wait for thread to die.
   bRunThread = FALSE;

   if ( WaitForSingleObject(hThread,INFINITE) == WAIT_FAILED) {
      AssembleError("WaitForSingleObject");
      //goto ErrorExit;
   }

   CloseHandle(hOutputRead);
   CloseHandle(hInputWrite);
   //@@CloseHandle(hUserSession_);

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)

   if ( logon_ok ) {
      ::RevertToSelf();
   }

#endif //IMPERSONATE_USER

   return true;


Cleanup:

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)

   if ( logon_ok ) {
      ::RevertToSelf();
   }

#endif //IMPERSONATE_USER

   if ( hStdIn         != INVALID_HANDLE_VALUE ) CloseHandle(hStdIn);
   if ( hInputRead     != INVALID_HANDLE_VALUE ) CloseHandle(hInputRead);
   if ( hOutputRead    != INVALID_HANDLE_VALUE ) CloseHandle(hOutputRead);
   if ( hOutputReadTmp != INVALID_HANDLE_VALUE ) CloseHandle(hOutputReadTmp);
   if ( hOutputWrite   != INVALID_HANDLE_VALUE ) CloseHandle(hOutputWrite);
   if ( hInputWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hInputWrite);
   if ( hInputWriteTmp != INVALID_HANDLE_VALUE ) CloseHandle(hInputWriteTmp);
   if ( hErrorWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hErrorWrite);
   //@@if ( hUserSession_  != INVALID_HANDLE_VALUE ) CloseHandle(hUserSession_);

   return false;
}


//-----------------------------------------------------------------------------
// Logout the user and unloads the user's profile
// SpawnPgm doesn't work after this call.
//-----------------------------------------------------------------------------
bool MCS_SpawnPgm::LogoutUser()
{

   // Free memory
   if ( pEnvironment_ )
      ::DestroyEnvironmentBlock(pEnvironment_);

   // Free user profile
   if ( !::UnloadUserProfile(hUserSession_,userProfile_.hProfile) ) {
      AssembleError("UnloadUserProfile");
   }

   // Close log session handle
   if ( hUserSession_ != INVALID_HANDLE_VALUE ) {
      ::CloseHandle(hUserSession_);
   }

   return true;
}



//-----------------------------------------------------------------------------
// Sets up STARTUPINFO structure, and launches redirected child.
//-----------------------------------------------------------------------------
bool MCS_SpawnPgm::PrepAndLaunchRedirectedChild(const string& work_dir,
                                                const string& child,
                                                HANDLE        hChildStdOut,
                                                HANDLE        hChildStdIn,
                                                HANDLE        hChildStdErr)
{
   PROCESS_INFORMATION pi;
   STARTUPINFO si;
   BOOL Ok = FALSE;

   // Set up the start up info struct.
   ZeroMemory(&si,sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.dwFlags = STARTF_USESTDHANDLES;
   si.hStdOutput = hChildStdOut;
   si.hStdInput  = hChildStdIn;
   si.hStdError  = hChildStdErr;
   // Use this if you want to hide the child:
   //     si.wShowWindow = SW_HIDE;
   // Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
   // use the wShowWindow flags.



   // Launch the process that you want to redirect (in this case,
   // Child.exe). Make sure Child.exe is in the same directory as
   // redirect.c launch redirect from a command line to prevent location
   // confusion.
   Ok = CreateProcessAsUser(hUserSession_,
                            NULL,
                            (char*)child.c_str(),
                            NULL,
                            NULL,
                            TRUE,
                            this->creationFlags_,
                            this->pEnvironment_,
                            (work_dir.length()==0 ? NULL : work_dir.c_str()),
                            &si,
                            &pi);
   if ( !Ok ) {
      AssembleError("CreateProcessAsUser");
      goto Cleanup;
   }


   // Succeeded
   Ok = TRUE;

   // Set global child process handle to cause threads to exit.
   hChildProcess_ = pi.hProcess;


   // Close any unnecessary handles.
   CloseHandle(pi.hThread);

Cleanup:

   return ( Ok ? true : false );
}


//-----------------------------------------------------------------------------
// CheckAccess()
//-----------------------------------------------------------------------------
bool MCS_SpawnPgm::CheckAccessToPgm(const std::string& pgm)
{
   HANDLE hFile = INVALID_HANDLE_VALUE;

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)
   // Impersonate user; This seems to be needed even if
   // when use the CreateProcessAsUser() call
   bool logon_ok = ::ImpersonateLoggedOnUser(hLogonSession);
   if ( !logon_ok ) {
      ::sprintf(tmpbuf,"ImpersonateLoggedOnUser(); user=%s;",user.c_str());
      AssembleError(tmpbuf);
      goto ErrorExit;
   }
#endif

   hFile = ::CreateFile(pgm.c_str(),
                        GENERIC_EXECUTE,        // file access mode
                        FILE_SHARE_READ,        // share mode
                        NULL,                   // security descriptor
                        OPEN_EXISTING,          // how to access
                        FILE_ATTRIBUTE_NORMAL,  // file attributes
                        NULL);                  // handle to template file
   if ( hFile != INVALID_HANDLE_VALUE ) {
      ::CloseHandle(hFile);
      return true;
   }
   else
      return false;
}


//-----------------------------------------------------------------------------
// Logon as the given user and spawns a process that runs the indicated
// program.
//-----------------------------------------------------------------------------
#undef LOGON_USER

bool MCS_SpawnPgm::Spawn(const std::string& program,
                               std::string& output,
                         const std::string& domain,
                         const std::string& user,
                         const std::string& psw,
                         const std::string& work_dir,
                               DWORD        logonType)
{
   HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
   HANDLE hOutputRead    = INVALID_HANDLE_VALUE;
   HANDLE hOutputWrite   = INVALID_HANDLE_VALUE;
   HANDLE hInputRead     = INVALID_HANDLE_VALUE;
   HANDLE hInputWrite    = INVALID_HANDLE_VALUE;
   HANDLE hInputWriteTmp = INVALID_HANDLE_VALUE;
   HANDLE hErrorWrite    = INVALID_HANDLE_VALUE;
   HANDLE hThread        = INVALID_HANDLE_VALUE;
   HANDLE hLogonSession  = INVALID_HANDLE_VALUE;
   //DWORD ThreadId;
   SECURITY_ATTRIBUTES sa;

   // Clear output variable
   output = "";

   // Clear error message
   errmsg_ = "";
   lastError_ = 0;


#ifdef LOGON_USER
   char tmpbuf[40];

   // Logon as given user
   BOOL logon_ok = ::LogonUser(  (char*)user.c_str(),
                                 (char*)domain.c_str(),
                                 (char*)psw.c_str(),
                                 logonType,
                                 LOGON32_PROVIDER_DEFAULT,
                                 &hLogonSession);

   if ( !logon_ok ) {
      ::sprintf(tmpbuf,"LogonUser(); domain=%s,user=%s;",domain.c_str(),user.c_str());
      AssembleError(tmpbuf);
      return false;
   }
#endif

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)
   // Impersonate user; This seems to be needed even if
   // when use the CreateProcessAsUser() call
   logon_ok = ::ImpersonateLoggedOnUser(hLogonSession);
   if ( !logon_ok ) {
      ::sprintf(tmpbuf,"ImpersonateLoggedOnUser(); user=%s;",user.c_str());
      AssembleError(tmpbuf);
      goto ErrorExit;
   }
#endif

   // Set up the security attributes struct.
   sa.nLength= sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;


   // Create the child output pipe.
   if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0)) {
      AssembleError("CreatePipe(child output pipe)");
      goto ErrorExit;
   }


   // Create a duplicate of the output write handle for the std error
   // write handle. This is necessary in case the child application
   // closes one of its std output handles.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
                        GetCurrentProcess(),&hErrorWrite,0,
                        TRUE,DUPLICATE_SAME_ACCESS)) {
      AssembleError("DuplicateHandle(output write handle)");
      goto ErrorExit;
   }


   // Create the child input pipe.
   if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0)) {
      AssembleError("CreatePipe(child input pipe)");
      goto ErrorExit;
   }


   // Create new output read handle and the input write handles. Set
   // the Properties to FALSE. Otherwise, the child inherits the
   // properties and, as a result, non-closeable handles to the pipes
   // are created.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                        GetCurrentProcess(),
                        &hOutputRead, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(output read handle)");
      goto ErrorExit;
   }

   if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
                        GetCurrentProcess(),
                        &hInputWrite, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(input write handle)");
      goto ErrorExit;
   }


   // Close inheritable copies of the handles you do not want to be
   // inherited.
   CloseHandle(hOutputReadTmp);
   CloseHandle(hInputWriteTmp);


   // Get std input handle so you can close it and force the ReadFile to
   // fail when you want the input thread to exit.
   if ( (hStdIn = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE ) {
      AssembleError("GetStdHandle(hStdIn)");
      goto ErrorExit;
   }

   // Expand working directory
   char startDir[512];

   if ( ExpandEnvironmentStrings(work_dir.c_str(),startDir,sizeof(startDir)) == 0 ) {
      strcpy(startDir,work_dir.c_str());
   }


   // Run program
   if ( !PrepAndLaunchRedirectedChild(user,program,hOutputWrite,hInputRead,hErrorWrite,hLogonSession,startDir) )
      goto ErrorExit;


   // Close pipe handles (do not continue to modify the parent).
   // You need to make sure that no handles to the write end of the
   // output pipe are maintained in this process or else the pipe will
   // not close when the child process exits and the ReadFile will hang.
   CloseHandle(hOutputWrite);
   CloseHandle(hInputRead );
   CloseHandle(hErrorWrite);


#ifdef SEND_PGM_INPUT

   // Launch the thread that gets the input and sends it to the child.
   hThread = CreateThread(NULL,0,GetAndSendInputThread,
                           (LPVOID)hInputWrite,0,&ThreadId);
   if (hThread == NULL) {
      AssembleError("CreateThread");
      goto ErrorExit;
   }

#endif //SEND_PGM_INPUT


   // Read the child's output.
   if ( !ReadAndHandleOutput(hOutputRead,output) )
      goto ErrorExit;


   //** Redirection is complete and child has ended **


   // Force the read on the input to return by closing the stdin handle.
   CloseHandle(hStdIn);


   // Tell the thread to exit and wait for thread to die.
   bRunThread = FALSE;

   if ( hThread != INVALID_HANDLE_VALUE ) {
      if ( WaitForSingleObject(hThread,INFINITE) == WAIT_FAILED) {
         AssembleError("WaitForSingleObject");
         //goto ErrorExit;
      }
   }

   CloseHandle(hOutputRead);
   CloseHandle(hInputWrite);
   if ( hLogonSession  != INVALID_HANDLE_VALUE ) CloseHandle(hLogonSession);
   if ( hThread        != INVALID_HANDLE_VALUE ) CloseHandle(hThread);
   if ( hChildProcess_ != INVALID_HANDLE_VALUE ) CloseHandle(hChildProcess_);

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)
   if ( logon_ok ) {
      ::RevertToSelf();
   }
#endif

   return true;


ErrorExit:

#if defined(IMPERSONATE_USER) && defined(LOGON_USER)
   if ( logon_ok ) {
      ::RevertToSelf();
   }
#endif

   if ( hStdIn         != INVALID_HANDLE_VALUE ) CloseHandle(hStdIn);
   if ( hInputRead     != INVALID_HANDLE_VALUE ) CloseHandle(hInputRead);
   if ( hOutputRead    != INVALID_HANDLE_VALUE ) CloseHandle(hOutputRead);
   if ( hOutputReadTmp != INVALID_HANDLE_VALUE ) CloseHandle(hOutputReadTmp);
   if ( hOutputWrite   != INVALID_HANDLE_VALUE ) CloseHandle(hOutputWrite);
   if ( hInputWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hInputWrite);
   if ( hInputWriteTmp != INVALID_HANDLE_VALUE ) CloseHandle(hInputWriteTmp);
   if ( hErrorWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hErrorWrite);
   if ( hLogonSession  != INVALID_HANDLE_VALUE ) CloseHandle(hLogonSession);
   if ( hThread        != INVALID_HANDLE_VALUE ) CloseHandle(hThread);
   if ( hChildProcess_ != INVALID_HANDLE_VALUE ) CloseHandle(hChildProcess_);

   return false;
}


//-----------------------------------------------------------------------------
// Spawns a process that runs the indicated program.
//-----------------------------------------------------------------------------
bool MCS_SpawnPgm::Spawn2(const char* program, string& output)
{
   HANDLE hOutputReadTmp = INVALID_HANDLE_VALUE;
   HANDLE hOutputRead    = INVALID_HANDLE_VALUE;
   HANDLE hOutputWrite   = INVALID_HANDLE_VALUE;
   HANDLE hInputWriteTmp = INVALID_HANDLE_VALUE;
   HANDLE hInputRead     = INVALID_HANDLE_VALUE;
   HANDLE hInputWrite    = INVALID_HANDLE_VALUE;
   HANDLE hErrorWrite    = INVALID_HANDLE_VALUE;
   HANDLE hThread        = INVALID_HANDLE_VALUE;
   DWORD ThreadId;
   SECURITY_ATTRIBUTES sa;

   // Clear output variable
   output = "";

   // Clear error message
   errmsg_ = "";
   lastError_ = 0;

   // Set up the security attributes struct.
   sa.nLength= sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;


   // Create the child output pipe.
   if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0)) {
      AssembleError("CreatePipe(child output pipe)");
      goto ErrorExit;
   }


   // Create a duplicate of the output write handle for the std error
   // write handle. This is necessary in case the child application
   // closes one of its std output handles.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
                        GetCurrentProcess(),&hErrorWrite,0,
                        TRUE,DUPLICATE_SAME_ACCESS)) {
      AssembleError("DuplicateHandle(output write handle)");
      goto ErrorExit;
   }


   // Create the child input pipe.
   if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0)) {
      AssembleError("CreatePipe(child input pipe)");
      goto ErrorExit;
   }


   // Create new output read handle and the input write handles. Set
   // the Properties to FALSE. Otherwise, the child inherits the
   // properties and, as a result, non-closeable handles to the pipes
   // are created.
   if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                        GetCurrentProcess(),
                        &hOutputRead, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(output read handle)");
      goto ErrorExit;
   }

   if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
                        GetCurrentProcess(),
                        &hInputWrite, // Address of new handle.
                        0,FALSE, // Make it uninheritable.
                        DUPLICATE_SAME_ACCESS)) {
      AssembleError("DupliateHandle(input write handle)");
      goto ErrorExit;
   }


   // Close inheritable copies of the handles you do not want to be
   // inherited.
   CloseHandle(hOutputReadTmp);
   CloseHandle(hInputWriteTmp);


   // Get std input handle so you can close it and force the ReadFile to
   // fail when you want the input thread to exit.
   if ( (hStdIn = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE ) {
      AssembleError("GetStdHandle(hStdIn)");
      goto ErrorExit;
   }

   // Run program
   if ( !PrepAndLaunchRedirectedChild("Guest",program,hOutputWrite,hInputRead,hErrorWrite,INVALID_HANDLE_VALUE) )
      goto ErrorExit;


   // Close pipe handles (do not continue to modify the parent).
   // You need to make sure that no handles to the write end of the
   // output pipe are maintained in this process or else the pipe will
   // not close when the child process exits and the ReadFile will hang.
   CloseHandle(hOutputWrite);
   CloseHandle(hInputRead );
   CloseHandle(hErrorWrite);


   // Launch the thread that gets the input and sends it to the child.
   hThread = CreateThread(NULL,0,GetAndSendInputThread,
                           (LPVOID)hInputWrite,0,&ThreadId);
   if (hThread == NULL) {
      AssembleError("CreateThread");
      goto ErrorExit;
   }


   // Read the child's output.
   if ( !ReadAndHandleOutput(hOutputRead,output) )
      goto ErrorExit;


   //** Redirection is complete and child has ended **


   // Force the read on the input to return by closing the stdin handle.
   CloseHandle(hStdIn);


   // Tell the thread to exit and wait for thread to die.
   bRunThread = FALSE;

   if ( WaitForSingleObject(hThread,INFINITE) == WAIT_FAILED) {
      AssembleError("WaitForSingleObject");
      //goto ErrorExit;
   }

   CloseHandle(hOutputRead);
   CloseHandle(hInputWrite);

   return true;


ErrorExit:

   if ( hStdIn         != INVALID_HANDLE_VALUE ) CloseHandle(hStdIn);
   if ( hInputRead     != INVALID_HANDLE_VALUE ) CloseHandle(hInputRead);
   if ( hOutputRead    != INVALID_HANDLE_VALUE ) CloseHandle(hOutputRead);
   if ( hOutputReadTmp != INVALID_HANDLE_VALUE ) CloseHandle(hOutputReadTmp);
   if ( hOutputWrite   != INVALID_HANDLE_VALUE ) CloseHandle(hOutputWrite);
   if ( hInputWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hInputWrite);
   if ( hInputWriteTmp != INVALID_HANDLE_VALUE ) CloseHandle(hInputWriteTmp);
   if ( hErrorWrite    != INVALID_HANDLE_VALUE ) CloseHandle(hErrorWrite);

   return false;
}


//-----------------------------------------------------------------------------
// Sets up STARTUPINFO structure, and launches redirected child.
//-----------------------------------------------------------------------------
#undef LOAD_USER_PROFILE

bool MCS_SpawnPgm::PrepAndLaunchRedirectedChild(const string& user,
                                                const string& child,
                                                HANDLE        hChildStdOut,
                                                HANDLE        hChildStdIn,
                                                HANDLE        hChildStdErr,
                                                HANDLE        hLogonSession,
                                                const string& work_dir)
{
   PROCESS_INFORMATION pi;
   STARTUPINFO si;
   BOOL Ok = FALSE;

   // Set up the start up info struct.
   ZeroMemory(&si,sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.dwFlags = STARTF_USESTDHANDLES;
   si.hStdOutput = hChildStdOut;
   si.hStdInput  = hChildStdIn;
   si.hStdError  = hChildStdErr;
   // Use this if you want to hide the child:
   //     si.wShowWindow = SW_HIDE;
   // Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
   // use the wShowWindow flags.


#ifdef LOAD_USER_PROFILE
   // Load user profile
   PROFILEINFO userProfile = {sizeof userProfile, 0, (char*)user.c_str()};

   if ( !::LoadUserProfile(hLogonSession,&userProfile) ) {
      AssembleError("LoadUserProfile");
      return false;
   }
#endif //LOAD_USER_PROFILE

   // Create the user's own environment
   void* pEnvironment = NULL;
   DWORD creationFlags = CREATE_NO_WINDOW | DETACHED_PROCESS;

   if ( !::CreateEnvironmentBlock(&pEnvironment, hLogonSession, FALSE) ) {
      if ( !::CreateEnvironmentBlock(&pEnvironment, hLogonSession, TRUE) ) {
         pEnvironment = NULL;
      }
   }

   if ( pEnvironment )
      creationFlags |= CREATE_UNICODE_ENVIRONMENT;


   // Launch the process that you want to redirect (in this case,
   // Child.exe). Make sure Child.exe is in the same directory as
   // redirect.c launch redirect from a command line to prevent location
   // confusion.
   if ( hLogonSession != INVALID_HANDLE_VALUE ) {

      Ok = CreateProcessAsUser(hLogonSession,
                               NULL,
                               (char*)child.c_str(),
                               NULL,
                               NULL,
                               TRUE,
                               creationFlags,
                               pEnvironment,
                               (work_dir.length()==0 ? NULL : work_dir.c_str()),
                               &si,
                               &pi);
      if ( !Ok ) {
         AssembleError("CreateProcessAsUser");
         goto Cleanup;
      }

   }
   else {

      Ok = CreateProcess(NULL,
                         (char*)child.c_str(),
                         NULL,
                         NULL,
                         TRUE,
                         creationFlags,
                         pEnvironment,
                         (work_dir.length()==0 ? NULL : work_dir.c_str()),
                         &si,
                         &pi);
      if ( !Ok ) {
         AssembleError("CreateProcess");
         goto Cleanup;
      }

   } // create process


   // Succeeded
   Ok = TRUE;

   // Set global child process handle to cause threads to exit.
   hChildProcess_ = pi.hProcess;


   // Close any unnecessary handles.
   CloseHandle(pi.hThread);

Cleanup:

   // Free memory
   if ( pEnvironment )
      ::DestroyEnvironmentBlock(pEnvironment);

#ifdef LOAD_USER_PROFILE
   // Free user profile
   if ( !::UnloadUserProfile(hLogonSession,userProfile.hProfile) ) {
      AssembleError("UnloadUserProfile");
   }
#endif //LOAD_USER_PROFILE

   return ( Ok ? true : false );
}


//-----------------------------------------------------------------------------
// Monitors handle for input. Exits when child exits or pipe breaks.
//-----------------------------------------------------------------------------
bool MCS_SpawnPgm::ReadAndHandleOutput(HANDLE hPipeRead, string& output)
{
   CHAR lpBuffer[256];
   DWORD nBytesRead;
   //DWORD nCharsWritten;

   while(TRUE)
   {
      nBytesRead = 0;

      if (!ReadFile(hPipeRead,lpBuffer,sizeof(lpBuffer)-1,&nBytesRead,NULL) || !nBytesRead)
      {
         if (GetLastError() == ERROR_BROKEN_PIPE)
            break; // pipe done - normal exit path.
         else {
            AssembleError("ReadFile"); // Something bad happened.
            return false;
         }
      }

      /* @@ Cannot write to console in a service
      // Display the character read on the screen.
      if (!WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),lpBuffer,
                        nBytesRead,&nCharsWritten,NULL))
         AssembleError("WriteConsole");
      */

      //Save received data in a string
      lpBuffer[nBytesRead] = 0;
      output += lpBuffer;
   }

   return true;
}


//-----------------------------------------------------------------------------
// Thread procedure that monitors the console for input and sends input
// to the child process through the input pipe.
// This thread ends when the child application exits.
//-----------------------------------------------------------------------------
DWORD WINAPI MCS_SpawnPgm::GetAndSendInputThread(LPVOID lpvThreadParam)
{
   CHAR read_buff[256];
   DWORD nBytesRead,nBytesWrote;
   HANDLE hPipeWrite = (HANDLE)lpvThreadParam;

   // Get input from our console and send it to child through the pipe.
   while (MCS_SpawnPgm::bRunThread)
   {
      if(!ReadConsole(hStdIn,read_buff,1,&nBytesRead,NULL))
         //AssembleError("ReadConsole");
         return GetLastError();

      read_buff[nBytesRead] = '\0'; // Follow input with a NULL.

      if (!WriteFile(hPipeWrite,read_buff,nBytesRead,&nBytesWrote,NULL))
      {
         if (GetLastError() == ERROR_NO_DATA)
            break; // Pipe was closed (normal exit path).
         else
            //AssembleError("WriteFile");
            return ::GetLastError();
      }
   }

   return 1;
}


//-----------------------------------------------------------------------------
// Displays the error number and corresponding message.
//-----------------------------------------------------------------------------
void MCS_SpawnPgm::AssembleError(char *pszAPI)
{
    LPVOID lpvMessageBuffer;
    CHAR szPrintBuffer[512];
    //DWORD nCharsWritten;

    lastError_ = GetLastError();

    FormatMessageA(
             FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
             NULL, lastError_,
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             (LPTSTR)&lpvMessageBuffer, 0, NULL);

    sprintf(szPrintBuffer,
      "ERROR: API    = %s\r\n   error code = %d\r\n   message    = %s\r\n",
             pszAPI, GetLastError(), (char *)lpvMessageBuffer);

    /*
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),szPrintBuffer,
                  lstrlen(szPrintBuffer),&nCharsWritten,NULL);
    */

    errmsg_ = szPrintBuffer;

    LocalFree(lpvMessageBuffer);
    //ExitProcess(GetLastError());
}




