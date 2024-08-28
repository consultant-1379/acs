//*****************************************************************************
// COPYRIGHT Ericsson Utvecklings AB, Sweden 2001.
// All rights reserved.
//
// The Copyright to the computer program(s) herein
// is the property of Ericsson Utvecklings AB, Sweden.
// The program(s) may be used and/or copied only with
// the written permission from Ericsson Utvecklings AB or in
// accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been
// supplied.

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME    DESCRIPTION
//      A               01-03-05        SJEN    First version
//                      01-05-23        SJEN    Changed from message mode to byte mode.
//								06-06-29			XCSVEKA	TR HG89314-Increased the timeout value
//													cout<<"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"<<endl;			from 1000MSec to 15000 MSec. The time out
//									 							period controls how long time the command
//													 			will wait for connection to the server(ADH)
//																before it times out.
//***                         Porting                      ***
//       REV NO          DATE            NAME    DESCRIPTION
//       PR_01           10-04-15        AI      First version
//*****************************************************************************
//#include "APGCC_warning_level4.H"

#include "ACS_APGCC_Util.H"
#include <cassert>
#include <ace/OS_NS_unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ACS_APGCC_PIPE_R1.H"
#define MAX_BUF_SIZE	255
using namespace std;

#ifdef NDEBUG
#  define IFDEBUG(x)
#else
#  define IFDEBUG(x)    x
#endif


#define MINIMAL_CONNECT_TIMEOUT 15*1000		//TRHG89314 timeout value to 15sec.


ACS_APGCC_PIPE_Addr_R1::ACS_APGCC_PIPE_Addr_R1()
{

}

ACS_APGCC_PIPE_Addr_R1::ACS_APGCC_PIPE_Addr_R1(const string& pipe_name)
{
   this->pipe_name_ = pipe_name;

}

ACS_APGCC_PIPE_Addr_R1::ACS_APGCC_PIPE_Addr_R1(const ACS_APGCC_PIPE_Addr_R1& addr)
{
   this->pipe_name_ = addr.pipe_name_;
}

ACS_APGCC_PIPE_Addr_R1::~ACS_APGCC_PIPE_Addr_R1()
{
}

ACS_APGCC_PIPE_Addr_R1& ACS_APGCC_PIPE_Addr_R1::operator=(const ACS_APGCC_PIPE_Addr_R1& addr)
{
   if ( this != &addr ) {
      this->pipe_name_ = addr.pipe_name_;
   }
   return *this;
}

void ACS_APGCC_PIPE_Addr_R1::set(const std::string& pipe_name)
{
   this->pipe_name_ = pipe_name;
}

void ACS_APGCC_PIPE_Addr_R1::set(const ACS_APGCC_PIPE_Addr_R1& addr)
{
   this->pipe_name_ = addr.pipe_name_;
}

// Returns complete pipe name.
const char* ACS_APGCC_PIPE_Addr_R1::get_pipe_path() const
{
   return this->pipe_name_.c_str();
}

// Returns first part of pipe name.
// Returns "ap99" if pipe name is "\\ap99\pipename.pipe".
const char* ACS_APGCC_PIPE_Addr_R1::get_domain() const
{
	ACS_APGCC::item( const_cast<ACS_APGCC_PIPE_Addr_R1*>(this)->domain_, pipe_name_, "\\/", 0 );
   return this->domain_.c_str();
}
ACS_APGCC_PIPE_Stream_R1::ACS_APGCC_PIPE_Stream_R1()
   :hPipe_(ACE_INVALID_HANDLE), server_side_(false)
{}

ACS_APGCC_PIPE_Stream_R1::~ACS_APGCC_PIPE_Stream_R1()
{
   this->close();
}

// Methods to support IStream interface
//=======================================

bool ACS_APGCC_PIPE_Stream_R1::Read(void* buf, size_t len)
{
	size_t  read;
	cout<<"ACS_APGCC_PIPE_Stream_R1::Read 1 hPipe_:"<<hPipe_<<endl;
	if (hPipe_==ACE_INVALID_HANDLE )
      return false;
	cout<<"ACS_APGCC_PIPE_Stream_R1::Read 2 hPipe_:"<<hPipe_<<endl;
   // Pipes can only read more than 64K in a single read if in streaming mode (byte)
 //  PR_01 if ( !::ReadFile(hPipe_,buf,len,&read,NULL) || read != len ) {

	if ( !ACE::read_n(hPipe_,buf,len,&read) || read != len  ) {
    /*  if (ACS_TRA_ON(traPIPEStream)) {
     //   PR_01 cast size_t structure to int
    	  ::sprintf(traStr_,"Read(); ERROR: wanted=%d,got=%d,ReadFile() failed with %s",(int)len,(int)read,"ACS_APGCC::getSystemErrorText()");
    	      ACS_TRA_event(&traPIPEStream,traStr_);
       */
		cout<<"ACS_APGCC_PIPE_Stream_R1::Read 3 hPipe_:"<<hPipe_<<endl;
		return false;
      }
	cout<<"ACS_APGCC_PIPE_Stream_R1::Read 4 hPipe_:"<<string((char *)buf)<<endl;
   return true;
  }


bool ACS_APGCC_PIPE_Stream_R1::Write(const void* buf, size_t len)
{

/*PR_01 	DWORD written;*/
	size_t written;
  // ASSERTM("ACS_APGCC_PIPE_Stream_R1::Write hPipe_ check  ",(hPipe_!=ACE_INVALID_HANDLE));
  //PR_01 if(hPipe_==INVALID_HANDLE_VALUE)
   cout<<"ACS_APGCC_PIPE_Stream_R1::Write 1 hPipe_:"<<hPipe_<<endl;
	if(hPipe_==ACE_INVALID_HANDLE)
       return false;
	 cout<<"ACS_APGCC_PIPE_Stream_R1::Write 2 hPipe_:"<<hPipe_<<endl;
   // Pipes can only write more than 64K in a single write if in streaming mode (byte)
	cout<<"ACS_APGCC_PIPE_Stream_R1::Write 1 "<<endl;
   if ( len > 0 ) {

	   //PR_01 if ( !::WriteFile(hPipe_,buf,len,&written,NULL) || written != len ) {
	   if ( !ACE::write_n(hPipe_,buf,len,&written) || written != len ) {
     /*    if (ACS_TRA_ON(traPIPEStream)) {
        	 //   PR_01 cast size_t structure to int
        	 ::sprintf(traStr_,"Write(); ERROR: len=%d,written=%d,WriteFile() failed with %s",(int)len,(int)written,ACS_APGCC::getSystemErrorText());
            ACS_TRA_event(&traPIPEStream,traStr_);
         }
       */
		return false;
      }

   }

   return true;
}

// Other methods
//=======================================

bool ACS_APGCC_PIPE_Stream_R1::RecvMsg(string& out)
{
  /*PR_01
   DWORD read;
   DWORD dataSize;
  */
   size_t read;
   size_t dataSize;

   out = "";

//PR_01  if ( hPipe_ == INVALID_HANDLE_VALUE || !::ReadFile(hPipe_,&dataSize,sizeof(dataSize),&read,NULL) || read != sizeof(dataSize) ) {
   if ( hPipe_ == ACE_INVALID_HANDLE || !ACE::read_n(hPipe_,&dataSize,sizeof(dataSize),&read) || read != sizeof(dataSize) ) {

	   return false;
   }

   char* buf = new char[dataSize+1];
  //PR_01 BOOL ok = ::ReadFile(hPipe_,buf,dataSize,&read,NULL);
   BOOL ok = ACE::read_n(hPipe_,buf,dataSize,&read);

   if ( !ok || read != dataSize ) {
      delete [] buf;
      return false;
   }

   buf[dataSize] = 0;

   out = buf;

   delete [] buf;

   return true;
}

bool ACS_APGCC_PIPE_Stream_R1::SendMsg(const string& in)
{
  /* PR_01
   *
   DWORD written;
   DWORD dataSize = in.length();
   */

   size_t written;
   size_t dataSize;


  //PR_01  if ( hPipe_ == INVALID_HANDLE_VALUE || !::WriteFile(hPipe_,&dataSize,sizeof(dataSize),&written,NULL) || written != sizeof(dataSize) )
   if ( hPipe_ == ACE_INVALID_HANDLE || !ACE::write_n(hPipe_,&dataSize,sizeof(dataSize),&written) || written != sizeof(dataSize) )
      return false;

   if ( dataSize > 0 ) {
      if ( !ACE::write_n(hPipe_,in.c_str(),in.length(),&written) || written != in.length() )
         return false;
   }

   return true;
}

int ACS_APGCC_PIPE_Stream_R1::close()
{
      return 0;
}


// Sets the underlaying pipe handle
void ACS_APGCC_PIPE_Stream_R1::set_handle(ACE_HANDLE h)
{
   hPipe_= h;

}


// Return the underlaying pipe handle
ACE_HANDLE ACS_APGCC_PIPE_Stream_R1::get_handle() const
{

	if ( hPipe_ != ACE_INVALID_HANDLE )
      return this->hPipe_;
   else
      return ACE_INVALID_HANDLE;
}

// Get the current position in the stream
long ACS_APGCC_PIPE_Stream_R1::GetPos()
{ return 0; }

// Set a position within the stream
bool ACS_APGCC_PIPE_Stream_R1::SetPos(long distance, size_t moveMethod)
{ return false; }

// Returns true if stream is empty
bool ACS_APGCC_PIPE_Stream_R1::Empty() const
{ return false; }

// Lock the access to the stream
bool ACS_APGCC_PIPE_Stream_R1::Lock()
{ return false; }

// Unlock the access to the stream
bool ACS_APGCC_PIPE_Stream_R1::Unlock()
{ return false; }



ACS_APGCC_PIPE_Acceptor_R1::ACS_APGCC_PIPE_Acceptor_R1()
   :hPipe_(ACE_INVALID_HANDLE), already_connected_(false), secattr_(NULL)
{
	ACE_OS::memset(&this->overlapped_, 0, sizeof(OVERLAPPED) );

}

ACS_APGCC_PIPE_Acceptor_R1::~ACS_APGCC_PIPE_Acceptor_R1()
{
}

/*  old open*/
int ACS_APGCC_PIPE_Acceptor_R1::Open(const ACS_APGCC_PIPE_Addr_R1& pipe_addr, LPSECURITY_ATTRIBUTES secattr)
{
   this->local_addr_ = pipe_addr;
   this->secattr_    = secattr;

   // Clear any active handle
   this->set_handle( ACE_INVALID_HANDLE );

   return this->create_new_instance();

}

// Accept a new client connection
int ACS_APGCC_PIPE_Acceptor_R1::accept(ACS_APGCC_PIPE_Stream_R1& client)
{

	return this->hPipeAcceptor_.accept(client);
}

/* old accept
// Accept a new client connection
/*int APGCC_PIPE_Acceptor_R1::accept(APGCC_PIPE_Stream_R1& client)
{
   client.close();

   if ( this->hPipe_ == INVALID_HANDLE_VALUE )
      return -1;

   // open() started the Connect in asynchronous mode. Wait for the
   // the event to be signalled, unless already_connected is set

   if ( this->already_connected_ == FALSE ) {

      if ( this->event_.wait() < 0 )
         return -1;

      // A client should now have connected
      // Reuse the already_connected flag
      DWORD unused;

      this->already_connected_ = ::GetOverlappedResult( this->hPipe_,
                                                        &this->overlapped_,
                                                        &unused,
                                                        FALSE );
   }

   if ( this->already_connected_ ) {

      // Let the client take over this pipe instance
      client.set_handle( this->hPipe_ );
      this->set_handle( INVALID_HANDLE_VALUE );

      // Create a new instance of the pipe for the next connection
      this->create_new_instance();

      return 0;
   }

   if (ACS_TRA_ON(traPIPEAccept)) {
      ::sprintf(traStr_,"accept(); ERROR: GetOverlappedResult() failed with %s",MCS::getSystemErrorText());
      ACS_TRA_event(&traPIPEAccept,traStr_);
   }

   return -1;

}*/

/*int APGCC_PIPE_Acceptor_R1::close()
{
  if ( hPipe_ != INVALID_HANDLE_VALUE ) {
      ::DisconnectNamedPipe(hPipe_);
      hPipe_ = INVALID_HANDLE_VALUE;
   }

   return 0;
}
*/

// Close down the passive-mode STREAM pipe listener.
int ACS_APGCC_PIPE_Acceptor_R1::close()
{
	return this->hPipeAcceptor_.close();
}





// Creates a new pipe instance
int ACS_APGCC_PIPE_Acceptor_R1::create_new_instance()
{
	int ret_val,rdfd,numread;
	cout<<"local_addr_.get_pipe_path()::"<<local_addr_.get_pipe_path()<<endl;
	ret_val = mkfifo(this->local_addr_.get_pipe_path(), 0666);
	 if (ret_val == -1) {
		/* if (ACS_TRA_ON(traPIPEAccept)) {
		         ::sprintf(traStr_,"create_new_instance(); ERROR: CreateNamedPipe() failed with %s",MCS::getSystemErrorText());
		         ACS_TRA_event(&traPIPEAccept,traStr_);
		      }
*/cout<<"11"<<endl;

perror( "Error opening file" );
            //printf( "Error opening file: %s\n", strerror( errno ) );


		      return -1;
	 	     }
	 this->already_connected_ = false;
	 // Clear event
	 this->event_.reset();
	 rdfd = open(local_addr_.get_pipe_path(), O_RDWR);
	 if (rdfd == -1)
	{
/*		 if (ACS_TRA_ON(traPIPEAccept)) {
		            ::sprintf(traStr_,"create_new_instance(); ERROR: ConnectNamedPipe() failed with %s",MCS::getSystemErrorText(status));
		            ACS_TRA_event(&traPIPEAccept,traStr_);
		   }
		 return -1;*/
		 cout<<"22"<<endl;

	}
	 this->already_connected_ = true;
	 this->set_handle( rdfd );
	 char rdbuf[MAX_BUF_SIZE];
	 numread = read(rdfd, rdbuf, MAX_BUF_SIZE);

	 rdbuf[numread] = '\0';

	 	   //  printf("Full Duplex Server : Read From the   pipe : %s\n", rdbuf);

	/*	ACE_HANDLE handle = ::CreateNamedPipe(this->local_addr_.get_pipe_path(),
                                     PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                     //@@ If message mode, use this instead: PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
                                     PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, // To allow more than 64K in a single WriteFile()
                                     PIPE_UNLIMITED_INSTANCES,
                                     1024 * 10,
                                     1024 * 10,
                                     NMPWAIT_USE_DEFAULT_WAIT,
                                     this->secattr_);


	 if ( handle == INVALID_HANDLE_VALUE ) {
      if (ACS_TRA_ON(traPIPEAccept)) {
         ::sprintf(traStr_,"create_new_instance(); ERROR: CreateNamedPipe() failed with %s",MCS::getSystemErrorText());
         ACS_TRA_event(&traPIPEAccept,traStr_);
      }

      return -1;
   }
   else {
      // Start the Connect (analogous to listen() for sockets). Completion
      // is noted by the event being signalled in the overlapped structure.
      // If a client connects before this call, the error status will be
      // ERROR_PIPE_CONNECTED, in this case that fact is remembered in the
      // already_connected flag and used in the accept() call.
      // Error status of ERROR_IO_PENDING indicating an Ok status and the
      // event will be signalled when a client connects.
      this->already_connected_ = FALSE;
      this->set_handle( handle );
      this->overlapped_.hEvent = this->event_.handle();

      // Clear event
      this->event_.reset();

      // Issue a connect request
      IFDEBUG(BOOL res =) ::ConnectNamedPipe( handle, &this->overlapped_ );
      assert( res == FALSE );

      int status = ::GetLastError();
create_new_instance
      if ( status == ERROR_PIPE_CONNECTED ) {
         // Client was quick to connect...
         this->already_connected_ = TRUE;
      }
      else if ( status != ERROR_IO_PENDING ) {
         // Failure
         this->close();

         if (ACS_TRA_ON(traPIPEAccept)) {
            ::sprintf(traStr_,"create_new_instance(); ERROR: ConnectNamedPipe() failed with %s",MCS::getSystemErrorText(status));
            ACS_TRA_event(&traPIPEAccept,traStr_);
         }

         return -1;
      }

   }

   return ( this->hPipe_ == OSF_INVALID_HANDLE ? -1 : 0 );
*/
	return 1;
}





// Sets the underlaying pipe handle
void ACS_APGCC_PIPE_Acceptor_R1::set_handle(ACE_HANDLE h)
{

	hPipe_ = h;
}


// Return the underlaying event handle
ACE_event_t ACS_APGCC_PIPE_Acceptor_R1::get_event_handle() const
{
   return this->event_.handle();
}


// Return the underlaying pipe handle
HANDLE ACS_APGCC_PIPE_Acceptor_R1::get_handle() const
{

	if ( hPipe_ != ACE_INVALID_HANDLE )
      return this->hPipe_;
   else
      return ACE_INVALID_HANDLE;
}





ACS_APGCC_PIPE_Connector_R1::ACS_APGCC_PIPE_Connector_R1()
{}

ACS_APGCC_PIPE_Connector_R1::~ACS_APGCC_PIPE_Connector_R1()
{
}



bool ACS_APGCC_PIPE::Talk(const string& pipe_name, string& msg)
{

	ACS_APGCC_PIPE_Connector_R1 server;
	ACE_SPIPE_Stream pipe;
	 ACE_HANDLE aa;
	pipe.set_handle(aa);

	ACE_SPIPE_Addr      xx;
	bool ok = false;
	cout<<" server.connect(pipe,pipe_name)........"<<endl;
	cout<<" server.connect(pipe,pipe_name):"<<server.connect(pipe,xx)<<endl;
/*   if ( server.connect(pipe,pipe_name) == 0 ) {
      if ( pipe.SendMsg(msg) && pipe.RecvMsg(msg) )
         ok = true;
      pipe.close();
   }*/
   return ok;
}
