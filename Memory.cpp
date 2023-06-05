#include "Memory.h"

void memoryAlloc(unsigned int size, BYTE* dst, BYTE* src)
{
	DWORD protocol{};
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &protocol);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, protocol, &protocol);
}
void memoryPatch(unsigned int size, BYTE* dst)
{
	DWORD protocol{};
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &protocol);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, protocol, &protocol);
}
uintptr_t AddyAddress(uintptr_t address, std::vector<unsigned int> offsets)
{
	uintptr_t theaddress{ address };
	for (unsigned int off : offsets)
	{
		theaddress = (*(uintptr_t*)address) + off;
	}


	return theaddress;

}
bool detourHook(void* toHook, void* ourFunction, int size)
{
	if (size < 5)
	{
		return false;
	}

	DWORD protocol{};
	VirtualProtect(toHook, size, PAGE_EXECUTE_READWRITE, &protocol);
	memset(toHook, 0x90, size);
	//our defined function would be further down the memory so the number would be greater;
	/*
	the address to hook -
	jmp (additional 5 bytes)
	/\
	 |   then - 5 to really  get to the addres sof the hook hey
	 |
	\/
	the function  address
	*/

	DWORD addressOffset = (DWORD)ourFunction - (DWORD)toHook - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = addressOffset;


	VirtualProtect(toHook, size, protocol, &protocol);

	return true;

}

DWORD trampHook(void* toHook, void* ourFunction, int size)
{
	if (size < 5)
	{
		return 0;
	}

	BYTE* gatewayAddr = (BYTE*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	memcpy_s(gatewayAddr, size, toHook, size);

	*(BYTE*)(gatewayAddr + size) = 0xE9; //jump opcode

	*(DWORD*)((DWORD)gatewayAddr + size + 1) = ((DWORD)toHook - (DWORD)gatewayAddr - 5);

	//this connects hooked address with our custom function, the function will jump back to the gateway
	//that will be returned by this function
	detourHook(toHook, ourFunction, size);

	return (DWORD)gatewayAddr;
}





HookingAuto::HookingAuto(BYTE* src, BYTE* dst, BYTE* PtrToGateWay, uintptr_t len)
{
	this->src = src;
	this->dst = dst;
	this->PtrToGateWay = PtrToGateWay;
	this->len = len;
}
HookingAuto::HookingAuto(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGateWay, uintptr_t len)
{
	HMODULE hModule = GetModuleHandleA(modName);
	//ex : modname- openGll exportName - wglSwapBuffers
	if (hModule)
	{
		this->src = (BYTE*)GetProcAddress(hModule, exportName);
	}
	this->dst = dst;
	this->PtrToGateWay = PtrToGateWay;
	this->len = len;
}

void HookingAuto::Enable()
{
	if (bDebug) std::cout << "preserving bytes .. \n";
	//originalBytes can allocate up to 15 bytes 
	memcpy(originalByte, src, len);
	if (bDebug) std::cout << "setting up trapHook .. \n";
	*(uintptr_t*)PtrToGateWay = (uintptr_t)trampHook(src, dst, len);


	bStatus = true;
}
void HookingAuto::Disable()
{
	if (bDebug) std::cout << "restoring original bytes .. \n";
	memoryAlloc(len, src, originalByte);

	bStatus = false;
}
void HookingAuto::Toggle()
{
	if (!bStatus) Enable();
	else Disable();
}

void HookingAuto::DebugToggle()
{
	bDebug = !bDebug;
	std::cout << "Debug is " << bDebug << std::endl;
}
