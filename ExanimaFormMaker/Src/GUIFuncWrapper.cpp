#include <dllmain.hpp>

/* Wrappers for the game functions will reside here */


uint64_t* createGUITemplate(uint64_t* FormData, const char FormHeader[], uint64_t AllocNew, uint64_t* ParentForm) {

	UNREFERENCED_PARAMETER(FormHeader);

	uint64_t* NewForm = createGUITemplateInternal(FormData, AllocNew, ParentForm);
	/* TODO
	* passing form header through "fConcatStringArray" with additional empty string
	* Then the game will manage header alloc and dealloc on close.
	* 
	*/
	return NewForm;
};