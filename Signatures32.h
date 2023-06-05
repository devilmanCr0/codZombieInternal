#pragma once
#include "Tools.h"
// Created with ReClass.NET 1.2 by KN4CK3R
// Padding technique by Public Void + Can1357 and other sources from the GuidedHacking forums
// Modified 
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name; }

struct byteSequence
{
	DWORD bytes;
	int byteSize;
	const BYTE* byteString;
};





//"BlackOps.exe"+00018DDC
class PlayerStats
{
	public:

		union
		{
			DEFINE_MEMBER_N(bool, isShooting, 0x4);
			DEFINE_MEMBER_N(Vector3, position, 0x18);
			DEFINE_MEMBER_N(Vector2, PitchYaw, 0x3C);
			DEFINE_MEMBER_N(float*, modelViewMatrix, 0x104);
			DEFINE_MEMBER_N(int32_t, health, 0x184);
			DEFINE_MEMBER_N(int32_t, maxHealth, 0x188);

		};

};

//"BlackOps.exe"+00056F10
class PlayerMovement
{
public:

	union
	{
		DEFINE_MEMBER_N(Vector3, playerPosition,0x24);
		DEFINE_MEMBER_N(Vector3, xyzSpeed, 0x30);
		DEFINE_MEMBER_N(int32_t, knockback, 0xC8);
		DEFINE_MEMBER_N(Vector2, pitchYaw, 0x180);
		DEFINE_MEMBER_N(int32_t, firstprimary, 0x354);
		DEFINE_MEMBER_N(int32_t, secondprimary, 0x358);
		DEFINE_MEMBER_N(int32_t, gunID, 0x3BC);
		DEFINE_MEMBER_N(int32_t, primaryAmmo, 0x3C0);
	};

};

class ZombieZ
{
public:

	union
	{
		int32_t ZombID; 
		DEFINE_MEMBER_N(int8_t, CurrentState, 0x6);
		DEFINE_MEMBER_N(Vector3, ZombPosition, 0x18);
		DEFINE_MEMBER_N(float, AxisDirection, 0x40);
		DEFINE_MEMBER_N(int32_t, Health, 0x184);
		DEFINE_MEMBER_N(int32_t, MaxHealth, 0x188);
	};

};

namespace signatures
{
	constexpr uintptr_t entityZombieList = 0x1BFBC84;
	constexpr uintptr_t localPlayerMovement = 0x01C08B40;
	namespace ClientState
	{
		constexpr uintptr_t yawpitchAddress = 0x2511E20;
		constexpr uintptr_t inGameCurrency = 0x180A6C8;
		constexpr uintptr_t addressToGod = 0x3DADCE;
		//constexpr uintptr_t zombieCount = 0x17FBC20;
		constexpr uintptr_t zombieCount = 0xA19368;
		constexpr uintptr_t localPlayerStats = 0x00018DDC;
		constexpr uintptr_t graphicSettings = 0x7A68AC;
		constexpr uintptr_t graphicDimensions = 0xB4;
		constexpr uintptr_t projectionMatrix = 0xC4;
	}	
}


//original god bytes \0x29\0xF8\x89\x85\x84\x04\x00\x00
//edited god bytes \\x83
//83 E8 00 89 85 84010000

//0xBE19A8
//Gun List

//0x1BFBC84
//class EntList ==> entStart + (0x8C*index)


