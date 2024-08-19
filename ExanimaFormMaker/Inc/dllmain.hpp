#pragma once
#include <windows.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include <bit>
#include <psapi.h>
#include <Sig/Sig.hpp>
#include <Signatures.hpp>
#include <GUIFuncWrapper.hpp>
#include <Forms.hpp>

extern uint8_t Verbose;
// Handle to a module gained at injection phase, needed for ejeting
extern HMODULE myhModule;
// Pseudo handle to a game process
extern HANDLE  hGameProcess;

extern uint64_t GameStaticMemoryStartAddress;
extern uint64_t GameStaticMemoryEndAddress;
extern uint64_t GameStaticMemorySize;
static FILE* fp; // for console's cout

static uint64_t GetModuleMemoryRange(uint64_t& ModuleStartAddress, uint64_t& ModuleEndAddress, uint64_t& ModuleSize);

extern void DefineAllSignatureStructs();