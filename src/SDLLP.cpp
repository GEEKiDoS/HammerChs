﻿// --------------------------------------+
// System Dynamic Link Library Proxy
// by momo5502
// --------------------------------------+

#include "stdinc.hpp"

// Macro to declare an export
// --------------------------------------+
#define EXPORT(_export) extern "C" __declspec(naked) __declspec(dllexport) void _export() { static FARPROC function = 0; if(!function) function = SDLLP::GetExport(__FUNCTION__, LIBRARY); __asm { jmp function } }  

// Static class
// --------------------------------------+
class SDLLP
{
private:
	static std::map<std::string, HINSTANCE> mLibraries;

	static void	Log(const char* message, ...);
	static void	LoadLibrary(const char* library);
	static bool	IsLibraryLoaded(const char* library);

public:
	static FARPROC GetExport(const char* function, const char* library);
};

//	Class variable declarations
// --------------------------------------+
std::map<std::string, HINSTANCE> SDLLP::mLibraries;

// Load necessary library
// --------------------------------------+
void SDLLP::LoadLibrary(const char* library)
{
	Log("[SDLLP] Loading library '%s'.\n", library);

	CHAR mPath[MAX_PATH];

	// GetSystemDirectoryA(mPath, MAX_PATH);
	// strcat_s(mPath, "\\");
	// strcat_s(mPath, library);

	mLibraries[library] = ::LoadLibraryA(library);

	if (!IsLibraryLoaded(library)) Log("[SDLLP] Unable to load library '%s'.\n", library);
}

// Check if export already loaded
// --------------------------------------+
bool SDLLP::IsLibraryLoaded(const char* library)
{
	return (mLibraries.find(library) != mLibraries.end() && mLibraries[library]);
}

// Get export address
// --------------------------------------+
FARPROC SDLLP::GetExport(const char* function, const char* library)
{
	Log("[SDLLP] Export '%s' requested from %s.\n", function, library);

	if (!IsLibraryLoaded(library)) LoadLibrary(library);

	FARPROC address = GetProcAddress(mLibraries[library], function);

	if (!address) Log("[SDLLP] Unable to load export '%s' from library '%s'.\n", function, library);
	return address;
}

// Write debug string
// --------------------------------------+
void SDLLP::Log(const char* message, ...)
{
	CHAR buffer[1024];
	va_list ap;

	va_start(ap, message);
	vsprintf_s(buffer, message, ap);
	va_end(ap);

	OutputDebugStringA(buffer);
}

// --------------------------------------+
//	Adapt export functions and library
// --------------------------------------+

#define LIBRARY "bass_o.dll"
EXPORT(BASS_ChannelPlay)
EXPORT(BASS_SampleGetChannel)
EXPORT(BASS_ChannelSetAttribute)
EXPORT(BASS_Stop)
EXPORT(BASS_Start)
EXPORT(BASS_Init)
EXPORT(BASS_ErrorGetCode)
EXPORT(BASS_SampleLoad)
