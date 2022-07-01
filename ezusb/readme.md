# ezusb.dll for the Pop'n Ultimate Controller

This is a dll meant to seamlessly replace the original one from the different games (and compatible with both the PopnForwarder and DJMame, or really anything else meant to be used with ezusb.dll).

For ease of use I included pre-compiled dll working on WinXP Embedded (for official Pop'n Music cabinets).

## Usage

Replace the game ezusb.dll file with this one and the game will read input and send lights to your Arduino as if it were the original IOBoard.

The dll makes the lights turn on in a wave pattern during the game's i/o check, so you'll know if it managed to load your device.

## How to retrieve devicepath

The dll will attempt to open the device automatically based on the custom VID/PID. In case it doesn't work you can put a `devicepath.dat` file in the same folder as the dll.