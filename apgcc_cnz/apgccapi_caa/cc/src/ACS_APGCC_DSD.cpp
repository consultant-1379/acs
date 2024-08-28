/*
 * ACS_APGCC_DSD.cpp
 *
 *  Created on: Apr 9, 2010
 *      Author: root
 */

#include <ace/OS.h>
#include "ACS_APGCC_DSD.H"
#include "ACS_APGCC_Util.H"
#include "ACS_TRA_trace.h"

#ifdef ACE_NLOGGING
#  undef DSD_DUMP_HEX
#else
  #undef DSD_DUMP_HEX
#endif


using namespace std;
//#define ACS_DSD_MAXBUFFER 64000
#define ACS_DSD_MAXBUFFER 524283


//Defined trace points
//--------------------
/*
ACS_TRA_trace traDSDAddr    = ACS_TRA_DEF ("ACS_APGCC_DSD_Addr     ", "C150");
ACS_TRA_trace traDSDAccept  = ACS_TRA_DEF ("ACS_APGCC_DSD_Acceptor ", "C150");
ACS_TRA_trace traDSDStream  = ACS_TRA_DEF ("ACS_APGCC_DSD_Stream   ", "C150");
ACS_TRA_trace traDSDConnect = ACS_TRA_DEF ("ACS_APGCC_DSD_Connector", "C150");
*/

//--------------------------------------------------
// MCS DSD Address
//--------------------------------------------------
namespace {
	ACS_TRA_trace trace(const_cast<char*>("ACS_APGCC_DSD"), const_cast<char *>("C300"));
}

ACS_APGCC_DSD_Addr::ACS_APGCC_DSD_Addr()
:network_(1), port_(0), ip_(0)
{
}


ACS_APGCC_DSD_Addr::ACS_APGCC_DSD_Addr(const char* addr)
:network_(1), port_(0), ip_(0)
{
   this->set(addr);
}


ACS_APGCC_DSD_Addr::ACS_APGCC_DSD_Addr(const char* addr, const char* node)
:network_(1), port_(0), ip_(0)
{
   this->set(addr);
   node_   = node;
}


ACS_APGCC_DSD_Addr::ACS_APGCC_DSD_Addr(const ACS_APGCC_DSD_Addr& addr)
:network_(addr.network_), port_(addr.port_), ip_(addr.ip_)
{
   this->set(addr);
}


ACS_APGCC_DSD_Addr::~ACS_APGCC_DSD_Addr()
{
}


ACS_APGCC_DSD_Addr& ACS_APGCC_DSD_Addr::operator=(const ACS_APGCC_DSD_Addr& addr)
{
   if ( this != &addr )
      this->set(addr);
   return *this;
}

void ACS_APGCC_DSD_Addr::set(const char* addr)
{
	appl_  = ACS_APGCC::before(addr,":");
	domain_ = ACS_APGCC::after(addr,":");
   node_   = "";
   network_= 1;
   port_   = 0;
   ip_     = 0;
}

void ACS_APGCC_DSD_Addr::set(const ACS_APGCC_DSD_Addr& addr)
{
   domain_  = addr.domain_;
   appl_    = addr.appl_;
   node_    = addr.node_;
   network_ = addr.network_;
   port_    = addr.port_;
   ip_      = addr.ip_;
}

void ACS_APGCC_DSD_Addr::setNetwork(const int network)
{
   network_ = network;
}

const char* ACS_APGCC_DSD_Addr::getDomain() const
{
   return domain_.c_str();
}


const char* ACS_APGCC_DSD_Addr::getAppl() const
{
   return appl_.c_str();
}


const char* ACS_APGCC_DSD_Addr::getNode() const
{
   return node_.c_str();
}


 int ACS_APGCC_DSD_Addr::getNetwork() const
{
   return network_;

}




//--------------------------------------------------
// ACS_APGCC_DSD_Stream
//--------------------------------------------------

ACS_APGCC_DSD_Stream::ACS_APGCC_DSD_Stream()
{
}

ACS_APGCC_DSD_Stream::~ACS_APGCC_DSD_Stream()
{
 //  dsdStream_.close();
}


//*******************************************************************
// recv_n()
// Receives a buffer from the CP blocking.
//*******************************************************************
int ACS_APGCC_DSD_Stream::recv_n(void* buf, size_t len, bool wrapData)
{
   return this->recv(buf,len,wrapData);
}


//*******************************************************************
// recv()
// Receives a buffer from the CP blocking.
//*******************************************************************
int ACS_APGCC_DSD_Stream::recv(void* buf, size_t bufSize, bool wrapData)
{
//return 1;

	// qvincon begin
   // Receive the message from DSD
   u_int msgSize = bufSize;
   //only remove warning

   wrapData=true;
   if ( dsdStream_.recv(buf,msgSize) <= 0  ) { // DSD returns a negative value if an error occurs, else it returns the number of bytes read
      return -1;
   }


   int len = msgSize;


#ifdef DSD_DUMP_HEX
   OSF_HEX_DUMP((LM_DEBUG,(const char*)buf,len+4,"(%t) ACS_APGCC_DSD_Stream::recv(); received from DSD:"));
#endif
	// QVINCON
	// They have been removed for the new one DSD
	/*
   if ( wrapData ) {

      // Wrap the four last positions of the buffer into
      // the first four positions
      this->unwrap_data(buf,len);

   }
   else if ( len > 0 ) {
      // Note that DSD returns the mesage size minus the size of the header
      // We return the total received size, which means that we increase it by 4
      len += 4;
   }
*/
   if ( len <= 0 ) {
      return -1;
   }


#ifdef DSD_DUMP_HEX
   OSF_HEX_DUMP((LM_DEBUG,(const char*)buf,(wrap_data?len:len+4),"(00) ACS_APGCC_DSD_Stream::recv(); returning data:"));
#endif

   return len;

}


//*******************************************************************
// recvBuffer()
// Receives a buffer blocking.
// Returns an allocated buffer !!
// The buffer must be freed by the user of this funciton.
//*******************************************************************
int ACS_APGCC_DSD_Stream::recvBuffer(char*& buf, bool wrapData)
{
   return this->recvBuffer( (unsigned char*&)buf, wrapData );
}


int ACS_APGCC_DSD_Stream::recvBuffer(unsigned char*& buf, bool wrapData)
{

   int len;
   //unsigned short allocSize = 0;

	// fixed dimension for the buffer of 64k
	unsigned int allocSize = ACS_DSD_MAXBUFFER;

   // Clear buffer
   buf = 0;
   // QVINCON
   // This method has been removed
//	if ( this->dsdStream_.getAllocSize(allocSize) ) {

      // Allocate a large enough buffer
      //---------------------------------

      buf = new (std::nothrow) unsigned char[ allocSize + 5 ];
      if ( !buf ) {
         //if (ACS_TRA_ON(traDSDStream)) {
         //   ::sprintf(traStr_,"recvBuffer(); ERROR: failed to allocate buffer of size %u",allocSize);
         //   ACS_TRA_event(&traDSDStream,traStr_);
         //}

         return -1;
         //NOTREACHED
      }

      // Receive message
      //------------------

      len = this->recv( buf, allocSize, wrapData );

		if ( len <= 0 ) {
         delete [] buf;
         return -1;
         //NOTREACHED
      }
	// QVINCON
	// They have been removed for the new one DSD
	/*
      if ( wrapData ) {

         // End buffer
         buf[len] = 0;

      }
      else if ( len >= 4 ) {

         // Decrease for DSD header size
         len -= 4;

         // Move the data four bytes down
         ::memmove( buf, &buf[4], len );

      }
      else {
         delete [] buf;

         if (ACS_TRA_ON(traDSDStream)) {
            ::sprintf(traStr_,"recvBuffer(); ERROR: received buffer too small to remove header;  len=%i",len);
            ACS_TRA_event(&traDSDStream,traStr_);
         }

         return -1;
         //NOTREACHED
      }
*/
      // End buffer
      buf[len] = 0;

#ifdef DSD_DUMP_HEX
      if ( len > 0 ) {
         ACS_APGCC::dump_hex(cp,(len>256?256:len),"ACS_APGCC_DSD_Stream::recvBuffer(); received data:");
         OSF_HEX_DUMP((LM_DEBUG,msg,(len>256?256:len),"(%t) ACS_APGCC_DSD_Stream::recvBuffer(); received DSD data:"));
      }
#endif

      return len;

 /* if ... getAllocSize
  }
   else {

      if (ACS_TRA_ON(traDSDStream)) {
         ::sprintf(traStr_,"recvBuffer(); ERROR: getAllocSize() failed with %s",this->getErrorText());
         ACS_TRA_event(&traDSDStream,traStr_);
      }

      return -1;
   }
*/

/***
   // Copy data

   if ( wrapData ) {

      memcpy(buf,msg,len);
      //@@ len -= 4;
      //@@ Already unwrapped by recv(); this->unwrap_data(buf,len);

   }
   else {
      len -= 4;
      memcpy(buf,&msg[4],len);

   }


   // End the buffer with a NULL sign
   cp[len] = 0;



   if ( len <= 0 ) {
      delete [] buf;
      buf = 0;
      return -1;
   }


#ifdef DSD_DUMP_HEX
   if ( len > 0 )
      //MCS::dump_hex(buf,(len>256?256:len),"ACS_APGCC_DSD_Stream::recvBuffer(); received data:");
      OSF_HEX_DUMP((LM_DEBUG,(const char*)buf,(len>256?256:len),"(%t) ACS_APGCC_DSD_Stream::recvBuffer(); returning received data:"));
#endif

   return len;
****/
}


//*******************************************************************
// send_n()
// Sends a buffer to the CP.
//*******************************************************************

int ACS_APGCC_DSD_Stream::send_n(const void* buf, size_t len, bool wrapData)
{
   return this->send(buf,len,wrapData);
}


//*******************************************************************
// send()
// Sends a buffer to the CP.
//*******************************************************************

int ACS_APGCC_DSD_Stream::send(const void* buf, size_t len, bool wrapData)
{
   // QVINCON
	// They have been removed for the new one DSD
	/*
   if ( wrapData ) {
      // Wrap the first four bytes of the buffer into
      // the four positions after the last used buffer position
      this->wrap_data((void*)buf,len);
   }
   else {
      // increase for header size
      len += 4;
   }
   */
	//only remove warning
	wrapData=true;
	size_t bufSize = len;

	// XCASALE: Use the new of "ACS_DSD_Session::sendf", in order to
	// avoid getting SIGPIPE signal and in order to return only an error
	if ( dsdStream_.sendf(buf, bufSize, MSG_NOSIGNAL) < 0 ) {
		if (trace.ACS_TRA_ON()){
				char buffer[1024];
				snprintf(buffer,1024,"KO!!! failed: error code == %d: error text == '%s' ",	 dsdStream_.last_error(),dsdStream_.last_error_text());
				trace.ACS_TRA_event(1, buffer);
		}
      return -1;
   }

#ifdef DSD_DUMP_HEX
   OSF_HEX_DUMP((LM_DEBUG,(const char*)buf,len,"(00) ACS_APGCC_DSD_Stream::send(); sent data:"));
#endif

   return len;
}


//*******************************************************************
// recv()
// Receives a buffer from the CP blocking.
//*******************************************************************

int ACS_APGCC_DSD_Stream::recv_cp(void* buf, size_t bufSize, bool moveData)
{

   // The CP sends data as we normally do within APs
   // the first four bytes is thus NOT copied from the back.

   // Receive DSD message without unwrapping it
   int len;
   //only remove warning
   moveData=true;
   len = this->recv(buf,bufSize,false);
   if ( len <= 0 ) {
      return len;
   }


	// QVINCON
	// They have been removed for the new one DSD
	/*
	// Move data if necessary

   unsigned char* cp = (unsigned char*) buf;

   if ( moveData ) {

      // Move the data downwards to remove the header info

      memcpy(cp, &cp[4], len);

      len -= 4;
   }


#ifdef DSD_DUMP_HEX
   //MCS::dump_hex(cp,len,"ACS_APGCC_DSD_Stream::recv_cp(); received data:");
   OSF_HEX_DUMP((LM_DEBUG,(const char*)cp,(moveData?len:len+4),"(%t) ACS_APGCC_DSD_Stream::recv_cp(); received data:"));
#endif
*/
   return len;
}


//*******************************************************************
// send_cp()
// Sends a buffer to the CP.
//*******************************************************************

int ACS_APGCC_DSD_Stream::send_cp(const void* buf, size_t len)
{
   // Data is sent to the CP wrapped

   return this->send(buf,len,true);
}


/***** Removed methods
*******************************************************************
// send_rpc()
// Pre-sends an RPC header before the data.
// The data remains unchanged.
// BE AWARE: Not to be used after SIO I4.
*******************************************************************

int ACS_APGCC_DSD_Stream::send_rpc(const void* buf, size_t len)
{

   if (impl_)
      return impl_->send_rpc_i(buf,len);
   else
      return -1;
}


*******************************************************************
// recv_rpc()
// Receives RPC data.
// The data buffer is untouched.
// BE AWARE: Not to be used after SIO I4.
*******************************************************************

int ACS_APGCC_DSD_Stream::recv_rpc(void* buf, size_t bufSize)
{

   // Receive the RPC header from our internal socket
   unsigned char head[4];

   if ( sockStream_.recv_n(head,4) <= 0 )
      return -1;

   // Calculate data size and receive message
   size_t len = 256*head[2] + head[3];

   unsigned char* cp = (unsigned char*) buf;

   // Receive data
   if ( sockStream_.recv_n(cp,len) <= 0 )
      return -1;

#ifdef DSD_DUMP_HEX
   //MCS::dump_hex(cp,len,"ACS_APGCC_DSD_Stream::recv_rpc(); received data:");
   OSF_HEX_DUMP((LM_DEBUG,(const char*)cp,len,"(%t) ACS_APGCC_DSD_Stream::recv_rpc(); received data:"));
#endif

   return len;
}

*****************/


//*******************************************************************
// close()
// Close down the stream.
//*******************************************************************

int ACS_APGCC_DSD_Stream::close()
{

 //  dsdStream_.close();

   return 0;
}


//*******************************************************************
// get_handle()
// Returns the file descriptor of the stream.
//*******************************************************************

ACE_HANDLE ACS_APGCC_DSD_Stream::get_handles(acs_dsd::HANDLE * handles, int & handle_count)
{
   ACE_HANDLE hDsd = 0;

   // Retrieve the DSD server handle
	hDsd = dsdStream_.get_handles(handles,handle_count);
	if ( hDsd >= 0)
		return hDsd;
   else
      return ACE_INVALID_HANDLE;
}


void ACS_APGCC_DSD_Stream::wrap_data(void* buf, size_t& len)
{

   // Get a pointer to the message
   unsigned char* cp = (unsigned char*) buf;

   if ( len > 4 ) {

      // If data is wrapped, move the first four bytes
      // to the end

      cp[len]   = cp[0];
      cp[len+1] = cp[1];
      cp[len+2] = cp[2];
      cp[len+3] = cp[3];

   }
   else {

      // less than header size, move to pos 4

      cp[4] = cp[0];
      cp[5] = cp[1];
      cp[6] = cp[2];
      cp[7] = cp[3];
   }

   // Set RPC header
   cp[0] = 0;
   cp[1] = 0;
   cp[2] = (len / 256) % 256;
   cp[3] = len % 256;

   // Increase message size by the size of RPC header
   len += 4;
}


void ACS_APGCC_DSD_Stream::unwrap_data(void* buf, int& len)
{

   // Get a pointer to the message
   unsigned char* cp = (unsigned char*) buf;

   if ( len > 4 ) {

      // If data is wrapped, move the last four bytes
      // to the beginning

      cp[0] = cp[len];
      cp[1] = cp[len+1];
      cp[2] = cp[len+2];
      cp[3] = cp[len+3];

   }
   else {

      // less than header size, move from pos 4

      cp[0] = cp[4];
      cp[1] = cp[5];
      cp[2] = cp[6];
      cp[3] = cp[7];
   }

}


size_t ACS_APGCC_DSD_Stream::getError()
{
   //return dsdStream_.getError();

	// only change the method's name
	return dsdStream_.last_error();

}


const char* ACS_APGCC_DSD_Stream::getErrorText()
{
   //return dsdStream_.getErrorText();

	// only change the method's name
	return dsdStream_.last_error_text();

}



//--------------------------------------------------
// ACS DSD Connector
//--------------------------------------------------


ACS_APGCC_DSD_Connector::ACS_APGCC_DSD_Connector()
{
}


ACS_APGCC_DSD_Connector::~ACS_APGCC_DSD_Connector()
{
}


int ACS_APGCC_DSD_Connector::connect(ACS_APGCC_DSD_Stream& newStream, const ACS_APGCC_DSD_Addr& addr)
{

	/* old DSD client
   bool connected = dsdCon_.connect( newStream.dsdStream_,
                                    (char*)addr.getAppl(),
                                    (char*)addr.getDomain() );
*/
	// new DSD client interface
	const ACS_DSD_Node node;
	//get_remote_node;

	//host.systemId = DSD_API::THISNODE;

	// the new connect method demand an extra parameter
	bool connected = dsdCon_.connect( newStream.dsdStream_,(char*)addr.getAppl(), (char*)addr.getDomain());
	return connected;

}


int ACS_APGCC_DSD_Connector::close()
{
   return 0;
}


size_t ACS_APGCC_DSD_Connector::getError()
{
   //return dsdCon_.getError();

	// only change the method's name
	return dsdCon_.last_error();

}


const char* ACS_APGCC_DSD_Connector::getErrorText()
{
   //return dsdCon_.getErrorText();

	// only change the method's name
	return dsdCon_.last_error_text();

}




ACS_APGCC_DSD_Acceptor::ACS_APGCC_DSD_Acceptor()
{
   // Create DSD server object
   //dsdServer_ = new ACS_DSD_Service;

	// Create the new DSD server object.
	// It must specify the type of IPC mechanism for the DSD server
	dsdServer_ = new ACS_DSD_Server();

}


ACS_APGCC_DSD_Acceptor::~ACS_APGCC_DSD_Acceptor()
{
 	 delete dsdServer_;
}


int ACS_APGCC_DSD_Acceptor::open(const string& appl, const string& domain)
{
   string tmp;
   ACS_APGCC_DSD_Addr addr;
   tmp  = appl;
   tmp += ":";
   tmp += domain;
   addr.set(tmp.c_str());

   return this->open(addr);
}


#define ACS_DSD_INIT_ERROR 9804  // Equals ACS_DSD_initError which is INTERNAL in DSD!!!

int ACS_APGCC_DSD_Acceptor::open(const ACS_APGCC_DSD_Addr& dsdaddr)
{
	// the new DSD server object demands the call of the open method
	// before the publish method.
	if ( dsdServer_->open(acs_dsd::SERVICE_MODE_INET_SOCKET)<0 )
	{
		if (trace.ACS_TRA_ON()){
				char buffer[1024];
				snprintf(buffer,1024,"KO!!## ERROR: 'server.open(...' failed!\n"
										"      last error == %d\n"
										"      last error text == '%s'\n", dsdServer_->last_error(), dsdServer_->last_error_text());

				trace.ACS_TRA_event(1, buffer);
		}

		return -1;
	}
	// Publish the DSD server
	  int  published = dsdServer_->publish(dsdaddr.getAppl(), dsdaddr.getDomain());
	  if ( published<0 ) {

      // Failed to publish server; Try to remove it
		// Probably a "already initiated" error, which means
      // that this DSD service object is useless
      // Destroy it and create a fresh service object
		this->close();
   }
   return published;



}


int ACS_APGCC_DSD_Acceptor::accept(ACS_APGCC_DSD_Stream& newStream)
{
	ACS_APGCC_DSD_Addr addr;
   return this->accept(newStream,addr);
}


int ACS_APGCC_DSD_Acceptor::accept(ACS_APGCC_DSD_Stream& newStream, ACS_APGCC_DSD_Addr& addr)
{
	ACE_UNUSED_ARG(addr);
	// Accept the DSD client
	int accepted = dsdServer_->accept(newStream.dsdStream_);
  /* these methods have been removed in the new DSD
  if ( accepted ) {

      // Update address object
      newStream.dsdStream_.getRemoteAddress( addr.remDsdAddr_ );
      newStream.dsdStream_.getLocalAddress( addr.locDsdAddr_ );

   }
  */
return accepted;
}


int ACS_APGCC_DSD_Acceptor::close()
{

   // Unregister server name from DSD
   dsdServer_->unregister();

	// before destroying the DSD server object it must close it
	dsdServer_->close();

   // The reason for this business of deleting and creating
   // new DSD server objects all the time, is because once the
   // DSD server object has been created, it cannit be reused.
   // DSD returns a fault code, so we must do this crazy thing
   delete dsdServer_;

   // Create new server object
   // dsdServer_ = new ACS_DSD_Service;
	dsdServer_ = new ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET);

   return 0;

}


ACE_HANDLE ACS_APGCC_DSD_Acceptor::get_handle(int index)
{

	//ACE_HANDLE hDsd[index+1];
	ACE_HANDLE hDsd[acs_dsd::CONFIG_NETWORKS_SUPPORTED+1];

   int nDsdHandles = acs_dsd::CONFIG_NETWORKS_SUPPORTED+1;
  if ( dsdServer_->get_handles(hDsd,nDsdHandles) < 0 )
   {
	   return ACE_INVALID_HANDLE;
   }
   if ( index >= 0 && index < nDsdHandles )
      return hDsd[index];
   else
      return ACE_INVALID_HANDLE;
}


size_t ACS_APGCC_DSD_Acceptor::getError()
{

	//return dsdServer_->getError();

	// only change the method's name
	return dsdServer_->last_error();

}
size_t getLastError(void);

const char* ACS_APGCC_DSD_Acceptor::getErrorText()
{
	// only change the method's name
	return dsdServer_->last_error_text();
}

