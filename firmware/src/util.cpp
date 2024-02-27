#include <Arduino.h>
#include <EEPROM.h>
#include <string.h>
#include <tusb.h> 
#include "util.h"
#include "gpio.h"
#include "compile_date.h"

extern "C" {
    const char buildTime[] = COMPILE_DATE " " COMPILE_TIME;

    const char serial_cmd_str[] = "~~~";

    struct EEPROM_settings persistConfig;

    // serial buffer
    char serial_buff[SERIAL_BUFF_SZ];
    bool debug_log_to_serial = 1;

    static uint16_t rd_ind = 0;
    static uint16_t wr_ind = 0;

    /* init log buffer and serial */
    void util_init() {
        
        rd_ind = 0;
        wr_ind = 0;
        memset(&serial_buff, ' ', SERIAL_BUFF_SZ);
        serial_buff[0] = 0;

        // echo version
        dbg_printf(PRODUCT_VENDOR " " PRODUCT_NAME" " PRODUCT_VERSION "\n");
	    dbg_printf("Built %s\n", buildTime);
	
        // both USB and hardware serial
        Serial.begin();
        
        /* open persistent settings */
	    EEPROM.begin(256);
        EEPROM.get(0, persistConfig);

    }

    void util_write_config() {
        dbg_printf("Committing persistent configuration\n");
        EEPROM.update(0, persistConfig);
        EEPROM.commit();
    }

    /* interrupt-safe print to rolling log buffer */
    void dbg_printf(const char *format, ...) {

        // timestamp
        wr_ind += snprintf(&serial_buff[wr_ind], SERIAL_MAX_TIMESTAMP_LEN, "%*lu ms  ",8,millis());
       
        va_list ap;
        va_start(ap, format);

        // entry content
        wr_ind += vsnprintf(&serial_buff[wr_ind], (SERIAL_MAX_LINE_LEN - SERIAL_MAX_TIMESTAMP_LEN), format, ap);;
        va_end(ap);

        // always keep SERIAL_MAX_LINE_LEN bytes free to receive new lines, wrap if we're in danger of overrun
        if ((SERIAL_BUFF_SZ - wr_ind) < SERIAL_MAX_LINE_LEN) {
            wr_ind = 0;
            serial_buff[wr_ind] = 0; // null terminate it
        }
    }

    /* flush log buffer to serial */
    void dbg_flush_log() {
        if (wr_ind != rd_ind) {
            if (!debug_log_to_serial) { 
                rd_ind = wr_ind;
            } else if (tud_cdc_connected()) {
                rd_ind += Serial.print(&serial_buff[rd_ind]);
                
                // wrap back to start
                if (wr_ind < rd_ind) 
                    rd_ind = 0;
            }
        }
    }

    #define CMD_MATCH(cmd, inp, len) (len && strncasecmp(cmd,inp,min(sizeof(cmd)-1,len)) == 0)

    /* serial simple command parser */
    void serial_exec_cmd(char * cmd, uint16_t len) {
        Serial.println();
                 
        if (CMD_MATCH("todo",cmd,len)) {
            uint32_t arg = atoi(&cmd[4]);
            
            if (arg > 0 && arg < SERIAL_BAUD_MAX) {
                Serial.printf("Set todo to %lu\n",arg);

                util_write_config();
             
            } else if (arg) { // bad argument
                Serial.printf("todo %lu out of range.\n",arg);
            } else  // no argument, print current
                Serial.printf("Current todo is %lu\n",persistConfig.unused);

        } else {
            if (len)
                dbg_printf("Unrecognized command: %s\n",cmd);

            Serial.println( "Unrecognized command. Valid commands:\n\n"
                            "HAHA TODO\n"           
                        );
        }
    }

    /* copy serial characters to/from the serial port */
    void poll_serial() {
        // escape string to enter command mode

        static uint8_t cmd_ind = 0;
        static char cmd_buff[128];
          
    
        /* xmit USB -> Serial, allowing commands*/
        while (Serial.peek() != -1) {
            char c = (char)Serial.read();

            // end of command string rec'd, execute it
            if (c == '\n' || c == '\r' || (cmd_ind == sizeof(cmd_buff)-1)) {
                cmd_buff[cmd_ind] = 0;
                serial_exec_cmd(cmd_buff, cmd_ind - 1 /*strlen*/);
                cmd_ind = 0;
            } else if (c == '\b') { // handle backspace
                if (cmd_ind) {
                    cmd_buff[--cmd_ind] = 0;
                    Serial.print("\b \b"); // clear the current character
                }
            } else { // capture the char
                Serial.write(c);
                cmd_buff[cmd_ind++] = c;  
            }    
        }
        
    }

}