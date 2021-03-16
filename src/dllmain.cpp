#include "stdinc.hpp"

HMODULE self;
extern void Hook();
extern void UnHook();

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		self = hModule;
		Hook();

		break;
	case DLL_PROCESS_DETACH:
		UnHook();

		break;
	}
	return TRUE;
}
