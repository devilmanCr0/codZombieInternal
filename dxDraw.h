#pragma once
#pragma comment(lib, "d3d9.lib")
#include "Tools.h"
#include <d3d9.h>
#include <Memory.h>

//DIRECT3D

extern int VIRTUAL_SCREEN_WIDTH;
extern int VIRTUAL_SCREEN_HEIGHT;
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;


BOOL CALLBACK enumWind(HWND handle, LPARAM lp);


namespace rgb
{
	const D3DCOLOR red[3] = { 255, 0, 0 };
	const D3DCOLOR green[3] = { 0, 255, 0 };
	const D3DCOLOR gray[3] = { 55, 55, 55 };
	const D3DCOLOR lightgray[3] = { 192, 192, 192 };
	const D3DCOLOR black[3] = { 0, 0, 0 };
}

namespace DXD
{

	class Font
	{
	public:
		bool bBuild{ false };
		unsigned int base;
		int height;
		int width;

		void Build(int height);
		void Print(float x, float y, float width, float height, float textWidth, float textHeight);

		Vector3 centerText(float x, float y, float width, float height, float textWidth, float textHeight);
		float centerText(float x, float width, float textWidth);
	};

	class Draw
	{
	private:
		LPDIRECT3DDEVICE9 pDevice;

	public:
		IDirect3DDevice9* pDummyDevice;
		Draw(void** pTable, size_t size, bool &truth);

		void Initalize(LPDIRECT3DDEVICE9 pDevice);


		void SetupOrtho();
		void RestoreGL();

		void DrawLine(float x1, float y1, float x2, float y2, int thickness, D3DCOLOR color);
		void DrawFilledRect(float x, float y, float width, float height, const D3DCOLOR color[3]);
		void DrawOutline(float x, float y, float width, float height, float lineWidth, const D3DCOLOR color[3]);

		bool WorldToScreen(Vector3 pos, Vector3& screen, float matrix[16], int windowWidth, int windowHeight);

		void DrawESP(PlayerMovement* localPlayer, ZombieZ* zombie, Vector3 screenCoords);
	};



}