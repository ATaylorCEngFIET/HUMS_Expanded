#include "../master_include.h"
#include <stdlib.h>

XUartLite Uart;

char receive_buffer[CLI_BUFFER_SIZE];    /* Buffer for Receiving Data */
char receive_buffer_lst[CLI_BUFFER_SIZE];    /* Buffer for Receiving Data */
int mesg_ready;

int test_id = 0;
int test_stop = 0;
int test_oneshot = 0;
char command_delim[] = " ";

extern PmodNAV nav;
extern PmodRTCC rtcc;
extern PmodHYGRO HygroDevice;
extern PmodALS ALS;

int init_uart0()
{
    int Status;

    Status = XUartLite_Initialize(&Uart, XPAR_XUARTLITE_0_BASEADDR);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;

}

void xil_printf_float(float x){
    int integer, fraction, abs_frac;
    integer = x;
    fraction = (x - integer) * 100;
    abs_frac = abs(fraction);
    xil_printf("%d.%3d\n\r", integer, abs_frac);

}

int read_serial(char *buffer)
{
	static u8 read_val[3] = {0,0,0};
    unsigned int received_count = 0;
    static int fisrt = 1;
    static int index = 0;
    static int index_lst = 0;


    received_count =    XUartLite_Recv(&Uart, &read_val[received_count], 1);
    if (received_count > 0)
    {

        if (read_val[0] == '\r'){
            buffer[index] = '\0';

            // Save the entered command for when we use Arrow up
            index_lst = index;
            strcpy(receive_buffer_lst,receive_buffer);

            index = 0;
            fisrt = 0;
            //xil_printf("\n\r");

            return 1;
        }

        // Handles "arrow up" key press
        if (read_val[2] == 27 && read_val[1] == 91 && read_val[0] == 65 && fisrt == 0){
            // restore last entered command
        	strcpy(receive_buffer,receive_buffer_lst);
            index = index_lst;

            xil_printf("\b%s",receive_buffer);

            return 0;
        }

        // Handles backspace
        if (read_val[0] == 8)
        {
        	xil_printf("\b \b");
        	if (index > 0)
        	{
        		index--;
        	}

        }

        // Prevents buffer overflow
        if (index >= CLI_BUFFER_SIZE-1)
        {
            buffer[index] = '\0';
            index = 0;
            xil_printf("\n\r");
            return 1;
        }

        // Prevents not printable characters from being added to the buffer
        if (read_val[0] >= 32 && read_val[0] <= 126)
        {
            buffer[index++] = read_val[0];
            //xil_printf("%c", read_val[0]);
        }

        read_val[2] = read_val[1];
        read_val[1] = read_val[0];
    }
    return 0;
}

void cli_parse_command()
{
    // /static u32 block = 0;
    static u32 val = 0;
    u8 light = 0;
    RTCC_Time time;
    float temp_degc, hum_perrh, temp_degf;

    mesg_ready = read_serial(receive_buffer);
    if (mesg_ready == 1)
    {
        //xil_printf("cmd> %s\n\r", receive_buffer);

        if (strcmp(receive_buffer, "") == 0)
		{
            val = 0; // NOP
		}
        else if (strcmp(receive_buffer, "read_accel_x") == 0)
        {
            //xil_printf("read_accel_x\n\r");
            NAV_GetData(&nav);
            xil_printf_float(nav.acclData.X);
      
      
        }
        else if (strcmp(receive_buffer, "read_accel_y") == 0)
        {
                //xil_printf("read_accel_y\n\r");
                NAV_GetData(&nav);
                xil_printf_float(nav.acclData.Y);

        }
        else if (strcmp(receive_buffer, "read_accel_z") == 0)
        {
            //xil_printf("read_accel_z\n\r");
            NAV_GetData(&nav);
            xil_printf_float(nav.acclData.Z);
        }
        else if (strcmp(receive_buffer, "get_humidity") == 0)
        {
            hum_perrh = HYGRO_getHumidity(&HygroDevice);
            xil_printf_float(hum_perrh);

        }
        else if (strcmp(receive_buffer, "get_temp") == 0)
        {
            TCGetTemp();

        }
        else if (strcmp(receive_buffer, "get_als") == 0)
        {
            light = ALS_read(&ALS);
            xil_printf("%d\n\r", light);

        }
        else if (strcmp(receive_buffer, "get_xadc") == 0)
        {
            get_xadc();

        }
        else if (strcmp(receive_buffer, "get_time") == 0)
        {
            GetTime( RTCC_TARGET_RTCC);

        }
        else 
        {
            char *ptr = strtok(receive_buffer, command_delim);
            if (strcmp(ptr, "set_time") == 0)
            {
                ptr = strtok(NULL, command_delim);
                time.day    = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.date   = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.month  = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.year   = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.hour   = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.minute = char_to_int(strlen(ptr), ptr);
                ptr = strtok(NULL, command_delim);
                time.second  = char_to_int(strlen(ptr), ptr);

                SetTime(RTCC_TARGET_RTCC,  time);
            }
            
            else if (strcmp(ptr, "r") == 0)
            {
                ptr = strtok(NULL, command_delim);
                u32 addr = char_to_int(strlen(ptr), ptr);
                val = Xil_In32( XPAR_XGPIO_0_BASEADDR + addr);
                xil_printf("Addr: 0x%x (%d)  Val: 0x%x (%d)\r\n",addr,addr, val, val);

            }
            else if (strcmp(ptr, "w") == 0)
            {
                ptr = strtok(NULL, command_delim);
                u32 addr = char_to_int(strlen(ptr), ptr);

                ptr = strtok(NULL, command_delim);
                val = char_to_int(strlen(ptr), ptr);

                Xil_Out32(XPAR_XGPIO_0_BASEADDR + addr,val);

                val = Xil_In32( XPAR_XGPIO_0_BASEADDR + addr);
                xil_printf("Addr: 0x%x (%d)  Val: 0x%x (%d)\r\n",addr,addr, val, val);

            }
            else
            {
                xil_printf("Command not found.\n\r");
            }

        }
    }

}

int char_to_int(int len, char* buffer)
{

    int value = 0, char_val, scalar = 1;

    for (int i = len - 1; i >= 0; i--) {
        char_val = (int)(buffer[i] - '0');
        value = value + (char_val * scalar);
        scalar = scalar * 10;
    }

    return value;
}