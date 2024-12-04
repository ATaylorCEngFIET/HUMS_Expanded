#include "../master_include.h"

PmodALS ALS;


void ALSInitialize() {
   ALS_begin(&ALS, XPAR_PMODALS_0_BASEADDR);
}

