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
		if ( wcsstr( pspdidd->DevicePath, L"vid_1ccf&pid_4148&mi_02" ) == NULL )
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
    DWORD	      uint8_tsRead = 0;
    uint8_t          res[4];
    unsigned char buf[6]; // gamepad report length is 3 uint8_ts in firmware, doubled because NumInputBuffer is set to 2

    buf[0] = 0x04; // gamepad report ID is 4 in firmware
    *pad_bits = 0;

    ReadFile(g_hid_handle, buf, 6, &uint8_tsRead, NULL);
    // uint8_tsRead should either be 6 (if it successfully read 2 reports) or 3 (only one)
    if ( uint8_tsRead != 6 && uint8_tsRead != 3 )
    {
#ifdef DEBUG
        printf("HID read error (got %u uint8_ts)\n",uint8_tsRead);
#endif
        return -1;
    }

    /* HID read ok, convert latest report uint8_ts to pop'n bitfield */
    res[3] = 0;
    res[2] = (buf[uint8_tsRead-1]<<3 | buf[uint8_tsRead-1]) & 0x41;
    res[1] = buf[uint8_tsRead-2];
    res[0] = 0;
    buf[uint8_tsRead-1] >>= 1;
    if ( buf[uint8_tsRead-1]&1 ) res[0] |= 0x80;
    buf[uint8_tsRead-1] >>= 1;
    if ( buf[uint8_tsRead-1]&1 ) res[0] |= 0x40;
    *pad_bits = *(uint32_t *)res;
	
	/* dip switches */
	*pad_bits |= g_dip_state << 24;

#ifdef DEBUG
    for (int i = 0; i<32; i++){
        printf("%c",( ((uint32_t )*pad_bits>>(31-i))&1)?'1':'0');
        if (((i+1)%4)==0) printf(" ");
    }
    printf("\n");
	if (*pad_bits & 0x8000000) printf("31kHz mode\n");
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
    DWORD	 bytesWritten = 0;
    uint8_t    *src = (uint8_t*) &lamp_bits; //cast as uint8_t array for fast conversion
    uint8_t     buf[5]; // HID lights report length is 5 bytes in Arduino firmware

    /* convert bitfield into HID Report format */
    buf[4] = 0;
    buf[3] = (src[1] >> 2);
    buf[2] = (src[1] << 6) | (src[0] << 1) | (src[3] >> 7);
    buf[1] = (src[3] << 1) | (src[2] >> 7);
    buf[0] = 0x05; // HID lights report id is 5 in Arduino firmware

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

#ifdef POPN15_FORMAT
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
        printf("Could not init device.\n");
        return  -1;
    }

    BOOLEAN hidres = HidD_SetNumInputBuffers(
            g_hid_handle,
            2);
    if (!hidres)
    {
        printf("error %d setnuminputbuff\n",GetLastError());
        return -1;
    }

	/* read dip switches */
	uint8_t feat[2] = {0x06, 0x00};
	if (!HidD_GetFeature(g_hid_handle, &feat, 2))
	{
		printf("error %d getFeature\n",GetLastError());
	};
	g_dip_state = feat[1];

#ifdef FORCE_DIP4
    g_dip_state |= 0x08;
#endif
    /* light everything for 1 sec to get visual confirmation
     * the device is working */
    controller_write_leds((uint32_t) 0xFFFFFFFF);
    Sleep(1000);
    controller_write_leds((uint32_t) 0x00000000);

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

#ifdef POPN15_FORMAT
}
#endif


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
