/* In this file included suit of primitivs for profiling purposes
 wci_pfinit_counters()  -  to use for initialization of profiling features
 wci_pfbegin_count()    -  to use for start of counting
 wci_pfstop_count()	  -  to use for stop of counting	
 wci_pfget_results()    -  to use for getting results of profiling
*/
#ifndef _PROFILE_H_
#define _PROFILE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <string.h>
#include <stdio.h>
#include "wci_portab.h"

// Add here new task for profile.
// Preset task's name into structure "PF_TASK_NAME" which defined in file "profile.c"
typedef enum  tagPF_TASK_ID
{
  PF_MAIN                 = 0,
  PF_UNPACK_HEADER,
  PF_INITIALIZATION,
  PF_PREPARE_DECODER,
  PF_DECODE_STRIPE,
  PF_OUT_STRIPE,
  PF_DONE,
  PF_STRIPE_SET_COUNTERS,
  PF_DECOMPRESSLL_STRIPE,
  PF_ADPCM_DECODE_STRIPE,
  PF_QSP_DECOMPRESS_STRIPE,
  PF_STRIPE_INSERT_LL,
  PF_DEQUANTSTEPSTRIPE,
  PF_STRIPE_INVWAVTRANS,
  PF_STRIPE_GETCOMPONENT,
  PF_VER_TRANSFORMATION,
  PF_HOR_STEP_STRIPE_INV,
  PF_ID_LAST  //must be last
} PF_TASK_ID;

//------------------------------------------------------------------------------

#ifndef _PROFILING_
#define PF_INIT_COUNTERS()  ((void) 0)
#define PF_BEGIN_COUNT(x)   ((void) 0)
#define PF_STOP_COUNT(x)    ((void) 0)
#define PF_GET_RESULTS()    ""

#else
#define PF_INIT_COUNTERS()  wci_pfinit_counters()
#define PF_BEGIN_COUNT(x)   wci_pfbegin_count(x)
#define PF_STOP_COUNT(x)    wci_pfstop_count(x)
#define PF_GET_RESULTS()    wci_pfget_results()

typedef struct  tagPF_TASK_NAME
{
  PF_TASK_ID  id;
  int8_t*     name;
} PF_TASK_NAME;

typedef struct  tagPF_COUNTER
{
  count_t tStartCycle;
  count_t tTotalCycles;
  int32_t tNumberOFEntrances;
} PF_COUNTER;

extern const int8_t *const  g_profile_result;
extern PF_COUNTER *const    g_pfPtrCountSet;

//------------------------------------------------------------------------------
const int8_t*               wci_pfget_results(void);
void                        wci_pfinit_counters(void);

//------------------------------------------------------------------------------
static __inline void wci_pfbegin_count(int32_t n)
{
  (g_pfPtrCountSet + n)->tStartCycle = read_counter();
  (g_pfPtrCountSet + n)->tNumberOFEntrances++;
}

//------------------------------------------------------------------------------
static __inline void wci_pfstop_count(int32_t n)
{
  count_t lTime;
  lTime = read_counter();
    (
      g_pfPtrCountSet +
      n
    )->tTotalCycles = (g_pfPtrCountSet + n)->tTotalCycles +
    lTime -
    (g_pfPtrCountSet + n)->tStartCycle;
}

//------------------------------------------------------------------------------
count_t read_counter(void);
count_t get_CPUSpeed(void);

#endif //_PROFILING_

#ifdef __cplusplus
}

#endif
#endif // #ifndef _PROFILE_H_
