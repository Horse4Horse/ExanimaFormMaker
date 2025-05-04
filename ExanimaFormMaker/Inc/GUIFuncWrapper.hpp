
//Allocates memory, makes a copy of the string according to Code Page, sets pointer to that string at Dst
// got some errors while trying to call MakeCPString at a neighboring locations
// MakeCPString(Dst)
// MakeCPString(Dst+8)
// Looks like a limit is 32 bytes? You can always set a pointer to stack and copy it to the target location.
extern void* MakeCPString(void* Dst, const char* String, uint16_t OEMCP = 0, BOOL CalcLength = TRUE);