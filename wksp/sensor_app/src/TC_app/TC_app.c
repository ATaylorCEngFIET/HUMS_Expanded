#include "../master_include.h"

PmodTC1 TCDevice;

void TCInitialize() {
   TC1_begin(&TCDevice, XPAR_PMODTC1_0_BASEADDR);
}

void TCGetTemp(){
    double celsius;

    celsius = TC1_getTemp(&TCDevice);
    xil_printf_float(celsius);
}
