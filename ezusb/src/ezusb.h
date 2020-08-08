#ifndef EZUSB_UPC_EZUSB_H
#define EZUSB_UPC_EZUSB_H

/* Pop'n Music Adventure ezusb.dll was compiled in C and thus used unmangled function names
 * set this POPN15_FORMAT to 1 to compile a compatible version */
#define POPN15_FORMAT 0

#if POPN15_FORMAT == 1
extern "C" {
#endif
__declspec(dllexport) int __cdecl usbCheckAlive();
__declspec(dllexport) int __cdecl usbCheckSecurityNew(int i);
__declspec(dllexport) int __cdecl usbCoinGet(int i);
__declspec(dllexport) int __cdecl usbCoinMode(int i);
__declspec(dllexport) int __cdecl usbEnd();
__declspec(dllexport) int __cdecl usbFirmResult();
__declspec(dllexport) int __cdecl usbGetKEYID(unsigned char *a, int i);
__declspec(dllexport) int __cdecl usbGetSecurity(int i, unsigned char *a);
__declspec(dllexport) int __cdecl usbLamp(int32_t lamp_bits);
__declspec(dllexport) int __cdecl usbPadRead(unsigned long *pad_bits);
__declspec(dllexport) int __cdecl usbPadReadLast(uint8_t *a1);
__declspec(dllexport) int __cdecl usbSecurityInit();
__declspec(dllexport) int __cdecl usbSecurityInitDone();
__declspec(dllexport) int __cdecl usbSecuritySearch();
__declspec(dllexport) int __cdecl usbSecuritySearchDone();
__declspec(dllexport) int __cdecl usbSecuritySelect(int i);
__declspec(dllexport) int __cdecl usbSecuritySelectDone();
__declspec(dllexport) int __cdecl usbSetExtIo(int i);
__declspec(dllexport) int __cdecl usbStart(int i);
__declspec(dllexport) int __cdecl usbWdtReset();
__declspec(dllexport) int __cdecl usbWdtStart(int i);
__declspec(dllexport) int __cdecl usbWdtStartDone();
#if POPN15_FORMAT == 1
}
#endif

#endif //EZUSB_UPC_EZUSB_H