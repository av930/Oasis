/******************************************************************************

PROJECT:     Wavelet Image Compression Algorithm (WICA) Codec
             LGTCM S/W Lab, St. Petersburg, Russia

MODULE NAME: ColorSpaceTable.c

ABSTRACT:    This file contains table for color space transformations

AUTHORS:     Alexandr  Maiboroda              <alm@lgsoftlab.ru>,
             Sergey    Gramnitsky             <sernigra@lgsoftlab.ru>,				
					   Alexander Ivanov								  <alexi@lgsoftlab.ru>

HISTORY:     2004.08.19   v1.00

*******************************************************************************/


#include "wci_color_space_table.h"

//------------------------------------------------------------------------------

#ifdef CSP_CONST_TABLE
 #ifdef CSP_ISBN
  const int RGB_Y_tab[256+128] =
  {
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
     -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,  -152560,
      -152560,  -143025,  -133490, -123955, -114420, -104885,  -95350,  -85815,
      -76280,   -66745,   -57210,  -47675,  -38140,  -28605,  -19070,   -9535,
           0,     9535,    19070,   28605,   38140,   47675,   57210,   66745,
       76280,    85815,    95350,  104885,  114420,  123955,  133490,  143025,
      152560,   162095,   171630,  181165,  190700,  200235,  209770,  219305,
      228840,   238375,   247910,  257445,  266980,  276515,  286050,  295585,
      305120,   314655,   324190,  333725,  343260,  352795,  362330,  371865,
      381400,   390935,   400470,  410005,  419540,  429075,  438610,  448145,
      457680,   467215,   476750,  486285,  495820,  505355,  514890,  524425,
      533960,   543495,   553030,  562565,  572100,  581635,  591170,  600705,
      610240,   619775,   629310,  638845,  648380,  657915,  667450,  676985,
      686520,   696055,   705590,  715125,  724660,  734195,  743730,  753265,
      762800,   772335,   781870,  791405,  800940,  810475,  820010,  829545,
      839080,   848615,   858150,  867685,  877220,  886755,  896290,  905825,
      915360,   924895,   934430,  943965,  953500,  963035,  972570,  982105,
      991640,  1001175,  1010710, 1020245, 1029780, 1039315, 1048850, 1058385,
     1067920,  1077455,  1086990, 1096525, 1106060, 1115595, 1125130, 1134665,
     1144200,  1153735,  1163270, 1172805, 1182340, 1191875, 1201410, 1210945,
     1220480,  1230015,  1239550, 1249085, 1258620, 1268155, 1277690, 1287225,
     1296760,  1306295,  1315830, 1325365, 1334900, 1344435, 1353970, 1363505,
     1373040,  1382575,  1392110, 1401645, 1411180, 1420715, 1430250, 1439785,
     1449320,  1458855,  1468390, 1477925, 1487460, 1496995, 1506530, 1516065,
     1525600,  1535135,  1544670, 1554205, 1563740, 1573275, 1582810, 1592345,
     1601880,  1611415,  1620950, 1630485, 1640020, 1649555, 1659090, 1668625,
     1678160,  1687695,  1697230, 1706765, 1716300, 1725835, 1735370, 1744905,
     1754440,  1763975,  1773510, 1783045, 1792580, 1802115, 1811650, 1821185,
     1830720,  1840255,  1849790, 1859325, 1868860, 1878395, 1887930, 1897465,
     1907000,  1916535,  1926070, 1935605, 1945140, 1954675, 1964210, 1973745,
     1983280,  1992815,  2002350, 2011885, 2021420, 2030955, 2040490, 2050025,
     2059560,  2069095,  2078630, 2088165, 2097700, 2107235, 2116770, 2126305,
     2135840,  2145375,  2154910, 2164445, 2173980, 2183515, 2193050, 2202585,
     2212120,  2221655,  2231190, 2240725, 2250260, 2259795, 2269330, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865,
     2278865,  2278865,  2278865, 2278865, 2278865, 2278865, 2278865, 2278865

  };

  const int B_U_tab[256+128] =
  {
  
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
   -2115968,  -2115968,  -2115968, -2115968, -2115968, -2115968, -2115968, -2115968,
      
    -2115968, -2099437, -2082906, -2066375, -2049844, -2033313, -2016782, -2000251,  
    -1983720, -1967189, -1950658, -1934127, -1917596, -1901065, -1884534, -1868003,  
    -1851472, -1834941, -1818410, -1801879, -1785348, -1768817, -1752286, -1735755,  
    -1719224, -1702693, -1686162, -1669631, -1653100, -1636569, -1620038, -1603507,  
    -1586976, -1570445, -1553914, -1537383, -1520852, -1504321, -1487790, -1471259,  
    -1454728, -1438197, -1421666, -1405135, -1388604, -1372073, -1355542, -1339011,  
    -1322480, -1305949, -1289418, -1272887, -1256356, -1239825, -1223294, -1206763,  
    -1190232, -1173701, -1157170, -1140639, -1124108, -1107577, -1091046, -1074515,  
    -1057984, -1041453, -1024922, -1008391,  -991860,  -975329,  -958798,  -942267,  
     -925736,  -909205,  -892674,  -876143,  -859612,  -843081,  -826550,  -810019,  
     -793488,  -776957,  -760426,  -743895,  -727364,  -710833,  -694302,  -677771,  
     -661240,  -644709,  -628178,  -611647,  -595116,  -578585,  -562054,  -545523,  
     -528992,  -512461,  -495930,  -479399,  -462868,  -446337,  -429806,  -413275,  
     -396744,  -380213,  -363682,  -347151,  -330620,  -314089,  -297558,  -281027,  
     -264496,  -247965,  -231434,  -214903,  -198372,  -181841,  -165310,  -148779,  
     -132248,  -115717,   -99186,   -82655,   -66124,   -49593,   -33062,   -16531,  
           0,    16531,    33062,    49593,    66124,    82655,    99186,   115717,  
      132248,   148779,   165310,   181841,   198372,   214903,   231434,   247965,  
      264496,   281027,   297558,   314089,   330620,   347151,   363682,   380213,  
      396744,   413275,   429806,   446337,   462868,   479399,   495930,   512461,  
      528992,   545523,   562054,   578585,   595116,   611647,   628178,   644709,  
      661240,   677771,   694302,   710833,   727364,   743895,   760426,   776957,  
      793488,   810019,   826550,   843081,   859612,   876143,   892674,   909205,  
      925736,   942267,   958798,   975329,   991860,  1008391,  1024922,  1041453,  
     1057984,  1074515,  1091046,  1107577,  1124108,  1140639,  1157170,  1173701,  
     1190232,  1206763,  1223294,  1239825,  1256356,  1272887,  1289418,  1305949,  
     1322480,  1339011,  1355542,  1372073,  1388604,  1405135,  1421666,  1438197,  
     1454728,  1471259,  1487790,  1504321,  1520852,  1537383,  1553914,  1570445,  
     1586976,  1603507,  1620038,  1636569,  1653100,  1669631,  1686162,  1702693,  
     1719224,  1735755,  1752286,  1768817,  1785348,  1801879,  1818410,  1834941,  
     1851472,  1868003,  1884534,  1901065,  1917596,  1934127,  1950658,  1967189,  
     1983720,  2000251,  2016782,  2033313,  2049844,  2066375,  2082906,  2099437,  

   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437,
   2099437,  2099437,  2099437, 2099437,2099437, 2099437,2099437, 2099437
  };

  const int G_U_tab[256+128] =
  {
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
   -409984,  -409984,  -409984, -409984,-409984, -409984,-409984, -409984,
  
    -409984, -406781, -403578, -400375, -397172, -393969, -390766, -387563, 
    -384360, -381157, -377954, -374751, -371548, -368345, -365142, -361939, 
    -358736, -355533, -352330, -349127, -345924, -342721, -339518, -336315, 
    -333112, -329909, -326706, -323503, -320300, -317097, -313894, -310691, 
    -307488, -304285, -301082, -297879, -294676, -291473, -288270, -285067, 
    -281864, -278661, -275458, -272255, -269052, -265849, -262646, -259443, 
    -256240, -253037, -249834, -246631, -243428, -240225, -237022, -233819, 
    -230616, -227413, -224210, -221007, -217804, -214601, -211398, -208195, 
    -204992, -201789, -198586, -195383, -192180, -188977, -185774, -182571, 
    -179368, -176165, -172962, -169759, -166556, -163353, -160150, -156947, 
    -153744, -150541, -147338, -144135, -140932, -137729, -134526, -131323, 
    -128120, -124917, -121714, -118511, -115308, -112105, -108902, -105699, 
    -102496,  -99293,  -96090,  -92887,  -89684,  -86481,  -83278,  -80075, 
     -76872,  -73669,  -70466,  -67263,  -64060,  -60857,  -57654,  -54451, 
     -51248,  -48045,  -44842,  -41639,  -38436,  -35233,  -32030,  -28827, 
     -25624,  -22421,  -19218,  -16015,  -12812,   -9609,   -6406,   -3203, 
          0,    3203,    6406,    9609,   12812,   16015,   19218,   22421, 
      25624,   28827,   32030,   35233,   38436,   41639,   44842,   48045, 
      51248,   54451,   57654,   60857,   64060,   67263,   70466,   73669, 
      76872,   80075,   83278,   86481,   89684,   92887,   96090,   99293, 
     102496,  105699,  108902,  112105,  115308,  118511,  121714,  124917, 
     128120,  131323,  134526,  137729,  140932,  144135,  147338,  150541, 
     153744,  156947,  160150,  163353,  166556,  169759,  172962,  176165, 
     179368,  182571,  185774,  188977,  192180,  195383,  198586,  201789, 
     204992,  208195,  211398,  214601,  217804,  221007,  224210,  227413, 
     230616,  233819,  237022,  240225,  243428,  246631,  249834,  253037, 
     256240,  259443,  262646,  265849,  269052,  272255,  275458,  278661, 
     281864,  285067,  288270,  291473,  294676,  297879,  301082,  304285, 
     307488,  310691,  313894,  317097,  320300,  323503,  326706,  329909, 
     333112,  336315,  339518,  342721,  345924,  349127,  352330,  355533, 
     358736,  361939,  365142,  368345,  371548,  374751,  377954,  381157, 
     384360,  387563,  390766,  393969,  397172,  400375,  403578,  406781, 
 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781, 
     406781,  406781,  406781,  406781,  406781,  406781,  406781,  406781 
  };

  const int G_V_tab[256+128] =
  {
 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
     -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480,  -852480, 
    
    -852480, -845820, -839160, -832500, -825840, -819180, -812520, -805860,  
    -799200, -792540, -785880, -779220, -772560, -765900, -759240, -752580,  
    -745920, -739260, -732600, -725940, -719280, -712620, -705960, -699300,  
    -692640, -685980, -679320, -672660, -666000, -659340, -652680, -646020,  
    -639360, -632700, -626040, -619380, -612720, -606060, -599400, -592740,  
    -586080, -579420, -572760, -566100, -559440, -552780, -546120, -539460,  
    -532800, -526140, -519480, -512820, -506160, -499500, -492840, -486180,  
    -479520, -472860, -466200, -459540, -452880, -446220, -439560, -432900,  
    -426240, -419580, -412920, -406260, -399600, -392940, -386280, -379620,  
    -372960, -366300, -359640, -352980, -346320, -339660, -333000, -326340,  
    -319680, -313020, -306360, -299700, -293040, -286380, -279720, -273060,  
    -266400, -259740, -253080, -246420, -239760, -233100, -226440, -219780,  
    -213120, -206460, -199800, -193140, -186480, -179820, -173160, -166500,  
    -159840, -153180, -146520, -139860, -133200, -126540, -119880, -113220,  
    -106560,  -99900,  -93240,  -86580,  -79920,  -73260,  -66600,  -59940,  
     -53280,  -46620,  -39960,  -33300,  -26640,  -19980,  -13320,   -6660,  
          0,    6660,   13320,   19980,   26640,   33300,   39960,   46620,  
      53280,   59940,   66600,   73260,   79920,   86580,   93240,   99900,  
     106560,  113220,  119880,  126540,  133200,  139860,  146520,  153180,  
     159840,  166500,  173160,  179820,  186480,  193140,  199800,  206460,  
     213120,  219780,  226440,  233100,  239760,  246420,  253080,  259740,  
     266400,  273060,  279720,  286380,  293040,  299700,  306360,  313020,  
     319680,  326340,  333000,  339660,  346320,  352980,  359640,  366300,  
     372960,  379620,  386280,  392940,  399600,  406260,  412920,  419580,  
     426240,  432900,  439560,  446220,  452880,  459540,  466200,  472860,  
     479520,  486180,  492840,  499500,  506160,  512820,  519480,  526140,  
     532800,  539460,  546120,  552780,  559440,  566100,  572760,  579420,  
     586080,  592740,  599400,  606060,  612720,  619380,  626040,  632700,  
     639360,  646020,  652680,  659340,  666000,  672660,  679320,  685980,  
     692640,  699300,  705960,  712620,  719280,  725940,  732600,  739260,  
     745920,  752580,  759240,  765900,  772560,  779220,  785880,  792540,  
     799200,  805860,  812520,  819180,  825840,  832500,  839160,  845820,  

     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820,  
     845820,  845820,  845820,  845820,  845820,  845820,  845820,  845820  

  };

  const int R_V_tab[256+128] =
  {
 
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    -1673472, -1673472,-1673472,-1673472, -1673472, -1673472, -1673472, -1673472,  
    
    
    -1673472, -1660398, -1647324, -1634250, -1621176, -1608102, -1595028, -1581954,  
    -1568880, -1555806, -1542732, -1529658, -1516584, -1503510, -1490436, -1477362,  
    -1464288, -1451214, -1438140, -1425066, -1411992, -1398918, -1385844, -1372770,  
    -1359696, -1346622, -1333548, -1320474, -1307400, -1294326, -1281252, -1268178,  
    -1255104, -1242030, -1228956, -1215882, -1202808, -1189734, -1176660, -1163586,  
    -1150512, -1137438, -1124364, -1111290, -1098216, -1085142, -1072068, -1058994,  
    -1045920, -1032846, -1019772, -1006698,  -993624,  -980550,  -967476,  -954402,  
     -941328,  -928254,  -915180,  -902106,  -889032,  -875958,  -862884,  -849810,  
     -836736,  -823662,  -810588,  -797514,  -784440,  -771366,  -758292,  -745218,  
     -732144,  -719070,  -705996,  -692922,  -679848,  -666774,  -653700,  -640626,  
     -627552,  -614478,  -601404,  -588330,  -575256,  -562182,  -549108,  -536034,  
     -522960,  -509886,  -496812,  -483738,  -470664,  -457590,  -444516,  -431442,  
     -418368,  -405294,  -392220,  -379146,  -366072,  -352998,  -339924,  -326850,  
     -313776,  -300702,  -287628,  -274554,  -261480,  -248406,  -235332,  -222258,  
     -209184,  -196110,  -183036,  -169962,  -156888,  -143814,  -130740,  -117666,  
     -104592,   -91518,   -78444,   -65370,   -52296,   -39222,   -26148,   -13074,  
           0,    13074,    26148,    39222,    52296,    65370,    78444,    91518,  
      104592,   117666,   130740,   143814,   156888,   169962,   183036,   196110,  
      209184,   222258,   235332,   248406,   261480,   274554,   287628,   300702,  
      313776,   326850,   339924,   352998,   366072,   379146,   392220,   405294,  
      418368,   431442,   444516,   457590,   470664,   483738,   496812,   509886,  
      522960,   536034,   549108,   562182,   575256,   588330,   601404,   614478,  
      627552,   640626,   653700,   666774,   679848,   692922,   705996,   719070,  
      732144,   745218,   758292,   771366,   784440,   797514,   810588,   823662,  
      836736,   849810,   862884,   875958,   889032,   902106,   915180,   928254,  
      941328,   954402,   967476,   980550,   993624,  1006698,  1019772,  1032846,  
     1045920,  1058994,  1072068,  1085142,  1098216,  1111290,  1124364,  1137438,  
     1150512,  1163586,  1176660,  1189734,  1202808,  1215882,  1228956,  1242030,  
     1255104,  1268178,  1281252,  1294326,  1307400,  1320474,  1333548,  1346622,  
     1359696,  1372770,  1385844,  1398918,  1411992,  1425066,  1438140,  1451214,  
     1464288,  1477362,  1490436,  1503510,  1516584,  1529658,  1542732,  1555806,  
     1568880,  1581954,  1595028,  1608102,  1621176,  1634250,  1647324,  1660398,  

     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  
     1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398,  1660398  
  };
 #else
   #error "Const table is not defined"
 #endif // #ifdef CSP_ISBN

#else

  int RGB_Y_tab[256];
  int B_U_tab[256];
  int G_U_tab[256];
  int G_V_tab[256];
  int R_V_tab[256];

#endif // #ifdef CSP_CONST_TABLE

//------------------------------------------------------------------------------

#ifndef CSP_CONST_TABLE

void wci_color_space_init_table(void)
{
// Initialize RGB lookup tables

	int i;

	for (i = 0; i < 256; i++)
  {
		RGB_Y_tab[i] = FIX_OUT(RGB_Y_OUT) * (i - Y_ADD_OUT);
		B_U_tab[i] = FIX_OUT(B_U_OUT) * (i - U_ADD_OUT);
		G_U_tab[i] = FIX_OUT(G_U_OUT) * (i - U_ADD_OUT);
		G_V_tab[i] = FIX_OUT(G_V_OUT) * (i - V_ADD_OUT);
		R_V_tab[i] = FIX_OUT(R_V_OUT) * (i - V_ADD_OUT);
	}
}

#endif
