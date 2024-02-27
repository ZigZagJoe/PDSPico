#ifndef UTIL_H
#define UTIL_H

#include "config.h"

// bit value helper macro
#define bv(V) (1UL << (V))

#ifdef __cplusplus
extern "C" {
#endif

/* settings struct - order-sensitive, do not re-order */
struct EEPROM_settings {
    uint32_t unused;
};

extern struct EEPROM_settings persistConfig;

/* globals ****************************************/

/* shared serial / log file buffer */
extern char serial_buff[SERIAL_BUFF_SZ];

/* controls if debug log goes to serial */
extern bool debug_log_to_serial;

/* prototypes *************************************/

/* copy serial characters to/from the serial port */
void poll_serial();

/* serial simple command parser */
void serial_exec_cmd(char * cmd, uint16_t len);

/* init log buffer and serial */
void util_init();

/* interrupt-safe print to rolling log buffer */
void dbg_printf(const char *format, ...);

/* flush log buffer to serial */
void dbg_flush_log();

/* update persistent config */
void util_write_config();

#ifdef __cplusplus
}
#endif


#endif