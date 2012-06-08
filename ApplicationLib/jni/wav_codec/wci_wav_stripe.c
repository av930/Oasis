/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: WavStripe.c

ABSTRACT:    This file contains procedures for stripe processing

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00
             2004.08.27   v1.01 Wavelet transform functions are moved to WavStripeTrans.c

*******************************************************************************/

#include "../utils/wci_mem_align.h"
#include "wci_wav_codec.h"
#include "wci_wav_stripe.h"

//------------------------------------------------------------------------------

#define STRIPE_SIZE (STRIPE_SIZE_STEP + STRIPE_SIZE_STEP)
//#define OUT_IMAGE // for debug only

//------------------------------------------------------------------------------

int wci_stripe_init(
  SIZE                 IN     block_size   //! Block size of image component
, SIZE                 IN     stripe_size  //! Stripe size of image component
, int                  IN     wav_steps    //! Inv. wavelet transform steps
, int                  IN     stripes      //! Number of stripes of inv. wavelet transformation
, ALIGNED_MEMMANAGER * IN OUT mem_heap
, WAV_STRIPE *            OUT stripe
)
{
  //assert(stripe_size.cy >= STRIPE_MIN_SIZE);
  assert(stripes >= 1);

  memset( stripe, 0, sizeof(WAV_STRIPE) );

  stripe->wav_steps = wav_steps;
  stripe->stripes   = stripes;

  if (wav_steps > 0)
  {
    int i;
    for ( i=0; i<wav_steps; i++ )
    {
      WAV_STRIPE_LAYER * layer = &stripe->layer[i];
      int32_t            height;

      layer->block_size  = block_size;
      layer->size        = stripe_size;
      layer->buffer_size = stripe_size;
      height = max( stripe_size.cy, STRIPE_SIZE - stripe_size.cy/2 );
      layer->buffer_size.cy = (height+1) & 0xFFFFFFFE; // rounding to 2

      layer->buffer = (int16_t *) wci_aligned_malloc( wci_get_square( layer->buffer_size ) * sizeof(int16_t), CACHE_LINE, mem_heap );
      if(layer->buffer == NULL)
        return ERR_MEMORY;

      block_size.cx /= 2;
      block_size.cy /= 2;

      stripe_size.cx /= 2;
      stripe_size.cy  = max(STRIPE_SIZE_STEP/2, stripe_size.cy/2);
    }
  }
  else
  {
    WAV_STRIPE_LAYER * layer = &stripe->layer[0];

    layer->block_size  = block_size;
    layer->size        = stripe_size;
    layer->buffer_size = stripe_size;

    layer->buffer = (int16_t *) wci_aligned_malloc( wci_get_square( layer->buffer_size ) * sizeof(int16_t), CACHE_LINE, mem_heap );
    if(layer->buffer == NULL)
      return ERR_MEMORY;
  }

  return ERR_NONE;
}

//------------------------------------------------------------------------------

int wci_stripe_get_buffer_size(
  SIZE IN stripe_size
, int  IN wav_steps
)
{
  int buffer_size;

  buffer_size = stripe_size.cx * (stripe_size.cy+2) * sizeof(int16_t) + CACHE_LINE;

  {
    int i;

    for ( i=0; i<max(wav_steps, 1); i++ )
    {
      int32_t           height;

      height = max( stripe_size.cy, STRIPE_SIZE - stripe_size.cy/2 );
      height = (height+1) & 0xFFFFFFFE; // rounding to 2 

      buffer_size += stripe_size.cx * height * sizeof(int16_t) + CACHE_LINE;

      stripe_size.cx /= 2;
      stripe_size.cy  = max(STRIPE_SIZE_STEP/2, stripe_size.cy/2);
    }
  }

  return buffer_size;
}

//------------------------------------------------------------------------------

int wci_stripe_get_LL_stride(
  const WAV_STRIPE * IN stripe
)
{
  const WAV_STRIPE_LAYER * layer = &stripe->layer[max(stripe->wav_steps-1, 0)];

  if (stripe->wav_steps > 0)
    return 2*layer->block_size.cx;
  else
    return layer->block_size.cx;
}

//------------------------------------------------------------------------------

int16_t * wci_stripe_get_LL_block(
  WAV_STRIPE * IN stripe
)
{
  WAV_STRIPE_LAYER * layer = &stripe->layer[max(stripe->wav_steps-1, 0)];

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " get LL(%3d), free %3d\n"
      , layer->block_size.cx
      , layer->free_line_pos
      );
  #endif

  return &layer->buffer[layer->free_line_pos*layer->block_size.cx];
}

//------------------------------------------------------------------------------

const WAV_SIZE * 
  wci_stripe_set_counters(
    int          IN     component_lines
  , WAV_STRIPE * IN OUT stripe
  )
{
  int needed_lines = component_lines;
  int n;

  for ( n=0; n<stripe->wav_steps; ++n )
  {
    WAV_STRIPE_LAYER * layer = &stripe->layer[n];

    #ifdef PRINT_STRIPE_COUNTERS
      DPRINTF( DPRINTF_DEBUG, " sc(%3d)", layer->block_size.cx );
    #endif

    if (needed_lines > layer->ready_lines)
    {
      int rest = layer->block_size.cy - layer->total_ready_lines - (layer->loaded_lines - layer->ready_lines);

      layer->skip_transformation = FALSE;

      stripe->needed_lines.iWC[n] = STRIPE_SIZE_STEP;

      if ( layer->total_ready_lines > 0 )
      {
        if ( rest > STRIPE_SIZE_STEP )
          layer->mode = STRIPE_MODE_MIDDLE;
        else
        {
          layer->mode = STRIPE_MODE_LAST;
          stripe->needed_lines.iWC[n] = rest;
        }
      }
      else
      {
        if ( layer->size.cy != layer->block_size.cy )
          layer->mode = STRIPE_MODE_FIRST;
        else
          layer->mode = STRIPE_MODE_FULL;
      }

      // Set needed lines for next layer
      needed_lines = stripe->needed_lines.iWC[n]/2;
    }
    else
    {
      layer->skip_transformation  = TRUE;
      layer->mode                 = STRIPE_MODE_NONE;
      stripe->needed_lines.iWC[n] = 0;
      needed_lines                = 0;
    }

    // Shift lines
    if( !layer->skip_transformation )
    {
      if (layer->free_line_pos + STRIPE_SIZE_STEP > layer->buffer_size.cy )
      {
        int pos = layer->prepared_line_pos - layer->ready_lines;
        assert(layer->prepared_line_pos > 0);
        assert(pos >= 0);

        #ifdef PRINT_STRIPE_COUNTERS
          DPRINTF( DPRINTF_DEBUG, " shift load(%d) in %d", layer->loaded_lines, layer->prepared_line_pos );
        #endif

        memcpy( layer->buffer, &layer->buffer[pos*layer->buffer_size.cx]
              , layer->loaded_lines*layer->buffer_size.cx*sizeof(int16_t) );

        layer->free_line_pos     -= pos;
        layer->ready_line_pos    -= pos;
        layer->prepared_line_pos  = layer->ready_lines;
      }
    }

    #ifdef PRINT_STRIPE_COUNTERS
      DPRINTF( DPRINTF_DEBUG, ", need %3d, ready(%3d %3d), %s\n"
            , stripe->needed_lines.iWC[n]
            , layer->total_ready_lines
            , layer->ready_lines
            , layer->skip_transformation > 0 ? "skip" : "trans"
          );
    #endif
  }

  if ( stripe->wav_steps > 0 )
  {
    stripe->needed_lines.iLL = needed_lines;
    if( needed_lines > 0 )
    {
      WAV_STRIPE_LAYER * layer = &stripe->layer[n-1];
      if ( layer->mode != STRIPE_MODE_LAST )
        stripe->needed_lines.iLL = STRIPE_SIZE_STEP/2;
      else
      {
        int rest = layer->block_size.cy - layer->total_ready_lines - STRIPE_RESPONSE;
        stripe->needed_lines.iLL = min(rest/2, STRIPE_SIZE_STEP/2);
      }
    }
  }
  else
    stripe->needed_lines.iLL = component_lines;

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " LL need %3d\n", stripe->needed_lines.iLL );
  #endif

  return &stripe->needed_lines;
}

//------------------------------------------------------------------------------

void wci_stripe_get_component(
  WAV_STRIPE *           IN OUT stripe
, int                    IN     component_lines
, DIFF_IMAGE_COMPONENT *    OUT component
)
{
  WAV_STRIPE_LAYER * layer = &stripe->layer[0];

  layer->ready_lines  -= component_lines;
  layer->loaded_lines -= component_lines;

  component->piData      = &layer->buffer[layer->ready_line_pos*layer->block_size.cx];
  component->iReadyLines = component_lines;

  if (stripe->wav_steps > 0)
    layer->ready_line_pos += component_lines;

  #ifdef PRINT_STRIPE_COUNTERS
    DPRINTF( DPRINTF_DEBUG, " gc, %d (%3d), %3d %3d \n"
            , component_lines
            , layer->ready_lines
            , layer->loaded_lines
            , layer->ready_line_pos
          );
  #endif
}

