// 5173io-dll-injector.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>
using namespace std;

// TODO: 在此处引用程序需要的其他标头。

bool fileExists(const std::string& name);

HMODULE getRemoteModuleHandle(DWORD dwProcessId, const wchar_t* szModule);

bool getProcessNameIdMap(multimap<string, int>& processNameIdMap);

void parseArgv(int argc, char* argv[], map<string, string>& opts);

int getProcessId(map<string, string>& opts, const multimap<string, int> processNameIdMap);

bool inject(int processId, const string& dllpath);