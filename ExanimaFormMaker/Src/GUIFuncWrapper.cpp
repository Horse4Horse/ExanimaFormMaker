#include <dllmain.hpp>

/* Wrappers for the game functions will reside here */

void* MakeCPString(void* Dst, const char* String, uint16_t OEMCP, BOOL CalcLength)
{
	fMakeCPString_Internal(Dst, String, strlen(String)-1, OEMCP, (uint64_t)0, (uint64_t)0, (uint64_t)0, (uint64_t)0, CalcLength);
	return nullptr;
};