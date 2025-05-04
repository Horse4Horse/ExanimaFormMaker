
#include <dllmain.hpp>

uint8_t Verbose = 1;

HMODULE myhModule;
HANDLE  hGameProcess;

uint64_t GameStaticMemoryStartAddress = 0;
uint64_t GameStaticMemoryEndAddress = 0;
uint64_t GameStaticMemorySize = 0;

DWORD static __stdcall EjectThread(LPVOID)
{
	if (fp)
		fclose(fp);
	FreeConsole();
	Sleep(100);
	FreeLibraryAndExitThread(myhModule, 0);
}

static uint64_t GetModuleMemoryRange(uint64_t& ModuleStartAddress, uint64_t& ModuleEndAddress, uint64_t& ModuleSize)
{
	LPVOID moduleBase = nullptr;
	SIZE_T moduleSize = 0;

	// Get the handle to the executable module associated with the current process
	HMODULE hModule = GetModuleHandle(NULL);

	if (!hModule)
	{
		std::cout << "Failed to get module handle." << std::endl;
		return 0;
	}

	// Get information about the module
	MODULEINFO modInfo;
	if (!GetModuleInformation(hGameProcess, hModule, &modInfo, sizeof(modInfo)))
	{
		std::cout << "Failed to get module information." << std::endl;
		return 0;
	}
	if (!Verbose)
		std::cout << "Module Base Address: " << modInfo.lpBaseOfDll << std::endl << "Module Size: " << std::hex << modInfo.SizeOfImage << std::endl;
	moduleBase = modInfo.lpBaseOfDll;
	moduleSize = modInfo.SizeOfImage;

	if (moduleBase != nullptr && ModuleStartAddress >= 0)
	{
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

/* TODO: For injecting a function inside a game
DWORD GetFunctionSize(void* function)
{
	HMODULE hModule;
	if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCSTR)function, &hModule))
	{
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);

		return pNtHeaders->OptionalHeader.SizeOfCode;
	}
	return 0;
}
*/

int static WINAPI Menu()
{
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);

	//Get handle to the game, don't forget to close it.
	hGameProcess = GetCurrentProcess();

	if (!hGameProcess)
	{
		std::cout << "Failed to open process." << std::endl;
		CreateThread(0, 0, EjectThread, 0, 0, 0);
		return 1;
	}
	if (!Verbose)
		std::cout << "ProcessHandle: " << (uint64_t)hGameProcess << std::endl;

	if (!GetModuleMemoryRange(GameStaticMemoryStartAddress, GameStaticMemoryEndAddress, GameStaticMemorySize))
	{
		std::cout << "Error reading game' static memory boundaries, aborting!" << std::endl;
		CreateThread(0, 0, EjectThread, 0, 0, 0);
		return 0;
	}

	DefineAllSignatureStructs();

	std::cout << "Press 0 to Exit | Press 1 for action" << std::endl;
	while (1)
	{
		Sleep(100);
		if (GetAsyncKeyState(VK_NUMPAD0))
			break;
		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			/* PoC
			* Spawn a basic form with a child form inside.
			*/

			TGUIForm* ParentForm = TGUIForm::Create("Parent Form");
			ParentForm->Style |= 0x90;		// Set Border to Double line
			ParentForm->FVisible = false;  
			ParentForm->Width = 400;
			ParentForm->Height = 500;

			TGUIForm* ChildForm = TGUIForm::Create("Child Form", ParentForm);
			ChildForm->Style |= 0x3010;	
			ChildForm->FormStyle = 0x03;			// Enable Caption and CloseCross, disable Shadow
			ChildForm->FormStyleEx &= 0xFFFFFF7F;   // Enable child's upper border caption support
			ChildForm->PosX = 7;
			ChildForm->PosY = 5;
			ChildForm->Width = 300;
			ChildForm->Height = 400;
			ChildForm->Border = 4;


			TGUITableView* TableView = TGUITableView::Create("Table View", ChildForm);
			TableView->PosX = 0;
			TableView->PosY = 0;
			TableView->Width = TableView->Parent->Width - (2 * TableView->Parent->Border);
			TableView->Height = TableView->Parent->Height - 100;
			TableView->Border = 2;
			TableView->Style |= 0x80; //Single outline for internal subforms.
			*(uint32_t*)&TableView->FormStyle = *(uint32_t*)&TableView->FormStyle & 0xFFFF3F6F | 0x90;
			TableView->Columns = 3;
			fSystem__DynArraySetLength_Internal(&TableView->Column, pTGUITableColumn_DynArray, 1, &TableView->Columns); //since we don't use second dimension - provide just a pointer to the columns
			int colWidth = ((TableView->Width / TableView->Columns) - (TableView->Border * 2)) ;
			(void*)TGUITableColumn::Create(&TableView->Column[0], "Column 1", colWidth); //Cast return to void since we will not set additional data to clumns
			(void*)TGUITableColumn::Create(&TableView->Column[1], "Column 2", colWidth);
			(void*)TGUITableColumn::Create(&TableView->Column[2], "Column 3", colWidth);



			TGUIControl buttonArray[2]; //create an array on the stack

			TGUIControl* buttonAdd = &buttonArray[0];
			buttonAdd->CntrlClass = 0x0001;
			buttonAdd->wdt = 110;
			buttonAdd->hgt = 30;
			buttonAdd->px = TableView->Border + 3;
			buttonAdd->py = (TableView->Height + 10) / 4;
			MakeCPString(&buttonAdd->FCaption, "Add item");
			buttonAdd->MsgProc = TGUIControl_MsgProc_Button_Internal;
			buttonAdd->ControlID = 1;
			buttonAdd->OnChange = ButtonCallback;

			TGUIControl* buttonRemove = &buttonArray[1];
			buttonRemove->CntrlClass = 0x0001;
			buttonRemove->wdt = 110;
			buttonRemove->hgt = 30;
			buttonRemove->px = TableView->Width - (TableView->Border * 2) - buttonRemove->wdt;
			buttonRemove->py = (TableView->Height + 10) / 4;
			MakeCPString(&buttonRemove->FCaption, "Remove item");
			buttonRemove->MsgProc = TGUIControl_MsgProc_Button_Internal;
			buttonRemove->ControlID = 2;
			buttonRemove->State |= 0x10; //Disable button by default
			buttonRemove->OnChange = ButtonCallback;

			ChildForm->AssignControls(buttonArray,1); //AssignControls with "staticControls = false" will alloc mem on the heap and transfer everthing there. Else it won't try to dealloc controls(Like from RO mem) by setting form->OwnsControls=false


			fRegisterForm_Internal(ParentForm, 1);
			ParentForm->FVisible = true;
		}
	}

	CreateThread(0, 0, EjectThread, 0, 0, 0);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, int  ul_reason_for_call, LPVOID)
{
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