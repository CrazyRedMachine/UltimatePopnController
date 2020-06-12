/* TO BE COMPILED WITH MINGW */
/* Visual C++ compilation didn't work for me on the BemaniPC (invalid win32 application) */
#include <windows.h>
#include <fstream>

int main(int argc, char* argv[])
{
    FILE    *file;
    char     path[256];
    HANDLE   hidHandle;
    char     OutputReport[5];
    int      res;
    int      mode;
    DWORD	 BytesWritten = 0;

    if ( argc < 2 )
    {
        printf("Usage: %s mode\r\n",argv[0]);
        return 1;
    }

    mode = atoi(argv[1]);
    if ( mode < 0 || mode > 3 )
    {
        printf("Invalid mode value %d\r\n", mode);
        return 2;
    }

    file = fopen("devicepath.dat", "r");
    while ( fgets(path,256,file) != NULL )
    {
        hidHandle = CreateFile(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if ( hidHandle != INVALID_HANDLE_VALUE )
            break;
    }
    fclose(file);

    if ( hidHandle == INVALID_HANDLE_VALUE )
    {
        printf("Couldn't open device. Make sure devicepath.dat contains the correct path.\r\n");
        return 3;
    }

    OutputReport[0] = 0x05;     //report ID
    OutputReport[1] = 0x00;
    OutputReport[2] = 0x00;
    OutputReport[3] = 0x00;
    OutputReport[4] = mode+16;  //mode+magicbit

    WriteFile(hidHandle, OutputReport, 5, &BytesWritten, NULL);
    if ( BytesWritten == 5 )
    {
        res = 0;
        printf("Succesfully switched to mode %u ", mode);
        switch(mode)
        {
            case 0:
                printf("(reactive)\r\n");
                break;
            case 1:
                printf("(HID)\r\n");
                break;
            case 2:
                printf("(mixed)\r\n");
                break;
            case 3:
                printf("(invert)\r\n");
        }
    }
    else
    {
        printf("Error sending HID report (%u bytes written)\r\n",BytesWritten);
        res = 4;
    }

    CloseHandle(hidHandle);
    return res;
}
