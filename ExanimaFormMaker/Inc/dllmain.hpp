#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <array>
#include <bit>
#include <vector>
#include <psapi.h>
#include <Sig/Sig.hpp>
#include <Signatures.hpp>
#include <GUIFuncWrapper.hpp>
#include <Forms.hpp>

extern uint8_t Verbose;
extern HMODULE myhModule;
extern HANDLE  hGameProcess;

extern uint64_t GameStaticMemoryStartAddress;
extern uint64_t GameStaticMemoryEndAddress;
extern uint64_t GameStaticMemorySize;
static FILE* fp;

static uint64_t GetModuleMemoryRange(uint64_t& ModuleStartAddress, uint64_t& ModuleEndAddress, uint64_t& ModuleSize);

extern void DefineAllSignatureStructs();