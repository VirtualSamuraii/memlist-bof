#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE   WINAPI KERNEL32$CreateToolhelp32Snapshot(DWORD dwFlags,DWORD th32ProcessID);
DECLSPEC_IMPORT WINBASEAPI DWORD    WINAPI KERNEL32$GetLastError (VOID);
DECLSPEC_IMPORT WINBOOL  WINAPI KERNEL32$Module32First(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
DECLSPEC_IMPORT WINBOOL  WINAPI KERNEL32$Module32Next(HANDLE hSnapshot,LPMODULEENTRY32 lpme);
DECLSPEC_IMPORT WINBASEAPI WINBOOL  WINAPI KERNEL32$CloseHandle (HANDLE hObject);

void ListModules(DWORD processID)
{
    HANDLE hSnapshot, hModule;
    MODULEENTRY32 moduleEntry;
    formatp buffer;

    hSnapshot = KERNEL32$CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        BeaconPrintf(CALLBACK_ERROR, "Process Snapshot Failed : %d", KERNEL32$GetLastError());
        return;
    }

    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    BeaconFormatAlloc(&buffer, 4096);

    BeaconFormatPrintf(&buffer, "[*] Listing modules in memory\n");

    if (KERNEL32$Module32First(hSnapshot, &moduleEntry))
    {
        do
        {
            BeaconFormatPrintf(&buffer, "[*] %s starting at 0x%llX\n", (char*)moduleEntry.szModule, (DWORD64)moduleEntry.modBaseAddr);       
        } while (KERNEL32$Module32Next(hSnapshot, &moduleEntry));
    }

    KERNEL32$CloseHandle(hSnapshot);

    BeaconPrintf(CALLBACK_OUTPUT, "%s\n", BeaconFormatToString(&buffer, NULL));
    BeaconFormatFree(&buffer);
}

int go(char *buff, int len)
{
    int processID;
    datap parser;
    
    BeaconDataParse(&parser, buff, len);

    // Specify the process ID here
    processID = BeaconDataInt(&parser);

    ListModules(processID);

    return 0;
}
