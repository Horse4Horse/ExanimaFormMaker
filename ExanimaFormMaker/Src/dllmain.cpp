// dllmain.cpp : Defines the entry point for the DLL application.

#include <dllmain.hpp>

uint8_t Verbose = 1;

HMODULE myhModule;
HANDLE  hGameProcess;

uint64_t GameStaticMemoryStartAddress = 0;
uint64_t GameStaticMemoryEndAddress = 0;
uint64_t GameStaticMemorySize = 0;

DWORD static __stdcall EjectThread(LPVOID) {
	CloseHandle(hGameProcess);
	if (fp)
		fclose(fp);
	FreeConsole();
	Sleep(100);
	FreeLibraryAndExitThread(myhModule, 0);
}

static uint64_t GetModuleMemoryRange(uint64_t& ModuleStartAddress, uint64_t& ModuleEndAddress, uint64_t& ModuleSize) {
	LPVOID moduleBase = nullptr;
	SIZE_T moduleSize = 0;

	// Get the handle to the executable module associated with the current process
	HMODULE hModule = GetModuleHandle(NULL);

	if (!hModule) {
		std::cout << "Failed to get module handle." << std::endl;
		return 0;
	}

	// Get information about the module
	MODULEINFO modInfo;
	if (!GetModuleInformation(hGameProcess, hModule, &modInfo, sizeof(modInfo))) {
		std::cout << "Failed to get module information." << std::endl;
		return 0;
	}
	if (!Verbose)
		std::cout << "Module Base Address: " << modInfo.lpBaseOfDll << std::endl << "Module Size: " << std::hex << modInfo.SizeOfImage << std::endl;
	moduleBase = modInfo.lpBaseOfDll;
	moduleSize = modInfo.SizeOfImage;

	if (moduleBase != nullptr && ModuleStartAddress >= 0) {
		ModuleStartAddress = (uint64_t)moduleBase;
		ModuleEndAddress = ModuleStartAddress + static_cast<uint64_t>(moduleSize);
		ModuleSize = static_cast<uint64_t>(moduleSize);
		if (!Verbose)
			std::cout << "Start address: " << std::hex << ModuleStartAddress << " End address: " << std::hex << ModuleEndAddress << " Size: " << std::hex << ModuleSize << std::endl;
	}
	else {
		std::cout << "Invalid module base address or initial integer value." << std::endl;
		return 0;
	}
	return 1;
}


int static WINAPI Menu() {
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);

	//Get handle to the game, don't forget to close it.
	hGameProcess = GetCurrentProcess();

	if (!hGameProcess) {
		std::cerr << "Failed to open process." << std::endl;
		CreateThread(0, 0, EjectThread, 0, 0, 0);
		return 1;
	}
	if (!Verbose)
		std::cout << "ProcessHandle: " << (uint64_t)hGameProcess << std::endl;

	if (!GetModuleMemoryRange(GameStaticMemoryStartAddress, GameStaticMemoryEndAddress, GameStaticMemorySize)) {
		std::cout << "Error reading game' static memory boundaries, aborting!" << std::endl;
		CreateThread(0, 0, EjectThread, 0, 0, 0);
		return 0;
	}

	DefineAllSignatureStructs();

	std::cout << "Press 0 to Exit | Press 1 for Scanning" << std::endl;
	while (1) {
		Sleep(100);
		if (GetAsyncKeyState(VK_NUMPAD0))
			break;
		if (GetAsyncKeyState(VK_NUMPAD1)) {

			/* PoC 
			* Spawn a basic form. 
			*/
			TGUIFormStructure* MyForm = createGUITemplate(TGUIForm, "My form!", 1, 0);

			registerFormInternal((uint64_t*)MyForm, 1);

			std::cout << "Form address: " << std::hex << MyForm << std::endl;
		}
	}

	CreateThread(0, 0, EjectThread, 0, 0, 0);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, int  ul_reason_for_call, LPVOID) {

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		myhModule = hModule;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Menu, NULL, NULL, NULL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}