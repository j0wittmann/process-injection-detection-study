#include <Windows.h>
#include <taskschd.h>
#include <comdef.h>
#include <string>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsuppw.lib")

bool CreateTask(const wchar_t* taskName, const wchar_t* exePath) {
    HRESULT hr = S_OK;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) return false;

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, 0, NULL);

    if (FAILED(hr) && hr != RPC_E_TOO_LATE) {
        CoUninitialize();
        return false;
    }

    ITaskService* pService = NULL;
    ITaskFolder* pRootFolder = NULL;
    ITaskDefinition* pTask = NULL;
    ITriggerCollection* pTriggerCollection = NULL;
    ITrigger* pTrigger = NULL;
    IDailyTrigger* pDailyTrigger = NULL;
    IRepetitionPattern* pRepetition = NULL;
    IActionCollection* pActionCollection = NULL;
    IAction* pAction = NULL;
    IExecAction* pExecAction = NULL;
    IRegisteredTask* pRegisteredTask = NULL;
    ITaskSettings* pSettings = NULL;
    std::wstring wsPath(exePath);
    size_t pos = wsPath.find_last_of(L"\\");

    bool success = false;

    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr)) goto Cleanup;

    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr)) goto Cleanup;

    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) goto Cleanup;

    hr = pService->NewTask(0, &pTask);
    if (FAILED(hr)) goto Cleanup;

    hr = pTask->get_Settings(&pSettings);
    if (SUCCEEDED(hr)) {
        pSettings->put_StartWhenAvailable(VARIANT_TRUE);

        pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
        pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);

        pSettings->put_MultipleInstances(TASK_INSTANCES_IGNORE_NEW);

        pSettings->Release();
    }

    hr = pTask->get_Triggers(&pTriggerCollection);
    if (FAILED(hr)) goto Cleanup;

    hr = pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
    if (FAILED(hr)) goto Cleanup;

    hr = pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDailyTrigger);
    if (FAILED(hr)) goto Cleanup;

    pDailyTrigger->put_StartBoundary(_bstr_t(L"2026-02-21T12:00:00"));
    pDailyTrigger->put_DaysInterval(1);

    hr = pDailyTrigger->get_Repetition(&pRepetition);
    if (SUCCEEDED(hr)) {
        pRepetition->put_Interval(_bstr_t(L"PT5M"));
        pRepetition->put_Duration(_bstr_t(L""));
    }

    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr)) goto Cleanup;

    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    if (FAILED(hr)) goto Cleanup;

    hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
    if (FAILED(hr)) goto Cleanup;

    hr = pExecAction->put_Path(_bstr_t(exePath));
    if (FAILED(hr)) goto Cleanup;

    if (pos != std::wstring::npos) {
        std::wstring workingDir = wsPath.substr(0, pos);
        pExecAction->put_WorkingDirectory(_bstr_t(workingDir.c_str()));
    }

    hr = pRootFolder->RegisterTaskDefinition(
        _bstr_t(taskName),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(),
        _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(L""),
        &pRegisteredTask
    );

    success = SUCCEEDED(hr);

Cleanup:
    if (pRegisteredTask) pRegisteredTask->Release();
    if (pExecAction) pExecAction->Release();
    if (pAction) pAction->Release();
    if (pActionCollection) pActionCollection->Release();
    if (pRepetition) pRepetition->Release();
    if (pDailyTrigger) pDailyTrigger->Release();
    if (pTrigger) pTrigger->Release();
    if (pTriggerCollection) pTriggerCollection->Release();
    if (pTask) pTask->Release();
    if (pRootFolder) pRootFolder->Release();
    if (pService) pService->Release();

    CoUninitialize();
    return success;
}

int main() {
    const wchar_t* myTaskName = L"DailyTask";
    const wchar_t* myExePath = L"C:\\Users\\whatever\\whatever.exe";

    CreateTask(myTaskName, myExePath);
}