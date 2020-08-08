# ezusb.dll for the Pop'n Ultimate Controller

This is a dll meant to seamlessly replace the original one from the different games (and compatible with both the PopnForwarder and DJMame, or really anything else meant to be used with ezusb.dll).

For ease of use I included pre-compiled dll working on WinXP Embedded (for official Pop'n Music cabinets).

## Usage

Replace the game ezusb.dll file with this one and the game will read input and send lights to your Arduino as if it were the original IOBoard.

Pop'n Music Adventure used a different dll format so you have to set a flag in `ezusb.h` for the dll to compile in a way that will work with this game.

The dll makes the lights turn on for 1 sec on load so you'll know if the game managed to load your device.

## How to retrieve devicepath

The dll will attempt to open the default paths from the Due and Leonardo versions of this firmware, in case it doesn't work you can put a `devicepath.dat` file in the same folder as the dll.

See instructions from ModeSwitch to know more about the path format and how to find the one corresponding to your device.