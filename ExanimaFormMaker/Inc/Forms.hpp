/* Define ingame form structures here
*  At first you need to parse their structure
*  TODO
*/

/** Basic form.
*  TODO:
* Define bitfield options like "Style", "State", etc.
*/
struct TGUIFormStructure {
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
	uint64_t pCaption;
	uint8_t	  padding7[8];
	uint32_t  Controls;			// Amount of controls, -1 is none
	uint8_t	  padding8[4];
	uint64_t pControl;			// Pointer to the PAGUIControl structure
	uint64_t pParent;			// Unknown consistency, TGSLines?
	uint32_t  Children;			// Amount of childrens, -1 is none
	uint8_t	  padding9[4];
	uint64_t pChild;			// Pointer to DynArray of child objects
	uint32_t  FormClassID;		// Unknown, size is unknown(at least dword)
	uint8_t	  padding10[4];
	uint64_t  Owner;			// Unknown, size unknown, most likely a pointer.
	uint32_t  ClntQds;			// Size of "ClntQd", for now usage is unknown, 0 if not defined
	uint8_t	  padding11[4];
	uint64_t pClntQd;			// Pointer to the DynArray of something unknown for now.
	float     HS_Pos;			// Horizontal position of the scrollbar
	float     VS_Pos;			// Vertical   position of the scrollbar
	float     VS_Step;			// Scrollbar step size
	uint8_t	  padding12[4];
	uint64_t  BkCol0;			// Unknown if still used
	uint64_t  BkCol1;			// Some of TGSLines goes here.
	uint8_t	  padding13[8];
	uint32_t  Glyph;			// Unknown, mostly -1
	uint8_t	  padding14[4];
	uint64_t pOnClose;
	uint64_t pOnScroll;
	uint8_t	  OwnsControls;		// Set 1 if form has controls.
	uint8_t	  FreeOnClose;		// Game will dealloc form and it's dependencies on close.
	uint8_t	  Closing;
	uint8_t	  WantsFree;
	uint8_t	  AniState;
	uint8_t	  padding15[3];
	uint64_t  AniTime;			// For both - unknown meaning, type, size and usage.
	uint64_t  AniType;			//
	uint64_t pTask;				// TGUITask goes here.
	uint32_t  ModalResult;		// Control element may set this dword.
	uint8_t	  padding16[28];	// May contain some additional rare variables. Previously used to contain some of scrollbar positions, maybe it is still true.
};