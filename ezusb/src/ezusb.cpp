// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ezusb.h"
extern "C" {
// This file is in the Windows DDK available from Microsoft.
#include <hidsdi.h>
#include <SetupAPI.h>
}
#include <cstdint>
#include <fstream>
#include "ezusb.h"

//#define DEBUG
//#define DEBUG_LAMP

HANDLE   g_hid_handle;
uint8_t  g_dip_state = 0;
// GUID_DEVCLASS_HIDCLASS
static GUID hidclass_guid = {0x745a17a0, 0x74d3, 0x11d0, 0xb6, 0xfe, 0x00, 0xa0, 0xc9, 0x0f, 0x57, 0xda};
    
BOOLEAN get_device_path(wchar_t *lPath)
{

    const GUID *guid = &hidclass_guid;
    HidD_GetHidGuid(&hidclass_guid);
// Get device interface info set handle
// for all devices attached to system
    HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL,  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); // Function class devices.
    if(hDevInfo == INVALID_HANDLE_VALUE)
        return FALSE;

// Retrieve a context structure for a device interface of a device information set.
    BYTE                             buf[1024];
    PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buf;
    SP_DEVICE_INTERFACE_DATA         spdid;
    SP_DEVINFO_DATA                  spdd;
    DWORD                            dwSize;

    spdid.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

// Iterate through all the interfaces and try to match one based on
// the device number.
    DWORD device_index = 0;
    
    for(DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL,guid, i, &spdid); i++)
    {
    // Get the device path.
        dwSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, NULL, 0, &dwSize, NULL);
        if(dwSize == 0 || dwSize > sizeof(buf))
            continue;
        
        pspdidd->cbSize = sizeof(*pspdidd);
        ZeroMemory((PVOID)&spdd, sizeof(spdd));
        spdd.cbSize = sizeof(spdd);
        if(!SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, pspdidd,
                                            dwSize, &dwSize, &spdd))
            continue;
        
        /* check if the device contains our wanted vid/pid */
        if ( wcsstr( pspdidd->DevicePath, L"vid_1ccf&pid_1000&mi_02" ) == NULL )
        {
            continue;
        }
#ifdef DEBUG
        printf("\r\nDevice found at %S\r\n", &pspdidd->DevicePath);
#endif
        //copy devpath into lPath
        wcscpy(lPath, pspdidd->DevicePath);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return TRUE;
     }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return FALSE;
}

/**
 * Initialize the g_hid_handle global variable
 * It will attempt to read paths from devicepath.dat file if it exists until it finds one which works.
 * If nothing is found, as a last resort it will try the two default paths from the Due and Leonardo
 * versions of the firmware.
 *
 * @return 0 on success, -1 on error
 */
static int controller_init(){
    static uint8_t err_count = 0;
    wchar_t lPath[256];
    char path[256];
    FILE *file;

    /* first check if there's a devicepath.dat override */
    file = fopen("devicepath.dat", "r");
    if (file != NULL)
    {
        while ( fgets(path,256,file) != NULL )
        {
            path[strcspn(path, "\r\n")] = 0;
            g_hid_handle = CreateFileA(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if ( g_hid_handle != INVALID_HANDLE_VALUE )
            {
                fclose(file);
                return 0;
            }
        }
        fclose(file);
    }

    /* auto detect device path */
    if (!get_device_path(lPath))
    {
        printf("\r\nPOPN MUSIC USB not detected.\r\n");
        err_count++;
        if (err_count > 2){
            printf("Could not init device after multiple attempts. Exiting.\r\n");
            exit(1);
        }
        return -1;
    }

    g_hid_handle = CreateFile(lPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    
    if ( g_hid_handle == INVALID_HANDLE_VALUE )
    {
        printf("Could not open detected device (err = %x).\r\n", GetLastError());
        return -1;
    }
    return 0;
}

/**
 * Read buttons state from the board and convert them into Pop'n Music format
 *
 * Pop'n Music processes button states as a uint32_t bitfield with the following format :
 * bits 0 (LSB) to 5 : nothing
 * bit 6 : service
 * bit 7 : test
 * bits 8 to 16 : buttons 1 to 9
 * bit 22 : coin mech
 * bits 24 to 27 : dip 1 to 4
 *
 * @param pad_bits[out] button state bitfield
 * @return 0 on success, -1 on error
 */
static int controller_read_buttons(uint32_t *pad_bits){
    DWORD          bytesRead = 0;
    uint8_t       res[4];
    unsigned char buf[6]; // gamepad report length is 3 bytes in firmware, doubled because NumInputBuffer is set to 2

    buf[0] = 0x04; // gamepad report ID is 4 in firmware
    *pad_bits = 0;

    ReadFile(g_hid_handle, buf, 6, &bytesRead, NULL);
    // bytesRead should either be 6 (if it successfully read 2 reports) or 3 (only one)
    if ( bytesRead != 6 && bytesRead != 3 )
    {
#ifdef DEBUG
        printf("HID read error (got %u bytes)\n",bytesRead);
#endif
        return -1;
    }

    /* HID read ok, convert latest report bytes to pop'n bitfield */
    res[3] = g_dip_state;                                      // dip switches
    res[2] = (buf[bytesRead-1]<<3 | buf[bytesRead-1]) & 0x41;  // button 9 (0x01) and coin mech (0x40)
    res[1] = buf[bytesRead-2];                                 // button 1 to 8
    res[0] = 0;
    buf[bytesRead-1] >>= 1;
    if ( buf[bytesRead-1]&1 ) res[0] |= 0x80;                  // test
    buf[bytesRead-1] >>= 1;
    if ( buf[bytesRead-1]&1 ) res[0] |= 0x40;                  // service
    *pad_bits = *(uint32_t *)res;

#ifdef DEBUG
    for (int i = 0; i<32; i++){
        printf("%c",( ((uint32_t )*pad_bits>>(31-i))&1)?'1':'0');
        if (((i+1)%4)==0) printf(" ");
    }
    printf("\n");
    if (*pad_bits & 0x8000000) printf("31kHz mode\r\n");
#endif
    return 0;
}

/**
 * Read lights state and send the corresponding HID report to the Arduino
 *
 * Pop'n Music sends lights state as a int32_t bitfield with the following format :
 * bits 0 (LSB) to 4 : neons from top to bottom
 * bits 8 to 11 : side lamps (left blue, left red, right blue, right red)
 * bits 16 to 19 : coin blocker (0x0 for on, 0xF for off)
 * bits 23 to 31 : button lamps from left to right (note: technically game sets the whole 20-23 to 0xF rather than 0x8 for lamp 1)
 *
 * @param lamp_bits[in] lamp state bitfield
 * @return 0 on success, -1 on error
 */
static int controller_write_leds(int32_t lamp_bits){
    DWORD     bytesWritten = 0;
    uint8_t    *src = (uint8_t*) &lamp_bits; //cast as uint8_t array for fast conversion
    uint8_t     buf[5]; // HID lights report length is 5 bytes in Arduino firmware
    
#ifdef DEBUG_LAMP
    for (int i = 0; i<32; i++){
        printf("%c",( ((uint32_t )lamp_bits>>(31-i))&1)?'1':'0');
        if (((i+1)%4)==0) printf(" ");
    }
    printf("\n");
#endif

    /* convert bitfield into HID Report format */
    buf[4] = 0;                                             // hid report padding
    buf[3] = ((src[1]&0x0F) >> 2);                          // right lamps
    if ( !(src[2] & 0x0F) ) buf[3] |= 0x04;                 // coin blocker
    if ( src[1] & 0xF0 ) buf[3] |= 0x08;                    // coin counter?
    buf[2] = (src[1] << 6) | (src[0] << 1) | (src[3] >> 7); // button 9, top and left lamps
    buf[1] = (src[3] << 1) | (src[2] >> 7);                 // button lamps 1-8
    buf[0] = 0x05;                                          // HID lights report id

    /* send HID Report */
    WriteFile(g_hid_handle, buf, 5, &bytesWritten, NULL);
    if ( bytesWritten == 5 )
        return 0;

#ifdef DEBUG_LAMP
    printf("Error sending HID report (%u bytes written)\r\n",bytesWritten);
#endif
    return -1;
}


/* EZUSB EXPORTS */

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
    /* This memset is what is being done by the usual hooks.
     * However, it breaks Pop'n 15~18 compatibility and seems
     * to have no effect on later Pop'n so I removed it... */
    //memset(a1, 0, 40);
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
        printf("Could not init device.\r\n");
        return  -1;
    }

    BOOLEAN hidres = HidD_SetNumInputBuffers(
            g_hid_handle,
            2);
    if (!hidres)
    {
        printf("Error %d setnuminputbuff\r\n",GetLastError());
        return -1;
    }

    /* read dip switches */
    uint8_t feat[2] = {0x06, 0x00};
    if (!HidD_GetFeature(g_hid_handle, &feat, 2))
    {
        printf("Cannot read dipswitches (error %d)\r\n",GetLastError());
    };
    g_dip_state = feat[1];

#ifdef FORCE_DIP4
    g_dip_state |= 0x08;
#endif
    /* light up each part of the cab in a wave pattern to get visual confirmation the device is working */
    for (int i = 0; i<=31; i++){
        if ((i > 4 && i < 8) || (i > 11 && i < 23)) { // unused bits
            continue;
        }
        controller_write_leds((uint32_t) 1 << i);
        Sleep(300);
    }

#ifdef DEBUG
    printf("Managed to open device %x\n",g_hid_handle);
    printf("Dip state is %x\n", g_dip_state);
#endif
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

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  fdwReason,
                       LPVOID lpReserved
                     ) {
    if (fdwReason == 1) {
        //https://blogs.msdn.microsoft.com/larryosterman/2004/06/03/little-known-win32-apis-disablethreadlibrarycalls/
        DisableThreadLibraryCalls(hModule);
    }

    return 1;
}

