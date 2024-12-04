/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */


#include "master_include.h"

#define SET_RTCC 1
int main()
{
    init_platform();

    NavDemo_Initialize();
    TCInitialize();
    RTCCInitialize(SET_RTCC);
    HygroInitialize();
    ALSInitialize();
    xil_printf("Command Parsing Active\n\r");
    
    while(1){
        init_uart0();
        
        cli_parse_command();
    }
    return 0;
}


