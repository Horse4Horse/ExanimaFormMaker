#include <dllmain.hpp>

/* Ingame forms */
uint64_t* TGUIForm_vmt;


/* Ingame functions */
fMakeCPString* makeCPStringInternal;
fRealloc* reallocInternal;
fCreateGUITemplate* createGUITemplateInternal;
fRegisterForm* registerFormInternal;
fConcatStringArray* concatStringArrayInternal;
fGetTGSLines* getTGSLinesInternal;



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
	MemorySignature fMakeCPString_Signature("55 48 89 E5 48 8D 64 24 ? 48 89 5D D8 48 89 7D E0 48 89 75 E8 4C 89 65 F0 4C 89 6D F8 48 89 CE 48 89 D7 4D 89 C4 66 45 89 CD", Function);
	makeCPStringInternal = (fMakeCPString*)fMakeCPString_Signature.ResultAddress;
	if (!makeCPStringInternal)
	{
		std::cout << "Can't find 'MakeCPString'! Trying to find by reference... " << std::endl;
		
		MemorySignature fMakeCPString_Signature_ByRef("48 8D ? ? 48 8D ? ? E8 ? ? ? ? 48 8B 45 ? 48 89 45 ? 48 8D 05", DisplacementFunction, 8);
		makeCPStringInternal = (fMakeCPString*)fMakeCPString_Signature_ByRef.ResultAddress;

		if (makeCPStringInternal)
		{
			std::cout << "'MakeCPString' successfully found by reference! " << std::endl;
		}
		else {
			std::cout << "'MakeCPString' was not found! " << std::endl;
		}
	}

	MemorySignature fRealloc_Signature("53 57 56 48 8D ? ? ? 48 89 CB 48 89 D6 48 85 D2 75 ? 48 83 ? ? 0F 84", Function);
	reallocInternal = (fRealloc*)fRealloc_Signature.ResultAddress;
	if (!reallocInternal)
		std::cout << "Can't find 'Realloc'! " << std::endl;

	MemorySignature TGUIFormVMT_Signature("49 89 C8 BA ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 C3 81 63", DisplacementData, 8, 7);
	TGUIForm_vmt = (uint64_t*)TGUIFormVMT_Signature.ResultAddress;
	if (!TGUIForm_vmt)
		std::cout << "Can't find 'TGUIForm'! " << std::endl;

	MemorySignature fCreateTemplateForm_Signature("55 48 89 E5 48 8D 64 24 ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 48 83 7D ? 01 75 ? 48 8B 45 ? 48 8B 55 ? 48 89 C1 FF 52 ? 48 89 45 ? 66 90 48 83 7D ? ? 0F 84 ? ? ? ? 90 90 48 C7 45 ? FF FF FF FF 48 8B 4D ? 31 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B 55", Function);
	createGUITemplateInternal = (fCreateGUITemplate*)fCreateTemplateForm_Signature.ResultAddress;
	if (!createGUITemplateInternal)
		std::cout << "Can't find 'CreateGUITemplate'! " << std::endl;


	MemorySignature fRegisterForm_Signature("53 57 56 48 8D 64 24 ? 48 89 CB 40 88 D6 48 83 BB ? ? ? ? 00 75 ? 48 89 DA 48 8B 0D ? ? ? ? E8", Function);
	registerFormInternal = (fRegisterForm*)fRegisterForm_Signature.ResultAddress;
	if (!registerFormInternal)
		std::cout << "Can't find 'RegisterForm'! " << std::endl;


	MemorySignature fConcatStringArray_Signature("55 48 89 E5 48 8D A4 ? ? ? ? ? 48 89 5D ? 48 89 7D ? 48 89 75 ? 4C 89 65 ? 4C 89 6D ? 4C 89 75 ? 4C 89 7D ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 66 45 89 CC", Function);
	concatStringArrayInternal = (fConcatStringArray*)fConcatStringArray_Signature.ResultAddress;
	if (!concatStringArrayInternal)
		std::cout << "Can't find 'ConcatStringArray'! " << std::endl;

	MemorySignature fGetTGSLines_Signature("48 8D 64 ? ? 48 83 3D ? ? ? ? 00 75 ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 05", Function);
	getTGSLinesInternal = (fGetTGSLines*)fGetTGSLines_Signature.ResultAddress;
	if (!getTGSLinesInternal)
		std::cout << "Can't find 'GetTGSLines'! " << std::endl;

}
