#include "5173io-dll-injector.h"
using namespace std;
int main(int argc, char* argv[])
{
	map<string, string> opts;
	parseArgv(argc, argv, opts);
	if (opts.size() == 0) {
		cout << "Pls input valid parameters!" << endl;
		return 0;
	}
	multimap<string, int> processNameIdMap;
	if (!getProcessNameIdMap(processNameIdMap)) {
		cout << "getProcessNameIdMap Error!" << endl;
		return 0;
	}
	int processId = getProcessId(opts, processNameIdMap);
	if (processId <= 0) {
		std::cout << "process id is invalid !" << endl;;
		return 0;
	}
	string dllpath = opts["-dll"];
	if (!fileExists(dllpath.c_str())) {
		std::cout << "dll is not exists !" << endl;;
		return 0;
	}
	inject(processId, dllpath);
	// Free the memory allocated for our dll path
	//VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);
	return 0;
}
