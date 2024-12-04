#include "../master_include.h"

PmodHYGRO HygroDevice;


void HygroInitialize() {
  
   xil_printf("Init Started\n\r");
   HYGRO_begin(
      &HygroDevice,
      XPAR_PMODHYGRO_0_BASEADDR,
      0x40, // Chip address of PmodHYGRO IIC
      0x44A10000,
      0,
      XPAR_CPU_CORE_CLOCK_FREQ_HZ // Clock frequency of AXI bus, used to convert timer data
   );
   xil_printf("Init Done\n\r");
}