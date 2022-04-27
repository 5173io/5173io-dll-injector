// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
DWORD   WINAPI    threadFunction(LPVOID lpParameter);
bool runThread() {
	CreateThread(NULL, 0, threadFunction, NULL, 0, NULL);
	return true;
}

DWORD   WINAPI    threadFunction(LPVOID lpParameter) {
	MessageBox(0, L"inject ok!", L"Inject", MB_OK);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		runThread();

	return TRUE;
}

