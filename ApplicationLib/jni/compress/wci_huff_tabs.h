/*.===************************ File : wci_huff_tabs.h ******************************
PROJECT:	Video Codec for DVR
			LGTCM S/W Lab, St. Petersburg, Russia 
			February 2004
*------------------------------------------------------------------------------*
REVISION: February 2004
ABSTRACT: This file contains definitions of tables for performing Huffman 
			compression/decompression
Contacts: 
Alexandr Maiboroda               <alm@lgsoftlab.ru>, 
Michael  Sall                    <msall@lgsoftlab.ru>,
Sergey  Gramnitsky               <sernigra@lgsoftlab.ru>,
Tikhotsky Anatoly                <ait@lgsoftlab.ru> 
**************************************************************************===.*/

#ifndef HUFF_TABS_H
#define HUFF_TABS_H
/*- Includes: ----------------------------------------------------------------*/
#include "../wci_portab.h"

/*- Local Defines: -----------------------------------------------------------*/
#define SH_MAXLENGTH (31) 

/*- Definition of datatypes: -------------------------------------------------*/
/*- Definitions of the Exported Global Variables: ----------------------------*/
extern uchar htab_hz[3][1];

extern uchar htab_hnz[3][1];

extern uchar htab_symb[3][256];

extern uchar htab_len[3][256];

extern uchar htab_code[3][256];

extern uchar htab_cache[3][256];

extern uchar htab_base[3][SH_MAXLENGTH];

extern uchar htab_offs[3][SH_MAXLENGTH];

/*- Prototypes of the Exported Functions: ------------------------------------*/

#endif // #ifndef HUFF_TABS_H
