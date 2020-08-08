# ModeSwitch for the Pop'n Ultimate Controller

This is a binary meant to set the LightMode used by the Pop'n Controller. It is mostly useful for multi-boot frontends as it allows to switch to the correct mode before launching a game.

For ease of use I included a pre-compiled binary working on WinXP Embedded (for official Pop'n Music cabinet).

While the autohotkey version works perfectly on my computer I couldn't make it work on the BemaniPC. I'm still including it here as it provides a good basis for HID light control with simple scripts.

## Usage

```modeswitch.exe <num>```

Where ``<num>`` can take the value

0 : Simulated mode (Reactive buttons + AC lighting simulation for emulators)

1 : HID only (original AC mode)

2 : Mixed (AC mode combined with lights from any button pressed)

3 : Invert (Like Mixed except ON is OFF and vice versa)

## How it works

On the Pop'n Ultimate Controller firmware, HID report ID 5 is a 32bit bitfield :

- bits  0 to 17 are the HID lamps.
- bit  28 is the magic lightMode change request bit
- bits 24 and 25 encode the requested lightMode if bit 28 is set

This binary opens the USB device whose path is written in ```devicepath.dat``` and sends the correct HID report.

The autohotkey version finds the path automatically based on VID=2341 and PID=003E.

## How to retrieve devicepath (for the C version)

The included devicepath.dat should work with both Due and Leonardo versions out of the box, but in case you get the message ``Couldn't open device. Make sure devicepath.dat contains the correct path.`` and you are sure the device is plugged in, you can either go into windows device manager and look for the device path in the advanced properties of your arduino device, or the easiest way is to look at the value inside spicetools or bemanitools configuration files.

devicepath.dat file should contain the device path on its own on a single line. Multiple paths on multiple lines should work too in which case it will open the first valid path it finds.

If no valid path is found or if there is no `devicepath.dat` file, it will attempt to open the default paths from the Due and Leonardo versions (same as the ones found in this included devicepath file).
 
### spicetools

open ```%appdata%/spicetools.xml```, look for a "devid" value starting with ```\\?\HID#VID_2341&amp;PID_003E``` (Due) or ```\\?\HID#VID_2341&amp;PID_8036``` (Leonardo)

Note that you'll have to replace "```&amp;```" occurrences by "```&```".

### bemanitools

open ```%appdata%/DJHACKERS/pnm_v4_25.bin``` and look for a string starting with ```\\?\HID#VID_2341&PID_003E``` (Due) or ```\\?\HID#VID_2341&PID_8036``` (Leonardo)
