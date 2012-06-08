/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: wci_wav_stripe.h

ABSTRACT:    This file contains definitions and data structures for stripe processing

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef WAV_STRIPE_H
#define WAV_STRIPE_H

#include "../wci_portab.h"
#include "../wci_codec.h"
#include "../image/wci_image_component.h"
#include "wci_wav_codec.h"

//------------------------------------------------------------------------------

//#define PRINT_STRIPE_COUNTERS

#define STRIPE_SIZE_STEP 4
#define STRIPE_MIN_SIZE  STRIPE_SIZE_STEP

#define STRIPE_RESPONSE  2

//------------------------------------------------------------------------------

typedef enum tagSTRIPE_MODE
{
  STRIPE_MODE_NONE   = 0
, STRIPE_MODE_FIRST  = 1
, STRIPE_MODE_MIDDLE = 2
, STRIPE_MODE_LAST   = 3
, STRIPE_MODE_FULL   = 4

} STRIPE_MODE;

typedef struct tagWAV_STRIPE_LAYER
{
  SIZE       block_size;
  SIZE       size;

  int16_t *  buffer;
  SIZE       buffer_size;

  STRIPE_MODE mode;
  int32_t    skip_transformation;
  int32_t    total_ready_lines;        //! controls inv. transformation mode, is compared with block size
  int32_t    ready_lines;              //! controls skipping of inv. transformation
  int32_t    ready_line_pos;           //! used for component getting with skipping inv. transformation

  int32_t    free_line_pos;            //! position of free space
  int32_t    loaded_lines;             //! quantity of loaded lines
  int32_t    prepared_line_pos;        //! position of prepared lines

} WAV_STRIPE_LAYER;

typedef struct tagWAV_STRIPE
{
  int32_t    stripes;                  //! Number of image stripes
  int32_t    wav_steps;                //! Number of inv. wavelet transformation steps

  WAV_STRIPE_LAYER layer[MAX_WAV_STEPS]; //! 

  WAV_SIZE   needed_lines;             //! Number of lines for different stripe mode
                                       //!   for example, three modes for first, middle and last stripe
} WAV_STRIPE;

//------------------------------------------------------------------------------

int wci_stripe_init(
  SIZE                 IN     block_size   //! Block size of image component
, SIZE                 IN     stripe_size  //! Stripe size of image component
, int                  IN     wav_steps    //! Inv. wavelet transform steps
, int                  IN     stripes      //! Number of stripes of inv. wavelet transformation
, ALIGNED_MEMMANAGER * IN OUT mem_heap
, WAV_STRIPE *            OUT stripe
);

//------------------------------------------------------------------------------

int wci_stripe_get_buffer_size(
  SIZE IN size
, int  IN wav_steps
);

//------------------------------------------------------------------------------

const WAV_SIZE * 
  wci_stripe_set_counters(
    int          IN     component_lines
  , WAV_STRIPE * IN OUT stripe
  );

//------------------------------------------------------------------------------

void wci_stripe_inv_transformation(
  int          IN     step
, WAV_STRIPE * IN OUT stripe
);

//------------------------------------------------------------------------------

void wci_stripe_get_component(
  WAV_STRIPE *           IN OUT stripe
, int                    IN     component_lines
, DIFF_IMAGE_COMPONENT *    OUT component
);

//------------------------------------------------------------------------------

int wci_stripe_get_LL_stride(
  const WAV_STRIPE * IN stripe
);

//------------------------------------------------------------------------------

int16_t * wci_stripe_get_LL_block(
  WAV_STRIPE * IN stripe
);

//------------------------------------------------------------------------------

typedef struct tagWAV_STATE
{
  int16_t *  State;

  int16_t *  ms_State[STRIPE_SIZE_STEP];

  int16_t *  wav_buffer;

  int        w_ptr, r_ptr;

  SIZE       Size, Full_Size;

  int        Skip;

  int        Prev_I, I;
  int        R,   Next_R;
  int        N,   Next_N;

  int        is_last;

} WAV_STATE;

typedef struct tagWAV_STRIPE_NO
{
  int        Qnt_Stripes;                  //! Number of stripes of inv. wavelet transformation

  int        iWavSteps;

  int16_t *  Temp_Buffer;

  WAV_STATE  Wav_State[MAX_WAV_STEPS];
  WAV_STATE  Wav_State_LL;

  WAV_SIZE   needed_lines;                 //! Number of lines for different stripe mode
                                           //!   for example, three modes for first, middle and last stripe

  int16_t *  Unclaimed_Lines[STRIPE_SIZE_STEP];

  int        UL_wr_ptr, UL_rd_ptr, UL_qnt, UL_saved;
                                            
} WAV_STRIPE_NO;

//------------------------------------------------------------------------------

ERROR_CODE wci_stripe_init_state(
  SIZE                 IN     rBlockSize   //! Block size of image component
, SIZE                 IN     rStripeSize  //! Stripe size of image component
, int                  IN     nWavSteps    //! Inv. wavelet transform steps
, int                  IN     iStripes     //! Number of stripes of inv. wavelet transformation
, ALIGNED_MEMMANAGER * IN OUT prMemHeap
, WAV_STRIPE_NO   *       OUT Wav_Stripe
);

int wci_stripe_get_buffersize(
  SIZE IN size
, int  IN iWavSteps
);

void wci_stripe_done_state( WAV_STRIPE_NO * OUT Wav_Stripe, ALIGNED_MEMMANAGER * IN prMemHeap );

//------------------------------------------------------------------------------

typedef int Function_SIWT( int16_t * img,   WAV_STATE * Wav_Stat, int16_t *pshTempBuffer );

//------------------------------------------------------------------------------

WAV_SIZE * wci_stripe_prepare(
  int             IN     needed_lines //! Value sets wavelet transformation parameters
, WAV_STRIPE_NO * IN OUT Wav_Stripe
);

//------------------------------------------------------------------------------

WAV_SIZE * wci_one_stripe_prepare(
  int             IN     needed_lines //! Value sets wavelet transformation parameters
, WAV_STRIPE_NO * IN OUT Wav_Stripe
);

//------------------------------------------------------------------------------

void  wci_stripe_update_inr(WAV_STRIPE_NO *Wav_Stripe);

//------------------------------------------------------------------------------

void wci_stripe_getcomponent(
  WAV_STRIPE_NO *         IN  Wav_Stripe
, int                     IN  needed_lines
, DIFF_IMAGE_COMPONENT *  OUT prComponent
);

//------------------------------------------------------------------------------

#endif // #ifndef WAV_STRIPE_H
