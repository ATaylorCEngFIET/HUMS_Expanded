#ifndef _MI_H_
#define _MI_H_

#include "xparameters.h"
#include "cli/cli.h"
#include "xstatus.h"
#include "xuartlite.h"
#include <string.h>
#include "xil_printf.h"
#include "xil_io.h"
#include "math.h"
#include "pmod_nav/PmodNAV.h"
#include "nav_app/nav_app.h"
#include "sleep.h"
#include <stdio.h>
#include "platform.h"

#include <xil_printf.h>

#include "pmod_rtcc/PmodRTCC.h"
#include "rtcc_app/rtcc_app.h"
#include "sleep.h"
#include "xil_cache.h"

#include "pmod_TC/PmodTC1.h"
#include "pmod_als/PmodALS.h"
#include "pmod_hygro/PmodHYGRO.h" 

typedef uint8_t u8;

typedef struct RTCC_Time {
   u8 second;
   u8 minute;
   u8 hour;
   u8 ampm;
   u8 day;
   u8 date;
   u8 month;
   u8 year;
} RTCC_Time;

RTCC_Time GetTime( RTCC_Target src);
void SetTime( RTCC_Target dest, RTCC_Time val);
void RTCCInitialize(u8 mode);
void TCGetTemp();
void TCInitialize();
void HygroInitialize();
void ALSInitialize();
void xil_printf_float(float x);
void get_xadc();
#endif