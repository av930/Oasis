/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: codec.c

ABSTRACT:    This file contains codec interface and data structures description

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 put some code inder ENCODER_SUPPORT define


*******************************************************************************/

#ifdef ASM_SUPPORT
  #ifdef _MSC_VER
    #include <windows.h>
  #else
    #include <signal.h>
    #include <setjmp.h>
  #endif
#endif

#include "wci_portab.h"
#include "wci_codec.h"
#ifdef ENCODER_SUPPORT
  #include "utils/wci_emms.h"
#endif
#include "bitstream/wci_bitstream.h"
#include "image/wci_color_space_table.h"
#include "image/wci_image_convert.h"
#include "compress/wci_vlc.h"
#include "wav_codec/wci_wavelet.h"
#include "wav_codec/wci_wav_quant.h"

//------------------------------------------------------------------------------

#ifdef ASM_SUPPORT
  ERROR_CODE wci_support_force_cpu_features( INIT_PARAM * IN OUT init_param );
#endif

//------------------------------------------------------------------------------

ERROR_CODE  wci3_get_version
(
 CODEC_VERSION* OUT    prCodecVersion
)
{
  if( prCodecVersion != NULL )
  {
    prCodecVersion->iAPI_version  = API_VERSION;
    prCodecVersion->iCoreBuild    = CORE_BUILD;
    prCodecVersion->iStreamFormat = STREAM_FORMAT_VERSION;
    return ERR_NONE;
  }
  else
    return ERR_PARAM;
}

//------------------------------------------------------------------------------
#ifdef ENCODER_SUPPORT

int wci_init(
  void *handle,
	int   opt,
	void *param1,
	void *param2)
{
	INIT_PARAM *init_param;

  handle=handle, opt=opt, param2=param2;

  wci_vlc_init_tables();
  
  wci_init_wav_quant_data();
  
#ifdef USE_BOUND_ARRAY
  wci_init_bound_array(); // Image transformation related functions
#endif

#ifndef CSP_CONST_TABLE
  wci_color_space_init_table(); // Initialize internal colorspace transformation tables
#endif

  // Restore FPU context : emms_c is a nop functions 
  emms = wci_emms_c;

	init_param = (INIT_PARAM *) param1;

	// Inform the client the API version 
	init_param->iAPI_version = API_VERSION;

	// Inform the client the core build 
	init_param->iCoreBuild = CORE_BUILD;

  init_param->iCPU_flags = 0;

#ifdef ASM_SUPPORT
  wci_support_force_cpu_features( init_param );
#endif

	return ERR_NONE;
}

#ifdef ASM_SUPPORT

#if defined(ARCH_IS_IA32)
#ifndef _MSC_VER

static jmp_buf g_rMark;

static void
wci_sigill_handler(int signal)
{
  longjmp(g_rMark, 1);
}
#endif

//------------------------------------------------------------------------------

int wci_sigill_check(void (*func)())
{
#ifdef _MSC_VER
	_try {
		func();
	}
	_except(EXCEPTION_EXECUTE_HANDLER) {

		if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
			return 1;
	}
	return 0;
#else
  void * old_handler;
  int jmpret;


  old_handler = signal(SIGILL, wci_sigill_handler);
  if (old_handler == SIG_ERR)
  {
      return -1;
  }

  jmpret = setjmp(g_rMark);
  if (jmpret == 0)
  {
      func();
  }

  signal(SIGILL, old_handler);

  return jmpret;
#endif
}
#endif

//------------------------------------------------------------------------------

ERROR_CODE wci_support_force_cpu_features( INIT_PARAM * IN OUT init_param )
{
  int iCPU_flags;

	// Do we have to force CPU features ?
	if ((init_param->iCPU_flags & CPU_FORCE)) {

		iCPU_flags = init_param->iCPU_flags;

	} else {

		iCPU_flags = check_cpu_features();

#if defined(ARCH_IS_IA32)
		if ((iCPU_flags & CPU_SSE) && wci_sigill_check(sse_os_trigger))
			iCPU_flags &= ~CPU_SSE;

		if ((iCPU_flags & CPU_SSE2) && wci_sigill_check(sse2_os_trigger))
			iCPU_flags &= ~CPU_SSE2;
#endif
	}

	if ((init_param->iCPU_flags & CPU_CHKONLY))
	{
		init_param->iCPU_flags = iCPU_flags;
		return ERR_NONE;
	}

	init_param->iCPU_flags = iCPU_flags;

#ifdef ARCH_IS_IA32
	if ((iCPU_flags & CPU_MMX) > 0)
	{
		// To restore FPU context after mmx use
		emms = wci_emms_mmx;

		// Block related functions
    transfer8x8_8to16sub  = transfer8x8_8to16sub_mmx;
    block_transfer8x8_8to16sub  = block_transfer8x8_8to16sub_mmx;
		
		transfer8x8_16to8add  = transfer8x8_16to8add_mmx;
    block_transfer8x8_16to8add = block_transfer8x8_16to8add_mmx;
		
    transfer8x8_copy   = transfer8x8_copy_mmx;

    // Image transformation related functions
    ConvertImageToDiffImage = ConvertImageToDiffImage_mmx;
    ConvertDiffImageToImage = ConvertDiffImageToImage_mmx;
  }

	if ((iCPU_flags & CPU_MMXEXT) > 0)
	{
    transfer16x16_copy = transfer16x16_copy_xmm;

		// Buffer transfer
		transfer8x8_8to16sub2 = transfer8x8_8to16sub2_xmm;
	}

	if ((iCPU_flags & CPU_SSE2) > 0) 
	{
    // Wavelet transformation functions
    wci_lift_wav_block    = LiftWavBlock_mmx   ;
    wci_inv_lift_wav_block = InvLiftWavBlock_mmx;
	}
#endif
	return ERR_NONE;
}
#endif // ifdef ENCODER_SUPPORT

#endif // ifdef ASM_SUPPORT
