#pragma once

#include "tools.h"

void memoryAlloc(unsigned int size, BYTE* dst, BYTE* src);
void memoryPatch(unsigned int size, BYTE* dst);
uintptr_t AddyAddress(uintptr_t address, std::vector<unsigned int> offsets);


bool detourHook(void* toHook, void* ourFunc, int size);
DWORD trampHook(void* toHook, void* ourFunction, int size);



struct HookingAuto
{
	bool bStatus{ false };
	bool bDebug{ false };

	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	BYTE* PtrToGateWay{ nullptr };

	int len{ 0 };

	BYTE originalByte[15]{ 0 };

	HookingAuto(BYTE* src, BYTE* dst, BYTE* PtrToGateWay, uintptr_t len);
	HookingAuto(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGateWay, uintptr_t len);

	void Enable();
	void Disable();
	void Toggle();

	void DebugToggle();



};