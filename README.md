[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)

# Ultimate Pop'n Controller

USB HID controller with 13 inputs (9 buttons, coin, test, reset, service), 20 named outputs (18 lights, coin blocker, coin counter), 4 features (dip switches), 12-key matrix numpad, high polling rate, custom dll for full cabinet compatibility, Pop'n beMouse and Lively native compatibility, and nice programmable features.

The goal was to replace the official IO Board from my pop'n cabinet with a DIY controller instead, for stability reasons (Konami IO boards are prone to failure, most Pop'n Music cabinets in the wild have broken lamps...) but also for QoL improvements (this allows me to use my panel system-wide and not only in-game, which means I can control a multiboot menu, play with emulators etc..).

In combination with the PN5180-cardio project, the whole IO from a Pop'n Music cabinet can be replaced pretty cheaply :)

# Leonardo version

This code was originally written for Arduino Due but is also compatible with Leonardo without any change required.

Due to lack of gpio, in this case there's only 11 buttons and 9 lights, no keypad support, and reactive mode won't include AC light simulation (there's no side or top lamps, but you might want to have a look at the ambilight branch for ws2812b side/top lamps). However, there is **Playstation compatibility** as well.

I'm also taking advantage of the Leonardo EEPROM. On manually switching, the resulting lightmode is stored in the EEPROM so that it persists on controller disconnect/reconnect.

You only need to select "Leonardo" as your board type in Arduino IDE before flashing.

# Demo

https://www.instagram.com/p/CKE9HCQFCYM/

https://www.instagram.com/p/CBWgSMklvvT/

# Acknowledgments

Other pop'n controller codes like Knuckleslee's https://github.com/knuckleslee/RhythmCodes/ or 4yn's https://github.com/4yn/iivx/tree/master/leoo/leoo are widely popular.

They are usually based on mon's https://github.com/mon/Arduino-HID-Lighting which is based on NicoHood's https://github.com/NicoHood/HID/blob/master/src/SingleReport/RawHID.cpp 

As I needed more gpio than usual, this project was my first time working with Arduino Due and unfortunately the USB HID definitions are not compatible with Leonardo code so a lot of changes had to be made.

The keypad code uses the Keypad library by Mark Stanley and Alexander Brevig.

Switch debouncing is done with Bounce2 library by Thomas O Fredericks.

Playstation compatibility uses parts of progmem's excellent https://github.com/progmem/re-usbemani/ project

# Supported devices and requirements

This code was designed for Arduino Due. It will compile for Leonardo as well but some features are stripped due to lack of gpio (only 11 buttons and 9 lights, no keypad, lower polling rate). 
Code is updated automatically depending on the selected target board, no changes are needed.

The controller code requires the Bounce2 library by Thomas O Fredericks. It can be installed from the Arduino IDE Library manager.

The keypad code requires the Keypad library by Mark Stanley and Alexander Brevig. It can be installed from the Arduino IDE Library manager.

# Features

## I/O

This controller has 13 buttons (9 buttons + coin + service + reset + test), 4 dip-switches (only DIP4 is used in cabinets to select between 15 or 31kHz monitor resolution on boot), 18 lights (9 buttons + 5 top neon + 4 side pillar), 2 coin outputs (coin blocker, coin counter).

It also has 7 pins for the 12-key numpad, following the original cabinet pinout. It is recognized as a separate keyboard, and it is mapped to the keyboard toprow rather than the numpad (due to lack of stability when sending the numlock command to the BemaniPC).

The 00 key is mapped to comma (for direct compatibility with spicetools) and the originally unused bottom right key is mapped to the default card scan key.

It also has a cool light animation on boot which you can easily adapt to your liking :)

## ezusb driver

The I/O is HID so it can be mapped with the usual IO emulation tools, and even has named outputs for ease of use, but it can also be used just like an official IOBoard, provided you replace the original `ezusb.dll` file with the one from this repo.

This way the firmware is fully compatible with anything that works on an official cabinet (PopnForwarder (a bit silly but why not :D), DJMame, ezPSXe pad plugin...)

## pop'n beMouse

The controller will enumerate in a way that is directly compatible with the old Pop'n beMouse software (it also directly works with Pop'n Lively).

## Light modes

There are 5 different modes :

### Reactive mode

This mode roughly simulates AC lighting and was meant to be used for emulators or other games with no HID light support.

Pressing a button will trigger blinking lights from the side pillars, blue most of the time, red with 1/7 probability, and purple with 1/20 probability.

The top neons follow a fill-empty pattern whose speed is dynamically adjusted with the rate at which you press buttons (might need more work tweaking values, but looks good enough already).

### HID mode

This is the original IO board mode where only messages from the game can control the lamps.

### Mixed mode (default mode)

This mode behaves like HID mode as soon as messages are received. If 3 seconds elapse without any received message, the firmware switches to reactive behavior (and will switch back to HID as soon as new HID messages are received).

### Combined mode

This combines the HID messages with button presses for instant lighting (you don't have to wait for the game to register the input and send a message back to light the lamp, and as a bonus it also allows you to play with the lights while the cabinet is booting ;) ).

### Invert mode

This is like the Mixed mode except ON and OFF states are switched around just for the fun of it. Might be even more fun to use with colored leds and nothing but white buttons.

## Switching lightmodes

You can either press button 2 (left yellow button) while holding service to switch between modes, or you can request a mode change with an HID message, which is incredibly useful in a multiboot environment as you can set the proper lightmode from the commandline before launching a game.

I included pre-compiled binaries and sources in the "ModeSwitch" folder. Refer to readme.md inside that folder for more details.

# Pinout (DUE)

The Arduino DUE has 3.3v logic whereas the Pop'n Music cabinet outputs use 12V.

Therefore I'm using transistors to do level shifting (the parts I used were three ULN2003APG or TBD62003APG chips). 

You can also buy pre-made level shifters such as this one https://www.tindie.com/products/ddebeer/12-channel-level-shifter-and-buffer/ (you'd need two of them since there are 20 outputs to control for a Pop'n cabinet).

Refer to ```pinout.png``` to see how it is all wired to a Pop'n Music cabinet.
	  
![pinout](https://github.com/CrazyRedMachine/UltimatePopnController/blob/master/pinout.png?raw=true)

## Pop'n IOShield

I've also built a custom arduino shield with integrated level-shifting circuitry. This allows you to simply connect a Pop'n Music cabinet through 2 connectors present in your cab (unplug CN7 and CN8 from the power distribution board and plug them on this shield instead, then splice 12V from CN63 connector and feed it to the shield 12V in connector). You can contact me for more information.

![shield](https://github.com/CrazyRedMachine/UltimatePopnController/blob/master/shield.png?raw=true)

Demo : https://www.instagram.com/p/CKE9HCQFCYM/

# Pinout (Leonardo)

Arduino Leonardo has 5V logic therefore one can directly connect 5V leds to it.

![pinout](https://github.com/CrazyRedMachine/UltimatePopnController/blob/master/pinout_leonardo.png?raw=true)

## Playstation compatibility

Arduino Leonardo version is also compatible with Playstation and Playstation 2 (it can be made to be plugged directly to the controller port, using the following pinout).

![pinout_psx](https://github.com/CrazyRedMachine/UltimatePopnController/blob/master/pinout_leonardo_psx.png?raw=true)

LEDs will be dimmer due to 3.3v power. Using the 7V rumble motor line to Vin instead, and using NPN transistors like 2N2222A on MISO and ACK lines to prevent backfeeding voltage into the console will solve the issue (set INVERT_CIPO and INVERT_ACK to 1 in `ps2.c`).

**BEWARE: DO NOT PLUG USB AND PSX AT THE SAME TIME, THIS CAN DAMAGE YOUR CONSOLE**

# Donation

If this project helps you and you want to give back, you can help me with my future projects.

While not necessary, donations are much appreciated and will only go towards funding future github projects (arcade hardware ain't cheap :( ).

Of course you'll also receive my gratitude and I'll remember you if you post a feature request ;)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=WT735CX4UMZ9U)
