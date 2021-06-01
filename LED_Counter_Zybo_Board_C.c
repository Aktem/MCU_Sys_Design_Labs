#include <xparameters.h>
#include <xgpio.h>
#include <xstatus.h>
#include <xil_printf.h>

/* Definistions */

#define LED_Device_ID XPAR_LED_DEVICE_ID
#define SWITCHES_BUTTONS_ID XPAR_BUTTONSWITCH_DEVICE_ID
#define WAIT_VAL 0x1000000


int main() {
	int count;
	int count_masked;
	int continuos_count;
	XGpio leds;
	XGpio switches_buttons;
	
	int status_leds;
	int status_switches_buttons;
	u_32 s_b;
	u_32 prev_s_b;
	
	/*
	bool s0; //0x01
	bool s1; //0x02
	bool s2; //0x04
	bool s3; //0x08
	bool b0; //0x10
	bool b1; //0x20
	bool b2; //0x40
	bool b3; //0x80
	*/
	
	// Initialize peripherals
	status_leds = XGpio_Initialize(&leds, LED_Device_ID);
	status_switches_buttons = XGpio_Initialize(&switches_buttons, SWITCHES_BUTTONS_ID);
	XGpio_SetDataDirection(&leds, 1, 0x00);
	XGpio_SetDataDirection(&switches_buttons, 1, 0xFF); //not sure what channel to use
    // checking success of connection to peripherals
	if(status_leds != XST_SUCCESS) { 
        xil_printf("Initialization of LEDs failed\n");    
    }
	if(status_switches_buttons != XST_SUCCESS) { 
        xil_printf("Initialization of SW failed\n");    
    }
	
	count = 0;
	while(1) {
		// read in switches and buttons state
		s_b = XGpio_DiscreteRead(&switches_buttons, 1);
		count_masked = count & 0xF;
		if (prev_s_b==s_b) { // if states are held -> refresh at 1Hz
			if (continuos_count < WAIT_VAL){ // not at refresh time yet
				continuos_count = continuos_count + 1;
			}
			else { // refresh and update values
				if ((s_w & 0x10)>>4) { //if Button0
					xil_printf("Button0 Held: Adding 1\n"); 
					count = count + 1;
				}
				if ((s_w & 0x20)>>5) { // if Button1
					xil_printf("Button1 Held: Subtracting 1\n"); 
					count = count - 1;
				}
				if ((s_w & 0x40)>>6) { // if Button2
					xil_printf("Button2 Held: Status of Switches\n"); 
					XGpio_DiscreteWrite(&leds, 1, w_p&0xF);
				}
				if ((s_w & 0x80)>>7) { // if Button3
					xil_printf("Button3 Held: Status of Count\n");
					XGpio_DiscreteWrite(&leds, 1, count_masked);
				}
				continuos_count = 0;
			}
		}
		else { // if new state
			if ((s_w & 0x10)>>4) { //if Button0
				xil_printf("Button0 Pressed: Adding 1\n"); 
				count = count + 1;
			}
			if ((s_w & 0x20)>>5) { // if Button1
				xil_printf("Button1 Pressed: Subtracting 1\n"); 
				count = count - 1;
			}
			if ((s_w & 0x40)>>6) { // if Button2
				xil_printf("Button2 Pressed: Status of Switches\n"); 
				XGpio_DiscreteWrite(&leds, 1, w_p&0xF);
			}
			if ((s_w & 0x80)>>7) { // if Button3
				xil_printf("Button3 Pressed: Immediate LED Update\n");
				XGpio_DiscreteWrite(&leds, 1, count_masked);
			}
			continuos_count = 0;
		}
		prev_s_b = s_b;
	}
	
	return 0;
}