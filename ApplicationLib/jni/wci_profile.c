#include "wci_profile.h"
#ifdef _PROFILING_
#ifndef PFERROR
#define PFERROR 0
#endif

//------------------------------------------------------------------------------
#define PROFILE_ROW_SIZE  (15 + 10 + 15 + 7 + 32)

static const int8_t         mstr1[] = {"There are results of profiling:\n\n"};
static count_t              pfCPUSpeed;
static int8_t               s_profile_result[(PROFILE_ROW_SIZE + 1) * (1 + PF_ID_LAST)];
const int8_t* const         g_profile_result = &s_profile_result[0];
static PF_COUNTER           g_pfCountSet[PF_ID_LAST];
PF_COUNTER* const           g_pfPtrCountSet = &g_pfCountSet[0];


static PF_TASK_NAME         pfProcBuf[] =
{
  { PF_MAIN                 , "PF_MAIN"                 },
  { PF_UNPACK_HEADER        , "PF_UNPACK_HEADER"        },
  { PF_INITIALIZATION       , "PF_INITIALIZATION"       },
  { PF_PREPARE_DECODER      , "PF_PREPARE_DECODER"      },
  { PF_DECODE_STRIPE        , "PF_DECODE_STRIPE"        },
  { PF_OUT_STRIPE           , "PF_OUT_STRIPE"           },
  { PF_DONE                 , "PF_DONE"                 },
  { PF_STRIPE_SET_COUNTERS  , "PF_STRIPE_SET_COUNTERS"  },
  { PF_STRIPE_INSERT_LL     , "PF_STRIPE_INSERT_LL"     },
  { PF_DECOMPRESSLL_STRIPE  , "PF_DECOMPRESSLL_STRIPE"  },
  { PF_ADPCM_DECODE_STRIPE  , "PF_ADPCM_DECODE_STRIPE"  },
  { PF_QSP_DECOMPRESS_STRIPE, "PF_QSP_DECOMPRESS_STRIPE"},
  { PF_DEQUANTSTEPSTRIPE    , "PF_DEQUANTSTEPSTRIPE"    },
  { PF_STRIPE_INVWAVTRANS   , "PF_STRIPE_INVWAVTRANS"   },
  { PF_STRIPE_GETCOMPONENT  , "PF_STRIPE_GETCOMPONENT"  },
  { PF_VER_TRANSFORMATION   , "PF_VER_TRANSFORMATION"   },
  { PF_HOR_STEP_STRIPE_INV  , "PF_HOR_STEP_STRIPE_INV"  }
};

//------------------------------------------------------------------------------
const int8_t* wci_pfget_results(void)
{
  int32_t i;

  int32_t j;
  int64_t aidv = 0;
  int8_t  buf[PROFILE_ROW_SIZE];
  aidv = sizeof(pfProcBuf) / sizeof(PF_TASK_NAME);
  sprintf(s_profile_result, "%s", mstr1);

#ifdef PRF_LEGEND
  sprintf(s_profile_result, PRF_LEGEND);
#endif
  for (i = 0; i < PF_ID_LAST; i++)
  {
    (g_pfPtrCountSet + i)->tTotalCycles -= (count_t) ((g_pfPtrCountSet + i)->tNumberOFEntrances * PFERROR);
  }

  for (i = 0; i < PF_ID_LAST; i++)
  {
    sprintf(buf, " %15llu ", (uint64_t) ((g_pfPtrCountSet + i)->tTotalCycles / pfCPUSpeed));
    strcat(s_profile_result, buf);
    sprintf(buf, "%10d     %15llu  ", (g_pfPtrCountSet + i)->tNumberOFEntrances,
            (uint64_t) (g_pfPtrCountSet + i)->tTotalCycles);
    strcat(s_profile_result, buf);
    sprintf(buf, "  unknown proc");

    for (j = 0; j < aidv; j++)
    {
      if (i == pfProcBuf[j].id)
      {
        sprintf(buf, "  %s ", pfProcBuf[j].name);
        break;
      }
    }

    strcat(s_profile_result, buf);
    strcat(s_profile_result, "\n");
  }

  return g_profile_result;
}

//------------------------------------------------------------------------------
void wci_pfinit_counters(void)
{
  int32_t i;
  for (i = 0; i < PF_ID_LAST; i++)
  {
    (g_pfPtrCountSet + i)->tStartCycle = (count_t) 0;
    (g_pfPtrCountSet + i)->tTotalCycles = (count_t) 0;
    (g_pfPtrCountSet + i)->tNumberOFEntrances = (int32_t) 0;
  }

  pfCPUSpeed = get_CPUSpeed();
}

//------------------------------------------------------------------------------
#endif //_PROFILING_
