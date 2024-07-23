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

struct SignatureStruct {
	uint64_t OffsetFromAOB;                // Actual opcode offset from the signature's first byte. (Signature + Offset = Target instruction)
	uint8_t  OpcodeSize;                   // Length of the opcode in bytes.
	uint8_t  Type;                         // Type of the signature. For determining where displacement offset is in the opcode bytes. Refer to the GetAddressFromDisplacementOpcode() for info.
	/* Struct hack for C++ new()[] */
	char AOB[1];                           // Signature string.
};


/* Typedefs for the ingame functions
** TODO
* A better way of this crap
* So an intellisense will work for the functions we are actually use, like "registerFormInternal", not for typedefs.
*/

/// <param name="FormData">= Game memory address for GUI form structure (like TGUIForm)</param>
/// <param name="AllocNew">= Flag for telling the game to allocate memory for our form (If we creating one for the first time)</param>
/// <param name="ParentForm">= Pointer to the form which will be parent(For things like text boxes, to put them into the TGUIForm)</param>
typedef uint64_t* __stdcall fCreateGUITemplate(uint64_t* FormData, uint64_t AllocNew, uint64_t* ParentForm);

/// <param name="Form">= Pointer to the created GUI.</param>
/// <param name="RegisterAtAssignedTGUITask">= If true then TGUITask set as address Form+0xA0 will be used for registering. Otherwise the main TGUITask will be used.</param>
typedef void __stdcall fRegisterForm(uint64_t* Form, uint64_t RegisterAtAssignedTGUITask);

/// <param name="Dst">= Address, where pointer to the result string will be set</param>
/// <param name="Src">= Array of (StringHeader)+Size+String</param>
/// <param name="Amount">= Amount of elements in the array, zero-based numbering. With amount less than 1 = "0" will be written to the Dst.</param>
/// <param name="UseOEMCP">= if set - game will call GetOEMCP() instead of relying on GetACP() which was called on game init. Almost(?) all the time is set to 0.</param>
typedef void __stdcall fConcatStringArray(uint64_t* Dst, uint64_t* Src, uint64_t Amount, uint16_t UseOEMCP);

//  Gives pointer to TGSLines. If it does not exists - creates one and gives pointer to it.
typedef uint64_t* __stdcall fGetTGSLines(void);

/* Ingame forms */
extern uint64_t* TGUIForm;

/* Ingame functions */
extern fCreateGUITemplate* createGUITemplateInternal;
extern fRegisterForm* registerFormInternal;
extern fConcatStringArray* concatStringArrayInternal;
extern fGetTGSLines* getTGSLinesInternal;


static BOOL ReadGameMemory(uint64_t Address, char* Buf, SIZE_T Size);

static uint64_t FindSignature(char* sig, int MemWritable);
static uint64_t GetAddressFromDisplacementOpcode(uint8_t Type, char opcode[], uint8_t OpcodeSize, uint64_t InstructionPointer);
static uint64_t FindObjectBySignatureStruct(SignatureStruct* Struct);
struct SignatureStruct* CreateSignatureStruct(const char SignatureString[], uint8_t Type, uint64_t OffsetFromAOB, uint8_t  OpcodeSize);

