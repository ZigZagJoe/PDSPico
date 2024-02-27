#include <Arduino.h>
#include <device/usbd.h>

#include "util.h"
#include "gpio.h"

void setup() {
	board_gpio_init();

	util_init();

	led_set(GREEN_DIM);
}

void setup1() {
	delay(100);
	dbg_printf("Core 1 start\n");
}

volatile uint16_t romaddr;
volatile uint16_t fifodata;
volatile uint8_t romdata;
	
//, 0x
uint8_t ROM_Data[4096] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD, 0xF0, 0x0D, 0xFE, 0xED, 0xFA, 0xCE, 0xCA,0xFE,0xBA,0xBE };

void loop1() {

	romdata = 0x79;
	fifodata = 0xADDE;

	cli();

	data_out();
	data_write(romdata);

	while(1) {
		uint32_t state = gpio_get_all();
		if (!(state & bv(SEL_PICO))) {

			if (!(state & bv(READ_WRITE))) {
				data_in();
				busy_wait_at_least_cycles(4);
				fifodata = data_read();
			} else {
				data_write(fifodata); // must swap bytes for 16 bit IO ie 0xDEAD becomes 0xADDE
				fifodata++; // test
			}

			gpio_put(DSACK_PICO,0);
			busy_wait_at_least_cycles(10);
			gpio_put(DSACK_PICO,1);
			
			data_out();
			data_write(romdata);

			led_set(GREEN);

		} else if (! (state & bv(SEL_ROM))) {
			gpio_set_dir_masked(ADDR_BUS_A0_MASK,0); // set BD8-15 as inputs
			busy_wait_at_least_cycles(4); // small delay required before reading address bits. 1 or more seesm to work
			romaddr = (gpio_get_all() & ADDR_BUS_A0_FULLMASK) >> 8;
				
			if (!(state & bv(READ_WRITE))) {
				data_in();
				busy_wait_at_least_cycles(4);
				romdata = data_read() & 0xFF;
				ROM_Data[romaddr] = romdata;
			} else {
				data_write(ROM_Data[romaddr]);
			}
			
			gpio_put(DSACK_PICO,0);
			busy_wait_at_least_cycles(10);
			gpio_put(DSACK_PICO,1);
			
			data_out();
			data_write(romdata);

			led_set(RED);
		}
	}
}

/* main loop. most code runs from tinyusb task rather than here. */
void loop() {

	uint16_t lastra = -1;
	uint16_t lastf = -1;

	while(1) {
		// platformio device monitor is dumb, and will purge any pending serial lines.
		// wait a moment before starting to dump serial logs out... as I am lazy.
		if (millis() > 3000) 
			dbg_flush_log();

		// serial bridge / command interface
		//poll_serial();

		if (romaddr != lastra) {
			lastra = romaddr;
			dbg_printf("addr: %x\n",lastra);	
			dbg_printf("data: %x\n",romdata);	
		}

		if (fifodata != lastf) {
			lastf = fifodata;
			dbg_printf("fifo: %x\n",lastf);	
		}
	}
}
