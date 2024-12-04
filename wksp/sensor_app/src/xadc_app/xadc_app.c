#include "../master_include.h"

void get_xadc(){
    int temp = Xil_In32(XPAR_STEIN_HART_IP_0_BASEADDR+0x100);
    temp = temp/ pow(2,10);
    xil_printf_float((float)temp);
}