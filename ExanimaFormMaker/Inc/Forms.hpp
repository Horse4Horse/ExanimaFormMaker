/* Define ingame form structures here
*  At first you need to parse their structure
*  TODO
*/

#define CONCATENATE_IMPL(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_IMPL(s1, s2)
#define PADDING(size) std::byte CONCATENATE(pad_, __LINE__)[size]

// Forward declarations to resolve circular dependency within classes
class TGUIForm;
class TGUIControl;


/* -------------------------- Virtual memory tables --------------------------- */
// They contain functions, which could be called without additional signature search.
// Child's VMT structs may not contain some of the parent's functions!

struct TGUIForm_VMT_struct
{
	uint64_t	ObjectSize;
	uint64_t	ObjectSizeInverted;
	void*		pParent_VMT;
	const char*	pName;
	PADDING(32);
	void*		ObjectDynArraysInfo;
	PADDING(24);
	void*		(*Destroy)();
	void*		(*NewInstance)();
	void*		(*FreeInstance)();
	void*		(*SafeCallException)();
	void*		unk1;
	void*		(*AfterConstruction)();		//nullsub
	void*		(*BeforeDestruction)();		//nullsub
	PADDING(sizeof(uint64_t) * 6);
	void*		(*Close)();
	PADDING(sizeof(uint64_t) * 2);
	void*		(*QueryDragDrop)();
	void*		(*DoDragDrop)();
	void*		(*DragDropped)();
	void		(*AssignControls)(void* form, void* Control, int32_t size, bool staticControls);
	void*		(*DragDrop)();			//nullsub
	void*		(*CalcSizes)();
	void*		(*OnScaleUI)();			//nullsub
	void*		(*SetVScroll)();
	void*		(*GetHitCode)();
	void*		(*SetHitCode)();
	void*		unk2;
	void*		(*CursorActivate)();
	void*		(*CursorDeactivate)();
	void*		(*CursorMove)();
	void*		unk3;
	void*		unk4;
	void*		unk5;
	void*		(*TimerActivate)();
	void*		(*FocusTimer)();
	void*		(*SetFocusCode)();
	void*		(*WheelScroll)();
	void*		(*MsgKeyDown)();
	void*		(*Draw)();
	void*		(*DrawPage)();
	void*		(*DrawClient)();
	void*		(*FadeEffect)();
	void*		(*Show)();
	void*		(*Hide)();
	void*		unk6;
	const char	Name[16];
	void*		pCurrentVMT;
};


struct TTextBox_VMT_struct
{
	uint64_t* ObjectSize;
	uint64_t* ObjectSizeInverted;
	void* ppParent_VMT;
	char* pName;
	void* field_20;
	void* field_28;
	void* field_30;
	void* field_38;
	void* field_40;
	void* field_48;
	void* field_50;
	void* field_58;
	void* Destroy;
	void* NewInstance;
	void* FreeInstance;
	void* SafeCallException;
	void* nullsub_5;
	void* anonymous_0;
	void* anonymous_1;
	void* anonymous_2;
	void* field_A0;
	void* field_A8;
	void* field_B0;
	void* field_B8;
	void* field_C0;
	void* Close;
	void* field_D0;
	void* field_D8;
	void* QueryDragDrop;
	void* DoDragDrop;
	void* DragDropped;
	void* AssignControls;
	void* field_100;
	void* CalcSizes;
	void* OnScaleUI;
	void* SetVScroll_;
	void* GetHitCode;
	void* SetHitCode;
	void* field_130;
	void* CursorActivate;
	void* CursorDeactivate;
	void* CursorMove;
	void* field_150;
	void* field_158;
	void* CursorHover;
	void* TimerActivate;
	void* FocusTimer;
	void* SetFocusCode;
	void* WheelScroll;
	void* MsgKeyDown;
	void* Draw;
	void* DrawPage;
	void* DrawClient;
	void* FadeEffect;
	void* Show;
	void* Hide;
	void* field_1C0;
	char		Name[16];
	uint64_t* pCurrentVMT;
};


struct TGUITableView_VMT_struct
{
	uint64_t	ObjectSize;
	uint64_t	ObjectSizeInverted;
	void*		pParent_VMT;
	const char* pName;
	PADDING(12);
	void*		ObjectDynArraysInfo1;
	void*		ObjectDynArraysInfo2;
	PADDING(12);
	void*		(*Destroy)();
	void*		(*NewInstance)();
	void*		(*FreeInstance)();
	void*		(*SafeCallException)();
	PADDING(16);						//nullsubs
	void*		unk1;
	void*		unk2;
	void*		unk3;
	void*		unk4;
	void*		unk5;
	void*		(*Close)();
	void*		unk6;
	void*		unk7;
	void*		(*QueryDragDrop)();
	void*		(*DoDragDrop)();
	void*		(*DragDropped)();
	void		(*AssignControls)(void* form, void* Control, int32_t size, bool staticControls);
	void*		unk8;
	void*		(*CalcSizes)();
	void*		(*OnScaleUI)();
	void*		(*SetVScroll)();
	void*		(*GetHitCode)();
	void*		unk9;
	void*		unk10;
	void*		(*CursorActivate)();
	void*		(*CursorDeactivate)();
	void*		(*CursorMove)();
	void*		unk11;
	void*		unk12;
	void*		(*CursorHover)();
	void*		(*TimerActivate)();
	void*		(*FocusTimer)();
	void*		(*SetFocusCode)();
	void*		(*WheelScroll)();
	void*		(*MsgKeyDown)();
	void*		(*Draw)();
	void*		(*DrawPage)();
	void*		(*DrawClient)();
	void*		(*FadeEffect)();
	void*		(*Show)();
	void*		(*Hide)();
	void*		unk13;
	const char	Name[16];
	void*		pCurrentVMT;
};


/* --------------------------- Objects and Classes --------------------------- */

/*Control class, for buttons, labels, icons, etc. In the future - move to the TGUIForm to make seamless integration
  But beware, after AssignControls() you cant change control class or use alignment features. */
class TGUIControl
{

public:

	//TGUIControl(uint16_t Class, int16_t x, int16_t y, const char* Caption) {	// We are making an array of controls on te stack, we need to provide variables for each one.
	//																			Need a way to allocate mem on the stack w\o providing this class.
	TGUIControl() {
		CntrlClass = 0;
		ControlID = 0;
		px = 0;
		py = 0;
		wdt = 0;
		hgt = 0;
		brd = 0;
		GroupID = 0;
		FCaption = 0;
		Style = 0;
		State = 0;
		Parent = 0;
		OnChange = 0;
		MsgProc = 0;
		Dat0 = 0;
		Dat1 = 0;
		Dat2 = 0;
		Dat3 = 0;
		Dat4 = 0;
		Dat5 = 0;
	};

	uint16_t	CntrlClass;
	uint16_t	ControlID;
	int16_t		px;			// Location values are doubled after assignment.
	int16_t		py;			// They also work as a bitmask auto-align options.
	int16_t		wdt;
	int16_t		hgt;
	int16_t		brd;
	uint16_t	GroupID;
	const char* FCaption;
	int32_t		Style;
	int32_t		State;
	TGUIForm*	Parent;
	void*		OnChange;	// Callback function
	void*		MsgProc;	// Draw function
	uint64_t	Dat0;       // Data can by of any type, but 8 byte aligned.
	uint64_t	Dat1;
	uint64_t	Dat2;
	uint64_t	Dat3;
	uint64_t	Dat4;
	uint64_t	Dat5;

};


// In the future - move to the TGUITableView to make seamless integration
// Dst - pointer to the location of the TableView Column array item
class TGUITableColumn
{
public:
	static TGUITableColumn* Create(TGUITableColumn* Dst, const char* TitleStrig, int ColumnWidth);

	const char*		Title;
	int				Width;
	void*			(*Draw)();		//0 for default string draw
	void*			(*GetString)();
};

class TGUITableItemField
{
public:
	uint64_t		Field[1]; //An array of fields corresponding to the amount of columns  
};

class TGUITableItem
{
public:
	uint32_t		Flags; //Set 0x02 to gray out text with default renderer
	PADDING(4);
	TGUITableItemField* ItemField;
};


/* Main class for everything in the game, just for compatibility */
class TObject {
	//uint64_t* ClassPointer;	// Pointer to the object's class vmt
};


/* Base parent class for every form within the game */
class TGUIForm : public TObject
{

public:
	// Really will be better to use virtual function and then override it on every subform that inherited.
	// But having a virtual function causes __vfptr pointer to appear on the object, which breaks another pointers to the structure
	// So for the game - we always passing correct address to the form
	// But when we interact through object with it's variables(like PosX) - we always write to the object+8 because of that __vfptr at the beginning.
	// So for now let's deal only with the static functions.

	static TGUIForm* Create(const char* Caption, TGUIForm* ParentForm = nullptr);

	//size is (amount of controls)-1 (pointer arithmetic). 
	void AssignControls(void* Control, int32_t size, bool staticControls = false);

	//static void Destroy(TGUIForm* Form);


	/** TODO:
	*	Define bitfield options like "Style", "State", etc.
	*/

	TGUIForm_VMT_struct*	pClassPointer;	// Pointer to the form's Class structure.
	uint32_t		CAp0;				// Last picked up at pos X, actually a word.
	uint32_t		CAp1;				// Last picked up at pos X, actually a word.
	float			HS_Stp;				// For now - unknown usage.
	PADDING(4);
	void*			StoreState;
	int				PosX;
	int				PosY;
	int				Width;
	int				Height;
	int				Border;
	int				BorderEx;
	int				BorderIn;
	int				ClientBtm;
	int				ClientTop;
	int				ClientRgt;
	int				ClientSpcX;			// Full size of scroll
	int				ClientSpcY;			//;
	int				BaseBorder;
	int				MinHeight;
	int				MinWidth;
	uint32_t		Style;				// Border outline type.
	uint8_t			State;
	PADDING(3);
	uint8_t			FormStyle;
	uint8_t			SpcType;			// Type of scroll
	uint8_t			FormStyleEx;		// StateMaster and OutlineType //bit1 - if not set = transparent, if set - transparency decided by "State" at 0x60, bit7 set = disable second line at the header, bit6 set = swap outline to single line // All of this is approximate
	PADDING(5);
	uint32_t		HitCode;
	uint32_t		CaptureCode;
	uint32_t		FocusCode;
	uint8_t			ÑaptureHit;
	uint8_t			FVisible;			// Form is visible, process it's task.
	PADDING(6);
	char*			Caption;
	PADDING(8);
	uint32_t		Controls;			// Amount of controls, -1 is none
	PADDING(4);
	TGUIControl*	Control;			// Pointer to the array of TGUIControl | TODO: declare TGUIControl here, so you would not need to static_cast it later.
	TGUIForm*		Parent;
	uint32_t		Children;			// Amount of childrens, -1 is none
	PADDING(4);
	TGUIForm**		Child;				// Pointer to an array of pointers to child objects
	uint32_t		FormClassID;		// Unknown, size is unknown(at least dword)
	PADDING(4);
	void*			Owner;
	uint32_t		ClntQds;			// Size of "ClntQd", for now usage is unknown, 0 if not defined
	PADDING(4);
	void*			ClntQd;				// Pointer to the DynArray of something unknown for now.
	float			HS_Pos;				// Horizontal position of the scrollbar
	float			VS_Pos;				// Vertical   position of the scrollbar
	float			VS_Step;			// Scrollbar step size
	uint32_t		Hgt;
	uint64_t		BkCol0;				// Unknown if still used
	uint64_t		BkCol1;				// Some of TGSLines goes here.
	PADDING(8);
	uint32_t		Glyph;				// Unknown, mostly -1
	PADDING(4);
	void*			OnClose;
	void*			OnScroll;
	uint8_t			OwnsControls;		// Set 1 if form has controls.
	uint8_t			FreeOnClose;		// Game will dealloc form and it's dependencies on close.
	uint8_t			Closing;
	uint8_t			WantsFree;
	uint8_t			AniState;
	PADDING(3);
	uint32_t		AniTime;
	uint32_t		AniType;
	void*			Task;
	void*			FGUISkin;
	uint32_t		ModalResult;
	PADDING(12);
	uint32_t		VS_TPos;
	uint32_t		VS_TLen;
	uint32_t		HS_TPos;
	uint32_t		HS_TLen;
};

/* Form for editing or presenting text data with formatting and icons support.
*
*/
class TTextBox : public TGUIForm
 {
 public:
	 static TTextBox* Create(const char* Caption, TTextBox* ParentForm = nullptr);



 };

class TGUITableView : public TGUIForm
{

public:
	static TGUITableView* Create(const char* Caption, TGUIForm* ParentForm = nullptr);
	void CopyColumnData(TGUITableColumn* InputData) const;
	void CopyItemData(TGUITableItem* InputData);


	int				Columns;
	PADDING(4);
	TGUITableColumn*	Column;			//TGUITableColumn array
	int				Items;
	PADDING(4);
	TGUITableItem*	Item;				//TGUITableItem array
	int				ItemHgt;
	uint32_t		ItemType;
	int				Highlighted;
	uint32_t		_UNK_184;
	void*			(*OnSelectItem)();
	void*			(*OnDropItem)();
	void*			(*OnOpenItem)();
	void*			(*OnQueryDrop)();
};