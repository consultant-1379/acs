#ifndef ACS_APGCC_SOCKETSTREAM_H
#define ACS_APGCC_SOCKETSTREAM_H


#  include <ace/INET_Addr.h>
# include <ace/SOCK_Connector.h>
#  include <ace/SOCK_Stream.h>
#  include "ACS_APGCC_Streams.H"




//
// Implementation class for FIFO File stream.
//============================================
//
class ACS_APGCC_SocketStream: public ACS_APGCC_IStream
{
public:

   // Default Constructor
   //---------------------------------------------
	ACS_APGCC_SocketStream()
      :close_stream_at_exit_(true)
   {
   }


   // Constructor which assign a stream handle
   //---------------------------------------------
	ACS_APGCC_SocketStream(ACE_HANDLE handle, bool take_over_ownership=true)
      :close_stream_at_exit_(take_over_ownership)
   {
      stream_.set_handle( handle );
   }


   // Constructor which assign a stream handle
   //---------------------------------------------
	ACS_APGCC_SocketStream(ACE_SOCK_Stream& stream, bool take_over_ownership=true)
      :close_stream_at_exit_(take_over_ownership)
   {
      stream_.set_handle( stream.get_handle() );
   }


   // Termination
   //---------------------------------------------
   ~ACS_APGCC_SocketStream()
   {
      this->Close();
   }


   // Called to open the stream
   // Address of remote server must be given
   // as <ipaddr>:<port>
   //---------------------------------------------
   bool Open(const std::string& addrstr)
   {
      ACE_INET_Addr addr( addrstr.c_str() );
      ACE_SOCK_Connector srv;

      // Connect to server
      if ( srv.connect(stream_,addr) < 0 ) {

         // Failed; No connection
         return false;
      }

      return true;
   } // Open


   // Called to open the stream
   //---------------------------------------------
   bool Open(const ACE_SOCK_Stream& stream, bool take_over_ownership=true)
   {
      close_stream_at_exit_ = take_over_ownership;

      stream_.set_handle( stream.get_handle() );

      return ( stream_.get_handle() != ACE_INVALID_HANDLE );
   } // Open


   // Called to close the stream
   //---------------------------------------------
   void Close()
   {
      if ( close_stream_at_exit_ && stream_.get_handle() != ACE_INVALID_HANDLE ) {
         stream_.close();
      }
   } // Close


   // Read an integer to the stream.
   //---------------------------------------------
   virtual bool ReadInt(int& out)
   {
      // Read int as string
      if ( !this->Read(&out,sizeof(out)) )
         return false;

      // Convert to host byte order
      out = ntohl( out );

      return true;
   }


   // Writes an integer to the stream.
   //---------------------------------------------
   virtual bool WriteInt(const int in)
   {
      // Convert to host byte order
      int tmp = htonl( in );

      return this->Write(&tmp,sizeof(tmp));
   }


   // Called to read data from the stream
   //---------------------------------------------
   virtual bool Read(void* buf, size_t len)
   {
      int rcvd = stream_.recv_n(buf,len);

      return (rcvd == len);
   } // Read


   // Called to write data to the stream
   //---------------------------------------------
   virtual bool Write(const void* buf, size_t len)
   {
      int sent = stream_.send_n(buf,len);

      return (sent == len);
   }


   // Returns true if stream is empty.
   //---------------------------------------------
   virtual bool Empty() const
   {
      if ( this->handle() != ACE_INVALID_HANDLE ) {

         fd_set rset;
         ACE_Time_Value tv;

         FD_ZERO( &rset );
         FD_SET( this->handle(), &rset );

         tv.msec( 500 );

         int n = ACE_OS::select( this->handle()+1, &rset, 0, 0, &tv );

         if ( n > 0 ) {
            return false;
         }
      }

      return true;
   } // Empty


   // Internal; Do not use!
   // Get access to internal handle
   //---------------------------------------------
   ACE_HANDLE handle() const
   {
      return stream_.get_handle();
   }


private:

   bool close_stream_at_exit_;
   // True if stream should be closed at exit/closure

   ACE_SOCK_Stream stream_;
   // Communication stream


private:

   /*
    *
    * NOT IMPLEMENTED FUNCTIONS BELOW
    *
    */

   // Returns current position in the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual long GetPos()
   {
      return 0;
   } // GetPos


   // Sets the current position in the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool SetPos(long distance, size_t moveMethod)
   {
      return false;
   } // SetPos


   // Lock the access to the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool Lock()
   {
      return false;
   }


   // Unlock the access to the stream
   // May not be possible for all types of streams
   //---------------------------------------------
   virtual bool Unlock()
   {
      return false;
   }


};



#endif
