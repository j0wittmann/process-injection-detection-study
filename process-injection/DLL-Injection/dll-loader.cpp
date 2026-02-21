#include <windows.h>
#include <TlHelp32.h>

int main() {
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
	const char* dllPath = "C:\\Users\\jonas\\Desktop\\dll.dll";

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

	HANDLE processHandle = OpenProcess(MAXIMUM_ALLOWED, FALSE, processEntry.th32ProcessID);

	LPVOID remoteBuffer = VirtualAllocEx(processHandle, NULL, strlen(dllPath) + 1, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);

	WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, strlen(dllPath) + 1, NULL);

	LPVOID loadLib = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLib, remoteBuffer, 0, NULL);

	CloseHandle(processHandle);

	return 0;
}
