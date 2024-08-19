/* Define ingame form structures here
*  At first you need to parse their structure
*  TODO
*/

/** Basic form.
*  TODO:
* Define bitfield options like "Style", "State", etc.
*/
 struct TGUIFormStructure
 {
	uint64_t* ClassPointer;		// Pointer to the form's Class structure.
	uint32_t  CAp0;				// Last picked up at pos X, actually a word.
	uint32_t  CAp1;				// Last picked up at pos X, actually a word.
	float     HS_Stp;			// For now - unknown usage.
	uint8_t	  padding0[4];
	uint32_t  StoreState;
	uint8_t	  padding1[4];
	uint32_t  PosX;
	uint32_t  PosY;
	uint32_t  Width;
	uint32_t  Height;
	uint32_t  Border;
	uint32_t  BorderEx;
	uint32_t  BorderIn;
	uint32_t  ClientBtm;
	uint32_t  ClientTop;
	uint32_t  ClientRgt;
	uint32_t  ClientSpcX;		// Full size of scroll
	uint32_t  ClientSpcY;
	uint32_t  BaseBorder;
	uint32_t  MinHeight;
	uint32_t  MinWidth;
	uint32_t  Style;			// Border outline type.
	uint8_t   State;
	uint8_t	  padding3[3];
	uint8_t	  FormStyle;
	uint8_t	  SpcType;			// Type of scroll
	uint8_t	  FormStyleEx;		// StateMaster and OutlineType //bit1 - if not set = transparent, if set - transparency decided by "State" at 0x60, bit7 set = disable second line at the header, bit6 set = swap outline to single line // All of this is approximate
	uint8_t	  padding4[1];
	uint32_t  HitCode;			// For now - unknown usage. //5 bit - disable dragging by header, 8 bit = draw internals outside of parent window bounds
	uint32_t  CaptureCode;
	uint32_t  FocusCode;
	uint8_t	  padding5[4];
	uint8_t   CaptureHit;
	uint8_t   FVisible;			// Form is visible, process it's task.
	uint8_t	  padding6[6];
	uint64_t* pCaption;
	uint8_t	  padding7[8];
	uint32_t  Controls;			// Amount of controls, -1 is none
	uint8_t	  padding8[4];
	uint64_t* pControl;			// Pointer to the PAGUIControl structure
	uint64_t* pParent;			// Unknown consistency, TGSLines?
	uint32_t  Children;			// Amount of childrens, -1 is none
	uint8_t	  padding9[4];
	uint64_t* pChild;			// Pointer to DynArray of child objects
	uint32_t  FormClassID;		// Unknown, size is unknown(at least dword)
	uint8_t	  padding10[4];
	uint64_t  Owner;			// Unknown, size unknown, most likely a pointer.
	uint32_t  ClntQds;			// Size of "ClntQd", for now usage is unknown, 0 if not defined
	uint8_t	  padding11[4];
	uint64_t* pClntQd;			// Pointer to the DynArray of something unknown for now.
	float     HS_Pos;			// Horizontal position of the scrollbar
	float     VS_Pos;			// Vertical   position of the scrollbar
	float     VS_Step;			// Scrollbar step size
	uint8_t	  padding12[4];
	uint64_t  BkCol0;			// Unknown if still used
	uint64_t  BkCol1;			// Some of TGSLines goes here.
	uint8_t	  padding13[8];
	uint32_t  Glyph;			// Unknown, mostly -1
	uint8_t	  padding14[4];
	uint64_t* pOnClose;
	uint64_t* pOnScroll;
	uint8_t	  OwnsControls;		// Set 1 if form has controls.
	uint8_t	  FreeOnClose;		// Game will dealloc form and it's dependencies on close.
	uint8_t	  Closing;
	uint8_t	  WantsFree;
	uint8_t	  AniState;
	uint8_t	  padding15[3];
	uint64_t  AniTime;			// For both - unknown meaning, type, size and usage.
	uint64_t  AniType;			//
	uint64_t* pTask;				// TGUITask goes here.
	uint32_t  ModalResult;		// Control element may set this dword.
	uint8_t	  padding16[28];	// May contain some additional rare variables. Previously used to contain some of scrollbar positions, maybe it is still true.
};



/* Main class for everything in the game */
class TObject {
	//uint64_t* ClassPointer;	// Pointer to the object's class vmt
};

/* Virtual Memory Table of the TGUIForm class
* Contains functions, pointers and it is read only.
*/
struct TGUIForm_VMT 
{
	uint64_t	ObjectSize;
	uint64_t	ObjectSizeInverted;
	uint64_t*	ppParent_VMT;
	char*		pName;
	uint64_t	padding1[4];
	uint64_t*	ObjectDynArraysInfo;
	uint64_t	padding2[3];
	uint64_t*	Destroy;
	uint64_t*	NewInstance;
	uint64_t*	FreeInstance;
	uint64_t*	SafeCallException;
	uint64_t*	unk1;
	uint64_t*   AfterConstruction;	//nullsub
	uint64_t	padding3[10];
	uint64_t*	QueryDragDrop;
	uint64_t*	DoDragDrop;
	uint64_t*	DragDropped;
	uint64_t*	AssignControls;
	uint64_t*	DragDrop;			//nullsub
	uint64_t*	CalcSizes;
	uint64_t*	OnScaleUI;			//nullsub
	uint64_t*	SetVScroll;
	uint64_t*	GetHitCode;
	uint64_t*	SetHitCode;
	uint64_t*	unk2;
	uint64_t*	CursorActivate;
	uint64_t*	CursorDeactivate;
	uint64_t*	CursorMove;
	uint64_t*	unk3;
	uint64_t*	unk4;
	uint64_t*	unk5;
	uint64_t*	TimerActivate;
	uint64_t*	FocusTimer;
	uint64_t*	SetFocusCode;
	uint64_t*	WheelScroll;
	uint64_t*	MsgKeyDown;
	uint64_t*	Draw;
	uint64_t*	DrawPage;
	uint64_t*	DrawClient;
	uint64_t*	FadeEffect;
	uint64_t*	Show;
	uint64_t*	Hide;
	uint64_t*	unk6;
	char		Name[16];
	uint64_t* pCurrentVMT;
};

/* Base class for all forms */
class TGUIForm : TObject
{


public:
	// Really will be better to use virtual function and then override it on every subform that inherited.
	// But having a virtual function causes __vfptr pointer to appear on the object, which breaks another pointers to the structure
	// So for the game - we always passing correct address to the form
	// But when we interact through object with it's variables(like PosX) - we always write to the object+8 because of that __vfptr at the beginning.
	// So for now let's deal only with the static functions.

	static TGUIForm* Create(const char* Caption, TGUIForm* ParentForm = nullptr);

	//static void Destroy(TGUIForm* Form);

	/** Basic form.
	*  TODO:
	* Define bitfield options like "Style", "State", etc.
	*/
public:
	TGUIForm_VMT* VMT;
	uint32_t	CAp0;			// Last picked up at pos X, actually a word.
	uint32_t	CAp1;			// Last picked up at pos X, actually a word.
	float		HS_Stp;			// Step size of the Horizontal scrollbar
	uint8_t		padding0[4];
	uint32_t	StoreState;
	uint8_t		padding1[4];
	uint32_t	PosX;
	uint32_t	PosY;
	uint32_t	Width;
	uint32_t	Height;
	uint32_t	Border;
	uint32_t	BorderEx;
	uint32_t	BorderIn;
	uint32_t	ClientBtm;
	uint32_t	ClientTop;
	uint32_t	ClientRgt;
	uint32_t	ClientSpcX;		// Full size of scroll
	uint32_t	ClientSpcY;
	uint32_t	BaseBorder;
	uint32_t	MinHeight;
	uint32_t	MinWidth;
	uint32_t	Style;			// Border outline type.
	uint32_t	State;
	uint8_t		FormStyle;
	uint8_t		SpcType;		// Type of scroll
	uint8_t		FormStyleEx;	// StateMaster and OutlineType //bit1 - if not set = transparent, if set - transparency decided by "State" at 0x60, bit7 set = disable second line at the header, bit6 set = swap outline to single line // All of this is approximate
	uint8_t		padding4[1];
	uint32_t	HitCode;		// For now - unknown usage. //5 bit - disable dragging by header, 8 bit = draw internals outside of parent window bounds
	uint32_t	CaptureCode;
	uint32_t	FocusCode;
	uint8_t		padding5[4];
	uint8_t		CaptureHit;
	uint8_t		FVisible;		// Form is visible, process it's task.
	uint8_t		padding6[6];
	uint64_t*   pCaption;
	uint8_t		padding7[8];
	uint32_t	Controls;		// Amount of controls, -1 is none
	uint8_t		padding8[4];
	uint64_t*	pControl;		// Pointer to the PAGUIControl structure
	uint64_t*	pParent;		// Unknown consistency, TGSLines?
	uint32_t	Children;		// Amount of childrens, -1 is none
	uint8_t		padding9[4];
	uint64_t*	pChild;			// Pointer to DynArray of child objects
	uint32_t	FormClassID;	// Unknown, size is unknown(at least dword)
	uint8_t		padding10[4];
	uint64_t	pOwner;			// Unknown, size unknown, most likely a pointer.
	uint32_t	ClntQds;		// Size of "ClntQd", for now usage is unknown, 0 if not defined
	uint8_t		padding11[4];
	uint64_t*	pClntQd;		// Pointer to the DynArray of something unknown for now.
	float		HS_Pos;			// Horizontal position of the scrollbar
	float		VS_Pos;			// Vertical   position of the scrollbar
	float		VS_Stp;			// Step size of the Vertical scrollbar
	uint8_t		padding12[4];
	uint64_t	BkCol0;			// Unknown if still used
	uint64_t	BkCol1;			// Some of TGSLines goes here.
	uint8_t		padding13[8];
	uint32_t	Glyph;			// Unknown, mostly -1
	uint8_t		padding14[4];
	uint64_t*	pOnClose;
	uint64_t*	pOnScroll;
	uint8_t		OwnsControls;	// Set 1 if form has controls.
	uint8_t		FreeOnClose;	// Game will dealloc form and it's dependencies on close.
	uint8_t		Closing;
	uint8_t		WantsFree;
	uint8_t		AniState;
	uint8_t		padding15[3];
	uint32_t	AniTime;		// For both - unknown meaning, type, size and usage.
	uint8_t		padding16[4];
	uint32_t	AniType;
	uint8_t		padding17[4];
	uint64_t*	pTask;			// TGUITask goes here.
	uint32_t	ModalResult;	// Control element may set this dword.
	uint8_t		padding18[28];	// May contain some additional rare variables. Previously used to contain some of scrollbar positions, maybe it is still true.

};

/* TODO */
 class TTextForm : TGUIForm
 {
 public:
	 static TTextForm* Create(const char* Caption, TGUIForm* ParentForm = nullptr);
 };