#pragma once

bool debugW(const wchar_t* text, const wchar_t* title = L"debug") {
	MessageBoxW(NULL, text, title, NULL);
	return true;
}
bool debugA(const char* text, const char* title = "debug") {
	MessageBoxA(NULL, text, title, NULL);
	return true;
}

DWORD GetProcessIdFromName(const wchar_t* name) {
	DWORD processID = NULL;
	PROCESSENTRY32 ProcessInfo = { sizeof(PROCESSENTRY32) };
	HANDLE SnapShot;

	SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot == INVALID_HANDLE_VALUE)return 0;

	if (Process32First(SnapShot, &ProcessInfo)) {
		do {
			if (!wcscmp(ProcessInfo.szExeFile, name)) {
				processID = ProcessInfo.th32ProcessID;
				break;
			}
		} while (Process32Next(SnapShot, &ProcessInfo));
	}
	CloseHandle(SnapShot);

	return processID;
}

DWORD GetRandomThreadIDFromProcess(DWORD processID) {
	if (!processID)return false;
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	DWORD threadID = NULL;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)return false;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)){CloseHandle(hThreadSnap);return false;}

	do
	{
		if (te32.th32OwnerProcessID == processID)
		{
			threadID = te32.th32ThreadID;
			break;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return threadID;
}
DWORD GetRandomThreadIDFromProcess(const wchar_t* name) {
	DWORD processID = NULL;
	processID = GetProcessIdFromName(name);
	if (!processID)return false;

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	DWORD threadID = NULL;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)return false;
	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32)) { CloseHandle(hThreadSnap); return false; }

	do
	{
		if (te32.th32OwnerProcessID == processID)
		{
			threadID = te32.th32ThreadID;
			break;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return threadID;
}

HANDLE GetProcess(DWORD processID, DWORD DesiredAccess = PROCESS_ALL_ACCESS) {
	if (!processID)return NULL;
	return OpenProcess(DesiredAccess, false, processID);
}
HANDLE GetProcess(const wchar_t* name, DWORD DesiredAccess = PROCESS_ALL_ACCESS) {
	DWORD processID = NULL;
	processID = GetProcessIdFromName(name);
	if (!processID)return NULL;
	return OpenProcess(DesiredAccess, false, processID);
}

HANDLE GetThread(DWORD threadID, DWORD DesiredAccess = THREAD_ALL_ACCESS) {
	return OpenThread(DesiredAccess, false, threadID);
}

BYTE IsWOW64(HANDLE process) {
	if (!process)return -1;
	BOOL isWOW64 = false;
	if (!IsWow64Process(process, &isWOW64))return -1;
	return isWOW64;
}
BYTE IsWOW64(DWORD processID) {
	HANDLE process = GetProcess(processID);
	if (!process)return -1;
	BOOL isWOW64 = false;
	if (!IsWow64Process(process, &isWOW64))return -1;
	return isWOW64;
}
BYTE IsWOW64(const wchar_t* name) {
	HANDLE process = GetProcess(name);
	if (!process)return -1;
	BOOL isWOW64 = false;
	if (!IsWow64Process(process, &isWOW64))return -1;
	return isWOW64;
}