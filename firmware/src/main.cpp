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

volatile struct {
	uint16_t romAddr;
	uint8_t romData;
	uint8_t fifoAddr;
	uint16_t fifoData;
	uint32_t fifoCount;
	uint32_t romCount;
	uint8_t direction;
} lastState;

	
//, 0x
uint8_t ROM_Data[4096] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD, 0xF0, 0x0D, 0xFE, 0xED, 0xFA, 0xCE, 0xCA,0xFE,0xBA,0xBE };

void loop1() {

	memset((void *)&lastState,0,sizeof(lastState));
	lastState.romData = 0x79;
	lastState.fifoData = 0xADDE;

	cli();

	data_out();
	data_write(lastState.romData);

	while(1) {
		uint32_t state = gpio_get_all();
		if (!(state & bv(SEL_PICO))) {
			lastState.fifoAddr = (state & ADDR_BUS_A8_MASK) >> 16;

			if (!(state & bv(READ_WRITE))) {
				data_in();
				busy_wait_at_least_cycles(2);
				lastState.fifoData = data_read();
				lastState.direction = 0;
			} else {
				data_write(lastState.fifoData); // must swap bytes for 16 bit IO ie 0xDEAD becomes 0xADDE
				lastState.direction = 1;
			}

			gpio_put(DSACK_PICO,0);
			busy_wait_at_least_cycles(10);
			gpio_put(DSACK_PICO,1);
			
			data_out();
			data_write(lastState.romData);

			led_set(GREEN);
			lastState.fifoCount++;

		} else if (! (state & bv(SEL_ROM))) {
			gpio_set_dir_masked(ADDR_BUS_A0_MASK,0); // set BD8-15 as inputs
			busy_wait_at_least_cycles(2); // small delay required before reading address bits. 1 or more seesm to work
			lastState.romAddr = (gpio_get_all() & ADDR_BUS_A0_FULLMASK) >> 8;
				
			if (!(state & bv(READ_WRITE))) {
				data_in();
				busy_wait_at_least_cycles(2);
				lastState.romData = data_read() & 0xFF;
				ROM_Data[lastState.romAddr] = lastState.romData;
				lastState.direction = 0;
			} else {
				data_write(ROM_Data[lastState.romAddr]);
				lastState.direction = 1;
			}
			
			gpio_put(DSACK_PICO,0);
			busy_wait_at_least_cycles(10);
			gpio_put(DSACK_PICO,1);
			
			data_out();
			data_write(lastState.romData);

			led_set(RED);
			lastState.romCount++;
		}
	}
}

/* main loop. most code runs from tinyusb task rather than here. */
void loop() {

	uint32_t lfc = 0, lrc = 0;

	while(1) {
		// platformio device monitor is dumb, and will purge any pending serial lines.
		// wait a moment before starting to dump serial logs out... as I am lazy.
		if (millis() > 3000) 
			dbg_flush_log();

		// serial bridge / command interface
		//poll_serial();

		if (lrc != lastState.romCount) {
			lrc = lastState.romCount;
			dbg_printf("rom count %d\n%c ($%x) = $%x\n",lastState.romCount,lastState.direction?'R':'W',lastState.romAddr,lastState.romData);	
		}

		if (lfc != lastState.fifoCount) {
			lfc = lastState.fifoCount;
			dbg_printf("fifo count %d\n%c ($%d) = $%x\n",lastState.fifoCount,lastState.direction?'R':'W',lastState.fifoAddr,lastState.fifoData);	
		}
	}
}
