#include <dllmain.hpp>

/* Wrappers for the game functions will reside here */

struct TGUIFormStructure* createGUITemplate(uint64_t* FormData, const char Caption[], uint64_t AllocNew = 1, uint64_t* ParentForm = 0) {

	UNREFERENCED_PARAMETER(Caption);

	TGUIFormStructure* NewForm = (TGUIFormStructure*)createGUITemplateInternal(FormData, AllocNew, ParentForm);
	if (!NewForm){
		std::cout << "Can't create template form! Caption: " << Caption << std::endl;
		return 0;
	}


	const char CaptionHeader[] = "\x00\x00\x01\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	size_t CaptionHeaderSize = 17;
	size_t CaptionSize = strlen(Caption);
	size_t TempCaptionLocSize = (sizeof(CaptionHeader) + sizeof(size_t)+ 8 + (sizeof(char) * strlen(Caption))) + (sizeof(CaptionHeader) + sizeof(size_t)) + sizeof(uint64_t) + sizeof(uint64_t);


	/** Alloc array for the caption string, 
	*  Explanation:
	* Some types of Strings in Exanima are prepended with header, followed by a string size QWord, followed by a string.
	* We make two of these strings for the fConcatStringArray function, first will be empty with just the header and size and second will contain actual caption. Then we set pointers to them into array and pass array pointer to the function.
	* We need at least two strings, otherwise fConcatStringArray will put 0 at the target location.
	* So fConcatStringArray will itself alloc memory for our string in game. And then game will dealloc header string memory by itself.
	* We just offloading allocing to a game.
	*  TODO:
	* Find a better way of doing this without two strings.
	*/
	//
	char* TempCaptionLoc = new char[TempCaptionLocSize](0);
	memcpy_s(TempCaptionLoc, TempCaptionLocSize, CaptionHeader, CaptionHeaderSize);
	*(size_t*)(TempCaptionLoc + 16) = 0;

	memcpy_s(TempCaptionLoc + 32, TempCaptionLocSize - (CaptionHeaderSize + 16), CaptionHeader, CaptionHeaderSize);
	*(size_t*)(TempCaptionLoc + 48) = CaptionSize;
	memcpy_s(TempCaptionLoc + 56, TempCaptionLocSize - ((CaptionHeaderSize*2) + 24), Caption, CaptionSize);
	
	uint64_t* CaptionPointer = (uint64_t*)(TempCaptionLoc + (56 + CaptionSize));
	*CaptionPointer = (uint64_t)TempCaptionLoc + 56;
	uint64_t* EmptyStringPointer = (uint64_t*)(TempCaptionLoc + (64 + CaptionSize));
	*EmptyStringPointer = (uint64_t)TempCaptionLoc + 24;

	concatStringArrayInternal(&NewForm->pCaption, CaptionPointer, 1, 0);

	delete[] TempCaptionLoc;

	NewForm->pTask = (uint64_t)getTGSLinesInternal();
	NewForm->Style		|= 0x0000B800;
	NewForm->SpcType	|= 0x10;

	return NewForm;
};