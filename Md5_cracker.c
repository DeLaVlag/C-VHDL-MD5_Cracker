/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * Md5_cracker.c: simple test application
 *
 * This application finds a pre-image hash collision for MD5
 * When a collision is found the corresponding message is transfered over UART baud rate 115200
 * Stack and heap size are adjusted. Default size not sufficient.
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "xil_printf.h"
#include "xmd5_hasher.h" 	// Device driver for HLS HW block
#include "xparameters.h"

#include "xscugic.h"		// interrupts
#include "xil_exception.h"	// interrupts
#include "../../Md5_cracker/src/platform.h"

#define XMD5_HASHER_INTERRUPT_INTR XPAR_FABRIC_MD5_HASHER_0_INTERRUPT_INTR

// HLS HW instance
XMd5_hasher HlsMd5_hasher;
XMd5_hasher_Config *Md5_hasherPtr;

char deviceName;

void Md5_hasher_isr(void *InstancePtr){
//    print("\nInterrupt hit\n\r");

    int enabled_list;
    int status_list;
    XMd5_hasher *pEx = (XMd5_hasher *) InstancePtr;

    //Disable Global Interrupt
    XMd5_hasher_InterruptGlobalDisable(pEx);

    //Get list of enabled interrupts. appdone is 0x1
    enabled_list = XMd5_hasher_InterruptGetEnabled(pEx);
//    printf("EnaLis: 0x%x\n",enabled_list);

    //Get interrupt status list. appdone is 0x1
    status_list = XMd5_hasher_InterruptGetStatus(pEx);
//    printf("StaLis: 0x%x\n",status_list);

    //Check ap_done created the interrupt
    if((enabled_list & 1) && (status_list & 1)){

    	//Clear the ap_done interrupt
    	XMd5_hasher_InterruptClear(pEx,1);
    	printf("Interrupt cleared\n");

    	XMd5_hasher_InterruptGlobalEnable(&HlsMd5_hasher);

//    	block_start();
    	pretty_printer();

    }
}

u32 init_ip(){

	// Get device information for init
	Md5_hasherPtr = XMd5_hasher_LookupConfig(XPAR_XMD5_HASHER_0_DEVICE_ID);

	if (!Md5_hasherPtr) {
		printf("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}

	// Initialize the Device
	int status = XMd5_hasher_CfgInitialize(&HlsMd5_hasher, Md5_hasherPtr);
	if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize accelerator.\n\r");
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

u32 setup_interrupt(){

	s32 result;
	XScuGic ScuGic;

	// global enable interrupts
	XMd5_hasher_InterruptGlobalEnable(&HlsMd5_hasher);

	// local enable ip block interrupt
	//1: ap_done; 2: ap_ready; 3: both
	XMd5_hasher_InterruptEnable(&HlsMd5_hasher,1);
	if (XMd5_hasher_InterruptGetEnabled(&HlsMd5_hasher)!=1)
		printf("Interrupt apdone NOT enabled\n");


	// Find the interrupt configuration table
	XScuGic_Config *pCfg = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	// Initialize the Interrupt Controller
	result = XScuGic_CfgInitialize(&ScuGic, pCfg, pCfg->CpuBaseAddress);

	//Initialize the exception handler
	Xil_ExceptionInit();

	//Register the exception handler
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler) XScuGic_InterruptHandler,&ScuGic);

	//Enable the exception handler
	Xil_ExceptionEnable();

	//Connect the Example ISR to the exception table
	//2arg=define in parameters.h, 3rd arg=ISRname, 4th arg=parameter for ISR
	result = XScuGic_Connect(&ScuGic,XMD5_HASHER_INTERRUPT_INTR,(Xil_InterruptHandler)Md5_hasher_isr,&HlsMd5_hasher);

	//Enable the Example ISR
	XScuGic_Enable(&ScuGic,XMD5_HASHER_INTERRUPT_INTR);
	return result;

}


void block_start(){

    XMd5_hasher_Start(&HlsMd5_hasher);
}

void pretty_printer(){

	int result=0;
	// changing stack&heap size stopt the infinite looping.(0x2000 and 0x3000).
	for (uint32_t i=0;i<16;i++){
		XMd5_hasher_Read_x_Words(&HlsMd5_hasher, i, &result, 1);
		printf("MSGs%d: %08x\n",i,((result & 0xFF)<<3*8) | ((result & 0xFF00) << 8) | ((result & 0xFF0000) >> 8) | ((result & 0xFF000000) >> 3*8));
	}
}

int main()
{
    init_platform();

    if (init_ip()!=XST_SUCCESS){
    	print("Init faillure\n\r");
    }

	if (setup_interrupt()!=XST_SUCCESS){
		printf("Interrupt setup failure\n");
	}

    // Kick start the hash cracking
	block_start();
	print("Block started\n\r");

	pretty_printer();

    cleanup_platform();
    return 0;
}
