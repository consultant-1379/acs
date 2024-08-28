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
//  Responsible ............ (start by filling in this field)
//
//  REVISION HISTORY
//
//  Rev.   Date        Sign     Description                             Reason
//  ----   ----        ----     -----------                             ------
//  @(#)revisionhistory
//
//< \file
//
//  This file defines functions that are specific on a per architecture
//  basis.
//>

#ifndef MACHINESPECIFIC_HXX
#define MACHINESPECIFIC_HXX

//#pragma ident "@(#)filerevision "
#pragma comment (user, "@(#)filerevision ")

//----------------------------------------------------------------------------
//  Imported Types & Definitions
//----------------------------------------------------------------------------
#include "PlatformTypes.hxx" // Get the platform defines
#include "hal.hxx"

//----------------------------------------------------------------------------
//  Exported Types & Definitions
//----------------------------------------------------------------------------
#ifdef _MSC_VER
#include <iosfwd>

// Defines the operator << for __int64.
ostream& operator<<(ostream& o, const __int64& value);

// Defines the operator << for the unsigned __int64.
ostream& operator<<(ostream& o, const unsigned __int64& value);
#endif // _MSC_VER

#if defined _ALPHA_TRU64 // This section is TRU64 on Alpha specific
#include <machine/builtins.h>
#include <c_asm.h>   // inline assembler routines
#include <sys/processor.h> // For processor information
#include <cstdlib> // For exit()
#include <sys/table.h> // For reading userCounter and tickResolution

//----------------------------------------------------------------------------
//< \var SystemPageSize
//
// The system page size is needed in a number of places to size/pad structures
// or page align data. The size is sometimes needed at compile time. Therefore
// a constant is provided that can be used for this purpose.
//>
//----------------------------------------------------------------------------
const int SystemPageSize = 8192;

//----------------------------------------------------------------------------
//< \var CacheLineSize
//
// The cache line size is needed in a number of places to size/pad
// structures to avoid extra cache misses. The size is needed at
// compile time. Therefore a constant is provided that can be used
// for this purpose.
// All current Alpha systems have a cache line size of 64 bytes.
//>
//----------------------------------------------------------------------------
const int CacheLineSize = 64;
const int CacheLineSizeSmall = CacheLineSize;
const int CacheLineSizeLarge = CacheLineSize;

//----------------------------------------------------------------------------
//< \fn u_int64 RPCC()
//
//  Inline function (preferred over macro in C++) to Read Process Cycle Counter
// 
//  \return           Process Cycle Counter
//>
//----------------------------------------------------------------------------
inline u_int64 RPCC()
{
   return __RPCC();
}

//----------------------------------------------------------------------------
//< \fn int CountLeadingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count leading zeros
//  Uses the Alpha instruction 'ctlz' on CPUs that have it and sequence of
//  instructions on CPUs that don't.
// 
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of leading zeros
//>
//----------------------------------------------------------------------------
inline int CountLeadingZeros(u_int64 bits)
{
   return static_cast<int>(_leadz(bits));
}

//----------------------------------------------------------------------------
//< \fn int CountTrailingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count trailing zeros
//  Uses the Alpha instruction 'cttz' on CPUs that have it and sequence of
//  instructions on CPUs that don't.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of trailing zeros
//>
//----------------------------------------------------------------------------
inline int CountTrailingZeros(u_int64 bits)
{
   return static_cast<int>(_trailz(bits));
}

//----------------------------------------------------------------------------
//< \fn int CountBitsSet(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count number of bits 
//  set in a 64 bit word.
//  Uses the Alpha instruction 'ctpop' on CPUs that have it and sequence of
//  instructions on CPUs that don't.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of bit(s) set
//>
//----------------------------------------------------------------------------
inline int CountBitsSet(u_int64 bits)
{
   return static_cast<int>(_popcnt(bits));
}

//----------------------------------------------------------------------------
//< \fn u_int64 userCounter()
//
//  Support method for calculating system load 
//
//  \return  Returns the user time or 0 if operation failed
//>
//----------------------------------------------------------------------------
inline u_int64 userCounter()
{
   struct tbl_sysinfo sibuf;
   if (table(TBL_SYSINFO, 0, &sibuf, 1, sizeof(struct tbl_sysinfo)) == 1)
   {
      return sibuf.si_user;
   }
 
   // Something went wrong with the table call
   return 0;
}

//----------------------------------------------------------------------------
//< \fn u_int32 tickResolution()
//
//  Support method for reading the user counter frequency
//
//  \return  Returns the user counter frequency in Hz or 0 if the
//           operation failed
//>
//----------------------------------------------------------------------------
inline u_int32 tickResolution()
{
   struct tbl_sysinfo sibuf;
   if (table(TBL_SYSINFO,0,&sibuf,1,sizeof(struct tbl_sysinfo)) == 1)
   {
      return static_cast<u_int32>(sibuf.si_hz);
   }

   // Something went wrong with the table call
   return 0;
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH(const void* a, u_int32 mask)
//
//  Prefetch cache-block
//
//  \pin  a       Target address
//  \pin  mask    The value to XOR with target address before the prefetch
//                This is an optional argument. If no mask is specified the
//                default value of h'20 will be used.
//>
//----------------------------------------------------------------------------
inline void PREFETCH(const void* a, u_int32 mask = 0x20)
{
   // The 'ldbu' instruction below is almost identical to the 'ldl' normal
   // cache line prefetch; Despite the fact that it is fairly undocumented,
   // 'ldbu' avoids potential alignment traps.
   asm("ldbu $31, 0(%a0);", reinterpret_cast<u_int64>(a) ^ mask);
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH_M(void* a, u_int32 mask)
//
//  Prefetch cache-block with modify intent
//
//  \pin  a       Target address
//  \pin  mask    The value to XOR with target address before the prefetch.
//                This is an optional argument. If no mask is specified the
//                default value of h'20 will be used. 
//>
//----------------------------------------------------------------------------
inline void PREFETCH_M(void* a, u_int32 mask = 0x20)
{
   asm("lds %f31, 0(%a0);", (reinterpret_cast<u_int64>(a) & ~0x3L) ^ mask);
}

//----------------------------------------------------------------------------
//< \fn void ECB(void* a)
//
//  Evict cache-block
//
//  The ECB function provides a hint that the addressed location will not be
//  referenced again in the near future, so any cache space is occupies
//  should  be made available to cache other memory locations. 
//
//  \pin  a       Target address
//>
//----------------------------------------------------------------------------
inline void ECB(void* a)
{
   asm("ecb (%a0);", a);
}

//----------------------------------------------------------------------------
//< \fn void WH64(void* a)
//
//  Write Hint
//
//  The WH64 function provides a hint that the current contents of the 
//  aligned 64-byte block (cache-block) containing the addressed byte will
//  never be read again but will be overwritten in the near future.
//
//  \pin  a       Target address
//>
//----------------------------------------------------------------------------
inline void WH64(void* a)
{
   asm("wh64 (%a0);", a);
}

//< \def MB()
//
//  Memory Barrier
//>
#define MB()  asm(" mb;");

//< \def WMB()
//
//  Write Memory Barrier
//>
#define WMB() asm(" wmb;");

//----------------------------------------------------------------------------
//< \fn int64 ZAP(int64 mask, int64 value)
//
//  Zap (zero) bytes in W64.
//
//  The Zap function provides an efficient mechanism for clearing bytes in
//  a W64 word. On Alpha it maps directly to a machine instruction. On other
//  achitectures the function may be inefficient.
//
//  A bit set to 1 in the mask argument means that the corresponding byte
//  in value shall be cleared.
//
//  \pin  mask    mask for bytes to clear. Only low 8 bits used.
//  \pin  value   64 bit word to clear bytes in.
//  \return       value with bytes cleared according to mask.
//>
//----------------------------------------------------------------------------
inline int64 ZAP(int64 mask, int64 value)
{
   return asm("zap %a1,%a0,%v0", mask, value);
}
#elif defined _IPF_LINUX // This section is Linux on Itanium specific
#include <ia64intrin.h>

//----------------------------------------------------------------------------
//< \var SystemPageSize
//
// The system page size is needed in a number of places to size/pad structures
// or page align data. The size is sometimes needed at compile time. Therefore
// a constant is provided that can be used for this purpose.
//>
//----------------------------------------------------------------------------
const int SystemPageSize = 16384;

//----------------------------------------------------------------------------
//< \var CacheLineSize
//
// The cache line size is needed in a number of places to size/pad
// structures to avoid extra cache misses. The size is needed at
// compile time. Therefore a constant is provided that can be used
// for this purpose.
// All current Itanium systems (Itanium 1 & 2) have an L1 cache line 
// size of 64 bytes.
//>
//----------------------------------------------------------------------------
const int CacheLineSize = 64; 
const int CacheLineSizeSmall = CacheLineSize;

//----------------------------------------------------------------------------
//< \var CacheLineSize
//
// The cache line size is needed in a number of places to size/pad
// structures to avoid extra cache misses. The size is needed at
// compile time. Therefore a constant is provided that can be used
// for this purpose.
// All current Itanium systems (Itanium 1 & 2) have L2 and L3 cache line 
// sizes of 128 bytes.
//>
//----------------------------------------------------------------------------
const int CacheLineSizeLarge = 128;

//----------------------------------------------------------------------------
//< \fn int CountBitsSet(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count number of bits 
//  set in a 64 bit word.
//  Uses the IA64 popcnt instruction to do the work.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of bit(s) set
//>
//----------------------------------------------------------------------------
inline int CountBitsSet(u_int64 bits)
{
#if defined(__ECC)
   return _m64_popcnt(bits);
#else // assume gcc, supports asm
   u_int64 res;
   __asm__("popcnt %0=%1" : "=r"(res) : "r"(bits));
   return static_cast<int>(res); //no loss possible as max value is 64
#endif
}

//----------------------------------------------------------------------------
//< \fn int CountTrailingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count trailing zeros
//  There is no IA64 builtin function to count number of trailing zeros.
//  A loop free implementation is possible with creative use of arithmetic,
//  XOR and the popcnt instruction.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of trailing zeros (0-64).
//>
//----------------------------------------------------------------------------
inline int CountTrailingZeros(u_int64 bits)
{
   int tz = 64; 
   if (bits != 0)
   {
      tz = CountBitsSet(bits ^ (bits - 1)) - 1;
   }
   return tz;
}

//----------------------------------------------------------------------------
//< \fn u_int64 userCounter()
//
//  Support method for calculating system load 
//
//  \return  Returns the user time or 0 if operation failed
//>
//----------------------------------------------------------------------------
inline u_int64 userCounter()
{
   // XXX Unmapped for now...
   //struct tbl_sysinfo sibuf;
   //if (table(TBL_SYSINFO, 0, &sibuf, 1, sizeof(struct tbl_sysinfo)) == 1)
   //{
   //   return sibuf.si_user;
   //}
 
   // Something went wrong with the table call
   return 0;
}

//----------------------------------------------------------------------------
//< \fn u_int32 tickResolution()
//
//  Support method for reading the user counter frequency
//
//  \return  Returns the user counter frequency in Hz or 0 if the
//           operation failed
//>
//----------------------------------------------------------------------------
inline u_int32 tickResolution()
{
   // XXX Unmapped for now...
   //struct tbl_sysinfo sibuf;
   //if (table(TBL_SYSINFO,0,&sibuf,1,sizeof(struct tbl_sysinfo)) == 1)
   //{
   //   return sibuf.si_hz;
   //}

   // Something went wrong with the table call
   return 0;
}

//< \def MB()
//
//  Memory Barrier
//
// Note: "MB()" and its variants cannot be used as a fence to order
// accesses to memory mapped I/O registers.  For that, mf.a needs to
// be used.  However, we don't want to always use mf.a because (a)
// it's (presumably) much slower than mf and (b) mf.a is supported for
// sequential memory pages only.
//
//>
#if defined(__ECC)
#define MB()  __mf();
#else
inline void MB()  { __asm__ __volatile__("mf": : :"memory");}
#endif

//< \def WMB()
//
//  Write Memory Barrier
//>
#if defined(__ECC)
#define WMB() MB();
#else
inline void WMB()  { __asm__ __volatile__("mf": : :"memory");}
#endif

//----------------------------------------------------------------------------
//< \fn int64 ZAP(int64 mask, int64 value)
//
//  Zap (zero) bytes in W64.
//
//  The Zap function provides an efficient mechanism for clearing bytes in
//  a W64 word. On Alpha it maps directly to a machine instruction. On other
//  achitectures the function may be inefficient.
//
//  A bit set to 1 in the mask argument means that the corresponding byte
//  in value shall be cleared.
//
//  \pin  mask    mask for bytes to clear. Only low 8 bits used.
//  \pin  value   64 bit word to clear bytes in.
//  \return       value with bytes cleared according to mask.
//>
//----------------------------------------------------------------------------
// XXX Use C++ version for now...
//inline int64 ZAP(int64 mask, int64 value)
//{
   //return asm("zap %a1,%a0,%v0", mask, value);
//}

//----------------------------------------------------------------------------
//< \fn int64 AtomicAnd32(int32* op1, int32 op2)
//
//  Atomic AND
//
//  \pin  op1    Pointer to operand 1
//  \pin  op2    Operand 2, 32 bit value to be ANDed with operand 1 
//>
//----------------------------------------------------------------------------
inline void AtomicAnd32(volatile u_int32* op1, int32 op2)
{
#if defined(__ECC)
   int32 oldValue;
   int32 newValue;
   do
   {  
      oldValue = *op1;
      newValue = oldValue & op2;
   } while (_InterlockedCompareExchange(reinterpret_cast<volatile int32*>(op1), newValue, oldValue) != oldValue); 
#else
   //XXX Needs verification for correctness!
   u_int64 oldValue;
   u_int64 newValue;
   u_int64 u_op2 = static_cast<u_int32>(op2);
   do
   {  
      oldValue = *reinterpret_cast<volatile u_int32*>(op1);
      newValue = oldValue & op2;
      u_int64 ia64_intri_res;
	   asm volatile ("mov ar.ccv=%0;;" :: "rO"(oldValue));
	   asm volatile ("cmpxchg4.acq %0=[%1],%2,ar.ccv":
			           "=r"(ia64_intri_res) : "r"(op1), "r"(newValue) : "memory");
   } while ( ia64_intri_res != oldValue); 
#endif
}

//----------------------------------------------------------------------------
//< \fn int64 AtomicOr32(int32* op1, int32 op2)
//
//  Atomic OR
//
//  \pin  op1    Pointer to operand 1
//  \pin  op2    Operand 2, 32 bit value to be ORed with operand 1 
//>
//----------------------------------------------------------------------------
inline void AtomicOr32(volatile u_int32* op1, int32 op2)
{
#if defined(__ECC)
   int32 oldValue;
   int32 newValue;
   do
   {  
      oldValue = *op1;
      newValue = oldValue | op2;
   } while (_InterlockedCompareExchange(reinterpret_cast<volatile int32*>(op1), newValue, oldValue) != oldValue);
#else
   //XXX Needs verification for correctness
   u_int64 oldValue;
   u_int64 newValue;
   u_int64 u_op2 = static_cast<u_int32>(op2);
   do
   {  
      oldValue = *reinterpret_cast<volatile u_int32*>(op1);
      newValue = oldValue | op2;
      u_int64 ia64_intri_res;
	   asm volatile ("mov ar.ccv=%0;;" :: "rO"(oldValue));
	   asm volatile ("cmpxchg4.acq %0=[%1],%2,ar.ccv":
			           "=r"(ia64_intri_res) : "r"(op1), "r"(newValue) : "memory");
   } while ( ia64_intri_res != oldValue); 
#endif
}
//----------------------------------------------------------------------------
//< \fn void PREFETCH(const void* a, u_int32 mask)
//
//  Prefetch data from memory to cache.
//
//  Issues an IA64 lfetch (line fetch, non-faulting variant) instruction.
//  The locailty hint used is nt1.
//
//  \pin a     Address of data to prefetch
//>
//----------------------------------------------------------------------------
inline void PREFETCH(const void* a, u_int32 = 0)
{
   __lfetch(__lfhint_nt1, a);
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH_M(void*, u_int32)
//
//  Prefetch data from memory to cache. This 
//
//  Issues an IA64 lfetch.excl (line fetch, exclusive, non-faulting variant)
//  instruction.
//  The locailty hint used is nt1.
//
//  \pin a     Address of data to prefetch
//>
//----------------------------------------------------------------------------
inline void PREFETCH_M(void* a, u_int32 = 0)
{
   __lfetch_excl(__lfhint_nt1, a);
}

#elif defined _ALPHA_WIN32 // This section is Windows on Alpha specific
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

//----------------------------------------------------------------------------
//< \var SystemPageSize
//
// The system page size is needed in a number of places to size/pad structures
// or page align data. The size is sometimes needed at compile time. Therefore
// a constant is provided that can be used for this purpose.
//>
//----------------------------------------------------------------------------
const int SystemPageSize = 8192;

//< \var CacheLineSize
//
// All current Alpha systems have a cache line size of 64 bytes,
//>
const int CacheLineSize = 64;
const int CacheLineSizeSmall = CacheLineSize;
const int CacheLineSizeLarge = CacheLineSize;

#pragma intrinsic (__asm, __dasm, __fasm)
#pragma intrinsic(_ReleaseSpinLock, _AcquireSpinLock)
#pragma intrinsic(__ADD_ATOMIC_LONG2)

//< \def RPCC()
//
// Macro used to access the processor cycle counter
//>
#define RPCC() ((u_int32)__asm(" rpcc v0;"))

//----------------------------------------------------------------------------
//< \fn int CountLeadingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count leading zeros.
//  Uses the Alpha instruction 'ctlz'. This instruction is emulated
//  on CPUs that does not have it. 
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of leading zeros
//>
//----------------------------------------------------------------------------
inline int CountLeadingZeros(u_int64 bits)
{
   return __asm("ctlz %a0, %v0", bits);
}

//----------------------------------------------------------------------------
//< \fn int CountTrailingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count trailing zeros.
//  Uses the Alpha instruction 'cttz'. This instruction is emulated
//  on CPUs that does not have it. 
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of trailing zeros
//>
//----------------------------------------------------------------------------
inline int CountTrailingZeros(u_int64 bits)
{
   return __asm("cttz %a0, %v0", bits);
}

//----------------------------------------------------------------------------
//< \fn int CountBitsSet(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count number of bits set
//  in a 64 bit word.
//  Uses the Alpha instruction 'ctpop'. This instruction is emulated
//  on CPUs that does not have it. 
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of bit(s) set
//>
//----------------------------------------------------------------------------
inline int CountBitsSet(u_int64 bits)
{
   return __asm("ctpop %a0, %v0", bits);
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH(const void* a, u_int32 mask)
//
//  Prefetch cache-block
//
//  \pin  a       Target address
//  \pin  mask    The value to XOR with target address before the prefetch
//                This is an optional argument. If no mask is specified the
//                default value of h'20 will be used.
//>
//----------------------------------------------------------------------------
inline void PREFETCH(const void* a, u_int32 mask = 0x20)
{
   // The 'ldbu' instruction below is almost identical to the 'ldl' normal
   // cache line prefetch; Despite the fact that it is fairly undocumented,
   // 'ldbu' avoids potential alignment traps.
   __asm("ldbu $31, 0(%a0);", reinterpret_cast<u_int64>(a) ^ mask);
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH_M(void* a, u_int32 mask)
//
//  Prefetch cache-block with modify intent
//
//  \pin  a       Target address
//  \pin  mask    The value to XOR with target address before the prefetch
//                This is an optional argument. If no mask is specified the
//                default value of h'20 will be used.
//>
//----------------------------------------------------------------------------
inline void PREFETCH_M(void* a, u_int32 mask = 0x20)
{
   __asm("lds %f31, 0(%a0);", (reinterpret_cast<u_int64>(a) & ~0x3L) ^ mask);
}

//----------------------------------------------------------------------------
//< \fn void ECB(void* a)
//
//  Evict cache-block
//
//  The ECB function provides a hint that the addressed location will not be
//  referenced again in the near future, so any cache space is occupies
//  should be made available to cache other memory locations. 
//
//  \pin  a       Target address
//>
//----------------------------------------------------------------------------
inline void ECB(void* a)
{
   __asm("ecb (%a0);", a);
}

//----------------------------------------------------------------------------
//< \fn void WH64(void* a)
//
//  Write Hint
//
//  The WH64 function provides a hint that the current contents of the 
//  aligned 64-byte block (cache-block) containing the addressed byte will
//  never be read again but will be overwritten in the near future.
//
//  \pin  a       Target address
//>
//----------------------------------------------------------------------------
inline void WH64(void* a)
{
   __asm("wh64 (%a0);", a);
}

//< \def MB()
//
//  Memory Barrier
//>
#define MB() __asm(" mb;");

//< \def WMB()
//
//  Write Memory Barrier
//>
#define WMB() __asm(" wmb;");

//----------------------------------------------------------------------------
//< \fn u_int64 userCounter()
//
//  Not implemented on Alpha/Win32
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int64 userCounter()
{
   return 0;
}

//----------------------------------------------------------------------------
//< \fn u_int32 tickResolution()
//
//  Not implemented on Alpha/Win32 
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int32 tickResolution()
{
   return 0;
}

//----------------------------------------------------------------------------
//< \fn int64 ZAP(int64 mask, int64 value)
//
//  Zap (zero) bytes in W64.
//
//  The Zap function provides an efficient mechanism for clearing bytes in
//  a W64 word. On Alpha it maps directly to a machine instruction. On other
//  achitectures the function may be inefficient.
//
//  A bit set to 1 in the mask argument means that the corresponding byte
//  in value shall be cleared.
//
//  \pin  mask    mask for bytes to clear. Only low 8 bits used.
//  \pin  value   64 bit word to clear bytes in.
//  \return       value with bytes cleared according to mask.
//>
//----------------------------------------------------------------------------
inline int64 ZAP(int64 mask, int64 value)
{
   return __asm("zap %a1,%a0,%v0", mask, value);
}

#elif defined _INTEL_WIN32 // This section is Windows on Intel specific
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>

//----------------------------------------------------------------------------
//< \var SystemPageSize
//
// The system page size is needed in a number of places to size/pad structures
// or page align data. The size is sometimes needed at compile time. Therefore
// a constant is provided that can be used for this purpose.
//>
//----------------------------------------------------------------------------
const int SystemPageSize = 4096;

//< \var CacheLineSize
// 
//  Is the CacheLineSize really 64 on an Intel x86/IA32 platform?
//>
const int CacheLineSize = 64;
const int CacheLineSizeSmall = CacheLineSize;
const int CacheLineSizeLarge = CacheLineSize;

//< \def MB()
//
//  Memory barriers has no equivalence on x86 platforms so it is
//  just defined as a NOP.
//>
#define MB()

//< \def WMB()
//
//  Memory barriers has no equivalence on x86 platforms so it is
//  just defined as a NOP.
//>
#define WMB()

//----------------------------------------------------------------------------
//< \fn u_int64 userCounter()
//
//  Not implemented on Intel/Win32
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int64 userCounter()
{
   return 0;
}

//----------------------------------------------------------------------------
//< \fn tickResolution()
//
//  Not implemented on Intel/Win32 
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int32 tickResolution()
{
   return 0;
}

#elif defined _SPARC_SOLARIS // This section is Solaris on SPARC specific
#include <sys/processor.h> // For processor information
#include <stdlib.h> // For exit()
#include <sys/time.h>

//< \def RPCC()
//
// There exists a processor cycle counter (TICK register) on more recent SPARC
// architectures but for now we use the high resolution timer returned by 
// gethrtime() instead.
//>
#define RPCC() gethrtime()

//< \def MB()
//
//  LoadLoad Memory Barrier
//>
#define MB()  asm("membar 0x0;");

//< \def WMB()
//
//  StoreStore Memory Barrier
//>
#define WMB() asm("membar 0x3;");

//----------------------------------------------------------------------------
//< \fn u_int64 userCounter()
//
//  Not implemented on Sparc/Solaris 
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int64 userCounter()
{
   return 0;
}

//----------------------------------------------------------------------------
//< \fn u_int32 tickResolution()
// 
//  Not implemented on Sparc/Solaris 
//
//  \return  Always zero.
//>
//----------------------------------------------------------------------------
inline u_int32 tickResolution()
{
   return 0;
}
#elif defined X86_LINUX_ // This section is Linux on Intel specific 

//< \var CacheLineSize
// 
//  Is the CacheLineSize really 64 on an Intel/Linux platform?
//>
const int CacheLineSize = 64;
const int CacheLineSizeSmall = CacheLineSize;
const int CacheLineSizeLarge = CacheLineSize;

//< \var SystemPageSize
//
// The system page size is needed in a number of places to size/pad structures
// or page align data. The size is sometimes needed at compile time. Therefore
// a constant is provided that can be used for this purpose.
//>
const int SystemPageSize = 8192;

#define RPCC() gethrtime()
inline u_int64 gethrtime()
{
   u_int64 value;
   HalCpu::CCGetValue(value);
   return value;
}

//----------------------------------------------------------------------------
//< \fn void PREFETCH(const void*, u_int32)
//
//  Prefetch data from memory to cache.
//
//  Issues an IA32 prefetch instruction.
//  The locality hint used is T1.
//
//  \pin a     Address of data to prefetch
//
//>
//----------------------------------------------------------------------------
inline void PREFETCH(const void* a, u_int32 = 0)
{
   __asm__ __volatile__( "prefetcht1 %0" : : "m" (((int*)a)[0]) );
   // +-------------------------------------------------^
   // +--- Be aware of the strange asm constraint syntax here!
}

//< \def MB()
//
//  Memory barriers has no equivalence on x86 platforms so it is
//  just defined as a NOP.
//>
#define MB()

//< \def WMB()
//
//  Memory barriers has no equivalence on x86 platforms so it is
//  just defined as a NOP.
//>
#define WMB()
#else
#error Your architechture is not supported in the __FILE__ file.
#endif // Machine specific defines

#if !defined( _M_ALPHA ) && !defined( __alpha )

//< \def SIMULATED_CTX__
//  Support for native/built-in bit count instructions is simulated
//>
#define SIMULATED_CTX__

//----------------------------------------------------------------------------
//< \fn inline int CountLeadingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count leading zeros.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of leading zeros
//>
//----------------------------------------------------------------------------
inline int CountLeadingZeros(u_int64 bits)
{
   const int64 NumberOfBits = sizeof(bits) * 8;

   int leadingZeros = 0;
   for (int i = NumberOfBits - 1; i >= 0; i--)
   {
      if ((bits & (static_cast<u_int64>(1) << i)) != 0) break;
      leadingZeros++;
   }
   return leadingZeros;
}

#if !defined(IPF_LINUX_)
//----------------------------------------------------------------------------
//< \fn inline int CountTrailingZeros(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count trailing zeros.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of trailing zeros
//>
//----------------------------------------------------------------------------
inline int CountTrailingZeros(u_int64 bits)
{
   const int64 NumberOfBits = sizeof(bits) * 8;

   int trailingZeros = 0;
   u_int64 bitProbe = 1; // shifted to examine the individual bits
   for (; trailingZeros < NumberOfBits; trailingZeros++)
   {
      if ((bits & bitProbe) != 0) break;
      bitProbe <<= 1;
   }
   return trailingZeros;
}
#endif   // !defined(IPF_LINUX_)

#if !defined( _IPF_LINUX )
//----------------------------------------------------------------------------
//< \fn inline int CountBitsSet(u_int64 bits)
//
//  Inline function (preferred over macro in C++) to count bits set.
//
//  \pin    bits     The bit pattern to scan
//  \return          Returns the number of bit(s) set
//>
//----------------------------------------------------------------------------
inline int CountBitsSet(u_int64 bits)
{
   const int64 NumberOfBits = sizeof(bits) * 8;
   int cnt = 0;

   for (int bitpos = 0; 
        bitpos < NumberOfBits; 
        bitpos++)
   {
      if ((bits & (static_cast<u_int64>(1) << bitpos)) != 0) ++cnt;
   }
   return cnt;
}
#endif   // !defined( _IPF_LINUX )

//----------------------------------------------------------------------------
//< \fn int64 ZAP(int64 mask, int64 value)
//
//  Zap (zero) bytes in W64.
//
//  The Zap function provides an efficient mechanism for clearing bytes in
//  a W64 word. On Alpha it maps directly to a machine instruction. On other
//  achitectures the function may be inefficient.
//
//  A bit set to 1 in the mask argument means that the corresponding byte
//  in value shall be cleared.
//
//  \pin  mask    mask for bytes to clear. Only low 8 bits used.
//  \pin  value   64 bit word to clear bytes in.
//  \return       value with bytes cleared according to mask.
//>
//----------------------------------------------------------------------------
inline int64 ZAP(int64 mask, int64 value)
{
   // Construct a mask with 0xff in the positions for bytes to keep
   int64 keepMask = 0;
   for (int bitmask = 0x80; bitmask != 0; bitmask >>= 1)
   {
      keepMask <<= 8;
      if ((mask & bitmask) == 0)
      {
         keepMask |= 0xff;   // keep this byte
      }
   }
   return value & keepMask;
}

#if !defined( IPF_LINUX_ ) && !defined( X86_LINUX_ )
//----------------------------------------------------------------------------
//< \fn void PREFETCH(const void*, u_int32)
//
//  NOP
//
//>
//----------------------------------------------------------------------------
inline void PREFETCH(const void*, u_int32 = 0)
{
}
#endif // !defined( IPF_LINUX_ ) && !defined( X86_LINUX_ )

#if !defined( IPF_LINUX_ )
//----------------------------------------------------------------------------
//< \fn void PREFETCH_M(void*, u_int32)
//
//  NOP
//
//>
//----------------------------------------------------------------------------
inline void PREFETCH_M(void*, u_int32 = 0)
{
}
#endif // !defined( IPF_LINUX_ )

//----------------------------------------------------------------------------
//< \fn void ECB(void* a)
//
//  NOP
//
//>
//----------------------------------------------------------------------------
inline void ECB(void* a)
{
}

//----------------------------------------------------------------------------
//< \fn void WH64(void* a)
//
//  NOP
//
//>
//----------------------------------------------------------------------------
inline void WH64(void* a)
{
}
#endif   // !defined( _M_ALPHA ) && !defined( __alpha )

#endif   // MACHINESPECIFIC_HXX
