/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_portab.h

ABSTRACT:    This file contains defenitions for porting codec to different platforms

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef _PORTAB_H_
#define _PORTAB_H_

//------------------------------------------------------------------------------
// Debug level masks 
#define DPRINTF_ERROR		  0x00000001
#define DPRINTF_WARNING	  0x00000002
#define DPRINTF_STATE    	0x00000010
#define DPRINTF_INFO  		0x00000100
#define DPRINTF_TIMECODE	0x00000008
#define DPRINTF_DEBUG			0x80000000

// Current debug level
#define DPRINTF_LEVEL	    0x00000003 // DPRINTF_ERROR, DPRINTF_WARNING

// Message buffer size for msvc implementation because it outputs to DebugOutput
#define DPRINTF_MSG_SIZE  2048

//------------------------------------------------------------------------------
// For MSVC

#if defined(_MSC_VER)
#    define int8_t   char
#    define uint8_t  unsigned char
#    define int16_t  short
#    define uint16_t unsigned short
#    define int32_t  int
#    define uint32_t unsigned int
#    define int64_t  __int64
#    define uint64_t unsigned __int64
#    define uint     unsigned int
#    define uchar    unsigned char

#ifdef PDK_EMULATOR
  #undef assert
  #include "Error.h"
	extern void				Error_Exception(char* pStrText1,char* pStrText2);
  #define assert( xx_exp ) \
     if( !(xx_exp) ) \
     { \
    		ERROR_EXCEPTION(""#xx_exp"");\
     }
#else
  #include <assert.h>
#endif

#elif defined(_EVC_VER)
#    include <types.h>

//------------------------------------------------------------------------------
// For ARM Linux
#elif defined(_LINUX_)

#   include <inttypes.h>

#   define BYTE uint8_t
#   define BOOL uint32_t
#   define uint uint32_t

#   define HANDLE void *
#   define DWORD unsigned long
#   define LPBYTE uint8_t *
#   define LPCSTR const char *

#define IN
#define OUT

#define GetLastError() 0

#ifndef FALSE
#      define  FALSE   (0)
#endif

#ifndef TRUE
#      define  TRUE    (1)
#endif

#ifndef NULL
#      define  NULL ((void *)0)
#endif

#define stricmp strcmp
#define strnicmp strncmp

#  ifndef max
#    define max(a,b)            (((a) > (b)) ? (a) : (b))
#  endif

#  ifndef min
#   define min(a,b)            (((a) < (b)) ? (a) : (b))
#  endif

//------------------------------------------------------------------------------
// For ARM BASE PDK (LP_3000)
#elif defined(T_ARM)
#if !defined (PDK_EMULATOR) && !defined (PDK_MAIN_VERSION)  &&  !defined (_WCI_M4410_)
#include	"..\..\..\Api\ApiLink\ApiPdk.Inc"
#endif

#define ARCH_IS_32BIT_ARM
#define ARCH_IS_GENERIC

#define IN
#define OUT

//#define ASSERT_ON

#ifdef ASSERT_ON
  #undef assert
  #include <stdio.h>
  #include "Error.h"
  extern void				Error_Exception(char* pStrText1,char* pStrText2);
  #define assert( xx_exp ) \
   if( !(xx_exp) ) \
   { \
  		ERROR_EXCEPTION(""#xx_exp"");\
   }
#else
  #undef assert
  #define assert( xx_exp ) ((void)0)
#endif //ifdef ASSERT_ON

#ifndef _WCI_M4410_
typedef unsigned int size_t;
#endif

typedef unsigned char BYTE;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned int uint;
typedef unsigned char uchar;

typedef unsigned int BOOL;

#ifndef NULL
#      define  NULL ((void *)0)
#endif

#  ifndef max
#    define max(a,b)            (((a) > (b)) ? (a) : (b))
#  endif

#  ifndef min
#   define min(a,b)            (((a) < (b)) ? (a) : (b))
#  endif

#ifndef FALSE
#      define  FALSE   (0)
#endif

#ifndef TRUE
#      define  TRUE    (1)
#endif

typedef struct tagSIZE
{
  long cx;
  long cy;

} SIZE;

typedef struct tagRECT
{
  int left;
  int top;
  int right;
  int bottom;

} RECT;
#elif defined(ANDROID)
#define ARCH_IS_32BIT_ARM
#define ARCH_IS_GENERIC
#define __inline

#define IN
#define OUT
#include <stdio.h>

//#define ASSERT_ON

#ifdef ASSERT_ON
  #undef assert
  #include <stdio.h>
  #include "Error.h"
  extern void				Error_Exception(char* pStrText1,char* pStrText2);
  #define assert( xx_exp ) \
   if( !(xx_exp) ) \
   { \
  		ERROR_EXCEPTION(""#xx_exp"");\
   }
#else
  #undef assert
  #define assert( xx_exp ) ((void)0)
#endif //ifdef ASSERT_ON

#ifndef _WCI_M4410_
typedef unsigned int size_t;
#endif

typedef unsigned char BYTE;

typedef unsigned char uchar;

typedef unsigned int BOOL;

#ifndef NULL
#      define  NULL ((void *)0)
#endif

#  ifndef max
#    define max(a,b)            (((a) > (b)) ? (a) : (b))
#  endif

#  ifndef min
#   define min(a,b)            (((a) < (b)) ? (a) : (b))
#  endif

#ifndef FALSE
#      define  FALSE   (0)
#endif

#ifndef TRUE
#      define  TRUE    (1)
#endif

typedef struct tagSIZE
{
  long cx;
  long cy;

} SIZE;

typedef struct tagRECT
{
  int left;
  int top;
  int right;
  int bottom;

} RECT;
//------------------------------------------------------------------------------
// For all other compilers, use the standard header file
// (compiler should be ISO C99 compatible, perhaps ISO C89 is enough)
#else

#    include <inttypes.h>

#endif

//------------------------------------------------------------------------------
//  Some things that are only architecture dependant

#if defined(ARCH_IS_32BIT)
#    define CACHE_LINE  16
#    define CACHE_LINE_ALIGNED
#    define ptr_t uint32_t
#elif defined(ARCH_IS_64BIT)
#    define CACHE_LINE  32
#    define ptr_t uint64_t
#elif defined(ARCH_IS_32BIT_ARM)
#    define CACHE_LINE  32
#    define ptr_t uint32_t
//#    define CACHE_LINE_ALIGNED __attribute__ ((__aligned__ (32)))
#    define CACHE_LINE_ALIGNED
#else
#    error You are trying to compile project without defining address bus size.
#endif

//------------------------------------------------------------------------------
//  MSVC compiler specific macros, functions

#if defined(_MSC_VER)

#include <windows.h>
#include <stdio.h>
#include <limits.h>

    /*
     * This function must be declared/defined all the time because MSVC does
     * not support C99 variable arguments macros.
     *
     * Btw, if the MS compiler does its job well, it should remove the nop
     * DPRINTF function when not compiling in _DEBUG mode
     */
#   if defined(_DEBUG) && (DPRINTF_LEVEL > 0)
    static __inline void DPRINTF(int level, const char *fmt, ...)
    {
        if (DPRINTF_LEVEL & level) {
            va_list args;
            char buf[DPRINTF_MSG_SIZE];
            va_start(args, fmt);
            vsprintf(buf, fmt, args);
            OutputDebugString(buf);
            fprintf(stderr, "%s\n", buf);
         }
     }
#    else
     static __inline void DPRINTF(int level, const char *fmt, ...)
     {
       level, fmt;
     }
#    endif

#    if _MSC_VER <= 1200
#        define DECLARE_ALIGNED_MATRIX(name,sizex,sizey,type,alignment) \
                type name##_storage[(sizex)*(sizey)+(alignment)-1]; \
                type * name = (type *) (((int32_t) name##_storage+(alignment - 1)) & ~((int32_t)(alignment)-1))
#    else
#        define DECLARE_ALIGNED_MATRIX(name,sizex,sizey,type,alignment) \
                __declspec(align(alignment)) type name[(sizex)*(sizey)]
#    endif


/*----------------------------------------------------------------------------
 | msvc x86 specific macros/functions
 *---------------------------------------------------------------------------*/
#    if defined(ARCH_IS_IA32)
#        define BSWAP(a) __asm mov eax,a __asm bswap eax __asm mov a, eax
#        define EMMS() __asm {emms}

#      ifdef _PROFILING_

typedef int64_t count_t;
#define PRF_LEGEND   "   microseconds:     calls:            cycles:     name:\n\n"
     
     
       static __inline int64_t read_counter(void)
             {
                 int64_t ts;
                 uint32_t ts1, ts2;
                 __asm {
                     rdtsc
                     mov ts1, eax
                     mov ts2, edx
                 }
                 ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
                 return ts;
             }
			 static  __inline int64_t get_CPUSpeed(void)
			 {
				 int32_t dwTimerHi, dwTimerLo; 
				 int64_t dRes; 
				 __asm { 
					 rdtsc
						 mov dwTimerLo, EAX 
						 mov dwTimerHi, EDX 
				 } 
				 Sleep (500); 
				 __asm { 
					 rdtsc
						 sub EAX, dwTimerLo 
						 sub EAX, dwTimerHi 
						 mov dwTimerLo, EAX 
						 mov dwTimerHi, EDX 
				 } 
				 dRes = (int64_t)(dwTimerLo/(1000.0*500));
				 return dRes;
			 }
#        endif

/*----------------------------------------------------------------------------
 | msvc GENERIC (plain C only) - Probably alpha or some embedded device
 *---------------------------------------------------------------------------*/
#    elif defined(ARCH_IS_GENERIC)
#        define BSWAP(a) \
                ((a) = (((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
                       (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))
#        define EMMS()

#        ifdef _PROFILING_
#            include <time.h>
             static __inline int64_t read_counter(void)
             {
                 return (int64_t)clock();
             }
#        error It s required to insert get_CPUSpeed() function for profiling mode.
#        endif

/*----------------------------------------------------------------------------
 | msvc Not given architecture - This is probably an user who tries to build
 | XviD the wrong way.
 *---------------------------------------------------------------------------*/
#    else
#        error You are trying to compile project without defining the architecture type.
#    endif

//------------------------------------------------------------------------------
//  GNU CC compiler stuff

#elif defined(__GNUC__) || defined(__ICC) /* Compiler test */

/*----------------------------------------------------------------------------
 | Common gcc stuff
 *---------------------------------------------------------------------------*/

/*
 * As gcc is (mostly) C99 compliant, we define DPRINTF only if it's realy needed
 * and it's a macro calling fprintf directly
 */
#    if defined(_DEBUG) && (DPRINTF_LEVEL > 0)

        /* Needed for all debuf fprintf calls */
#       include <stdio.h>
#       include <stdarg.h>

        static __inline void DPRINTF(int level, const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            if(DPRINTF_LEVEL & level) {
                   vfprintf(stderr, format, args);
                   fprintf(stderr, "\n");
			}
		}

#    else /* _DEBUG */
        static __inline void DPRINTF(int level, const char *format, ...) {}
#    endif /* _DEBUG */


#    define DECLARE_ALIGNED_MATRIX(name,sizex,sizey,type,alignment) \
            type name##_storage[(sizex)*(sizey)+(alignment)-1]; \
            type * name = (type *) (((ptr_t) name##_storage+(alignment - 1)) & ~((ptr_t)(alignment)-1))

/*----------------------------------------------------------------------------
 | gcc IA32 specific macros/functions
 *---------------------------------------------------------------------------*/
#    if defined(ANDROID)
#        define BSWAP(a) \
                ((a) = (((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
                       (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))
#        define EMMS()

#    elif defined(ARCH_IS_IA32)
#        define BSWAP(a) __asm__ ( "bswapl %0\n" : "=r" (a) : "0" (a) );
#        define EMMS() __asm__ ("emms\n\t");

#        ifdef _PROFILING_
             static __inline int64_t read_counter(void)
             {
                 int64_t ts;
                 uint32_t ts1, ts2;
                 __asm__ __volatile__("rdtsc\n\t":"=a"(ts1), "=d"(ts2));
                 ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
                 return ts;
             }
#        error It s required to insert get_CPUSpeed() function for profiling mode.
#        endif

/*----------------------------------------------------------------------------
 | gcc PPC and PPC Altivec specific macros/functions
 *---------------------------------------------------------------------------*/
#    elif defined(ARCH_IS_PPC)
#        define BSWAP(a) __asm__ __volatile__ \
                ( "lwbrx %0,0,%1; eieio" : "=r" (a) : "r" (&(a)), "m" (a));
#        define EMMS()

#        ifdef _PROFILING_
             static __inline unsigned long get_tbl(void)
             {
                 unsigned long tbl;
                 asm volatile ("mftb %0":"=r" (tbl));
                 return tbl;
             }

             static __inline unsigned long get_tbu(void)
             {
                 unsigned long tbl;
                 asm volatile ("mftbu %0":"=r" (tbl));
                 return tbl;
             }

             static __inline int64_t read_counter(void)
             {
                 unsigned long tb, tu;
                 do {
                     tu = get_tbu();
                     tb = get_tbl();
                 }while (tb != get_tbl());
                 return (((int64_t) tu) << 32) | (int64_t) tb;
             }
#        error It s required to insert get_CPUSpeed() function for profiling mode.
#        endif

/*----------------------------------------------------------------------------
 | gcc IA64 specific macros/functions
 *---------------------------------------------------------------------------*/
#    elif defined(ARCH_IS_IA64)
#        define BSWAP(a)  __asm__ __volatile__ \
                ("mux1 %1 = %0, @rev" ";;" \
                 "shr.u %1 = %1, 32" : "=r" (a) : "r" (a));
#        define EMMS()

#        ifdef _PROFILING_
             static __inline int64_t read_counter(void)
             {
                 unsigned long result;
                 __asm__ __volatile__("mov %0=ar.itc" : "=r"(result) :: "memory");
                 return result;
             }
#        error It s required to insert get_CPUSpeed() function for profiling mode.
#        endif

/*----------------------------------------------------------------------------
 | gcc GENERIC (plain C only) specific macros/functions
 *---------------------------------------------------------------------------*/
#    elif defined(ARCH_IS_GENERIC)
#        define BSWAP(a) \
                ((a) = (((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
                       (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))
#        define EMMS()

#        ifdef _PROFILING_
#            include <time.h>
             static __inline int64_t read_counter(void)
             {
                 return (int64_t)clock();
             }
#        error It s required to insert get_CPUSpeed() function for profiling mode.
#        endif

/*----------------------------------------------------------------------------
 | gcc Not given architecture - This is probably an user who tries to build
 | XviD the wrong way.
 *---------------------------------------------------------------------------*/
#    else
#        error You are trying to compile XviD without defining the architecture type.
#    endif

/*****************************************************************************
 *  Unknown compiler
 ****************************************************************************/
#else /* Compiler test */

      /*
	   * Ok we know nothing about the compiler, so we fallback to ANSI C
	   * features, so every compiler should be happy and compile the code.
	   *
	   * This is (mostly) equivalent to ARCH_IS_GENERIC.
	   */

#    if defined(_DEBUG) && (DPRINTF_LEVEL > 0)

        /* Needed for all debuf fprintf calls */
/*#       include <stdio.h>
#       include <stdarg.h>

        static __inline void DPRINTF(int level, char *format, ...)
        {
            va_list args;
            va_start(args, format);
            if(DPRINTF_LEVEL & level) {
                   vfprintf(stderr, format, args);
                   fprintf(stderr, "\n");
			}
		}
*/
#    else /* _DEBUG */
        static __inline void DPRINTF(int level, char *format, ...) {}
#    endif /* _DEBUG */

#    define BSWAP(a) \
            ((a) = (((a) & 0xff) << 24)  | (((a) & 0xff00) << 8) | \
                   (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))

#    define EMMS()

#    ifdef _PROFILING_
#define int64_t __int64
#define uint64_t unsigned __int64
#define int8_t char
#define GEN_PURPS_TIMER_ADDR                                0x80000740
#define TIMER_BYTE_MASK                                     0xff
#define CNT_PER_TICK                                        96.15
#define CNT_PER_MS                                          19
#define PRF_LEGEND   "   milliseconds:     calls:            cycles:     name:\n\n"
typedef unsigned int uint32_t;
typedef uint32_t count_t;

              static __inline count_t read_counter(void) 
              {
                 int8_t  cont;
                 count_t cl1, cl2;

                 cl1 = System_GetTickCount();
                 cont = (int8_t)(*((volatile int8_t *) (GEN_PURPS_TIMER_ADDR)));
                 cl2 = System_GetTickCount();
                 if(cl1 != cl2)
                 {
                   cl1 = cl2; 
                   cont = (int8_t)(*((volatile int8_t *) (GEN_PURPS_TIMER_ADDR)));
                 }
                 return (count_t)(cl1 * CNT_PER_TICK + cont);
              }

              static __inline count_t get_CPUSpeed(void) 
              {
                 return (count_t)CNT_PER_MS;
              }


#    endif
#    define DECLARE_ALIGNED_MATRIX(name,sizex,sizey,type,alignment) \
                type name[(sizex)*(sizey)]

#endif /* Compiler test */

#ifdef ARCH_IS_BIG_ENDIAN
	#define GET_WORD32(pbt) ( (uint)((BYTE *)(pbt))[3] | (((uint)((BYTE *)(pbt))[2])<<8) | ( ((uint)((BYTE *)(pbt))[1])<<16) | ( ((uint)((BYTE *)(pbt))[0])<<24) )
	#define GET_WORD16(pbt) ( (uint)((BYTE *)(pbt))[1] | (((uint)((BYTE *)(pbt))[0])<<8) ) 
#else
	#define GET_WORD32(pbt) ( (uint)((BYTE *)(pbt))[0] | (((uint)((BYTE *)(pbt))[1])<<8) | ( ((uint)((BYTE *)(pbt))[2])<<16) | ( ((uint)((BYTE *)(pbt))[3])<<24) )
	#define GET_WORD16(pbt) ( (uint)((BYTE *)(pbt))[0] | (((uint)((BYTE *)(pbt))[1])<<8) ) 
#endif

#endif // #ifndef _PORTAB_H_
