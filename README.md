# Cubed
Modular LED cubes that can be controlled via serial port or be allowed to idly
cycle through colors.

## Requirements
- [ ] Base
  - [ ] Write serial protocol documentation
  - [ ] Finalize hardware layout
    - [ ] Enclosure
    - Number of LEDs per cube
    - Connector style (likely just female headers)
  - [ ] Decide ambient functionality
    - Time based (RTC required?)
    - Thermometer based
    - Random
    - Other?
  - [ ] Firmware development
    - [ ] Communication protocol
    - [ ] Color smoothing
    - [ ] Ambient modes

- [ ] Stretch
  - [ ] Manufacture PCBs
  - [ ] Capacitive touch
  - [ ] Desktop side program to control based on events
    - [ ] Discord notifications (game-sdk)
    - [ ] Weather
    - [ ] Other?

## Protocol v0
The protocol used for communication is fairly simple. All commands must begin
with a one byte opcode, followed by arguments. All arguments **must** be
provided for any given opcode, and no extraneous arguments can be provided.
These restrictions apply to both transmitted and received communications.

Arguments to opcodes are typically byte-long pieces of information. Each opcode
will provide information on their contents and format.

### `0xBE` - BEGIN
Begins serial communications and/or requests information on the connected
device. This is a bidirectional opcode.

#### Transmission
No arguments.

#### Receiving
This opcode has three arguments. They consist of the protocol version, the
number of cubes currently connected, and the global brightness setting. Below is
an example of bytes that might be seen when receiving this opcode.

```
opcode    ver       #cubes    brightness
10111110  00000000  00000101  11111111
0xBE      0x00      0x05      0xFF
```