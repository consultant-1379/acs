//  **************************************************************************
//  **************************************************************************
//  ***                                                                    ***
//  ***  COPYRIGHT (C) Ericsson Utvecklings AB 1999                        ***
//  ***  Box 1505, 125 25 Alvsjo, Sweden, Phone: +46 8 7273000             ***
//  ***                                                                    ***
//  ***  The computer program(s) herein may be used and/or copied only     ***
//  ***  with the written permission of Ericsson Utvecklings AB or in      ***
//  ***  accordance with the terms and conditions stipulated in the        ***
//  ***  agreement/contract under which the program(s) have been supplied. ***
//  ***                                                                    ***
//  **************************************************************************
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

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Imported Interfaces, Types & Definitions
//----------------------------------------------------------------------------
#include "hal.hxx"
#include <string.h>


//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The class constructor. Sets the file handle to an initial value.
//----------------------------------------------------------------------------
HalHdi::HalHdi() 
{
   m_handle = HALNOFILE;
}

//----------------------------------------------------------------------------
//  The class destructor.
//----------------------------------------------------------------------------
HalHdi::~HalHdi() {}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The HalHdiMsg class constructor. Sets the initial sync state.
//----------------------------------------------------------------------------
HalHdiMsg::HalHdiMsg() 
{
   m_syncState = SYNC_SEND_ECHO;
   m_syncPattern = 0;
   // Zero the test signal
   memset(&m_testSig, 0, sizeof(CptbCpubEchoSig));
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  On success this method returns Hal::OK and the member variable m_handle
//  is assigned the file descriptor number associated with the logical unit of
//  the device.
//  On failure the method returns an error code instead of Hal::OK. It is not
//  possible for an instance of HalHdi to have more than one "device" opened.
//  A "device" can not be opened more than once.
//
//  ERROR HANDLING
//
//  INPUT  DESCRIPTION
//  name - The name of the driver to open. The name is specified by using 
//   the enum defined in the Hal class.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::ALREADYOPEN - Already open
//  Hal::FAIL - Error, might work later
//  Hal::NODEVICE - Fatal error
//  Hal::NOTSUPPORTED - Functionality is unavailable
//----------------------------------------------------------------------------
Hal::HalStatus HalHdiMsg::open(HalHdiName name)
{
#ifdef __unix
   // Make sure no device is lost. If m_handle contains a file descriptor abort.
   if(m_handle != HALNOFILE)
   {  
      return Hal::ALREADYOPEN; 
   }
   
   switch(name)
   {
   case CPTB:
      m_handle = ::open("/dev/upbb/cptb", O_RDWR);
      break;
   case CPUB:
      m_handle = ::open("/dev/upbb/cpub", O_RDWR);
      break;
   case KLOG:
      m_handle = ::open("/dev/erklog", O_RDWR);
      break;
   default:
      return Hal::NODEVICE; 
   }   
   
   //
   // Specific error code is stored in 'errno'
   //
   if(m_handle == -1)
   {
      m_handle=HALNOFILE; 
      switch (errno) //Sort out all errors that are permanent
      {
      case EBUSY:
         return Hal::ALREADYOPEN;
      default: 
         return Hal::FAIL;
      }
   }
   else
   {
      return Hal::OK; 
   }
#else // end TRU64 code
   return Hal::NOTSUPPORTED; 
#endif 
}


//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The method closes (releases) the device which is associated to the file
//  descriptor held by the m_handle member variable.
//
//  ERROR HANDLING
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//  Hal::NODEVICE - Fatal error
//  Hal::NOTSUPPORTED - Functionality is unavailable  
//----------------------------------------------------------------------------
Hal::HalStatus HalHdiMsg::close()
{
#ifdef __unix
   int32 status;
   if(m_handle != HALNOFILE)
   {
      status = ::close(m_handle);
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
         m_handle=HALNOFILE;
         return Hal::OK;
      }
   }
   else
   {
      return Hal::NODEVICE; //Not a valid device 
   }
#else // _TRU64
   return Hal::NOTSUPPORTED; 
#endif 
}

//----------------------------------------------------------------------------
//  This internal method creates an echo signal. The receiver only cares about the 
//  signal type.
//
//  INPUT  DESCRIPTION
//  pattern - Data pattern to initialise the echo signal with.
//
//----------------------------------------------------------------------------
void HalHdiMsg::createEchoSig(u_int64 pattern)
{
   m_testSig.u8SigNo[0] = 0;
   m_testSig.u8SigNo[1] = 0;
   m_testSig.u8SigNo[2] = 0x7f;
   m_testSig.u8SigNo[3] = 0xff;
   
   // Write a pattern in the signal that we can verify 
   memcpy(&m_testSig.u8TestData, &pattern, sizeof(u_int64));
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  The method sends an echo signal. And polls for a reply. When an echo signal has
//  been sent and received we return OK.
//
//  INPUT  DESCRIPTION
//  flags - See enum in HalHdiMsg class.
//          SYNC_ALL - What to sync. Don't care at the moment. 
//          FORCE_TIMEOUT - If the user decides that we should timeout waiting
//          for a reply and resend the echo signal. This function times out 
//          after X number of retries, no "wall clock" timeout!
//          If this flag is specified when we are to send an echo signal it is
//          disregarded.
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Call method again, not yet in sync
//  Hal::EFATAL - Timed out.
//  Hal::NODEVICE - Fatal error
//  Hal::NOTSUPPORTED - Functionality is unavailable  
//----------------------------------------------------------------------------

Hal::HalStatus HalHdiMsg::resync(int flags)
{
#ifdef __unix
   struct timespec current;
   u_int64 diffTime;
   if(m_handle != HALNOFILE)
   {
      if(m_syncState == SYNC_SEND_ECHO)
      {  
         // Reset, that is clear both the read and write FIFO:s 
	int status = 0; //XXX ::ioctl(m_handle, UPBB_IOC_RESET);
         if(status == -1)
         {
            return Hal::NODEVICE;
         }
         
         // Send the echo signal 
         createEchoSig(++m_syncPattern);
         if(write((u_int8*)&m_testSig, sizeof(CptbCpubEchoSig)) == Hal::OK)
         {
            // If the echo signal was sent change state 
            m_syncState = SYNC_WAIT_FOR_ECHO_REPLY;
            clock_gettime(CLOCK_REALTIME, &m_syncStart);  
         }
      }
      else if(m_syncState == SYNC_WAIT_FOR_ECHO_REPLY)
      {
         u_int8 buf[CPXB_MAX_SIGNAL_SIZE];
         u_int32 size;
         if((flags & FORCE_TIMEOUT) == 0)
         {
            size = CPXB_MAX_SIGNAL_SIZE;
            read(buf, size);
            if(size) // We read something 
            {
               // Reset timer
               clock_gettime(CLOCK_REALTIME, &m_syncStart);  

               // If unexpected size 
               if(size != sizeof(CptbCpubEchoSig))
               {
                  return Hal::FAIL;
               }
               // Received same signal as we sent?
               if(memcmp(buf, &m_testSig, size) == 0) 
               {
                  m_syncState = SYNC_SEND_ECHO;
                  return Hal::OK;
               }
            }
         }
         else
         {
            // Force timeout 
            m_syncState = SYNC_SEND_ECHO;
         }
         
         // Check if we have timed out waiting for a reply
         clock_gettime(CLOCK_REALTIME, &current);  
         diffTime = (current.tv_sec - m_syncStart.tv_sec) * 1000000000L + // ns
            (current.tv_nsec - m_syncStart.tv_nsec); // ns
         if(diffTime > SYNC_TIMEOUT)
         {            
            m_syncState = SYNC_SEND_ECHO;
            return Hal::EFATAL;
         }
      }
   }
   else
   {
      return Hal::NODEVICE; //Not a valid device 
   }
   return Hal::FAIL;

#else // _TRU64
   return Hal::NOTSUPPORTED; 
#endif 
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This that the interrupt mechanism are working ok. 
//  A driver is needed for this test that's why this method has been 
//  placed in HalHdiMsg (were we have access to the CPT/RTC driver).
//
//  INPUT  DESCRIPTION
//  intr - See enum in HalHdiMsg class.
//          NXM -  Test the Non eXistent Memory interrupt.
//          NXIO - Test the Non eXistent IO interrupt.
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Bad parameter, test not supported by driver.    
//  Hal::EFATAL - Test failed
//  Hal::NODEVICE - No device.
//  Hal::NOTSUPPORTED - Functionality is unavailable  
//----------------------------------------------------------------------------
Hal::HalStatus HalHdiMsg::testInterrupt(enum IntTestType intr)
{
#ifdef _TRU64
   int status;
   if(m_handle != HALNOFILE)
   {
      upbb_ioc_type arg;
      switch (intr) // What to test?
      {
      case NXM:
         arg.reg = UPBB_TEST_INT_NXM;
         break;
      case NXIO:
         arg.reg = UPBB_TEST_INT_NXIO;
         break;
      default:
         return Hal::FAIL;
      }
      // Call the interrupt testing ioctl
      status = ::ioctl(m_handle, UPBB_IOC_TEST_INTERRUPT, &arg);
      if(status == -1)
      {
         if(errno == EINVAL) // Unknown test
         {
            return Hal::FAIL;
         }
         return Hal::EFATAL;
      }
      else
      {
         // For the NXM interrupts the driver return the memory size in GB
         if(intr == NXM)
         {
            int memSize;
            const int MB_MULTIPLE = 1024;
            // Get the total memory size in KB
            if(getsysinfo(GSI_PHYSMEM, (caddr_t) &memSize, sizeof(int), 0) > 0)
            {
               // Round up to the next GB
               memSize = (memSize/1024 + MB_MULTIPLE - 1) & ~(MB_MULTIPLE - 1);
               memSize = memSize/1024; 
               if(arg.reg != memSize)
               {
                  return Hal::EFATAL;
               }
            }
         }
      }
   }
   else
   {
      return Hal::NODEVICE; //Not a valid device 
   }

   return Hal::OK;
#else // _TRU64
   SUPPRESS_NON_REF_ARG1(enum IntTestType, intr);
   return Hal::NOTSUPPORTED; 
#endif 
}

//----------------------------------------------------------------------------
//  The HalRtc class constructor. Sets the RTC file handle to an initial value.
//----------------------------------------------------------------------------
HalRtc::HalRtc()
{
   m_handle = HALNOFILE;
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function opens the RTC device and the file handle is stored internally 
//  in the class.
//
//  ERROR HANDLING
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//----------------------------------------------------------------------------
Hal::HalStatus HalRtc::open()
{
#ifdef __unix
   //If m_handle contains a file descriptor return success
   if(m_handle != HALNOFILE)
   {  
      return Hal::OK; 
   }
   m_handle = ::open("/dev/upbb/rtc", O_RDWR);
   //-1 indicates that something went wrong
   if(m_handle == -1)
   {
      m_handle=HALNOFILE; //reset file handle
      return Hal::FAIL;
   }
   else
   {
      return Hal::OK;
   }
#else //end TRU64 code
   return Hal::NOTSUPPORTED;
#endif
}

//----------------------------------------------------------------------------
//  The HalMasErr class constructor. Sets the MAS register file handle to 
//  an initial value.
//----------------------------------------------------------------------------
HalMasErr::HalMasErr()
{
   m_handle = HALNOFILE;
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function opens the MAS error register device and the file handle is 
//  stored internally in the class.
//
//  ERROR HANDLING
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//----------------------------------------------------------------------------
Hal::HalStatus HalMasErr::open()
{
#ifdef __unix
   //If m_handle contains a file descriptor return success
   if(m_handle != HALNOFILE)
   {  
      return Hal::OK; 
   }
   m_handle = ::open("/dev/upbb/mas", O_RDWR);
   //-1 indicates that something went wrong
   if(m_handle == -1)
   {
      m_handle=HALNOFILE; //reset file handle
      return Hal::FAIL;
   }
   else
   {
      return Hal::OK;
   }
#else //end unix code
   return Hal::NOTSUPPORTED;
#endif
}

//  @(#)method visibility=otherBlocks
//----------------------------------------------------------------------------
//  This function closes the MAS error register device.
//
//  ERROR HANDLING
//
//  RETURNS  DESCRIPTION
//  Hal::OK - Successful
//  Hal::FAIL - Error, might work later
//----------------------------------------------------------------------------
Hal::HalStatus HalMasErr::close()
{
#ifdef __unix
   //-1 indicates that something went wrong
   if(::close(m_handle) == -1)
   {
      m_handle=HALNOFILE; //reset file handle
      return Hal::FAIL;
   }
   else
   {
      m_handle=HALNOFILE; //reset file handle
      return Hal::OK;
   }
#else //end TRU64 code
   return Hal::NOTSUPPORTED;
#endif
}

