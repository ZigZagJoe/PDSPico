/* file for compile time configurable constants */
#ifndef CONFIG_H
#define CONFIG_H

#include "version.h"

#define PRODUCT_NAME                "PDSPico"
// max 8 chars, it is used as USB VID
#define PRODUCT_VENDOR              "ZZJ"
#define PRODUCT_VERSION             VERSION_SHORT

/** SERIAL PARAMETERS ********************************************/

// serial logging and logfile shared buffer
#define SERIAL_BUFF_SZ              4096

#define SERIAL_MAX_LINE_LEN         256
#define SERIAL_MAX_TIMESTAMP_LEN    20

// delay in ms after escape sequence
#define SERIAL_CMD_BACKOFF          500
#define SERIAL_DEFAULT_BAUD         9600
#define SERIAL_BAUD_MAX             1000000

/** FLASH PARAMETERS ********************************************/

#define FLASH_BANKS                 (2)

/* flash bank size in words */
#define FLASH_BANK_WORDS            (4 * 1024 * 1024)

/* flash bank size in bytes */
#define FLASH_BANK_BYTES            (FLASH_BANK_WORDS * 2)

#define FLASH_SIZE_WORDS            (FLASH_BANKS * FLASH_BANK_WORDS)
#define FLASH_SIZE_BYTES            (FLASH_BANKS * FLASH_BANK_BYTES)

// number of sectors. require uniform sector ROM (do not support boot sector ROM)
#define FLASH_SECTORS               128

// size of flash chip's write buffer
#define FLASH_BLOCKSIZE             256

// roughly map to flash cycle time (X*7.5ns at 133mhz)
#define FLASH_DELAY_CYCLES          9

/** UF2 PARAMETERS *********************************************/

#define BOARD_UF2_FAMILY_ID         0x10C68030

#define UF2_VOLUME_LABEL            PRODUCT_NAME

// flash bytes per UF2 file
#define CFG_UF2_FLASH_SIZE          FLASH_BANK_BYTES

// Number of 512-byte blocks in the exposed filesystem, 0x10109 is just under 32MB and works with CFG_UF2_SECTORS_PER_CLUSTER = 1
// The filesystem needs space for the current file, text files, uploaded file, and FAT
#define CFG_UF2_NUM_BLOCKS          (0x10109 * 2)

// Sectors per FAT cluster, must be increased proportionally for larger filesystems
#define CFG_UF2_SECTORS_PER_CLUSTER (4)

// timeout after which the current operation is aborted
#define WATCHDOG_TIMEOUT            5000

// small grace period after writes finish before ejecting USB storage
// required so host doesn't fail the copy transfer
#define MSC_RESET_TIMEOUT           1000

#endif