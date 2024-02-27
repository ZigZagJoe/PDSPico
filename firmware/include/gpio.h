#ifndef _GPIO_H_
#define _GPIO_H_

#include <hardware/gpio.h>
#include "util.h"

/* PIN DEFINITIONS *****************************************/
// data bus, 16 bits wide
#define DATA_BUS_D0		0
#define DATA_BUS_WIDTH	16
#define DATA_BUS_MASK 	0xFFFF // 16 bits

#define ADDR_BUS_A0		8
#define ADDR_BUS_A0_MASK 	0xFF00 // 8 bits
#define ADDR_BUS_A0_FULLMASK 	0xFFF00 // 12 bits

/* BA8-BA11 */
#define ADDR_BUS_A8		16
#define ADDR_BUS_WIDTH	4

#define SEL_ROM		22
#define SEL_PICO		21

#define READ_WRITE		20

#define DSACK_PICO		26
#define RESET_IN		26

#define IRQ_PICO		27

/* LED output
 hi-Z = off
	1 = green
	0 = red
	pull up/down for dim colors
*/
#define LED_OUT			28

// detect vbus present
#define VBUS_SENSE		24

/* misc defines *******************************************/

/* LED states for set_led */
enum LED_STATE { GREEN, GREEN_DIM, BOTH, RED, RED_DIM, OFF };

/* prototypes *********************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern enum LED_STATE last_led_state;

/* set pin to GPIO mode, with direction and initial state */
void init_gpio_pin(uint gpio, bool dir, bool state);

/* board GPIO setup */
void board_gpio_init();

/* set LED state */
void led_set(enum LED_STATE s);

/* static inline IO functions for speed */
static inline void data_out() { 
	gpio_set_dir_masked(DATA_BUS_MASK, DATA_BUS_MASK); 
}

static inline void data_in()  { 
	gpio_set_dir_masked(DATA_BUS_MASK, 0); 
}

static inline uint16_t data_read() {
	return gpio_get_all() & DATA_BUS_MASK;
}

static inline void data_write(uint16_t d) {
	gpio_put_masked(DATA_BUS_MASK,d);
}


#ifdef __cplusplus
}
#endif

#endif