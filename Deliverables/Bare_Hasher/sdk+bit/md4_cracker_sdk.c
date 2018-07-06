

/*
 * Md4_cracker.c: simple test application
 *
 * This application finds a pre-image hash collision for MD4
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
#include "xtime_l.h"
#include "xil_printf.h"
#include "xmd4_hasher.h" 	// Device driver for HLS HW block
#include "xparameters.h"

#include "xscugic.h"		// interrupts
#include "xil_exception.h"	// interrupts
#include "../../Md4_cracker/src/platform.h"

#define XMD4_HASHER_INTERRUPT_INTR XPAR_FABRIC_MD4_HASHER_0_INTERRUPT_INTR
#define HASHES 1000

// HLS HW instance
XMd4_hasher HlsMd4_hasher;
XMd4_hasher_Config *Md4_hasherPtr;


void Md4_hasher_isr(void *InstancePtr){
//    print("\nInterrupt hit\n\r");

    int enabled_list;
    int status_list;
    XMd4_hasher *pEx = (XMd4_hasher *) InstancePtr;

    //Disable Global Interrupt
    XMd4_hasher_InterruptGlobalDisable(pEx);

    //Get list of enabled interrupts. appdone is 0x1
    enabled_list = XMd4_hasher_InterruptGetEnabled(pEx);
//    printf("EnaLis: 0x%x\n",enabled_list);

    //Get interrupt status list. appdone is 0x1
    status_list = XMd4_hasher_InterruptGetStatus(pEx);
//    printf("StaLis: 0x%x\n",status_list);

    //Check ap_done created the interrupt
    if((enabled_list & 1) && (status_list & 1)){

    	//Clear the ap_done interrupt
    	XMd4_hasher_InterruptClear(pEx,1);
//    	printf("Interrupt cleared\n");

    	XMd4_hasher_InterruptGlobalEnable(&HlsMd4_hasher);

//    	if (hashCount<HASHES){
//    		block_start();
//    		hashCount++;
//    	}
//    	else printf("hashCount=%d\n\r", hashCount);

    }
}

u32 init_ip(){

	// Get device information for init
	Md4_hasherPtr = XMd4_hasher_LookupConfig(XPAR_XMD4_HASHER_0_DEVICE_ID);

	if (!Md4_hasherPtr) {
		printf("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}

	// Initialize the Device
	int status = XMd4_hasher_CfgInitialize(&HlsMd4_hasher, Md4_hasherPtr);
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
	XMd4_hasher_InterruptGlobalEnable(&HlsMd4_hasher);

	// local enable ip block interrupt
	//1: ap_done; 2: ap_ready; 3: both
	XMd4_hasher_InterruptEnable(&HlsMd4_hasher,1);
	if (XMd4_hasher_InterruptGetEnabled(&HlsMd4_hasher)!=1)
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
	result = XScuGic_Connect(&ScuGic,XMD4_HASHER_INTERRUPT_INTR,(Xil_InterruptHandler)Md4_hasher_isr,&HlsMd4_hasher);

	//Enable the Example ISR
	XScuGic_Enable(&ScuGic,XMD4_HASHER_INTERRUPT_INTR);
	return result;

}


void block_start(){

    XMd4_hasher_Start(&HlsMd4_hasher);
}

void pretty_printer(){

	int result=0;
	// changing stack&heap size stopt the infinite looping.(0x2000 and 0x3000).
	for (uint32_t i=0;i<16;i++){
		XMd4_hasher_Read_x_Words(&HlsMd4_hasher, i, &result, 1);
		printf("MSGs%d: %08x\n",i,((result & 0xFF)<<3*8) | ((result & 0xFF00) << 8) | ((result & 0xFF0000) >> 8) | ((result & 0xFF000000) >> 3*8));
	}
}

int main()
{
	unsigned long long int hashCount=0;
	XTime tStart, tEnd;

    init_platform();

    if (init_ip()!=XST_SUCCESS){
    	print("Init faillure\n\r");
    }

    // interrupt init
//	if (setup_interrupt()!=XST_SUCCESS){
//		printf("Interrupt setup failure\n");
//	}

	print("Start\n\r");

	//kickstart
	block_start();

//	time_t t0 = time(0);

	XTime_GetTime(&tStart);

	while(XMd4_hasher_Get_msgColl(&HlsMd4_hasher)!=1&&(hashCount<HASHES)){
		if (XMd4_hasher_IsDone(&HlsMd4_hasher)){
			block_start();
//			pretty_printer();
//			printf("m%d\n\r",XMd4_hasher_Get_msgColl(&HlsMd4_hasher));
			hashCount++;
		}
	}

	XTime_GetTime(&tEnd);

	float time = 1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND);

	printf("Output took %llu clock cycles.\n", 2*(tEnd - tStart));
	printf("Output took %.8f sec.\n",time);
	printf("Hashes per second %f\n\r",HASHES/time);


	//print msg when broken
//	pretty_printer();

    cleanup_platform();
    return 0;
}
