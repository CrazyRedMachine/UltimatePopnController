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

/* HID DESCRIPTOR */
static const byte PROGMEM _hidReportPOPN[] = {
    0x05, 0x01,                    /* USAGE_PAGE (Generic Desktop) */ 
    0x09, 0x05,                    /* USAGE (Game Pad) */ 
    0xa1, 0x01,                    /* COLLECTION (Application) */ 

    /*Buttons */ 
    0x85, 0x04,                    /*   REPORT_ID 4 */
    0x05, 0x09,                    /*     USAGE_PAGE (Button) */ 
    0x19, 0x01,                    /*     USAGE_MINIMUM (Button 1) */ 
    0x29, 0x0c,                    /*     USAGE_MAXIMUM (Button 12)*/ 
    0x15, 0x00,                    /*     LOGICAL_MINIMUM (0) */ 
    0x25, 0x01,                    /*     LOGICAL_MAXIMUM (1) */
#if defined(ARDUINO_ARCH_SAM) 
    0x95, 0x0c,                    /*     REPORT_COUNT (12) */ 
#else
    0x95, 0x0b,                    /*     REPORT_COUNT (11) */ 
#endif
    0x75, 0x01,                    /*     REPORT_SIZE (1) */ 
    0x81, 0x02,                    /*     INPUT (Data,Var,Abs) */ 
    /* Reserved bits */ 
    0x95, 0x01,                      /*   REPORT_COUNT (1) */ 
#if defined(ARDUINO_ARCH_SAM)
    0x75, 0x04,                      /*   REPORT_SIZE (4) */ 
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
    0x75, 0x01,                    /*       REPORT_SIZE (1) */ 
    0x95, 0x01,                    /*       REPORT_COUNT (1) */ 
    0x91, 0x02,                    /*       OUTPUT (Data,Var,Abs) */ 
    0xc0,                          /*     END_COLLECTION */ 
    /*  Reserved 14 bits */ 
    0x95, 0x01,            /*   REPORT_COUNT (1) */ 
    0x75, 0x0E,            /*   REPORT_SIZE (14) */ 
    0x91, 0x03,            /*   OUTPUT (Cnst,Var,Abs) */ 
#else
    /*  Reserved 23 bits */ 
    0x95, 0x01,            /*   REPORT_COUNT (1) */ 
    0x75, 0x17,            /*   REPORT_SIZE (23) */ 
    0x91, 0x03,            /*   OUTPUT (Cnst,Var,Abs) */ 
#endif
    /*Footer */ 
    0xc0                          /* END_COLLECTION */ 
};

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
        D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01) // this last parameter is the bInterval (requested polling rate)
      };
      return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
    }
    
    int POPNHID_::getDescriptor(USBSetup& setup)
    {
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
        return true;
      }
      
      if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
        if (request == HID_SET_REPORT) {
          if(setup.wValueH == HID_REPORT_TYPE_OUTPUT && setup.wLength == 5){
            USB_RecvControl(led_data, 5);
            return true;
          }
        }
      }

      return false;
    }
    
    uint8_t POPNHID_::getShortName(char *name)
    {
      name[0] = 'P';
      name[1] = 'O';
      name[2] = 'P';
      name[3] = 'N';
      return 4;
    }

/* CUSTOM POPN FUNCTIONS */
    
    uint8_t POPNHID_::getLightMode(){
      return lightMode;
    }

    void POPNHID_::setLightMode(uint8_t mode){
      if ((mode > 3) || (mode < 0)) {
        lightMode = 2;
        return;
      }
      lightMode = mode;
    }
    
    void POPNHID_::updateLightMode(){
      uint32_t* bitfield = (uint32_t*)&(led_data[1]);
      if (*bitfield>>28&1){
        uint8_t mode = (*bitfield>>24) & 0x0F;
        setLightMode(mode); 
        *bitfield &= ~((uint32_t)0xFF<<24);
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
      for(int i = 9; i < 18; i++) {
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
