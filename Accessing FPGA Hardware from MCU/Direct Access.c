

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "multiply.h"
#include "xil_io.h"


int main()
{
    init_platform();

    //print("Hello World\n\r");
    int i;
    u32 write_data = 0;
    u32 product;
    for (i=0; i<17; i++)
    {
    	// Write to Register 0 and 1, writing write_data
    	MULTIPLY_mWriteReg(XPAR_MULTIPLY_0_S00_AXI_BASEADDR, MULTIPLY_S00_AXI_SLV_REG0_OFFSET, write_data);
    	MULTIPLY_mWriteReg(XPAR_MULTIPLY_0_S00_AXI_BASEADDR, MULTIPLY_S00_AXI_SLV_REG1_OFFSET, write_data);

    	// Saving Contents of Register 2 to variable product
    	product = MULTIPLY_mReadReg(XPAR_MULTIPLY_0_S00_AXI_BASEADDR, MULTIPLY_S00_AXI_SLV_REG2_OFFSET);

    	// Printing Read and Write Variables to console
    	xil_printf("Data Written to Registers: %d\n", write_data);
    	xil_printf("Data Read from Registers: %d\n", product);


    	write_data = write_data + 1;
    }

    cleanup_platform();
    return 0;
}
