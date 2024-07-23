#include <dllmain.hpp>

/* TODO
* Make a better way of defining game funcitons.
*/

/* Ingame forms */
uint64_t* TGUIForm;

/* Ingame functions */
fCreateGUITemplate* createGUITemplateInternal;
fRegisterForm* registerFormInternal;
fConcatStringArray* concatStringArrayInternal;
fGetTGSLines* getTGSLinesInternal;

static BOOL ReadGameMemory(uint64_t Address, char* Buf, SIZE_T Size) {

	SIZE_T bytesRead;
	BOOL success = ReadProcessMemory(hGameProcess, (LPCVOID)(Address), Buf, Size, &bytesRead);

	if (!success || bytesRead != Size) {
		std::cout << "Failed to read memory." << std::endl;
		return 0;
	}
	return 1;
}

struct SignatureStruct* CreateSignatureStruct(const char SignatureString[], uint8_t Type, uint64_t OffsetFromAOB = 0, uint8_t  OpcodeSize = 0) {

	size_t SignatureStringSize = strlen(SignatureString);
	//Allocate memory for the whole struct
	SignatureStruct* StructS = new SignatureStruct[sizeof(SignatureStruct) + (sizeof(char) * (SignatureStringSize+1))]{ 0 }; // Add 1 to allocated area to zero-terminate AOB string so we can use strlen() later.
	if (!StructS) {
		return NULL;
	}

	StructS->OffsetFromAOB = OffsetFromAOB;
	StructS->OpcodeSize = OpcodeSize;
	StructS->Type = Type;
	memcpy_s(StructS->AOB, SignatureStringSize, SignatureString, SignatureStringSize);

	return StructS;
}

static uint64_t  GetAddressFromDisplacementOpcode(uint8_t Type, char opcode[], uint8_t OpcodeSize, uint64_t InstructionPointer) {
	uint32_t Displacement = 0;

	switch (Type) {
	case DisplacementData:
		Displacement = ((uint8_t)opcode[6] << 24) | ((uint8_t)opcode[5] << 16) | ((uint8_t)opcode[4] << 8) | ((uint8_t)opcode[3] << 0); //little-endian format
		break;
	case DisplacementFunction:
		Displacement = ((uint8_t)opcode[4] << 24) | ((uint8_t)opcode[3] << 16) | ((uint8_t)opcode[2] << 8) | (uint8_t)opcode[1];
	}
	uint64_t Address = InstructionPointer + Displacement + OpcodeSize;
	std::cout << "InstructionPointer: " << std::hex << InstructionPointer << std::endl;
	std::cout << "Displacement: " << std::hex << Displacement << std::endl;
	std::cout << "Result address: " << std::hex << Address << std::endl;
	return Address;
}

static uint64_t FindSignature(char* sig) {

	MEMORY_BASIC_INFORMATION mbi{ 0 };
	uint64_t protectFlagsExclude = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

	for (uint64_t i = GameStaticMemoryStartAddress; i < GameStaticMemoryEndAddress - sizeof(sig); i++) {

		if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {

			if (mbi.Protect & protectFlagsExclude || !(mbi.State & MEM_COMMIT) || !mbi.Protect & PAGE_EXECUTE_READ) {
				if (Verbose)
					std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (uint64_t)((uint64_t)mbi.BaseAddress + mbi.RegionSize) << std::endl;
				i += mbi.RegionSize;
				continue; // If address is bad then we don't need to read from it
			}

			if (Verbose)
				std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (uint64_t)((uint64_t)mbi.BaseAddress + mbi.RegionSize) << std::endl;

			uint64_t found = (uint64_t)Sig::find(mbi.BaseAddress, mbi.RegionSize, sig);
			if (Verbose)
				std::cout << "Result: " << std::hex << found << std::endl;
			return found;

			i = (uint64_t)mbi.BaseAddress + mbi.RegionSize;
		}
	}
	std::cout << "Signature not found: " << sig << std::endl;

	return 0;
}


static uint64_t FindObjectBySignatureStruct(SignatureStruct* Struct) {
	uint64_t ObjectSignatureAddress = FindSignature(Struct->AOB);
	if (!ObjectSignatureAddress) {
		std::cout << "Can't find signature: " << Struct->AOB << std::endl;
		return 0;
	}

	uint64_t ObjectAddress = 0;
	ObjectSignatureAddress += Struct->OffsetFromAOB;

	if (Struct->Type != Function) {
		char* OpcodeBytes = new char[Struct->OpcodeSize];

		if (!ReadGameMemory(ObjectSignatureAddress, OpcodeBytes, Struct->OpcodeSize)) {
			delete[] OpcodeBytes;
			return 0;
		}

		ObjectAddress = GetAddressFromDisplacementOpcode(Struct->Type, OpcodeBytes, Struct->OpcodeSize, ObjectSignatureAddress);
		delete[] OpcodeBytes;
	}
	else // If we are searching for function's beginning AOB's - we already found our target. Get the address of signature as address of the function.
	{
		ObjectAddress = ObjectSignatureAddress;
	}

	return ObjectAddress;
}

extern void DefineAllSignatureStructs() {
	SignatureStruct* TGUIFormSignature = CreateSignatureStruct("49 89 C8 BA ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 C3 81 63", DisplacementData, 8, 7);
	TGUIForm = (uint64_t*)FindObjectBySignatureStruct(TGUIFormSignature);
	if (!TGUIForm)
		std::cout << "Can't find 'TGUIForm'! " << std::endl;
	delete[] TGUIFormSignature;

	SignatureStruct* fCreateTemplateFormSignature = CreateSignatureStruct("55 48 89 E5 48 8D 64 24 ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 48 83 7D ? 01 75 ? 48 8B 45 ? 48 8B 55 ? 48 89 C1 FF 52 ? 48 89 45 ? 66 90 48 83 7D ? ? 0F 84 ? ? ? ? 90 90 48 C7 45 ? FF FF FF FF 48 8B 4D ? 31 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B 55", Function);
	createGUITemplateInternal = (fCreateGUITemplate*)FindObjectBySignatureStruct(fCreateTemplateFormSignature);
	if (!createGUITemplateInternal)
		std::cout << "Can't find 'CreateGUITemplate'! " << std::endl;
	delete[] fCreateTemplateFormSignature;

	SignatureStruct* fRegisterFormSignature = CreateSignatureStruct("53 57 56 48 8D 64 24 ? 48 89 CB 40 88 D6 48 83 BB ? ? ? ? 00 75 ? 48 89 DA 48 8B 0D ? ? ? ? E8", Function);
	registerFormInternal = (fRegisterForm*)FindObjectBySignatureStruct(fRegisterFormSignature);
	if (!registerFormInternal)
		std::cout << "Can't find 'RegisterForm'! " << std::endl;
	delete[] fRegisterFormSignature;

	SignatureStruct* fConcatStringArraySignature = CreateSignatureStruct("55 48 89 E5 48 8D A4 ? ? ? ? ? 48 89 5D ? 48 89 7D ? 48 89 75 ? 4C 89 65 ? 4C 89 6D ? 4C 89 75 ? 4C 89 7D ? 48 89 4D ? 48 89 55 ? 4C 89 45 ? 66 45 89 CC", Function);
	concatStringArrayInternal = (fConcatStringArray*)FindObjectBySignatureStruct(fConcatStringArraySignature);
	if (!concatStringArrayInternal)
		std::cout << "Can't find 'ConcatStringArray'! " << std::endl;
	delete[] fConcatStringArraySignature;

	SignatureStruct* fGetTGSLinesSignature = CreateSignatureStruct("48 8D 64 ? ? 48 83 3D ? ? ? ? 00 75 ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 89 05 ? ? ? ? 48 8D 05", Function);
	getTGSLinesInternal = (fGetTGSLines*)FindObjectBySignatureStruct(fGetTGSLinesSignature);
	if (!getTGSLinesInternal)
		std::cout << "Can't find 'GetTGSLines'! " << std::endl;
	delete[] fGetTGSLinesSignature;

}
