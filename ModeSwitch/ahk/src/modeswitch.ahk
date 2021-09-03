#SingleInstance Force
#Persistent
#include AHKHID.ahk

MsgBox Sorry, this script is currently broken (I now need to get it to send a proper feature report instead of an output report)
ExitApp

hArd := HID_Open("1ccf","1000")
while (hLED = -1) {
 hArd := HID_Open("1ccf","1000")
 Sleep, 5000
}
if 0 < 1  ; The left side of a non-expression if-statement is always the name of a variable.
{
	MsgBox This script requires a parameter
    ExitApp
}

param := %A_Index%
mode := %param% + 16
if (mode > 21) or (mode < 16){
 MsgBox, Incorrect value %mode%
 ExitApp
}
changeMode(hArd,mode)
HID_Close(hArd)
ExitApp

changeMode(hArd,Num){
;MsgBox, sending %Num%
TestString = "aa"
NumPut(5,TestString,0,"UInt")
NumPut(0,TestString,1, "UInt")
NumPut(0,TestString,2, "UInt")
NumPut(0,TestString,3, "UInt")
NumPut(Num,TestString,4, "UInt")
;outputdebug  teststring %TestString%
HID_Write(hArd,TestString,5)
}

;DLL CALLS
HID_Open(VID,PID){
str = VID_%VID%`&PID_%PID%
iCount := AHKHID_GetDevCount()
Loop , %icount% 
{
	HID0 += 1
	name := AHKHID_GetDevName(HID0)
	type :=	AHKHID_GetDevType(HID0)
	;outputdebug str to find is %str%
	if type = 2
	{
		if name contains %str%
		{
			;outputdebug found %VID%:%PID% under name: %name%
			return HandleFromName(name)
		}
	}
}
		;outputdebug (%VID%:%PID% not found)
		return -1
}

HandleFromName(FileName){
GENERIC_READ = 0x80000000  ; Open the file for reading rather than writing.
GENERIC_WRITE = 0x40000000
OPEN_EXISTING = 3  ; This mode indicates that the file to be opened must already exist.
FILE_SHARE_READ = 0x1 ; This and the next are whether other processes can open the file while we have it open.
FILE_SHARE_WRITE = 0x2
;outputdebug Calling Createfile on %FileName%
hFile := DllCall("CreateFile", Str, FileName, UInt, GENERIC_READ|GENERIC_WRITE, UInt, FILE_SHARE_READ|FILE_SHARE_WRITE, Ptr, 0, UInt, OPEN_EXISTING, UInt, 0, Ptr, 0)
If (hFile = -1) Or ErrorLevel 
{
    ErrorLevel = CreateFile call failed.`nReturn value: %r%`nErrorLevel: %ErrorLevel%`nLine: %A_LineNumber%`nLast Error: %A_LastError%
		outputdebug (%A_LineNumber%) %errorlevel%
        Return -1
}
;outputdebug file handle %hFile%
return hFile
}

HID_Write(handle,data,len=-1){
BytesToWrite := (len>0) ? len : VarSetCapacity(data, -1)
;outputdebug length %BytesToWrite%
r := DllCall("WriteFile", Ptr, handle, Str, data, UInt, BytesToWrite, UIntP, BytesActuallyWritten, Ptr, 0)
If (r = -1) Or ErrorLevel 
{
    ErrorLevel = WriteFile call failed.`nReturn value: %r%`nErrorLevel: %ErrorLevel%`nLine: %A_LineNumber%`nLast Error: %A_LastError%
		outputdebug (%A_LineNumber%) %errorlevel%
        Return -1
}
;outputdebug writefile %r%
return BytesActuallyWritten
}

HID_Close(handle){
DllCall("CloseHandle", Ptr, handle)  ; Close the file.
}
