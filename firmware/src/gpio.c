#include <Arduino.h>
#include "gpio.h"

enum LED_STATE last_led_state;

/* board GPIO setup */
void board_gpio_init() {

	/*  data bus */
	for (uint i = DATA_BUS_D0; (i-DATA_BUS_D0) < DATA_BUS_WIDTH; i++) { 
		init_gpio_pin(i,		GPIO_IN,	0);
		gpio_set_pulls(i, 0, 0); // disable pullup/down
	}

	/* address bus */
	for (uint i = ADDR_BUS_A8; (i-ADDR_BUS_A8) < ADDR_BUS_WIDTH; i++) { 
		init_gpio_pin(i,		GPIO_IN,	0);
		gpio_set_pulls(i, 0, 0); // disable pullup/down
	}
	
	/* shared lines with external pullup/down */
	init_gpio_pin(SEL_ROM,		GPIO_IN,	1);
	init_gpio_pin(SEL_PICO,		GPIO_IN,	1);
	init_gpio_pin(READ_WRITE,		GPIO_IN,	1);

	init_gpio_pin(DSACK_PICO,		GPIO_OUT,	1);
	init_gpio_pin(IRQ_PICO,		GPIO_OUT,	1);
}

/* set pin to GPIO mode, with direction and initial state */
void init_gpio_pin(uint gpio, bool dir, bool state)
{
    gpio_init(gpio); // sets to enable and clears value
	gpio_put(gpio, state);
    gpio_set_dir(gpio, dir);
   // gpio_set_pulls(gpio, ); 
}

/* set LED state */
void led_set(enum LED_STATE s) {
	last_led_state = s;
	if (s == BOTH) { // PWM causes both LEDs to light
		analogWrite(LED_OUT, 90);
	} else {
		gpio_init(LED_OUT); // sets to enable and clears value
		switch (s) {
			case GREEN:
			case RED:
				gpio_set_dir(LED_OUT, GPIO_OUT);
				gpio_put(LED_OUT,s == GREEN);
				break;
			default:	
				gpio_set_pulls(LED_OUT,s == GREEN_DIM, s == RED_DIM);
				gpio_set_dir(LED_OUT, GPIO_IN);
				break;
		}
	}
}

