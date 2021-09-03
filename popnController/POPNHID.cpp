/* Arduino-HID-Lighting-Library
 * 
 * This Arduino-HID-Lighting-Library is derived from Arduino-HID-Lighting, whose copyriht owner is mon.
 * More information about Arduino-HID-Lighting you can find under:
 * 
 * mon's Arduino-HID-Lighting
 * https://github.com/mon/Arduino-HID-Lighting
 * 
 * 2018 (C) Arduino-HID-Lighting-Library, Knuckleslee
*/
#include "POPNHID.h"

byte extern LightPins[];
byte extern DipPins[];

/* HID DESCRIPTOR */
static const byte PROGMEM _hidReportPOPN[] = {
    0x05, 0x01,                    /* USAGE_PAGE (Generic Desktop) */ 
    0x09, 0x05,                    /* USAGE (Game Pad) */ 
    0xa1, 0x01,                    /* COLLECTION (Application) */ 

    /*Buttons */ 
    0x85, 0x04,                    /*   REPORT_ID 4 */
    0x05, 0x09,                    /*     USAGE_PAGE (Button) */ 
    0x19, 0x01,                    /*     USAGE_MINIMUM (Button 1) */ 
    0x29, 0x0d,                    /*     USAGE_MAXIMUM (Button 13)*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */
#if defined(ARDUINO_ARCH_SAM) 
    0x95, 0x0d,                    /*     REPORT_COUNT (13) */ 
#else
    0x95, 0x0b,                    /*     REPORT_COUNT (11) */ 
#endif
    0x75, 0x01,                    /*     REPORT_SIZE (1) */ 
    0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */ 
    /* Reserved bits */ 
    0x95, 0x01,                      /*   REPORT_COUNT (1) */ 
#if defined(ARDUINO_ARCH_SAM)
    0x75, 0x03,                      /*   REPORT_SIZE (3) */ 
#else
    0x75, 0x05,                      /*   REPORT_SIZE (5) */ 
#endif
    0x81, 0x03,                      /*   INPUT (Cnst,Var,Abs) */
    
    /*Lights */
    0x85, 0x05,                    /*   REPORT_ID 5*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */ 
    /*Led 1 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x01,                    /*     USAGE (Instance 1) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x04,                    /*       STRING INDEX (4) */
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 2 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x02,                    /*     USAGE (Instance 2) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x05,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 3 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x03,                    /*     USAGE (Instance 3) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x06,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 4 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x04,                    /*     USAGE (Instance 4) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x07,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 5 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x05,                    /*     USAGE (Instance 5) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x08,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 6 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x06,                    /*     USAGE (Instance 6) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x09,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 7 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x07,                    /*     USAGE (Instance 7) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0a,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 8 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x08,                    /*     USAGE (Instance 8) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0b,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 9 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x09,                    /*     USAGE (Instance 9) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0c,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
#if defined(ARDUINO_ARCH_SAM)
    /*Led 10 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0a,                    /*     USAGE (Instance 10) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0d,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 11 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0b,                    /*     USAGE (Instance 11) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0e,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 12 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0c,                    /*     USAGE (Instance 12) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x0f,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 13 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0d,                    /*     USAGE (Instance 13) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */ 
    0x79, 0x10,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 14 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0e,                    /*     USAGE (Instance 14) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x11,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 15 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x0f,                    /*     USAGE (Instance 15) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x12,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 16 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x10,                    /*     USAGE (Instance 16) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x13,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 17 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x11,                    /*     USAGE (Instance 17) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x14,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*Led 18 */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x12,                    /*     USAGE (Instance 18) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x08,                    /*       USAGE_PAGE (LEDs) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x15,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /* Coin blocker */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x13,                    /*     USAGE (Instance 19) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x0a,                    /*       USAGE_PAGE (Ordinals) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x16,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /* Coin counter */ 
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x09, 0x14,                    /*     USAGE (Instance 20) */ 
    0xa1, 0x02,                    /*     COLLECTION (Logical) */ 
    0x05, 0x0a,                    /*       USAGE_PAGE (Ordinals) */ 
    0x09, 0x4b,                    /*       USAGE (Generic Indicator 1) */  
    0x79, 0x17,
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*  Reserved 12 bits */ 
    0x95, 0x01,            /*   REPORT_COUNT (1) */ 
    0x75, 0x0C,            /*   REPORT_SIZE (12) */ 
    0x91, 0x03,            /*   OUTPUT (Cnst,Var,Abs) */ 
#else
    /*  Reserved 23 bits */ 
    0x95, 0x01,            /*   REPORT_COUNT (1) */ 
    0x75, 0x17,            /*   REPORT_SIZE (23) */ 
    0x91, 0x03,            /*   OUTPUT (Cnst,Var,Abs) */ 
#endif

#if defined(ARDUINO_ARCH_SAM)
    /*Dip switches*/ 
    0x85, 0x06,                    /*   REPORT_ID 6 */
    0x05, 0x09,                    /*     USAGE_PAGE (Button) */ 
    0x19, 0x01,                    /*     USAGE_MINIMUM (Button 1) */ 
    0x29, 0x04,                    /*     USAGE_MAXIMUM (Button 4)*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */
    0x95, 0x04,                    /*     REPORT_COUNT (4) */ 
    0x75, 0x01,                    /*     REPORT_SIZE (1) */ 
    0xb1, 0x02,                     /*    FEATURE (Data,Var,Abs) */ 
    /* Reserved bits */ 
    0x95, 0x01,                      /*   REPORT_COUNT (1) */ 
    0x75, 0x04,                      /*   REPORT_SIZE (4) */ 
    0xb1, 0x03,                      /*   FEATURE (Cnst,Var,Abs) */
#endif

    /*Light mode switch*/ 
    0x85, 0x07,                    /*   REPORT_ID 7 */
    0x05, 0x0a,                    /*     USAGE_PAGE (Ordinals) */ 
    0x19, 0x00,                    /*     USAGE_MINIMUM (Button 1) */ 
    0x29, 0x04,                    /*     USAGE_MAXIMUM (Button 4)*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x04,                    /*     LOGICAL_MAXIMUM (4) */
    0x95, 0x01,                    /*     REPORT_COUNT (1) */ 
    0x75, 0x04,                    /*     REPORT_SIZE (4) */ 
    0xb1, 0x02,                     /*    FEATURE (Data,Var,Abs) */ 
    /* Reserved bits */ 
    0x95, 0x01,                      /*   REPORT_COUNT (1) */ 
    0x75, 0x04,                      /*   REPORT_SIZE (4) */ 
    0xb1, 0x03,                      /*   FEATURE (Cnst,Var,Abs) */
    
    /*Footer */ 
    0xc0                          /* END_COLLECTION */ 
};

static bool SendControl(uint8_t d)
{
  return USB_SendControl(0, &d, 1) == 1;
}

static bool USB_SendStringDescriptor(const char *string_P, uint8_t string_len, uint8_t flags) {
        SendControl(2 + string_len * 2);
        SendControl(3);
        bool pgm = flags & TRANSFER_PGM;
        for(uint8_t i = 0; i < string_len; i++) {
                bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
                r &= SendControl(0); // high byte
                if(!r) {
                        return false;
                }
        }
        return true;
}

/* HID Light descriptor strings */
const char* const PROGMEM String_Manufacturer = "CrazyRedMachine";
const char* const PROGMEM String_Product = "Pop'n controller";
const char* const PROGMEM String_Serial = "POPNHID";

const char* const PROGMEM LEDString_00 = "Button 1";
const char* const PROGMEM LEDString_01 = "Button 2";
const char* const PROGMEM LEDString_02 = "Button 3";
const char* const PROGMEM LEDString_03 = "Button 4";
const char* const PROGMEM LEDString_04 = "Button 5";
const char* const PROGMEM LEDString_05 = "Button 6";
const char* const PROGMEM LEDString_06 = "Button 7";
const char* const PROGMEM LEDString_07 = "Button 8";
const char* const PROGMEM LEDString_08 = "Button 9";
const char* const PROGMEM LEDString_09 = "Hi Lamp 1";
const char* const PROGMEM LEDString_10 = "Hi Lamp 2";
const char* const PROGMEM LEDString_11 = "Hi Lamp 3";
const char* const PROGMEM LEDString_12 = "Hi Lamp 4";
const char* const PROGMEM LEDString_13 = "Hi Lamp 5";
const char* const PROGMEM LEDString_14 = "Left Lamp 1";
const char* const PROGMEM LEDString_15 = "Left Lamp 2";
const char* const PROGMEM LEDString_16 = "Right Lamp 1";
const char* const PROGMEM LEDString_17 = "Right Lamp 2";
const char* const PROGMEM BlockerString = "Coin blocker";
const char* const PROGMEM CounterString = "Coin counter";

const char* LEDString_indiv[] = {LEDString_00,LEDString_01,LEDString_02,LEDString_03,LEDString_04,LEDString_05,LEDString_06,LEDString_07,LEDString_08,LEDString_09,LEDString_10,LEDString_11,LEDString_12,LEDString_13,LEDString_14,LEDString_15,LEDString_16,LEDString_17,BlockerString,CounterString};

#if defined(ARDUINO_ARCH_SAM)
uint8_t STRING_ID_LED_Count = 20;
#else
uint8_t STRING_ID_LED_Count = 9;
#endif

const DeviceDescriptor PROGMEM USB_DeviceDescriptorIAD =
  D_DEVICE(0xEF,0x02,0x01,64,0x1ccf,0x1000,0x100,IMANUFACTURER,IPRODUCT,ISERIAL,1);
 
/* PluggableUSBModule IMPLEMENTATION */

    POPNHID_::POPNHID_(void) : PluggableUSBModule(1, 1, epType) {
      epType[0] = EP_TYPE_INTERRUPT_IN;
      PluggableUSB().plug(this);
    }

    int POPNHID_::getInterface(byte* interfaceCount) {
      *interfaceCount += 1; // uses 1
      HIDDescriptor hidInterface = {
        D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
        D_HIDREPORT(sizeof(_hidReportPOPN)),
        D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, USB_EP_BINTERVAL)
      };
      return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
    }
    
    int POPNHID_::getDescriptor(USBSetup& setup)
    {
      if(setup.wValueH == USB_DEVICE_DESCRIPTOR_TYPE) {
        return USB_SendControl(TRANSFER_PGM, (const uint8_t*)&USB_DeviceDescriptorIAD, sizeof(USB_DeviceDescriptorIAD));
      }
      if(setup.wValueH == USB_STRING_DESCRIPTOR_TYPE) { 
        if (setup.wValueL == IPRODUCT) {
            return USB_SendStringDescriptor(String_Product, strlen(String_Product), 0);
        }
        else if (setup.wValueL == IMANUFACTURER) {
            return USB_SendStringDescriptor(String_Manufacturer, strlen(String_Manufacturer), 0);
        }
        else if (setup.wValueL == ISERIAL) {
            return USB_SendStringDescriptor(String_Serial, strlen(String_Serial), 0);
        }
        else if(setup.wValueL >= STRING_ID_LED_Base && setup.wValueL < (STRING_ID_LED_Base + STRING_ID_LED_Count)) {
          return USB_SendStringDescriptor(LEDString_indiv[setup.wValueL - STRING_ID_LED_Base], strlen(LEDString_indiv[setup.wValueL - STRING_ID_LED_Base]), 0);
        }                       
      }
      // Check if this is a HID Class Descriptor request
      if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
      if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) { return 0; }

      // In a HID Class Descriptor wIndex contains the interface number
      if (setup.wIndex != pluggedInterface) { return 0; }

      return USB_SendControl(TRANSFER_PGM, _hidReportPOPN, sizeof(_hidReportPOPN));
    }
    
    bool POPNHID_::setup(USBSetup& setup)
    {
      if (pluggedInterface != setup.wIndex) {
        return false;
      }

      byte request = setup.bRequest;
      byte requestType = setup.bmRequestType;

      if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
      {
#if defined(ARDUINO_ARCH_SAM)
/* dip switches */
        if (setup.wValueH == HID_REPORT_TYPE_FEATURE && setup.wValueL == 6) {
          uint8_t dip_data[2] = {6,0x00};
          for(int i=0; i<4; i++)
          {
            if (digitalRead(DipPins[i]) == LOW)
            {
              dip_data[1] |= 1<<i;
            }
          }
          USB_SendControl(0, &dip_data, 2);
          return true;
        }
#endif
/* lightmode info */
        if (setup.wValueH == HID_REPORT_TYPE_FEATURE && setup.wValueL == 7) {
          uint8_t lm_data[2] = {7,0x00};
          lm_data[1] = lightMode;
          USB_SendControl(0, &lm_data, 2);
          return true;
        }
        
        return true;
      }
      
      if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_REPORT) {
          if(setup.wValueH == HID_REPORT_TYPE_OUTPUT && setup.wLength == 5){
            lastHidUpdate = millis();
            USB_RecvControl(led_data, 5);
            return true;
          }
        else if (setup.wValueH == HID_REPORT_TYPE_FEATURE ) {
/* lightmode switch */
          byte usb_data[5];
          USB_RecvControl(usb_data, setup.wLength);
          if (usb_data[0] == 7)
          {
            mode_data = usb_data[1];
            return true;
          }
        }
        
        }
      }

      return false;
    }
    
/* CUSTOM POPN FUNCTIONS */
    
    uint8_t POPNHID_::getLightMode(){
      return lightMode;
    }
    
    unsigned long POPNHID_::getLastHidUpdate(){
      return lastHidUpdate;
    }
    
    uint8_t POPNHID_::setLightMode(uint8_t mode){
      uint8_t effective_mode = mode;
      if ((effective_mode > 4)) {
        effective_mode = 2;
      }
      lightMode = effective_mode;
      mode_data = lightMode;
      return lightMode;
    }
    
    void POPNHID_::updateLightMode(){
        uint8_t mode = mode_data;
        if (mode != lightMode) {
          mode_data = setLightMode(mode);
        }
    }
    
    void POPNHID_::updateLeds(uint32_t buttonsState, bool invert){
      uint32_t* bitfield = (uint32_t*)&(led_data[1]);
      uint32_t leds = (*bitfield|buttonsState);
      if (invert)
        leds = ~leds;
      for(int i = 0; i < 9; i++) {
        if (leds>>i&1)
          digitalWrite(LightPins[i],HIGH);
        else
          digitalWrite(LightPins[i],LOW);
      }
#if defined(ARDUINO_ARCH_SAM)
      for(int i = 9; i < 20; i++) {
        if (leds>>i&1)
          digitalWrite(LightPins[i],HIGH);
        else
          digitalWrite(LightPins[i],LOW);
      }
#endif
      
    }

    int POPNHID_::sendState(uint32_t buttonsState){
      uint8_t data[3];
      data[0] = (uint8_t) 4; //report id
      data[1] = (uint8_t) (buttonsState & 0xFF);
      data[2] = (uint8_t) (buttonsState >> 8) & 0xFF;
      return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, 3);
    }
