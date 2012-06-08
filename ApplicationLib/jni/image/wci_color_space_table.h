/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ColorSpaceTable.c

ABSTRACT:    This file contains definitions for color space transformations

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/

#ifndef COLOR_SPACE_TABLE_H
#define COLOR_SPACE_TABLE_H

#include "../wci_portab.h"

//------------------------------------------------------------------------------

#define CSP_CONST_TABLE

#define CSP_ISBN
//#define CSP_CCIR

//------------------------------------------------------------------------------

#ifdef CSP_ISBN

// This following constants are "official spec"	Video Demystified" (ISBN 1-878707-09-4)

// RGB<->YUV transformation is lossy, since most programs do the conversion differently
// SCALEBITS/FIX taken from ffmpeg

#define Y_R_IN				0.257
#define Y_G_IN				0.504
#define Y_B_IN				0.098
#define Y_ADD_IN			16
                    
#define U_R_IN				0.148
#define U_G_IN				0.291
#define U_B_IN				0.439
#define U_ADD_IN			128
                    
#define V_R_IN				0.439
#define V_G_IN				0.368
#define V_B_IN				0.071
#define V_ADD_IN			128

#define SCALEBITS_IN  8
#define FIX_IN(x)		 ((uint16_t) ((x) * (1L<<SCALEBITS_IN) + 0.5))

#define RGB_Y_OUT		  1.164
#define B_U_OUT		  	2.018
#define Y_ADD_OUT  		16

#define G_U_OUT		  	0.391
#define G_V_OUT		  	0.813
#define U_ADD_OUT	  	128

#define R_V_OUT			  1.596
#define V_ADD_OUT	  	128

#define SCALEBITS_OUT	13
#define FIX_OUT(x)		((uint16_t) ((x) * (1L<<SCALEBITS_OUT) + 0.5))

#endif // #ifdef CSP_ISBN

#ifdef CSP_CCIR
// CCIR 601-1

#define Y_R_IN				0.29900
#define Y_G_IN				0.58700
#define Y_B_IN				0.11400
#define Y_ADD_IN			0

#define U_R_IN				0.16874
#define U_G_IN				0.33126
#define U_B_IN				0.50000
#define U_ADD_IN			128

#define V_R_IN				0.50000
#define V_G_IN				0.41869
#define V_B_IN				0.08131
#define V_ADD_IN			128

#define SCALEBITS_IN  8
#define FIX_IN(x)		 ((uint16_t) ((x) * (1L<<SCALEBITS_IN) + 0.5))

#define RGB_Y_OUT		  1.00000
#define B_U_OUT		  	1.77200
#define Y_ADD_OUT  		0

#define G_U_OUT		  	0.34414
#define G_V_OUT		  	0.71414
#define U_ADD_OUT	  	128

#define R_V_OUT			  1.40200
#define V_ADD_OUT	  	128

#define SCALEBITS_OUT	13
#define FIX_OUT(x)		((uint16_t) ((x) * (1L<<SCALEBITS_OUT) + 0.5))

#endif // #ifdef CSP_CCIR


#define BGR_SIZE      3
#define BGRA_SIZE     4
#define RGB565_SIZE   2
#define RGB666_SIZE   3

// Module-Global Variables: ----------------------------------------------------

#ifdef CSP_CONST_TABLE

  extern const int RGB_Y_tab[256+128];
  extern const int B_U_tab[256+128];
  extern const int G_U_tab[256+128];
  extern const int G_V_tab[256+128];
  extern const int R_V_tab[256+128];


#else

  extern int RGB_Y_tab[256];
  extern int B_U_tab[256];
  extern int G_U_tab[256];
  extern int G_V_tab[256];
  extern int R_V_tab[256];

#endif // #ifdef CSP_CONST_TABLE


//------------------------------------------------------------------------------

#ifndef CSP_CONST_TABLE

  void wci_color_space_init_table(void); // Initialize RGB lookup tables

#endif


#endif // #ifndef COLOR_SPACE_TABLE_H

