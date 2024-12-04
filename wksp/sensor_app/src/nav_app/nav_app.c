#include "../master_include.h"




float NavDemo_ComputePref(float hPa, float altitudeMeters);
float NavDemo_ConvPresToAltF(float Pref, float hPa);
float NavDemo_ConvPresToAltM(float Pref, float hPa);

float NavDemo_ConvTempCToTempF(float tempC);
float NavDemo_ConvFeetToMeters(float feet);
float NavDemo_ConvMetersToFeet(float meters);

float NavDemo_AngleInXY(NAV_RectCoord r);
float NavDemo_DegreesFromVertical(NAV_RectCoord r);
float NavDemo_ScalarProjection(NAV_RectCoord orient, NAV_RectCoord r);

void NavDemo_EnableCaches(void);
void NavDemo_DisableCaches(void);


 PmodNAV nav;

void NavDemo_Initialize(void) {
    //NavDemo_EnableCaches();
    xil_printf("Pmod Nav Demo Initializing...\n\r");
    NAV_begin ( // intialize the PmodNAV driver device
        &nav,
        0x44a20000,
        0x44a30000
    );
    xil_printf("Pmod Nav Demo Initialized\n\r");
    NAV_Init(&nav); // initialize the connection with each spi slave
}

float NavDemo_ConvFeetToMeters(float feet) {
   return feet * 0.3048;
}

float NavDemo_ConvMetersToFeet(float meters) {
   return meters * 3.28084;
}

float NavDemo_ConvTempCToTempF(float tempC) {
   return 32 + (tempC * 1.8);
}

float NavDemo_ConvPresToAltF(float Pref, float hPa) {
   return ((1 - pow(hPa / Pref, 0.190284)) * 145366.45);
}

float NavDemo_ComputePref(float hPa, float altitudeMeters) {
   float altitudeFeet = NavDemo_ConvMetersToFeet(altitudeMeters);
   float temp = 1 - (altitudeFeet / 145366.45);
   temp = hPa / (powf(temp, 1.0 / 0.190284));
   return temp;
}

float NavDemo_DegreesFromVertical(NAV_RectCoord r) {
   // Determine the magnitude of the vector r.
   float rM = sqrtf(powf(r.X, 2) + powf(r.Y, 2) + powf(r.Z, 2));
   if (rM == 0)
      return 0.0;
   return acosf(r.Z / rM) * (180.0 / M_PI);
}

float NavDemo_ScalarProjection(NAV_RectCoord orient, NAV_RectCoord r) {
   float oM = sqrtf(powf(orient.X, 2) + powf(orient.Y, 2) + powf(orient.Z, 2));
   return (r.X * orient.X + r.Y * orient.Y + r.Z * orient.Z) / oM;
}

float NavDemo_AngleInXY(NAV_RectCoord r) {
   float d;
   if (r.X == 0)
      d = (r.Y < 0) ? 90 : 0;
   else
      d = atan2f(r.Y, r.X) * 180 / M_PI;
   if (d > 360)
      d -= 360;
   else if (d < 0)
      d += 360;
   return d;
}



void NavDemo_Run(void) {
   //float Pref,
         //tempF,
         //dps,
         //magXYd,
    //     alt;
    //char *compass[8] = {"North", "North-East", "East", "South-East", "South",
    //      "South-West", "West", "North-West"};
    //char *str;
    // Set the known altitude as reference for future measurements of altitude
    // Uncomment the below line to improve accuracy of the altimeter device by
    // specifying the known altitude for your current location

   xil_printf("Pmod Nav Demo Started\n\r");
   NAV_ReadPressurehPa(&nav);
   //alt = NavDemo_ConvFeetToMeters(175.0); // altitude for Seattle, WA in meters
   //Pref = NavDemo_ComputePref(nav.hPa, alt);
   usleep(100000);
   //while(1){
      NAV_GetDeviceID(&nav);

      xil_printf("Device IDs\n\r");
      xil_printf("A/G ID:%d  MAG ID:%d  ALT ID:%d\n\r\n\r", nav.idData.ag,
                 nav.idData.mag, nav.idData.alt);

      NAV_GetData(&nav);

      
      xil_printf_float(nav.acclData.Z);
      xil_printf_float(nav.acclData.Y);
      xil_printf_float(nav.acclData.X);


      
    /*
      printf("    %.2f degrees from vertical\n\r",
             NavDemo_DegreesFromVertical(nav.acclData));
      // Format and serial print the data read from accelerometer instrument,
      // stored in acclData variables and expressed in G

      printf("Gyro dps values   X:%.2f dps, Y:%.2f dps, Z:%.2f dps\n\r",
             nav.gyroData.X, nav.gyroData.Y, nav.acclData.Z);
      dps = NavDemo_ScalarProjection(nav.acclData, nav.gyroData);
      if (dps < -4) // Remove some noise around 0
        str = "Clockwise";
      else if (dps > 4)
        str = "Counter-Clockwise";
      else
        str = "Marginal";
      printf("    rotating about vertical at %6.2f dps (%s)\n\r", dps, str);
      // Format and serial print the data read from Gyro instrument, stored in
      // gyroData variables and expressed in degrees per second

      printf(
          "Magnetometer Gauss values   X:%.2f Gauss, Y:%.2f Gauss, Z:%.2f Gauss \
            \n\r",
          nav.magData.X, nav.magData.Y, nav.magData.Z);
      // Format and serial print the data read from magnetometer instrument,
      // stored in magData variables and expressed in Gauss

      magXYd = NavDemo_AngleInXY(nav.magData);
      str = compass[(int)((magXYd + 22.5) / 45.0) % 8];
      // Get appropriate compass string - North is centered on 0 degrees
      printf("    Heading in Degrees: %.2f   Y Direction: %s\n\r", magXYd, str);

      printf("Pressure: %.1f hPa\n\r", nav.hPa);
      alt = NavDemo_ConvPresToAltF(Pref, nav.hPa);
      printf("Altitude: %.1f feet\n\r", alt);

      tempF = NavDemo_ConvTempCToTempF(nav.tempC);
      printf("Temperature: %.2f degrees Celsius\n\r", nav.tempC);
      printf("             %.2f degrees Fahrenheit\n\r", tempF);
      xil_printf("\n\r\n\r");
    
      usleep(500000);
   }*/
}


