#include <dllmain.hpp>

/* Wrappers for the game functions will reside here */

void* MakeCPString(uint64_t* Dst, const char* String, size_t MaxLengthMinusOne, uint16_t OEMCP = 0, BOOL CalcLength = TRUE)
{
	makeCPStringInternal(Dst, String, MaxLengthMinusOne, OEMCP, (uint64_t)0, (uint64_t)0, (uint64_t)0, (uint64_t)0, CalcLength);
	return nullptr;
};