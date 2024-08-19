#include <dllmain.hpp>

TGUIForm* TGUIForm::Create(const char* Caption, TGUIForm* ParentForm)
{
	if (TGUIForm_vmt)
	{
		TGUIForm* NewForm = (TGUIForm*)createGUITemplateInternal(TGUIForm_vmt, 1, (uint64_t*)ParentForm);
		if (!NewForm) {
			std::cout << "Can't create TGUIForm! Caption: " << Caption << std::endl;
			return nullptr;
		}

		MakeCPString((uint64_t*)&NewForm->pCaption, Caption, strlen(Caption) - 1, FALSE, TRUE); // You need to pass an address of a pCaption pointer of the form

		NewForm->pTask = getTGSLinesInternal();
		NewForm->Style |= 0xB800;
		NewForm->FormStyle = 0x22;
		NewForm->FormStyleEx = 0x03;
		NewForm->SpcType = 0x90;

		std::cout << "Form created: " << Caption << " " << std::hex << NewForm << std::endl;

		return NewForm; // Game will handle dealloc by Newform->FreeOnClose.
	};
	return nullptr;
}

/** TODO: Define all basic TGUIForm functions and wrap them.
void TGUIForm::Destroy(TGUIForm* Form)
{

}
*/


/* TODO */
TTextForm* TTextForm::Create(const char* Caption, TGUIForm* ParentForm)
{
	if (TGUIForm_vmt)
	{
		TTextForm* NewForm = (TTextForm*)createGUITemplateInternal(TGUIForm_vmt, 1, (uint64_t*)ParentForm);
		if (!NewForm)
		{
			std::cout << "Can't create TGUIForm! Caption: " << Caption << std::endl;
			return nullptr;
		}

		// Old style, was offloaded to specialized game function. 
		size_t CaptionLength = strlen(Caption) + 1;
		uint64_t StringHeader[3] = { 0x0000000000010000 , 0xFFFFFFFFFFFFFFFF, CaptionLength };			// Make an array of header + length of a caption
		size_t StringHeaderSize = sizeof(StringHeader);

		uint64_t addr = 0;
		NewForm->pCaption = reallocInternal(&addr, CaptionLength + StringHeaderSize);					 // Set pointer to allocated memory to a pCaption
		if (!NewForm->pCaption)
		{
			std::cout << "Can't alloc memory for the form's Caption! Caption string: " << Caption << std::endl;
			/**TODO:
			* dealloc form memory through a game function.
			*/
			return nullptr;
		}
		memcpy_s(NewForm->pCaption, CaptionLength + StringHeaderSize, StringHeader, StringHeaderSize);	 // Copy caption header to a memory
		memcpy_s((uint8_t*)NewForm->pCaption + StringHeaderSize, CaptionLength, Caption, CaptionLength); // Copy Caption after a header
		NewForm->pCaption = (uint64_t*)((uint8_t*)NewForm->pCaption + StringHeaderSize);				 // Change pointer so it will point to the actual string and not the header.

		NewForm->pTask = getTGSLinesInternal();
		NewForm->Style |= 0x0000B800;
		NewForm->SpcType |= 0x10;

		return NewForm; // Game will handle dealloc by Newform->FreeOnClose.
	};
	return nullptr;
}