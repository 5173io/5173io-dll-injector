// 5173io-dll-injector.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>
#include "5173io-dll-injector.h"
using namespace std;

bool fileExists(const std::string& name) {
	FILE* filepoint;
	errno_t err;
	if ((err = fopen_s(&filepoint, name.c_str(), "r")) == 0) {
		fclose(filepoint);
		return true;
	}
	else {
		return false;
	}
}

HMODULE getRemoteModuleHandle(DWORD dwProcessId, const wchar_t* szModule)
{
	HANDLE tlh = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	MODULEENTRY32 modEntry;

	modEntry.dwSize = sizeof(MODULEENTRY32);

	Module32First(tlh, &modEntry);
	do
	{
		if (_wcsicmp(szModule, modEntry.szModule) == 0)
		{
			CloseHandle(tlh);
			return modEntry.hModule;
		}
	} while (Module32Next(tlh, &modEntry));

	CloseHandle(tlh);

	return NULL;
}

bool getProcessNameIdMap(multimap<string, int>& processNameIdMap)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		cout << "CreateToolhelp32Snapshot Error!" << endl;;
		return false;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	int num(0);
	while (bResult)
	{
		char temp[300];
		WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, temp, sizeof(temp), NULL, NULL);
		string name = string(temp);
		int id = pe32.th32ProcessID;
		processNameIdMap.insert(pair<string, int>(name, id));
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return true;
}

void parseArgv(int argc, char* argv[], map<string, string>& opts) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-name") == 0 && argc > i + 1) {
			opts.insert({ argv[i], argv[i + 1] });
		}
		if (strcmp(argv[i], "-pid") == 0 && argc > i + 1) {
			opts.insert({ argv[i], argv[i + 1] });
		}
		if (strcmp(argv[i], "-dll") == 0 && argc > i + 1) {
			opts.insert({ argv[i], argv[i + 1] });
		}
	}
	return;
}

int getProcessId(map<string, string>& opts, const multimap<string, int> processNameIdMap) {
	int processId;
	if (opts.find("-pid") != opts.end()) {
		processId = std::stoi(opts["-pid"], nullptr);
	}
	else {
		if (processNameIdMap.find(opts["-name"]) == processNameIdMap.end()) {
			cout << opts["-name"] << " is not exist !" << endl;
			return 0;
		}
		if (processNameIdMap.count(opts["-name"]) > 1) {
			cout << opts["-name"] << " has more than one process, pls use -pid !" << endl;
			return 0;
		}
		processId = processNameIdMap.find(opts["-name"])->second;
	}
	return processId;
}

bool inject(int processId, const string& dllpath) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (hProcess == NULL) {
		std::cout << "hProcess is invalid";
		return 0;
	}
	LPVOID dllpathRemote = VirtualAllocEx(hProcess, 0, strlen(dllpath.c_str()) + 1,
		MEM_COMMIT, PAGE_READWRITE);
	if (dllpathRemote == nullptr) {
		std::cout << "VirtualAllocEx failed";
		return 0;
	}

	bool ret = WriteProcessMemory(hProcess, dllpathRemote, (LPVOID)dllpath.c_str(),
		strlen(dllpath.c_str()) + 1, 0);
	if (ret == false) {
		std::cout << "WriteProcessMemory failed";
		return 0;
	}

#if defined(_WIN64)
	HMODULE hRemoteKernel = getRemoteModuleHandle(processId, L"kernel32.dll");
	if (hRemoteKernel == NULL) {
		std::cout << "getRemoteModuleHandle failed";
		return 0;
	}
	HMODULE hKernel = LoadLibraryA("kernel32.dll");
	if (hRemoteKernel == NULL) {
		std::cout << "LoadLibraryA failed";
		return 0;
	}
	DWORD64 dwLoadLibraryA = (DWORD64)GetProcAddress(hKernel, "LoadLibraryA") - (DWORD64)hKernel;
	DWORD64 dwRemoteLoadLibraryAddress = ((DWORD64)hRemoteKernel + dwLoadLibraryA);
	HANDLE hLoadThread = CreateRemoteThread(hProcess,
		0,
		0,
		(LPTHREAD_START_ROUTINE)dwRemoteLoadLibraryAddress,
		dllpathRemote,
		0,
		0);
#else
	HANDLE hLoadThread = CreateRemoteThread(hProcess,
		0,
		0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"),
		dllpathRemote,
		0,
		0);
#endif
	if (hLoadThread == NULL) {
		std::cout << "CreateRemoteThread failed";
		return 0;
	}

	// Wait for the execution of our loader thread to finish
	WaitForSingleObject(hLoadThread, INFINITE);

	std::cout << "Dll path allocated at: " << std::hex << dllpathRemote << std::endl;
	return true;
}
