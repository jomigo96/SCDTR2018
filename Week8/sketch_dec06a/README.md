# Calibration code

Code is modularized as:

+ `sketch_nov15a.ino` - main setup and loop.
+ `comms.h` - contains the communication protocols.
+ `controller.h` - contains a C++ class which wich will later be the controller
+ `calibration.h` - Contains the calibration function, which is called on setup.

## LDR parameters

Code is the same for every node, however, each LDR has its own b and m parameters. These are loaded apropriately by defining or the `NODE1` macro in the begining of the code, like:
```C
#define NODE1
```

The LDR corresponding node 2 was loosely calibrated at home. The new values are:

| Parameter     | Node 1| Node 2|
| ------------- |------:|------:|
| m             |-0.9355|-1.4760|
| b             |5.8515 |7.3250 |
| `address`     |0x01   | 0x09  |

## Other macros

```C
#define DEBUG
// Lots of debug messages thrown to the Serial Monitor

#define SUPRESS_LUX
// The PID controller does not output the LUX value to the serial monitor

#define DEBUG_MSG
// Debug information about outgoing and incomming messages
```
