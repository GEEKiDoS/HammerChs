#include <stdinc.hpp>

extern HMODULE self;

std::map<int, bool> is_resource_found;

HGLOBAL(WINAPI* LoadResource_Orig)(HMODULE, HRSRC);
HGLOBAL WINAPI HkLoadResource(HMODULE hModule, HRSRC hResInfo)
{
	if (is_resource_found.contains(hResInfo->unused) && is_resource_found[hResInfo->unused])
		return LoadResource_Orig(self, hResInfo);

	return LoadResource_Orig(hModule, hResInfo);
}

DWORD(WINAPI* SizeofResource_Orig)(HMODULE, HRSRC);
DWORD WINAPI HkSizeofResource(HMODULE hModule, HRSRC hResInfo)
{
	if (is_resource_found.contains(hResInfo->unused) && is_resource_found[hResInfo->unused])
		return SizeofResource_Orig(self, hResInfo);

	return SizeofResource_Orig(hModule, hResInfo);
}

HRSRC(WINAPI* FindResourceA_Orig)(HMODULE, LPCSTR, LPCSTR);
HRSRC WINAPI HkFindResourceA(HMODULE hModule, LPCSTR lpName, LPCSTR lpType)
{
	auto resource = FindResourceA_Orig(self, lpName, lpType);

	if (resource)
	{
		is_resource_found.emplace(resource->unused, true);
	}
	else
	{
		resource = FindResourceA_Orig(hModule, lpName, lpType);

		if(resource)
			is_resource_found.emplace(resource->unused, false);
	}

	return resource;
}


void Hook()
{
	char moduleName[MAX_PATH];
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	if (std::string(moduleName).find("hammerplusplus.exe", 0) != std::string::npos)
	{
		if (MH_Initialize() != MH_OK)
		{
			std::cout << "Failed to init minhook\n";
			return;
		}

		MH_CreateHook(FindResourceA, HkFindResourceA, reinterpret_cast<LPVOID*>(&FindResourceA_Orig));
		MH_CreateHook(SizeofResource, HkSizeofResource, reinterpret_cast<LPVOID*>(&SizeofResource_Orig));
		MH_CreateHook(LoadResource, HkLoadResource, reinterpret_cast<LPVOID*>(&LoadResource_Orig));

		MH_EnableHook(&FindResourceA);
		MH_EnableHook(&SizeofResource);
		MH_EnableHook(&LoadResource);
	}

}

void UnHook()
{
	char moduleName[MAX_PATH];
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	if (std::string(moduleName).find("hammerplusplus.exe", 0) != std::string::npos)
	{
		MH_DisableHook(FindResourceA);
		MH_DisableHook(SizeofResource);
		MH_DisableHook(LoadResource);

		MH_Uninitialize();
	}
}
