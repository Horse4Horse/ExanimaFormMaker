#include <dllmain.hpp>

/* Ingame forms */
void* pTGUIForm_vmt;
void* pTTextBox_vmt;
void* pTGUITableView_vmt;


/* Ingame functions for forms, when they are not present in the VMT
 * You can still call VMT->NewInstance(FormData), but you will have to handle the rest by yourself. */

// TGUIForm:
fTGUIForm__Create* fTGUIForm__Create_Internal;

//TTextBox:
fTTextBox__Create* fTTextBox__Create_Internal;
fTTextBox__SetText* fTTextBox__SetText_Internal;

//TGUITableView:
fTGUITableView__Create* fTGUITableView__Create_Internal;
void* pTGUITableItem_DynArray;
void* pTGUITableColumn_DynArray;


/* Ingame functions */
fMakeCPString* fMakeCPString_Internal;
fRealloc* fRealloc_Internal;
fRegisterForm* fRegisterForm_Internal;
fConcatStringArray* fConcatStringArray_Internal;
fGetTGSLines* fGetTGSLines_Internal;
void* TGUIControl_MsgProc_Button_Internal;
void* TGUIControl_MsgProc_Label_Internal;
fSystem__DynArraySetLength* fSystem__DynArraySetLength_Internal;
fSystem__Realloc* fSystem__Realloc_Internal;
fAtomicSetPointer* fAtomicSetPointer_Internal;


static BOOL ReadGameMemory(uint64_t Address, std::vector<char>& Buf)
{
	SIZE_T bytesRead;
	BOOL success = ReadProcessMemory(hGameProcess, (LPCVOID)(Address), Buf.data(), Buf.size(), &bytesRead);

	if (!success || bytesRead != Buf.size())
	{
		std::cout << "Failed to read memory." << std::endl;
		return 0;
	}
	return 1;
}

MemorySignature::MemorySignature(const std::string& SignatureString, SignatureType Type, uint64_t OffsetFromAOB, uint8_t OpcodeSize) {
	this->Type = Type;
	this->OffsetFromAOB = OffsetFromAOB;

	// DF's are always 5 bytes in length: "Call OFFSET"
	if (Type == DisplacementFunction) {
		this->OpcodeSize = 5;
	}
	else {
		this->OpcodeSize = OpcodeSize;
	}

	//AOB.insert(AOB.end(), SignatureString.begin(),SignatureString.end());

	this->ResultAddress = this->FindObjectBySignature(SignatureString);

};

uint64_t MemorySignature::GetAddressFromDisplacementOpcode(std::vector<char>& opcode)
{
	uint32_t Displacement = 0;
	uint64_t Address = 0;

	switch (this->Type)
	{
	case DisplacementData:
		Displacement = ((uint8_t)opcode[6] << 24) | ((uint8_t)opcode[5] << 16) | ((uint8_t)opcode[4] << 8) | ((uint8_t)opcode[3]); //little-endian format
		break;
	case DisplacementFunction:
		Displacement = ((uint8_t)opcode[4] << 24) | ((uint8_t)opcode[3] << 16) | ((uint8_t)opcode[2] << 8) | (uint8_t)opcode[1];
	}
	Address = this->InstructionPointer + Displacement + opcode.size();
	if (Verbose) {
		std::cout << "InstructionPointer: " << std::hex << this->InstructionPointer << std::endl;
		std::cout << "Displacement: " << std::hex << Displacement << std::endl;
		std::cout << "Result address: " << std::hex << Address << std::endl;
	}
	return Address;
}

uint64_t MemorySignature::FindSignature(const std::string& Signature)
{
	MEMORY_BASIC_INFORMATION mbi{ 0 };
	uint64_t protectFlagsExclude = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

	for (uint64_t i = GameStaticMemoryStartAddress; i < GameStaticMemoryEndAddress - Signature.length(); i++)
	{
		if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi)))
		{
			if (mbi.Protect & protectFlagsExclude || !(mbi.State & MEM_COMMIT) || !(mbi.Protect & PAGE_EXECUTE_READ))
			{
				if (Verbose)
					std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (uint64_t)((uint64_t)mbi.BaseAddress + mbi.RegionSize) << std::endl;
				i += mbi.RegionSize;
				continue; // If address is bad then we don't need to read from it
			}

			if (Verbose)
				std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (uint64_t)((uint64_t)mbi.BaseAddress + mbi.RegionSize) << std::endl;

			uint64_t found = (uint64_t)Sig::find(mbi.BaseAddress, mbi.RegionSize, Signature.data());
			if (Verbose)
				std::cout << "Result: " << std::hex << found << std::endl;
			return found;

		}
	}
	std::cout << "Signature not found: " << Signature.data() << std::endl;

	return 0;
}

uint64_t MemorySignature::FindObjectBySignature(const std::string& Signature)
{
	if (!Signature.data()) {
		std::cout << "Provide a signature before searching for it!" << std::endl;
		return 0;
	}

	this->InstructionPointer = this->FindSignature(Signature);
	if (!this->InstructionPointer)
	{
		std::cout << "Can't find signature: " << Signature.data() << std::endl;
		return 0;
	}

	this->InstructionPointer += this->OffsetFromAOB;

	if (this->Type != Function)
	{
		std::vector<char>OpcodeBytes(this->OpcodeSize);

		if (!ReadGameMemory(this->InstructionPointer, OpcodeBytes))
			return 0;

		return this->GetAddressFromDisplacementOpcode(OpcodeBytes);

	}
	else // If we are searching for function's beginning AOB's - we already found our target. Get the address of signature as address of the function.
	{
		return this->InstructionPointer;
	}

}

extern void DefineAllSignatureStructs()
{

	/* TGUIForm signature search */

	MemorySignature TGUIFormVMT_Signature("49 89 C8 BA ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 C3 81 63", DisplacementData, 8, 7);
	pTGUIForm_vmt = (uint64_t*)TGUIFormVMT_Signature.ResultAddress;
	if (!pTGUIForm_vmt)
		std::cout << "Can't find 'TGUIForm'! " << std::endl;

	MemorySignature fTGUIForm__Create_Signature("55 48 89 E5 48 8D 64 24 ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 48 83 7D ? 01 75 ? 48 8B 45 ? 48 8B 55 ? 48 89 C1 FF 52 ? 48 89 45 ? 66 90 48 83 7D ? ? 0F 84 ? ? ? ? 90 90 48 C7 45 ? FF FF FF FF 48 8B 4D ? 31 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B 55", Function);
	fTGUIForm__Create_Internal = (fTGUIForm__Create*)fTGUIForm__Create_Signature.ResultAddress;
	if (!fTGUIForm__Create_Internal)
		std::cout << "Can't find 'CreateGUITemplate'! " << std::endl;




	/* TTextBox signature search */

	MemorySignature TTextBoxVMT_Signature("FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? BA 01 00 00 00 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 83 ? ? ? ? 48 89 C2", DisplacementData, 17, 7);
	pTTextBox_vmt = (uint64_t*)TGUIFormVMT_Signature.ResultAddress;
	if (!pTTextBox_vmt)
		std::cout << "Can't find 'TTextBoxVMT'! " << std::endl;

	MemorySignature fTTextBox__Create_Signature("55 48 89 E5 48 8D 64 24 ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 48 83 7D ? ? 75 ? 48 8B 45 ? 48 8B 55 ? 48 89 C1 FF 52 ? 48 89 45 ? 66 90 48 83 7D ? ? 0F 84 ? ? ? ? 90 90 48 C7 45 ? ? ? ? ? 4C 8B 45 ? 48 8B 4D ? 31 D2 E8 ? ? ? ? 48  8B 55 ? 8B 42 ? 0D ? ? ? ? 83 C8 ? 83 C8 ? 89", Function);
	fTTextBox__Create_Internal = (fTTextBox__Create*)fTTextBox__Create_Signature.ResultAddress;
	if (!fTTextBox__Create_Internal)
		std::cout << "Can't find 'TTextBox__Create'! " << std::endl;

	MemorySignature fTTextBox__SetText_Signature("53 56 48 8D 64 24 ? 48 89 CB 44 88 C6 48 8D 8B ? ? ? ? E8 ? ? ? ? 40 84 F6", Function);
	fTTextBox__SetText_Internal = (fTTextBox__SetText*)fTTextBox__SetText_Signature.ResultAddress;
	if (!fTTextBox__SetText_Internal)
		std::cout << "Can't find 'TTextBox__SetText'! " << std::endl;




	/* TGUITableView signature search */

	MemorySignature TGUITableViewVMT_Signature("48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B 55 ? 48 89 82 ? ? ? ? 48 8B 45 ? 48 8B 98 ? ? ? ? C7 43 20 00 00 00 00 C7 43 24 38 00 00 00 C7 43 28 C4 00 00 00 C7 43 2C B4 00 00 00 48 8D 05", DisplacementData, 0, 7);
	pTGUITableView_vmt = (uint64_t*)TGUITableViewVMT_Signature.ResultAddress;
	if (!pTGUITableView_vmt)
		std::cout << "Can't find 'TGUITableView'! " << std::endl;

	MemorySignature fTGUITableView__Create_Signature("55 48 89 ? 48 8D 64 24 ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 48 83 7D ? 1 75 14 48 8B 45 ? 48 8B 55 ? 48 89 C1 FF 52 ? 48 89 45 ? 66 90 48 83 7D ? 00 0F 84 ? ? ? ? 90 90 48 C7 45 ? FF FF FF FF 4C 8B 45 ? 48 8B 4D ? 31 D2 E8 ? ? ? ? 48 8B 55 ? 8B 42 ? 0D 80 00 00 00 83 C8 08 89 42 ? 48 8B 55 ?", Function);
	fTGUITableView__Create_Internal = (fTGUITableView__Create*)fTGUITableView__Create_Signature.ResultAddress;
	if (!fTGUITableView__Create_Internal)
		std::cout << "Can't find 'TGUITableView__Create'! " << std::endl;

	MemorySignature TGUITableItem_DynArray_Signature("7F ? 0F 1F 00 48 C7 44 24 ? ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 8B ? ? ? ? 4C 8D 4C 24 ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 C7 44", DisplacementData, 14, 7);
	pTGUITableItem_DynArray = (void*)TGUITableItem_DynArray_Signature.ResultAddress;
	if (!pTGUITableItem_DynArray)
		std::cout << "Can't find 'pTGUITableItem_DynArray'! " << std::endl;

	MemorySignature TGUITableColumn_DynArray_Signature("7F ? 0F 1F 00 48 C7 44 24 ? ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 8B ? ? ? ? 4C 8D 4C 24 ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 C7 44", DisplacementData, 53, 7);
	pTGUITableColumn_DynArray = (void*)TGUITableColumn_DynArray_Signature.ResultAddress;
	if (!pTGUITableColumn_DynArray)
		std::cout << "Can't find 'pTGUITableColumn_DynArray'! " << std::endl;
	



	/* Misc and helper functions */

	MemorySignature fMakeCPString_Signature("55 48 89 E5 48 8D 64 24 ? 48 89 5D ? 48 89 7D ? 48 89 75 ? 4C 89 65 ? 4C 89 6D ? 48 89 CE 48 89 D7 4D 89 C4 66 45 89 CD 8A 45 ? 84", Function);
	fMakeCPString_Internal = (fMakeCPString*)fMakeCPString_Signature.ResultAddress;
	if (!fMakeCPString_Internal)
	{
		std::cout << "Can't find 'MakeCPString'! Trying to find by reference... " << std::endl;

		MemorySignature fMakeCPString_Signature_ByRef("48 8D ? ? 48 8D ? ? E8 ? ? ? ? 48 8B 45 ? 48 89 45 ? 48 8D 05", DisplacementFunction, 8);
		fMakeCPString_Internal = (fMakeCPString*)fMakeCPString_Signature_ByRef.ResultAddress;

		if (fMakeCPString_Internal)
		{
			std::cout << "'MakeCPString' successfully found by reference! " << std::endl;
		}
		else {
			std::cout << "'MakeCPString' was not found! " << std::endl;
		}
	}

	MemorySignature fRealloc_Signature("53 57 56 48 8D ? ? ? 48 89 CB 48 89 D6 48 85 D2 75 ? 48 83 ? ? 0F 84", Function);
	fRealloc_Internal = (fRealloc*)fRealloc_Signature.ResultAddress;
	if (!fRealloc_Internal)
		std::cout << "Can't find 'Realloc'! " << std::endl;

	MemorySignature fRegisterForm_Signature("53 57 56 48 8D 64 24 ? 48 89 CB 40 88 D6 48 83 BB ? ? ? ? 00 75 ? 48 89 DA 48 8B 0D ? ? ? ? E8", Function);
	fRegisterForm_Internal = (fRegisterForm*)fRegisterForm_Signature.ResultAddress;
	if (!fRegisterForm_Internal)
		std::cout << "Can't find 'RegisterForm'! " << std::endl;

	MemorySignature fConcatStringArray_Signature("55 48 89 E5 48 8D A4 ? ? ? ? ? 48 89 5D ? 48 89 7D ? 48 89 75 ? 4C 89 65 ? 4C 89 6D ? 4C 89 75 ? 4C 89 7D ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 66 45 89 CC", Function);
	fConcatStringArray_Internal = (fConcatStringArray*)fConcatStringArray_Signature.ResultAddress;
	if (!fConcatStringArray_Internal)
		std::cout << "Can't find 'ConcatStringArray'! " << std::endl;

	MemorySignature fGetTGSLines_Signature("48 8D 64 ? ? 48 83 3D ? ? ? ? 00 75 ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 05", Function);
	fGetTGSLines_Internal = (fGetTGSLines*)fGetTGSLines_Signature.ResultAddress;
	if (!fGetTGSLines_Internal)
		std::cout << "Can't find 'GetTGSLines'! " << std::endl;

	MemorySignature fTGUIControl_MsgProc_Button_Signature("53 57 56 48 8D 64 24 ? 48 89 CB 44 89 44 24 ? 44 89 4C 24 ? BE 00 00 00 00  83 FA 01 0F 82 ? ? ? ? 83 EA 01 74 ? 83 EA 01 74 ? 83 EA 1E 0F 84 ? ? ? ? 83 EA 10 0F 84 ? ? ? ? 83 EA 10 0F 84 ? ? ? ? 83 EA 10 0F 84 ? ? ? ? 83 EA 70 0F 84 ? ? ? ? E9 ? ? ? ? 90 48 89 DA 48 8b 0d ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 00 FF 90 ? ? ? ? E9", Function);
	TGUIControl_MsgProc_Button_Internal = (void*)fTGUIControl_MsgProc_Button_Signature.ResultAddress;
	if (!TGUIControl_MsgProc_Button_Internal)
		std::cout << "Can't find 'TGUIControl_MsgProc_Button'! " << std::endl;

	MemorySignature fTGUIControl_MsgProc_Label_Signature("53 57 56 41 54 48 8D 64 24 ? 48 89 CB 44 89 C6 41 BC 00 00 00 00 83 FA 01 0F 82 ? ? ? ? 83 EA 01 74 ? 83 EA", Function);
	TGUIControl_MsgProc_Label_Internal = (void*)fTGUIControl_MsgProc_Label_Signature.ResultAddress;
	if (!TGUIControl_MsgProc_Label_Internal)
		std::cout << "Can't find 'TGUIControl_MsgProc_Button'! " << std::endl;
	
	MemorySignature fSystem__DynArraySetLength_Signature("55 48 89 E5 48 8D A4 24 ? ? ? ? 48 89 5D ? 48 89 7D ? 48 89 75 ? 4C 89 65 ? 4C 89 6D ? 4C 89 75 ? 4C 89 7D ? 48 89 4D ? 48 89 D7 4C 89 45 ? 4C 89 4D ? 4C 89 C8 48 83 38 00", Function);
	fSystem__DynArraySetLength_Internal = (fSystem__DynArraySetLength*)fSystem__DynArraySetLength_Signature.ResultAddress;
	if (!fSystem__DynArraySetLength_Internal)
		std::cout << "Can't find 'System__DynArraySetLength'! " << std::endl;

	MemorySignature fSystem__Realloc_Signature("53 48 8D 64 24 ? 48 89 CB 48 89 D1 FF 15", Function);
	fSystem__Realloc_Internal = (fSystem__Realloc*)fSystem__Realloc_Signature.ResultAddress;
	if (!fSystem__Realloc_Internal)
		std::cout << "Can't find 'fSystem__Realloc'! " << std::endl;
	

	MemorySignature fAtomicSetPointer_Internal_Signature("53 56 48 8D 64 24 ? 48 89 CB 48 89 D6 48 3B 33 74", Function);
	fAtomicSetPointer_Internal = (fAtomicSetPointer*)fAtomicSetPointer_Internal_Signature.ResultAddress;
	if (!fAtomicSetPointer_Internal)
		std::cout << "Can't find 'SetPointerInterlocked_Internal'! " << std::endl;

	
	
	
}
