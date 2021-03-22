# Cubed
Modular LED cubes that can be controlled via serial port or be allowed to idly
cycle through colors.

## Requirements
- [ ] Base
  - [x] Write serial protocol documentation
  - [x] Finalize hardware layout
    - [x] Enclosure
    - [x] Number of LEDs per cube
      - There will only be one LED per cube due to hardware resource
      constraints.
    - Connector style (just female headers)
  - [x] Decide ambient functionality
    - Time based (GPS being used)
    - Blink
    - Solid color
    - Rainbow
    - Disabled
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
will provide information on their contents and format. The return of each
each command is the original opcode followed by any return packets.


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


### `0xCA` - UPDATE
Updates the state of all cubes.

#### Transmission
No arguments.

#### Receiving
Returns original opcode, no additional data.


### `0xCB` - SET MODE
Set the mode of a cube. The following modes are supported.

| `CODE` | Description |
| ------ | ----------- |
| `0x00` | Disabled, the cube will not be lit. |
| `0x01` | Solid color, defaults to black but can be overridden by `SET COLOR` opcode. | 
| `0x02` | Blink, cube will cycle between black and the color specified by `SET COLOR` every 500ms. |
| `0x03` | Daylight, changes according to the current time of day. |
| `0x04` | Rainbow, cycles through RGB values in a rainbow. |

#### Transmission
Two arguments, the cube and the specified mode.

```
opcode    cube      mode
11001011  00000010  00000011
0xCB      0x02      0x03
```

#### Receiving
Returns original opcode, no additional data.


### `0xCC` - GET MODE
Used to get the mode of the specified cube.

#### Transmitting
One argument, the cube that you'd like to get the mode of.

```
opcode    cube
11001101  00000010
0xCC      0x02
```

#### Receiving
Two arguments, the number and the mode of the cube that you requested.

```
opcode    cube      mode
11001011  00000010  00000011
0xCB      0x02      0x03
```   0x03
```


### `0xCD` - SET COLOR
Sets the specified cube to the specified color. Does not update the cube.

#### Transmission
Four arguments, specifing the cube and RBG value to set it to. The below example
sets the second cube (zero indexed) to white.

```
opcode    cube      R         G         B
11001101  00000010  11111111  11111111  11111111
0xCD      0x02      0xFF      0xF       0xFF
```

#### Receiving
Returns original opcode, no additional data.


### `0xCE` - GET COLOR
Used to get the mode of the specified cube.

#### Transmitting
One argument, the cube that you'd like to get the color of.

```
opcode    cube
11001110  00000010
0xCE      0x02
```

#### Receiving
Four arguments, the number and the RGB color values of the cube that you
requested.

```
opcode    cube      R         G         B
11001110  00000010  11111111  11111111  11111111
0xCE      0x02      0xFF      0xF       0xFF
```
