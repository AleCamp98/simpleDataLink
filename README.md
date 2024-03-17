# Simple Data Link protocol - simpleDataLink.h/.c
This library implement a simple unreliable data link layer to be used with serial lines.
The concept behint the library is basically to take a payload buffer from the user, create a frame from that and send it on a generic serial line.
The user will need to implement I/O functions towards the hardware (for example an UART), this allows porting the library easily to any architecture.

The frames are crude and there's not an header part, this should be implemented on upper layers by the user if needed, the frame format is the following:

| 0x7E | PAYLOAD | CRC16 | 0x7E |

The frame is enclosed between two 0x7E flag bytes and contains the payload plus a 16 bit CRC.

## Byte Stuffing
The library implements an HDLC-like byte stuffing algorithm in order to eliminate any occurrence of 0x7E inside the payoad or the CRC, the algorithm works by adding an escape byte 0x7D before any occurrence of the flag byte 0x7E or the escape byte itself, the latter gets its 5-th byte inverted, like in the following table:

|Occurrence|Replaced by|
|---|---|
|0x7E| 0x7D 0x5E |
|0x7D| 0x7D 0x5D |

Byte stuffing allows an easy search of frames since it allows to have the 0x7E flag only at the begin/end of frames.

## CRC-16
By default, the CRC-16 is implemented by using polynomial 0x1021 and initialization value 0xFFFF and uses a pre-generated look-up table to increase performance, the user can use another polynomial by generating a new look-up table with the printCRCLUT() function which is commented out in simpleDataLink.c, the LUT can be tested against the classic shift-xor algorithm by using the computeCRC() function which is also commented out on the same file.

## Serial line handle and I/O functions
A serial line is represented by a serial_line_handle structure, this needs to be initialized with the sdlInitLine() function, this function needs two function pointers which point to I/O functions defined by the user, those functions will implement the transmission/reception of a single byte on the specifi serial line hardware (see simpleDataLink.h for more informations), allowing the library to be ported or used with different types of lines and drivers.

## Frame send/receive functions
Finally, the library can be used with sdlSend() and sdlReceive() functions, those will handle everything, from frame creation/extraction to CRC creation/verification, byte stuffing and I/O on the line. The maximum payload length can be defined with the SDL_MAX_PAY_LEN macro.
An example of usage of the library is provided in examples/communicationExample.c

## compile instructions
The source code can be compiled into a static library (simpleDataLink.a) by running **make** on the root directory, by default this will compile all sources into object files on the **build** folder and then pack them in the static library, to compile the example you can instead call **make example**, again this will compile the example executable on the **build** folder.
You can change the build folder or compiler flags (default **-Wall**) by passing variables to make like **make builddir=newbuilddirectory compflags=newcompilerflags**
