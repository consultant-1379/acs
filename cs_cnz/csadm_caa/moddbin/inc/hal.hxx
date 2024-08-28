//  **************************************************************************
//  * ********************************************************************** *
//  * *                                                                    * *
//  * *  COPYRIGHT (C) Ericsson Utvecklings AB 2000                        * *
//  * *  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             * *
//  * *                                                                    * *
//  * *  The computer program(s) herein may be used and/or copied only     * *
//  * *  with the written permission of Ericsson Utvecklings AB or in      * *
//  * *  accordance with the terms and conditions stipulated in the        * *
//  * *  agreement/contract under which the program(s) have been supplied. * *
//  * *                                                                    * *
//  * ********************************************************************** *
//  **************************************************************************
//
//  File Name ........@(#)fn 
//  Document Number ..@(#)dn 
//  Revision Date ....@(#)rd 
//  Current Version ..@(#)cv 
//  Responsible ............ UAB/B/TP Tobias Carlsson, Magnus Forsstrom
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory

//  @(#)overview
//----------------------------------------------------------------------------
//  This header file contains class declarations of the HAL interface.
//----------------------------------------------------------------------------
#ifndef HAL_HXX
#define HAL_HXX

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "halPlatformDefinitions.hxx"
#include "halHardwareSpecific.hxx"

#if defined(_TRU64) || defined(IPF_LINUX_) || defined(X86_LINUX_)
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>
#include "upbb.h"


#if defined(_TRU64)
#include <c_asm.h>
#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>
#endif //_TRU64

#if defined(IPF_LINUX_)
#if defined(__INTEL_COMPILER)
#include <ia64intrin.h>
#endif //__INTEL_COMPILER
#endif //IPF_LINUX_

#if defined(X86_LINUX_)
#include <asm/cpufeature.h>
#endif //X86_LINUX_

#elif defined(_WIN32)
#include <winbase.h>

#else
#error Hal does not support your platform, __FILE__, __LINE__
#endif //_TRU64

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------
//
// There are more than one class declaration in this file, which is not 
// according to the "Gemini C++ design rules and recommendations".
//
// The methods that are believed to be frequently called are using the
// inline directive.

#define HALNOFILE -1
#define RCC_CONFIGURATION_BIT INT64_C_(0x8000000000000000)


typedef int HalHdiHandle;
namespace HalInternal
{
   const char HalVersion[] = "COA101056_P6A_1";
}

//
// HAL STATUS 
//
class Hal
{
public:
   static void version(char *version, int len);

   //This enum defines all possible errror codes returned by HAL
   enum HalStatus 
   {
      OK = 0,
      FAIL,         //Non fatal errors might want to try again
      ALREADYOPEN,
      NODEVICE,     //Unrecoverable errors
      NOTSUPPORTED, //No support for this platform
      EFATAL,       //Fatal error!
      ERIOCTLFAIL,  //An IOCTL to the driver failed - see the LastStatus in the Handle

      EMAXREGIONENTRIES,//Too many region entries for entering UniDirectional Mode
      EBADREGIONCOUNT,  //The number of entries requested doesn't match ours
      EBADREGIONCOUNTNONAK,  //Same but we failed to send a NAK back to the master!
      EBADREGIONLENGTH,      //A mismatched region length
      EBADREGIONLENGTHNONAK, //Same but we failed to send a NAK
      EOFFSETTOOBIG,         //A Region offset is larger than 56 bits!
      EREGIONSMAPPED,   //The regions are already mapped - cannot re-map them!
      EBADREGIONVA,     //The Usermode address supplied to the Driver is not valid
      EBADREGIONNOMAP,  //The driver was unable to Map the Usermode Buffer in one PCI region
      EBADSERVER_NOACK, //Tried to send an ACK for server mode and failed!
      EBADSERVER_NOACKSTILLSLAVE,  // And couldn't even drop out of Slave mode :-(
    
      EBADFLAGS,      //A "Flags" word containied an unexpected flag bit.
      EBADWRITELOG,   //A WriteLog Entry has an unexpected Byte Count
      EBADPARAMETER,  //A function was passed an Invalid PArameter - usually NULL pointers!
    
      EBADBUILD,      //Driver and Library mismatch build levels
      EPACKETTOOBIG,  //An attempt was made to read/write too many bytes
      ENOTSIGNAL,     //The received packet was NOT a signal packet
      EBADSIGNAL,     //The received packet was a signal but the check word failed
      EBADSIGNALSIZE, //The data size for the signal was unexpected! 
      ERESYNC         //Error in transfer. Need to resyncronise both sides. 
   };
   //
   //This enum defines the different memory barrier types
   //these constants are passed as parameters to the 
   //memory barrier routine
   enum HalMbMask
   {
      READ_READ,
      READ_WRITE,
      WRITE_READ,
      WRITE_WRITE
   };
};

//
// HAL DRIVER INTERFACE
//
class HalHdi
{
public:
   HalHdi();
   virtual ~HalHdi();
   //
   //Enum that defines the devices that are supported by message drivers
   enum HalHdiName
   {
      CPUB = 1,
      CPTB,
      KLOG
   };
   
protected:
   HalHdiHandle m_handle; //The file handle
};

//
//This is the interface routines to the message drivers
class HalHdiMsg : private HalHdi 
{
public:
   enum
   {
      CPXB_MAX_SIGNAL_SIZE = 256,
      // Maximum size of messages retrieved from the log device
      KLOG_MAX_SIZE = 200
   };
   enum ResyncFlags
   {
      SYNC_ALL = 1,
      FORCE_TIMEOUT = 2
   };
   /* The type of interrupt to test */
   enum IntTestType
   {
      NXM, //Non eXistent Memory
      NXIO //Non eXistent IO
   };
   HalHdiMsg();
   Hal::HalStatus open(HalHdiName name); 
   Hal::HalStatus close();   
   Hal::HalStatus read(u_int8 *data, u_int32 &size);
   Hal::HalStatus write(const u_int8 *data, u_int32 size);
   Hal::HalStatus resync(int flags);
   Hal::HalStatus testInterrupt(enum IntTestType);
private:
   typedef struct 
   {
      u_int8 u8SigNo[4] ;
      u_int8 u8TestData[248];
   }CptbCpubEchoSig;
   enum ResyncState
   {
      SYNC_SEND_ECHO,
      SYNC_WAIT_FOR_ECHO_REPLY,
      SYNC_TIMEOUT = 60000000 //ns
   };
   void createEchoSig(u_int64 pattern);
   ResyncState m_syncState;
   /* Data pattern for echo signal */
   u_int64 m_syncPattern;
   CptbCpubEchoSig m_testSig;
#ifdef __unix
   struct timespec m_syncStart;
#endif
}; 


// REAL_TIME CLOCK
class HalRtc 
{
public:
   HalRtc();
   Hal::HalStatus available();
   Hal::HalStatus open();
   Hal::HalStatus getBits(u_int32 &noOfBits);
   Hal::HalStatus getFrequency(u_int64 &rtcFreq);
   Hal::HalStatus getValue(u_int64 &value, u_int8 &isReset);
   Hal::HalStatus setValue(u_int64 value);
private: 
   HalHdiHandle m_handle; 
};

// MAS error register
class HalMasErr
{
public:
   HalMasErr();
   Hal::HalStatus open();
   Hal::HalStatus close();
   Hal::HalStatus read(u_int32 &regVal);
   Hal::HalStatus write(u_int32 regVal);
  int fdHandle() { return m_handle; }
private: 
   HalHdiHandle m_handle; 
};

// 
//The interface to CPU specific functionality
class HalCpu
{
public:
   static Hal::HalStatus CCAvailable();
   static Hal::HalStatus CCGetBits(u_int32 &noOfBits);
   static Hal::HalStatus CCGetFrequency(u_int64 &ccFreq);
   static Hal::HalStatus CCGetValue(u_int64& CCVal);
   static void memBarrier(Hal::HalMbMask mask);
   static Hal::HalStatus memGetCacheLineSize(u_int32 &clSize);
   static Hal::HalStatus cntLeadingZero(u_int64 value, u_int8 &zeros);
};

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Returns the HAL version number.
//
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  Version pointer to a buffer where the version string will be put.
//  len      The buffer size in bytes.
//
//----------------------------------------------------------------------------
inline void Hal::version(char* version, int len)
{
   len--; //Reserve last position for NULL
   if(len > 0)
   {
      strncpy(version, HalInternal::HalVersion, len); 
      //Make sure the string is terminated
      version[len] = '\0';
   }
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The method reads data from the device associated to the member variable
//  m_handle. The size pointer is an input/output argument. Before invoking 
//  this method, allocate a buffer which should be no smaller than the 
//  largest signal possible to receive. If using CPXB-buffers, the largest
//  signal is 256 bytes, use the CPXB_MAX_SIGNAL_SIZE definition when allo-
//  cating and when assigning a value to the parameter. When returning from
//  the method, size contains to the number of bytes actually passed along to
//  the caller. The method returns ESUCCESS or an error code. (If zero bytes 
//  are read, and no errors occured the method returns ESUCCESS.)
//
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  data - A pointer to a location where the read data will be put
//  size - An in/out parameter. In the maximum number of bytes to read.
//
//  OUTPUT  DESCRIPTION
//  size - An in/out parameter. Out the actual number of bytes read into 
// the location specified by the data pointer.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//  Hal::NODEVICE - Fatal error
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalHdiMsg::read(u_int8 *data, u_int32& size)
{ 
#if defined(__unix)
   int32 status;
   //
   // comment on the typeconversion:
   // For Tru64 the read system call returns a 64 bit integer.
   // HAL interface is defined to receive a 32 bit unsigned
   // integer. An error will occur if the size of the data
   // read is over about 2 000 000 000 bytes.
   //
   status = static_cast<int32>(::read(m_handle, data, size));
   if(status == -1)
   {
      size = 0; // No data available
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;
      }
   }
   else
   {
      size = status; //Copy number of bytes read
      return Hal::OK; 
   }
#else
   return Hal::NOTSUPPORTED; 
#endif //__unix
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The method writes to "the device" associated with the m_handle member
//  variable. The method writes as many bytes as specified by the size
//  parameter. The method returns ESUCCESS or an error code.
//
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  data - A pointer to the data to write
//  size - The number of bytes to write
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//  Hal::NODEVICE - Fatal error
//  Hal::NOTSUPPORTED - Functionality is unavailable  
//----------------------------------------------------------------------------
inline Hal::HalStatus HalHdiMsg::write(const u_int8 *data, u_int32 size)
{
#if defined(__unix)
   if(::write(m_handle, data, size) == -1)
   {
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;  
      }
   }
   else
   {
      return (Hal::OK); 
   }
#else
   return Hal::NOTSUPPORTED; 
#endif //__unix 
} 

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Determines if the Cycle Clock functionality is available.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//
//  OUTPUT  DESCRIPTION
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Functionality is available
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::CCAvailable()
{
#if defined(ALPHA_TRU64_) //|| defined(SPARC_SOLARIS_)
   return Hal::OK;
#elif defined(IPF_LINUX_) || defined(X86_LINUX_)
   return Hal::OK;
#elif  defined(ALPHA_WIN32_) || defined(INTEL_WIN32_)
   return Hal::OK;
#else
   return Hal::NOTSUPPORTED;
#endif //ALPHA_TRU64_
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Read the number of bits in the cycle counter. The value is stored 
//  in the parameter by noOfBits
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  noOfBits - A reference to a location where the result will be stored
//
//  OUTPUT  DESCRIPTION
//  The number of bits in the cycle counter
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::CCGetBits(u_int32 &noOfBits)
{
#if defined(ALPHA_TRU64_) || defined(SPARC_SOLARIS_)
   noOfBits = 64; 
   return Hal::OK;
#elif defined(IPF_LINUX_) || defined(X86_LINUX_)
   noOfBits = 64; 
   return Hal::OK;
#elif defined(ALPHA_WIN32_) || defined(INTEL_WIN32_)
   noOfBits = 32; 
   return Hal::OK;
#else
   return Hal::NOTSUPPORTED;
#endif //ALPHA_TRU64_ || SPARC_SOLARIS_
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Read the cycle clock frequency in Hz. The frequency is stored in the 
//  parameter ccFreq.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  ccFreq - A reference to a location where the result will be stored.
//
//  OUTPUT  DESCRIPTION
//  The frequency of the cycle counter
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Failed.
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::CCGetFrequency(u_int64 &ccFreq)
{
#if defined(_TRU64)
   struct cpu_info cpuStruct;
   
   if(getsysinfo(GSI_CPU_INFO, (caddr_t) &cpuStruct,
                 sizeof(cpuStruct), 0) > 0)
   {
      ccFreq = cpuStruct.mhz * 1000000; 
      return Hal::OK;
   }
   else
   {
      return Hal::FAIL;
   }

#elif defined(_WIN32)
//Note that the value returned is not the actual frequency on some machines
   LARGE_INTEGER qpc;
   QueryPerformanceFrequency(&qpc);
   ccFreq = 0;
   
   ccFreq = static_cast<u_int64>(qpc.QuadPart);
   
   // Return whether the CPU frequency could be read or not.
   return ccFreq != 0 ? Hal::OK : Hal::NOTSUPPORTED;

#elif defined(__linux)
   FILE* fp;
   char buffer[1024];
   size_t length;
   char* match;
   float clockSpeed;
   static u_int64 cpu_freq=0;

   if(cpu_freq!=0)
   {
      ccFreq=cpu_freq;
      return Hal::OK;
   }

   fp = fopen("/proc/cpuinfo", "r");
   length = fread(buffer, 1, sizeof(buffer), fp);
   fclose(fp);
   if (length == 0 || length == sizeof(buffer))
      return Hal::FAIL;
   buffer[length] = '\0';
   match = strstr( buffer, "itc MHz");
   if (match == NULL)
   {
      match = strstr( buffer, "cpu MHz");
      if (match == NULL) 
         return Hal::FAIL;
   }
   match = strstr(match, ":");
   if (match == NULL)
      return Hal::FAIL;
   ++match; // move past the ":"
   clockSpeed = strtof(match, NULL);
   ccFreq = static_cast<u_int64>(clockSpeed * 1000000);
   if (ccFreq == 0)
      return Hal::FAIL;
   cpu_freq=ccFreq;
   return Hal::OK;

#else //Not TRU64, WIN32 or Linux
   // XXX Solaris Support
   return Hal::NOTSUPPORTED; 
#endif //_TRU64
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Read the cycle clock
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  CCVal  - A reference to a location where the result will be stored.
//
//  OUTPUT  DESCRIPTION
//  The current clock cycle value. The CC value is stored in the lower 32 bits.
//  Note that 64 bits are returned which contain OS specific parts. The 
//  CCGetBits routine tells how many bits of the CC that contain the 
//  actual counter.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality unavailable.
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::CCGetValue(u_int64 &CCVal)
{
#if defined(__unix)
   CCVal = RPCC();
   return Hal::OK;

#elif defined(_WIN32)
/*
   LARGE_INTEGER counterValue;

   if ( QueryPerformanceCounter( &counterValue ) )
   {
      CCVal = counterValue.QuadPart;
      return Hal::OK;
   }
   else
   {
      return Hal::FAIL;
   }
*/
   CCVal = RPCC();
   return Hal::OK;
   
#else
   return Hal::NOTSUPPORTED;
#endif //_TRU64
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Issue a memory barrier. A memory barrier causes loads/stores to be
//  serialised.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  mask - Specifes the kind of memory barrier (READ_READ,READ_WRITE, 
//    WRITE_READ, WRITE_WRITE). 
//----------------------------------------------------------------------------
inline void HalCpu::memBarrier(Hal::HalMbMask mask)
{
#if defined(__unix) || defined(_WIN32)
   switch (mask)
   {
      case Hal::READ_READ:
      case Hal::READ_WRITE:
      case Hal::WRITE_READ:
         MB();
	 break;
      case Hal::WRITE_WRITE:
	 //Guarantees that all previous store instructions access memory
	 //before any store instructions issued after the wmb instruction.
         WMB();
	 break;
      default:  //Not supported mask value, do as best as we can.
         MB();
	 break;
   }
#endif //__unix || _WIN32
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Returns the CPU cacheline size.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  clSize - Refernce to a location where the cache line size will be stored.
//
//  OUTPUT  DESCRIPTION
//  The cache line size
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::memGetCacheLineSize(u_int32 &clSize)
{
#if defined(ALPHA_TRU64)
   // This depends of the version of the CPU
   clSize = 64; 
   return Hal::OK;

#elif defined(IPF_LINUX_)
   clSize = 64;
   return Hal::OK;

#elif defined(X86_LINUX_)
   clSize = 32;
   return Hal::OK;

#elif defined(INTEL_WIN32_)
   clSize = 32;
   return Hal::OK;

#else
   return Hal::NOTSUPPORTED; 
#endif
}

//  //  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Returns the number of leading zeros in value starting at the most 
//  significant bit. When executing this function on a alpha CPU where the
//  ctlz instruction is not implemented in HW. Then a warning is issued
//  and the instruction is emulated. 
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  value  - The input value. 
//
//  OUTPUT  DESCRIPTION
//  zeros - The number of leading zeros in value
//
//  RETURN  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality unavailable.
//----------------------------------------------------------------------------
inline Hal::HalStatus HalCpu::cntLeadingZero(u_int64 value, u_int8 &zeros)
{
#if defined(_TRU64)
   zeros = (u_int8) asm(" ctlz %a0, %v0", value); 
   return Hal::OK;

#else
   u_int64 mask = UINT64_C_(0xFFFFFFFF00000000);
   int32 mask_size = 64;
   int32 shift = 0;

   mask_size /= 2;
   while ( mask_size > 0 )
   {
      if ((mask & value) > 0)
      {
         value = value >> mask_size;
         shift += mask_size;
      }
      mask_size /= 2;
      mask = mask >> mask_size;
   }
   zeros = static_cast<u_int8>(64 - shift - (1&value));
   
   return Hal::OK;
#endif //_TRU64
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  Determines if real time clock functionality is available.
//
//  ERROR HANDLING
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Functionality is available
//  Hal::NOTSUPPORTED - Functionality unavailable.
//----------------------------------------------------------------------------
inline Hal::HalStatus HalRtc::available()
{
#if defined(_TRU64) || defined(X86_LINUX_)
   return Hal::OK; 

#else
   return Hal::NOTSUPPORTED;
#endif //
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function gets the number of clock bits. The clock consists of two
//  32 bits registers. Bit 64 is used for signalling if the clock has been
//  read since the last reset.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  A reference to a location where the number of clock bits will be stored.
//
//  OUTPUT  DESCRIPTION
//  The number of clock bits.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful operation
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalRtc::getBits(u_int32 &noOfBits)
{
#if defined(__unix)
   noOfBits=63;
   return Hal::OK;

#else
   return  Hal::NOTSUPPORTED;
#endif //__unix
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function gets the frequency of the real-time clock.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  A reference where the clock frequency will be stored.
//
//  OUTPUT  DESCRIPTION
//  The clock frequency in Hz
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalRtc::getFrequency(u_int64 &rtcFreq)
{
#if defined(__unix)
   //1000Hz
   //A clock tick is one millisecond
   rtcFreq = 1000; 
   return Hal::OK; 

#else
   return Hal::NOTSUPPORTED;
#endif //__unix
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function gets the current clock value.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  value - A reference to a location where the clock value will be stored
//  isReset - This reference variable is set if the clock has been reset
//      since the last read.
//
//  OUTPUT  DESCRIPTION
//  The clock value and the isReset flag.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NODEVICE - Fatal error
//  Hal::FAIL - Error, might work later
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalRtc::getValue(u_int64 &value, u_int8 &isReset)
{
#if defined(__unix)
   int32 status;
   isReset=0;
   u_int64 clockVal;
   //
   // comment on the typeconversion:
   // For Tru64 the read system call returns a 64 bit integer.
   // HAL interface is defined to receive a 32 bit unsigned
   // integer. An error will occur if the size of the data
   // read is over about 2 000 000 000 bytes.
   //
   status = static_cast<int32>(::read(m_handle, &clockVal, 8));
   if(status == -1)
   {
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;
      }
   }
   else
   {
      //Check status of bit 64 (configuration bit)
      if(clockVal & RCC_CONFIGURATION_BIT)
      {
         clockVal = clockVal & (~RCC_CONFIGURATION_BIT);
         isReset=1;
      }
   }
   value=clockVal;
   return Hal::OK;

#else
   return Hal::NOTSUPPORTED; 
#endif //__unix
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function sets the clock value.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  value - A reference to a location where the new clock value is stored
//
//  OUTPUT  DESCRIPTION
//  The clock value and the isReset flag.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NODEVICE - Fatal error
//  Hal::FAIL - Error, might work later
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalRtc::setValue(u_int64 value)
{
#if defined(__unix)
   if(::write(m_handle, &value, 8) == -1)
   {
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;    
      }
   }
   else
   {
      return Hal::OK; 
   }
#else
   return Hal::NOTSUPPORTED; 
#endif 
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function reads the contents of the MAS error register.
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  A reference to a location where register contents will be stored.
//
//  OUTPUT  DESCRIPTION
//  The register value.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful operation
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalMasErr::read(u_int32 &regVal)
{
#if defined(__unix)
   if(::read(m_handle, &regVal, 4) == -1)
   {
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;
      }
   }
   else
   {
      return Hal::OK; 
   }
 
#else
   return  Hal::NOTSUPPORTED;
#endif //__unix
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function writes to the MAS error register
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  The new register value
//
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
inline Hal::HalStatus HalMasErr::write(u_int32 regVal)
{
#if defined(__unix)
   if(::write(m_handle, &regVal, 4) == -1)
   {
      switch (errno) //Sort out all errors that are permanent
      {
      case EBADF:
         return Hal::NODEVICE;
      default: 
         return Hal::FAIL;    
      }
   }
   else
   {
      return Hal::OK; 
   }
#else
   return Hal::NOTSUPPORTED;
#endif //__unix
}
#endif //HAL_HXX
