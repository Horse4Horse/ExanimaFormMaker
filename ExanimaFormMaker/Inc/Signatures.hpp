/** Header for declaring internal functions and data.
*	Game's internal function typedef prepends with "f"
*	Game's declared function postpends with "Internal" and starts with lowercase letter.
*/

/** Type description for signature structs.
* "DisplacementData" is an opcode like "mov rcx,[1002BAEF0]" when we need to extract address to the data from IP + Offset + OpcodeSize
* "DisplacementFunction" is a "call 100029CC0", so always a +1 from IP.
* "Function" is an pointer to the function's first byte(Address of the function code itself), AoB signatures of this type tends to be very large.
*/
enum SignatureType { DisplacementData, DisplacementFunction, Function };

class MemorySignature {

private:
	uint64_t FindSignature(const std::string& Signature);
	uint64_t GetAddressFromDisplacementOpcode(std::vector<char>& Opcode);
	uint64_t FindObjectBySignature(const std::string& Signature);

public:
	MemorySignature(const std::string& SignatureString, SignatureType Type, uint64_t OffsetFromAOB = 0, uint8_t  OpcodeSize = 0);
	
	uint64_t OffsetFromAOB;        // Actual opcode offset from the signature's first byte. (Signature + Offset = Target instruction)
	uint64_t InstructionPointer;   // Address of the IP to calculate "target address = this->InstructionPointer + Displacement + OpcodeSize"
	uint64_t ResultAddress;		   //
	uint8_t  OpcodeSize;           // Length of the opcode in bytes.
	SignatureType Type;            // Type of the signature. For determining where displacement offset is in the opcode bytes. Refer to the GetAddressFromDisplacementOpcode() for info.
};

/* Typedefs for the ingame functions
* Better to make a wrappers for them. But they can still be used on their own.
** TODO
* A better way of this crap
* So an intellisense will work for the functions we are actually use, like "registerFormInternal", not for typedefs.
*/

/// <summary>In-game function to alloc heap regions and make Code Page aware strings.
/// <para>Only the usable parameters described, the rest of them are undefined or unused and can be (maybe) anything.</para></summary>
/// <param name="Dst">= Address to where set a pointer to a new heap.</param>
/// <param name="String">= Address of a string.</param>
/// <param name="MaxLengthMinusOne">= Maximum possible length of a string ~MINUS ONE~. Weird implementation. Can be a normal string length or, for a large possible strings - a limit, so in conjuction with the <c>CalcLength</c> will be calculated real size of the string and no excess memory will be copied.</param>
/// <param name="OEMCP">= Can be "0"; "1"; or "Code Page" int. With 0 will be used CP got by "GetACP" command for the whole game instance. With 1 will be used "GetOEMCP" function. Or with any other value you can set Code Page by yourself. (0 is mainly used in the game).</param>
typedef void* __stdcall fMakeCPString(uint64_t* Dst, const char* String, size_t MaxLengthMinusOne, uint16_t OEMCP, uint64_t unk1, uint64_t unk2, uint64_t unk3, uint64_t unk4, BOOL CalcLength);

/// <summary>In-game memory management function(Fancy wrapper for <c>HeapAlloc</c>).
/// <para>You still need to use it to dealloc memory if it is not parent-dependent.</para>
/// <para>Be aware of the value of the pointer - this function has access to a Whole game memory.</para>
/// <para>Almost identical to a <c>std::realloc</c></para></summary>
/// <param name="ptr">= Pointer to a memory location on which you want to take operation on. Can be an address or a nullptr.
/// In case of nullptr new memory will be allocated with the given size.</param>
/// <param name="new_size">= New size of the allocated memory. 0 will free the memory at the pointer.</param>
typedef uint64_t* __stdcall fRealloc(void* ptr, size_t new_size);

/// <param name="FormData">= Address of GTUIForm_vmt or existing form to set some of it's values to default.</param>
/// <param name="AllocNew">= Flag for telling the game to allocate memory for our form (If we creating one for the first time)</param>
/// <param name="ParentForm">= Pointer to the form which will be parent(For things like text boxes, to put them into the TGUIForm)</param>
typedef uint64_t* __stdcall fCreateGUITemplate(uint64_t* FormData, uint64_t AllocNew, uint64_t* ParentForm);

/// <param name="Form">= Pointer to the created GUI.</param>
/// <param name="RegisterAtAssignedTGUITask">= If true then TGUITask set as address Form+0xA0 will be used for registering. Otherwise the main TGUITask will be used.</param>
typedef void __stdcall fRegisterForm(void* Form, uint64_t RegisterAtAssignedTGUITask);

/// <param name="Dst">= Address, where pointer to the result string will be set</param>
/// <param name="Src">= Array of (StringHeader)+Size+String</param>
/// <param name="Amount">= Amount of elements in the array, zero-based numbering. With amount less than 1 = "0" will be written to the Dst.</param>
/// <param name="UseOEMCP">= if set - game will call GetOEMCP() instead of relying on GetACP() which was called on game init. Almost(?) all the time is set to 0.</param>
typedef void __stdcall fConcatStringArray(uint64_t* Dst, uint64_t* Src, uint64_t Amount, uint16_t UseOEMCP);

//  Gives pointer to TGSLines. If it does not exists - creates one and gives pointer to it.
typedef uint64_t* __stdcall fGetTGSLines(void);



/* Ingame forms */
extern uint64_t* TGUIForm_vmt; //This is an ingame array of pointers to functions like "Create"



/* Ingame functions */
extern fMakeCPString* makeCPStringInternal;
extern fRealloc* reallocInternal;
extern fCreateGUITemplate* createGUITemplateInternal;
extern fRegisterForm* registerFormInternal;
extern fConcatStringArray* concatStringArrayInternal;
extern fGetTGSLines* getTGSLinesInternal;


static BOOL ReadGameMemory(uint64_t Address, std::vector<char>& Buf);
