/* TO BE COMPILED WITH VISUAL STUDIO SO THAT EXPORTS ARE PROPERLY MANGLED */
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
extern "C" {
// This file is in the Windows DDK available from Microsoft.
#include <hidsdi.h>
}
#include <cstdint>
#include <fstream>
#include "ezusb.h"

HANDLE   g_hid_handle;

/**
 * Initialize the g_hid_handle global variable
 * It will attempt to read paths from devicepath.dat file if it exists until it finds one which works.
 * If nothing is found, as a last resort it will try the two default paths from the Due and Leonardo
 * versions of the firmware.
 *
 * @return 0 on success, -1 on error
 */
static int controller_init(){
    FILE *file;
    char  path[256];

    file = fopen("devicepath.dat", "r");
    if (file == NULL)
    {
        goto last_resort;
    }

    while ( fgets(path,256,file) != NULL )
    {
        path[strcspn(path, "\r\n")] = 0;
        g_hid_handle = CreateFile(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if ( g_hid_handle != INVALID_HANDLE_VALUE )
            break;
    }
    fclose(file);

    if ( g_hid_handle == INVALID_HANDLE_VALUE )
    {
        goto last_resort;
    }
    return 0;

last_resort:
    g_hid_handle = CreateFile("\\\\?\\HID#VID_2341&PID_003E&MI_02#7&3156e204&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}",
                              GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if ( g_hid_handle != INVALID_HANDLE_VALUE )
        return 0;

    g_hid_handle = CreateFile("\\\\?\\HID#VID_2341&PID_8036&MI_02#7&63200bf&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}",
                              GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if ( g_hid_handle != INVALID_HANDLE_VALUE )
        return 0;

    printf("Couldn't open device. Make sure devicepath.dat exists and contains the correct path.\r\n");
    return -1;
}

/**
 * Read buttons state from the Arduino and convert them into Pop'n Music format
 *
 * Pop'n Music processes button states as a uint32_t bitfield with the following format :
 * bits 0 (LSB) to 5 : nothing
 * bit 6 : service
 * bit 7 : test
 * bits 8 to 16 : buttons 1 to 9
 * bit 22 : coin mech
 *
 * @param pad_bits[out] button state bitfield
 * @return 0 on success, -1 on error
 */
static int controller_read_buttons(uint32_t *pad_bits){
    DWORD	      BytesRead = 0;
    byte          res[4];
    unsigned char buf[6]; // gamepad report length is 3 bytes in Arduino firmware, doubled because NumInputBuffer is set to 2

    buf[0] = 0x04; // gamepad report ID is 4 in Arduino firmware
    *pad_bits = 0;

    ReadFile(g_hid_handle, buf, 6, &BytesRead, NULL);
    // BytesRead should either be 6 (if it successfully read 2 reports) or 3 (only one)
    if ( BytesRead != 6 && BytesRead != 3 )
    {
        return -1;
    }

    /* HID read ok, convert latest report bytes to pop'n bitfield */
    res[3] = 0;
    res[2] = (buf[BytesRead-1]<<3 | buf[BytesRead-1]) & 0x41;
    res[1] = buf[BytesRead-2];
    res[0] = 0;
    buf[BytesRead-1] >>= 1;
    if ( buf[BytesRead-1]&1 ) res[0] |= 0x80;
    buf[BytesRead-1] >>= 1;
    if ( buf[BytesRead-1]&1 ) res[0] |= 0x40;
    *pad_bits = *(uint32_t *)res;
#ifdef DEBUG
    for (int i = 0; i<32; i++){
        printf("%c",( ((uint32_t )*pad_bits>>(31-i))&1)?'1':'0');
        if (((i+1)%4)==0) printf(" ");
    }
    printf("\n");
#endif
    return 0;
}

/**
 * Read lights state and send the corresponding HID report to the Arduino
 *
 * Pop'n Music sends lights state as a int32_t bitfield with the following format :
 * bits 0 (LSB) to 4 : neons from top to bottom
 * bits 8 to 11 : side lamps (left blue, left red, right blue, right red)
 * bits 23 to 31 : button lamps from left to right
 *
 * @param lamp_bits[in] lamp state bitfield
 * @return 0 on success, -1 on error
 */
static int controller_write_leds(int32_t lamp_bits){
    DWORD	 BytesWritten = 0;
    byte    *src = (byte*) &lamp_bits; //cast as byte array for fast conversion
    byte     buf[5]; // HID lights report length is 5 bytes in Arduino firmware

    /* convert bitfield into HID Report format */
    buf[4] = 0;
    buf[3] = (src[1] >> 2);
    buf[2] = (src[1] << 6) | (src[0] << 1) | (src[3] >> 7);
    buf[1] = (src[3] << 1) | (src[2] >> 7);
    buf[0] = 0x05; // HID lights report id is 5 in Arduino firmware

    /* send HID Report */
    WriteFile(g_hid_handle, buf, 5, &BytesWritten, NULL);
    if ( BytesWritten == 5 )
        return 0;

#ifdef DEBUG
    printf("Error sending HID report (%u bytes written)\r\n",BytesWritten);
#endif
    return -1;
}


/* EZUSB EXPORTS */

#if POPN15_FORMAT == 1
extern "C" {
#endif

__declspec(dllexport) int __cdecl usbCheckAlive() {
    return 1;
}

__declspec(dllexport) int __cdecl usbCheckSecurityNew(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbCoinGet(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbCoinMode(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbEnd() {
    CloseHandle(g_hid_handle);
    return 0;
}

__declspec(dllexport) int __cdecl usbFirmResult() {
    return 0;
}

__declspec(dllexport) int __cdecl usbGetKEYID(unsigned char *a, int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbGetSecurity(int i, unsigned char *a) {
    return 0;
}

__declspec(dllexport) int __cdecl usbLamp(int32_t lamp_bits) {
    return controller_write_leds(lamp_bits);
}

__declspec(dllexport) int __cdecl usbPadRead(unsigned long *pad_bits) {
    return controller_read_buttons((uint32_t *)pad_bits);
}

__declspec(dllexport) int __cdecl usbPadReadLast(uint8_t *a1) {
    memset(a1, 0, 4);
    return 0;
}

__declspec(dllexport) int __cdecl usbSecurityInit() {
    return 0;
}

__declspec(dllexport) int __cdecl usbSecurityInitDone() {
    return 0;
}

__declspec(dllexport) int __cdecl usbSecuritySearch() {
    return 0;
}

__declspec(dllexport) int __cdecl usbSecuritySearchDone() {
    return 0;
}

__declspec(dllexport) int __cdecl usbSecuritySelect(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbSecuritySelectDone() {
    return 0;
}

__declspec(dllexport) int __cdecl usbSetExtIo(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbStart(int i) {
    if (controller_init() == -1)
    {
        printf("Could not init device.\n");
        return  -1;
    }

    bool hidres = HidD_SetNumInputBuffers(
            g_hid_handle,
            2);
    if (!hidres)
    {
        printf("error %d setnuminputbuff\n",GetLastError());
        return -1;
    }

    /* light everything for 1 sec to get visual confirmation
     * the device is working */
    controller_write_leds((uint32_t) 0xFFFFFFFF);
    Sleep(1000);
    controller_write_leds((uint32_t) 0x00000000);

    return 0;
}

__declspec(dllexport) int __cdecl usbWdtReset() {
    return 0;
}

__declspec(dllexport) int __cdecl usbWdtStart(int i) {
    return 0;
}

__declspec(dllexport) int __cdecl usbWdtStartDone() {
    return 0;
}

#if POPN15_FORMAT == 1
}
#endif

int __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, void *) {
    if (fdwReason == 1) {
        //https://blogs.msdn.microsoft.com/larryosterman/2004/06/03/little-known-win32-apis-disablethreadlibrarycalls/
        DisableThreadLibraryCalls(hinstDLL);
    }

    return 1;
}