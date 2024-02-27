# PDSPico

early days of a project to interface a Pico W microcontroller to a 68030 bus
hardware has been validated, code WIP

## modes of access.

### PICO starts at 0xfX000000

16 bit port, 4 address bits A8-A11

intended for use as FIFOs between Pico and 68030 for bulk data transfer
anticipated to be accelerated by PIOs

### Status  at     0xfX400000

8 bit port, no address bits. 

automatically dsack by GAL. intent is to allow the pico to leave status flags on its bus and the 68030 can poll without needing to interrupt pico

### ROM starts at  0xfXc00000

8 bit port, 12 address bits A0-A11 (4KB)

intended to provide DeclROM emulation, but as this will also be writable it can be used for ancillary data transfer/registers
expected to be slower as pico CPU will need to be interrupted to handle these accesses.

X is slot, A or B selectable

## IO resources:

- USB-A host port connected to pico's USB port: possibility for mass storage or other fun things
- Wifi on the Pico module
- One free output on the GAL
- No free IO on the Pico. One output used for two LEDs: could be repurposed.
- The upper section of the multiplexed data bus could possibly be used for shared IO, but this would require blocking of the host's access to anything but status bits....