## Hawk-nest is MOS 6502 cpu emulator built in C. emu/mos6502/mos6502.c contains all the Instruction Set translation handlers for MOS6502, uses decode and dispath architecture.

### Build ###
`make`

### Build ROMS ###
`make tests`

### Run ###
`bin/hawknest-gcc-debug <rom-path>`
#### test code example: ####
`bin/hawknest-gcc-debug bin/hello`
#### to open shell ####
`bin/hawknest-gcc-debug -i bin/hello`
#### This displays the PC address at each step of the code for debugging purpose.
