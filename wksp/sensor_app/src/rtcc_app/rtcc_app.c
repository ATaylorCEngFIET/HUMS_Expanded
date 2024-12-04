#include "../master_include.h"

PmodRTCC rtcc;




const char *weekdays[7] = {
   "Monday",
   "Tuesday",
   "Wednesday",
   "Thursday",
   "Friday",
   "Saturday",
   "Sunday"
};


// Additional demo functions to manage the driver
//RTCC_Time GetTime( RTCC_Target src);
RTCC_Time IncrementTime(RTCC_Time time, int delta_seconds);

void PrintTime(RTCC_Target src);
u8 bcd2int(u8 data);
u8 int2bcd(u8 data);


void RTCCInitialize(u8 mode) {
   RTCC_Time time;


   RTCC_begin(&rtcc, XPAR_PMODRTCC_0_BASEADDR, 0x6F);

   // Print the power-fail time-stamp
   xil_printf("Lost Power at: ");
   PrintTime(RTCC_TARGET_PWRD);
   xil_printf("\r\n");

   xil_printf("Power was back at: ");
   PrintTime(RTCC_TARGET_PWRU);
   xil_printf("\r\n");

   if (!RTCC_checkVbat(&rtcc) || mode) {
      // Set the real time clock to Tuesday 2/6/18 12:24:36 PM
      RTCC_stopClock(&rtcc);

      time.second = 0x36;
      time.minute = 0x24;
      time.hour   = 0x12;
      time.ampm   = RTCC_PM;
      time.day    = 0x01;
      time.date   = 0x06;
      time.month  = 0x02;
      time.year   = 0x18;

      time = IncrementTime(time, 0); // TEST
      SetTime( RTCC_TARGET_RTCC, time);

      RTCC_startClock(&rtcc);
      xil_printf("The time has been set \r\n");
      // Set vbat high
      RTCC_enableVbat(&rtcc);
   } else {
      time = GetTime( RTCC_TARGET_RTCC);
   }

   // Sset alarm 0 for 30 seconds from now
   time = IncrementTime(time, 30);
   SetTime( RTCC_TARGET_ALM0, time);

   // Sset alarm 1 for 1 minute from now
   time = IncrementTime(time, 30);
   SetTime( RTCC_TARGET_ALM1, time);

   // Pprint current time
   xil_printf("Current time is: ");
   PrintTime(RTCC_TARGET_RTCC);
   xil_printf("\r\n");

   // Print alarm 0
   xil_printf("Alarm 0 is set to : ");
   PrintTime(RTCC_TARGET_ALM0);
   xil_printf("\r\n");

   // Print alarm 1
   xil_printf("Alarm 1 is set to : ");
   PrintTime(RTCC_TARGET_ALM1);
   xil_printf("\r\n");

   // Enables alarm 0
   // Set configuration bits to:
   //    RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0
   // This will drive the MPF pin high when the alarm triggered
   // It also sets the alarm to be triggered when the alarm matches
   // Seconds, Minutes, Hour, Day, Date, Month of the RTCC
   RTCC_enableAlarm(&rtcc, RTCC_TARGET_ALM0,
         RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0);

   // Enable alarm 1
   // Set configuration bits to RTCC_ALM_POL
   // This will drive the MPF pin high when the alarm triggered
   // It also sets the alarm to be triggered when the alarm matches
   // Seconds of the RTCC
   RTCC_enableAlarm(&rtcc, RTCC_TARGET_ALM1,
         RTCC_ALM_POL | RTCC_ALMC2 | RTCC_ALMC1 | RTCC_ALMC0);

   // Enable back up battery
   RTCC_enableVbat(&rtcc);

   RTCC_clearPWRFAIL(&rtcc);
}


/** RTCC_Time GetTime(PmodRTCC *InstancePtr, RTCC_Target src)
**
**  Parameters:
**     InstancePtr - the target device to retrieve data from
**     src         - RTCC_TARGET_RTCC - real-time clock
**                   RTCC_TARGET_ALM0 - Alarm 0
**                   RTCC_TARGET_ALM1 - Alarm 1
**                   RTCC_TARGET_PWRD - power-down time-stamp
**                   RTCC_TARGET_PWRU - power-up time-stamp
**
**  Return Value:
**     val - the contents of all time registers in the target area
**
**  Description:
**     This function retrieves the contents of one of the Pmod RTCC's time areas
*/
RTCC_Time GetTime( RTCC_Target src) {
   RTCC_Time val;

   if (src != RTCC_TARGET_PWRD && src != RTCC_TARGET_PWRU) {
      val.second = RTCC_getSec(&rtcc, src);
   }

   val.minute = RTCC_getMin(&rtcc, src);
   val.hour   = RTCC_getHour(&rtcc, src);
   val.ampm   = RTCC_getAmPm(&rtcc, src);
   val.day    = RTCC_getDay(&rtcc, src);
   val.date   = RTCC_getDate(&rtcc, src);
   val.month  = RTCC_getMonth(&rtcc, src);

   if (src == RTCC_TARGET_RTCC) {
      val.year = RTCC_getYear(&rtcc);
   } else {
      val.year = 0;
   }

    xil_printf("%s %x/%x", weekdays[val.day], val.month, val.date);
    xil_printf("/%02x", val.year);
    xil_printf(" %x:%02x", val.hour, val.minute);
    xil_printf(":%02x", val.second);
    if (val.ampm) {
        xil_printf(" PM\r\n");
    } else {
        xil_printf(" AM\r\n");
    }

   return val;
}

/** void SetTime(PmodRTCC *InstancePtr, RTCC_Target src, RTCC_Time val)
**
**  Parameters:
**     InstancePtr - the target device to retrieve data from
**     src         - RTCC_TARGET_RTCC - real-time clock
**                   RTCC_TARGET_ALM0 - Alarm 0
**                   RTCC_TARGET_ALM1 - Alarm 1
**                   RTCC_TARGET_PWRD - power-down time-stamp
**                   RTCC_TARGET_PWRU - power-up time-stamp
**     val         - container for the time data to be written into the target
**                   area's registers
**
**  Return Value:
**     None
**
**  Description:
**     This function writes data into each of the registers of one of the
**     PmodRTCC's time fields
*/
void SetTime( RTCC_Target dest, RTCC_Time val) {
   if (dest != RTCC_TARGET_PWRD && dest != RTCC_TARGET_PWRU) {
      RTCC_setSec(&rtcc, dest, val.second);
   }

   RTCC_setMin(&rtcc, dest, val.minute);
   RTCC_setHour12(&rtcc, dest, val.hour, val.ampm);
   RTCC_setDay(&rtcc, dest, val.day);
   RTCC_setDate(&rtcc, dest, val.date);
   RTCC_setMonth(&rtcc, dest, val.month);

   if (dest == RTCC_TARGET_RTCC) {
      RTCC_setYear(&rtcc, val.year);
   }
   GetTime( RTCC_TARGET_RTCC);
}

/** void PrintTime(RTCC_Target src)
**
**  Description:
**     This function prints the current time over UART, formatted to the
**     equivalent of "Monday 1/1/00 01:00:00 AM"
*/
void PrintTime(RTCC_Target src) {
   RTCC_Time time;

   // Fetch the time from the device
   time = GetTime( src);

   xil_printf("%s %x/%x", weekdays[time.day], time.month, time.date);

   // Year is only available for the RTCC
   if (src == RTCC_TARGET_RTCC) {
      xil_printf("/%02x", time.year);
   }

   xil_printf(" %x:%02x", time.hour, time.minute);

   // Second is not supported by the power fail registers
   if (src != RTCC_TARGET_PWRD && src != RTCC_TARGET_PWRU) {
      xil_printf(":%02x", time.second);
   }

   if (time.ampm) {
      xil_printf(" PM");
   } else {
      xil_printf(" AM");
   }
}

/** RTCC_Time IncrementTime(RTCC_Time time, int delta_seconds)
**
**  Parameters:
**     time          - container for time data
**     delta_seconds - how many seconds to increment time forward by
**
**  Return:
**     result - time data incremented forward
**
**  Description:
**     This function steps the time parameter forward by delta_seconds,
**     returning the result after ensuring that all modified fields are in the
**     proper range.
**
**  Errors:
**     This function will return the time parameter if requested to cross the
**     midnight/noon boundary.
*/
RTCC_Time IncrementTime(RTCC_Time time, int delta_seconds) {
   RTCC_Time result;
   int temp;
   result = time;
   temp = bcd2int(result.second) + delta_seconds;
   result.second = int2bcd(temp % 60);          // Convert seconds
   temp = bcd2int(result.minute) + temp / 60;   // Carry seconds -> minutes
   result.minute = int2bcd(temp % 60);          // Convert minutes
   temp = bcd2int(result.hour) + temp / 60 - 1; // Carry minutes -> hours
   result.hour = int2bcd((temp % 12) + 1);      // Convert hours
   return result;
}

/** u8 bcd2int(u8 data)
**
**  Description:
**     This function converts 8 bit binary coded decimal numbers to 8 bit
**     unsigned integers.
*/
u8 bcd2int(u8 data) {
   return ((data >> 4) * 10) + (data & 0xF);
}

/** u8 bcd2int(u8 data)
**
**  Description:
**     This function converts 8 bit unsigned integers to 8 bit binary coded
**     decimal numbers.
**
**  Notes:
**     This function will lose data if requested to convert numbers larger than
**     99. However, numbers in this range are not needed for operating the RTCC.
*/
u8 int2bcd(u8 data) {
   return (((data / 10) & 0xF) << 4) + ((data % 10) & 0xF);
}
