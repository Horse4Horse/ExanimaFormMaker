#include <dllmain.hpp>

/* Misc functions for the project
   Like for creating data to assign it later to something in the game.
   Or for function callbacks, custom draw functions, etc.
*/



uint32_t ButtonCallback(TGUIControl* sender) {

	TGUITableView* TableView = (TGUITableView*)sender->Parent->Child[0];

	std::cout << "Control's address: " << std::hex << sender << std::endl;
	std::cout << "Control's caption: " << sender->FCaption << std::endl;

	if (sender->ControlID == 1) {
		std::cout << "Adding an item" << std::endl;
		TableView->Items += 1;
		fSystem__DynArraySetLength_Internal(&TableView->Item, pTGUITableItem_DynArray, 1, &TableView->Items);

		TGUITableItem* item = &TableView->Item[TableView->Items - 1];
		item->Flags = 0;

		if (!(TableView->Items % 2)) //Disable every second item
			item->Flags = 2;

		fSystem__Realloc_Internal(&item->ItemField, sizeof(TGUITableItemField) * TableView->Columns);

		for (int f = 0; f < TableView->Columns; f++) {
			char buff[256];
			snprintf(buff, sizeof(buff), "I %d C %d", TableView->Items, f+1);
			uint64_t str = 0;
			MakeCPString(&str, buff);
			item->ItemField->Field[f] = str;
		}

		sender->Parent->Control[1].State &= 0xFFFFFFEF; //Enable "remove" button

		if (TableView->Items == 1)
			TableView->Highlighted = TableView->Items; //Highlight first created item

		if ((TableView->ItemHgt * (TableView->Items + 1)) > TableView->Height) { // if new amount of items plus header exceeds table size
			TableView->SpcType |= 0x2;
			TableView->ClientSpcY = ((TableView->ItemHgt * TableView->Items) - TableView->Height) + TableView->Height; //Update full scroll size by adding size of elements which do not fit in into a form.
			TableView->VS_Step = static_cast<float>(TableView->Height) / 4 / static_cast<float>(TableView->ClientSpcY); // update scroll step size
		}

	}
	else if (sender->ControlID == 2 && TableView->Items > 0) {
		std::cout << "Removing an item" << std::endl;

		--TableView->Items;
		fSystem__DynArraySetLength_Internal(&TableView->Item, pTGUITableItem_DynArray, 1, &TableView->Items);


		if (TableView->Items == 0) {
			TableView->Highlighted = 0;
			sender->State |= 0x10;  //Disable "remove" button
		}
		else {

			if (TableView->Highlighted > TableView->Items) {
				if (TableView->Item[TableView->Items-1].Flags == 2){  //If last item to highlight is disabled - highlight one before it. Also, "Items-1" because of pointer arithmetic.
					TableView->Highlighted = TableView->Items - 1;
				} else {
					TableView->Highlighted = TableView->Items;
				}
			}

			if ((TableView->ItemHgt * (TableView->Items + 1)) > TableView->Height) { 
				TableView->ClientSpcY = ((TableView->ItemHgt * TableView->Items) - TableView->Height) + TableView->Height;
				TableView->VS_Step = static_cast<float>(TableView->Height) / 4 / static_cast<float>(TableView->ClientSpcY);
			} else {
				TableView->SpcType &= 0xFFFFFFFD; //Disable scroll if amount of items plus header is less than table size
				TableView->ClientSpcY = TableView->Height;
				TableView->VS_Pos = 0;
			}
		}

	}


	return 0; //we must return some code to the handler.
}

