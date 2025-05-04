#include <dllmain.hpp>



TGUIForm* TGUIForm::Create(const char* Caption, TGUIForm* ParentForm)
{
	if (pTGUIForm_vmt)
	{
		TGUIForm* NewForm = (TGUIForm*)fTGUIForm__Create_Internal(pTGUIForm_vmt, 1, (uint64_t*)ParentForm);
		if (!NewForm) {
			std::cout << "Can't create TGUIForm! Caption: " << Caption << std::endl;
			return nullptr;
		}

		MakeCPString((uint64_t*)&NewForm->Caption, Caption); // You need to pass an address of a Caption of the form

		NewForm->FGUISkin		= fGetTGSLines_Internal();
		NewForm->Style			|= 0xB800;
		NewForm->FormStyle		= 0x22;
		NewForm->FormStyleEx	= 0x03;
		NewForm->SpcType		= 0x90;

		std::cout << "Form created: " << Caption << " " << std::hex << NewForm << std::endl;

		return NewForm; // Game will handle dealloc by Newform->FreeOnClose.
	};
	return nullptr;
}

void TGUIForm::AssignControls(void* ControlArray, int32_t size, bool staticControls)
{
	if (ControlArray)
	{
		this->pClassPointer->AssignControls(this, ControlArray, size, staticControls);
	}
}

/** TODO: Define all basic TGUIForm functions and wrap them.
void TGUIForm::Destroy(TGUIForm* Form)
{

}
*/



TTextBox* TTextBox::Create(const char* Caption, TTextBox* ParentForm)
{
	if (pTTextBox_vmt)
	{
		TTextBox* NewForm = (TTextBox*)fTTextBox__Create_Internal(pTTextBox_vmt, 1, (uint64_t*)ParentForm);
		if (!NewForm) {
			std::cout << "Can't create TTextBox! Caption: " << Caption << std::endl;
			return nullptr;
		}

		if (Caption)
			MakeCPString((uint64_t*)&NewForm->Caption, Caption);

		NewForm->FGUISkin		= fGetTGSLines_Internal();
		NewForm->Style			|= 0xB800;
		NewForm->FormStyle		= 0x22;
		NewForm->FormStyleEx	= 0x03;
		NewForm->SpcType		= 0x90;

		std::cout << "Form created: " << Caption << " " << std::hex << NewForm << std::endl;

		return NewForm;
	};
	return nullptr;
}


TGUITableView* TGUITableView::Create(const char* Caption, TGUIForm* ParentForm)
{
	if (pTGUITableView_vmt)
	{
		TGUITableView* NewForm = (TGUITableView*)fTGUITableView__Create_Internal(pTGUITableView_vmt, 1, (uint64_t*)ParentForm);
		if (!NewForm) {
			std::cout << "Can't create TTextBox! Caption: " << Caption << std::endl;
			return nullptr;
		}

		if (Caption)
			MakeCPString((uint64_t*)&NewForm->Caption, Caption);

		NewForm->FGUISkin = fGetTGSLines_Internal();

		std::cout << "Form created: " << Caption << " " << std::hex << NewForm << std::endl;

		return NewForm;
	};
	return nullptr;
}

void TGUITableView::CopyColumnData(TGUITableColumn* InputData) const
{
	memcpy_s(Column, sizeof(TGUITableColumn) * Columns, InputData, sizeof(TGUITableColumn) * Columns);
}

void TGUITableView::CopyItemData(TGUITableItem* InputData)
{
	for (size_t i = 0; i < Items; ++i) {
		
		Item[i].Flags = InputData[i].Flags;

		if (!realloc(&Item[i].ItemField, sizeof(TGUITableItemField) * Columns))
			std::cout << "CopyFieldData error: Can't allocate mem for item fields! Caption:  " << Caption << " Address: " << std::hex << this << std::endl;

		for (size_t c = 0; c < Columns; ++c) {
			Item[i].ItemField[c] = InputData[i].ItemField[c];
		}
	}
}

// Get rid of warnings

TGUITableColumn* TGUITableColumn::Create(TGUITableColumn* Dst, const char* TitleStrig, int ColumnWidth)
{
	TGUITableColumn* column = Dst;
	MakeCPString(&column->Title, TitleStrig);
	column->Width = ColumnWidth;
	column->Draw = 0;
	column->GetString = 0;
	return column;
}
