#define MAX_COUNTERS_NUMBER 4096
#define MAX_COUNTER_LEN 128

char
    tmpCountersArray[MAX_COUNTERS_NUMBER][MAX_COUNTER_LEN] = {0};

char
    localDn[64]     =   {0},
    elementType[32] =   {0},
    elementName[32] =   {0},
    mcc[4]          =   {0},
    mnc[4]          =   {0},
    swVersion[32]   =   {0},
    measInfoId[64]  =   {0},
    endTime[32]     =   {0},
    duration[7]     =   {0},
    measObjLdn[254] =   {0},
    suspect[5]      =   {0};

struct structLocalDn {
    char 
        elementName[32],
        mcc[8],
        mnc[8];
} sLocalDn;

char buf[100]       =   {0};

struct tm 
    *tmPtr,
    tmIsoTime;

time_t gmt_ts;
