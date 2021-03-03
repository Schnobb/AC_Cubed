#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <list>

namespace processUtils
{
	DWORD GetProcessId(const wchar_t* processName);
	HANDLE GetProcessHandle(DWORD pid);
	HANDLE GetProcessHandle(const wchar_t* processName);
	uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName);
	uintptr_t GetModuleBaseAddress(const wchar_t* moduleName);
	std::list<MODULEENTRY32>* ListModules(DWORD pid);
}