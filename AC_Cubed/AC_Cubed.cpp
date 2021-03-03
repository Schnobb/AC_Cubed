#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#include "memUtils.h"
#include "processUtils.h"
#include "assaultCubeInfo.h"

void DrawHeader()
{
	std::wcout << L"================================================================================" << std::endl;
	std::wcout << L"|     AC_Cubed - Assault Cube External Trainer                                 |" << std::endl;
	std::wcout << L"================================================================================" << std::endl;
	std::wcout << std::endl;
}

void DrawMenu(DWORD pid, uintptr_t baseAddress)
{
	std::wcout << L"\tProcess " << AC_MODULE_NAME << " (PID " << pid << L") hooked!" << std::endl;
	std::wcout << std::endl;
	wprintf_s(L"\tBase Address:\t\t0x%08X\n", baseAddress);
	wprintf_s(L"\tPlayer Entity Pointer:\t0x%08X\n", baseAddress + AC_PLAYER_ENTITY_OFF);
	std::wcout << std::endl;
	std::wcout << L"\t + NUMPAD_0 - Infinite Ammo" << std::endl;
	std::wcout << L"\t + NUMPAD_1 - Infinite Grenades" << std::endl;
	std::wcout << L"\t + NUMPAD_2 - Infinite Health" << std::endl;
	std::wcout << L"\t + NUMPAD_3 - Infinite Armor" << std::endl;
	std::wcout << L"\t + NUMPAD_4 - Infinite Akimbo" << std::endl;
	std::wcout << L"\t + NUMPAD_5 - (WIP) No Recoil / Kickback" << std::endl;
	std::wcout << L"\t + NUMPAD_6 - (WIP) Higher RoF" << std::endl;
	std::wcout << std::endl;
	std::wcout << L"\t + END      - Unhook and exit" << std::endl;
}

void DrawExit()
{
	std::wcout << std::endl << L"Unhooked. Thanks for playing!" << std::endl;
}

int main()
{
	DrawHeader();

	// Open process handle
	DWORD pid = processUtils::GetProcessId(AC_MODULE_NAME);
	if (pid <= 0)
	{
		std::wcout << L"ERROR: Could not get process PID. Is ac_client.exe running?" << std::endl;
		return 1;
	}

	HANDLE hProcess = processUtils::GetProcessHandle(pid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"ERROR: Could not get process handle" << std::endl;
		return 2;
	}

	// Get ac_client.exe base address
	uintptr_t baseAddress = processUtils::GetModuleBaseAddress(pid, AC_MODULE_NAME);
	if (baseAddress <= 0)
	{
		std::wcout << L"ERROR: Could not get base address of module " << AC_MODULE_NAME << std::endl;
		return 3;
	}

	DrawMenu(pid, baseAddress);

	DWORD exitCode;

	// Toggles
	bool infiniteAmmo = false;
	bool infiniteGrenades = false;
	bool infiniteHealth = false;
	bool infiniteArmor = false;
	bool infiniteAkimbo = false;
	bool recoilHack = false;
	bool rofHack = false;

	// Freeze values
	UINT32 grenadeCount = AC_MAX_GRENADE;
	UINT32 health = AC_MAX_HEALTH;
	UINT32 armor = AC_MAX_ARMOR;
	UINT32 akimboAmmoMag = AC_MAX_AKIMBO_AMMO_MAG;
	UINT32 akimboAmmoInv = AC_MAX_AKIMBO_AMMO_INV;
	BYTE akimboEnabled = TRUE;
	BYTE akimboDisabled = FALSE;

	// Find addresses
	uintptr_t grenadesAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_GRENADES_OFF);
	uintptr_t healthAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_HEALTH_OFF);
	uintptr_t armorAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_ARMOR_OFF);

	uintptr_t akimboAmmoMagAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_AKIMBO_AMMO_MAG_OFF);
	uintptr_t akimboAmmoInvAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_AKIMBO_AMMO_INV_OFF);
	uintptr_t akimboEnabledAddress = memUtils::FindDMAAddressEx(hProcess, baseAddress + AC_PLAYER_ENTITY_OFF, AC_PLAYER_AKIMBO_ENABLED_OFF);

	// While ac_client.exe process is running
	while (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE)
	{
		// Infinite ammo patch
		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			infiniteAmmo = !infiniteAmmo;

			if (infiniteAmmo)
				memUtils::NopMemoryEx(hProcess, baseAddress + AC_AMMO_FUNCTION_OFFSET, AC_AMMO_FUNCTION_SIZE);
			else
				memUtils::WriteMemoryEx(hProcess, baseAddress + AC_AMMO_FUNCTION_OFFSET, (void*)AC_AMMO_FUNCTION_ORIGINAL, AC_AMMO_FUNCTION_SIZE);
		}

		// Freeze value toggles
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			infiniteGrenades = !infiniteGrenades;
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
			infiniteHealth = !infiniteHealth;
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
			infiniteArmor = !infiniteArmor;

		// Infinite akimbo stuff
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			infiniteAkimbo = !infiniteAkimbo;

			if (infiniteAkimbo)
			{
				memUtils::WriteMemoryEx(hProcess, akimboAmmoMagAddress, &akimboAmmoMag, sizeof(akimboAmmoMag));
				memUtils::WriteMemoryEx(hProcess, akimboAmmoInvAddress, &akimboAmmoInv, sizeof(akimboAmmoInv));
			}
			else
				memUtils::WriteMemoryEx(hProcess, akimboEnabledAddress, &akimboDisabled, sizeof(akimboDisabled));
		}

		// No recoil hack
		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
			recoilHack = !recoilHack;

		// Rate of fire hack
		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
			rofHack = !rofHack;

		// Unhook and exit command
		if (GetAsyncKeyState(VK_END) & 1)
		{
			if (infiniteAmmo)
				memUtils::WriteMemoryEx(hProcess, baseAddress + AC_AMMO_FUNCTION_OFFSET, (void*)AC_AMMO_FUNCTION_ORIGINAL, AC_AMMO_FUNCTION_SIZE);
			if (infiniteAkimbo)
				memUtils::WriteMemoryEx(hProcess, akimboEnabledAddress, &akimboDisabled, sizeof(akimboDisabled));

			break;
		}

		// Freeze values
		if (infiniteGrenades)
			memUtils::WriteMemoryEx(hProcess, grenadesAddress, &grenadeCount, sizeof(grenadeCount));
		if (infiniteHealth)
			memUtils::WriteMemoryEx(hProcess, healthAddress, &health, sizeof(health));
		if (infiniteArmor)
			memUtils::WriteMemoryEx(hProcess, armorAddress, &armor, sizeof(armor));
		if (infiniteAkimbo)
			memUtils::WriteMemoryEx(hProcess, akimboEnabledAddress, &akimboEnabled, sizeof(akimboEnabled));

		Sleep(10);
	}

	CloseHandle(hProcess);
	DrawExit();
	return 0;
}