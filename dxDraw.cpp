#include "dxDraw.h"
#include <DirectXMath.h>
int VIRTUAL_SCREEN_WIDTH;
int VIRTUAL_SCREEN_HEIGHT;
static HWND window;

BOOL CALLBACK enumWind(HWND handle, LPARAM lp)
{
	DWORD procId;
	GetWindowThreadProcessId(handle, &procId);
	if (GetCurrentProcessId() != procId)
	{
		return TRUE;
	}

	window = handle;
	return FALSE;

}

DXD::Draw::Draw(void** pTable, size_t size, bool &truth)
{
	if (!pTable)
	{
		std::cout << "invalid pTable!" << std::endl;
		truth = false;
	}

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
	{
		std::cout << "error in creating a Direct3D instance" << std::endl;
		truth = false;
	}


	IDirect3DDevice9* pDummyDevice = nullptr;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	window = NULL;

	EnumWindows(enumWind, NULL);

	RECT sizeOfWindow;
	if (window)
	{
		GetWindowRect(window, &sizeOfWindow);
		VIRTUAL_SCREEN_WIDTH = sizeOfWindow.right - sizeOfWindow.left;
		VIRTUAL_SCREEN_HEIGHT = sizeOfWindow.bottom - sizeOfWindow.top;

	}
	else {
		std::cout << " we don't have a window to aquire rectangle size";
	}
	
	
	

	VIRTUAL_SCREEN_HEIGHT -= 29;
	VIRTUAL_SCREEN_WIDTH -= 5; // centering screen


	d3dpp.hDeviceWindow = window;

	HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);


	if (dummyDevCreated != S_OK)
	{
		d3dpp.Windowed = !d3dpp.Windowed;
		HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDevCreated != S_OK)
		{
			pD3D->Release();
			std::cout << "D3D DIDNTWORK FUCKKKK \n";
			truth = false;
		}
	}
	if (pTable)
	{
		memcpy(pTable, *(void***)(pDummyDevice), size);
	}
	else {
		std::cout << " no peepeeTable ;(" << std::endl;
	}
	
	pDummyDevice->Release();
	pD3D->Release();


}

void DXD::Draw::Initalize(LPDIRECT3DDEVICE9 pDevice)
{
	this->pDevice = pDevice;
}

void DXD::Draw::DrawFilledRect(float x, float y, float width, float height, const D3DCOLOR color[3])
{
	D3DRECT rect = { x,y,x + width,y + height };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, *color, 0, 0);

}

void DXD::Draw::DrawLine(float x1, float y1, float x2, float y2, int thickness, D3DCOLOR color)
{
	DirectX::XMFLOAT2 poop;


}

void DXD::Draw::DrawOutline(float x, float y, float width, float height, float lineWidth, const D3DCOLOR color[3])
{


	D3DRECT rect = { x / 2,y / 2, 50 ,50  };  // top line bar
	D3DRECT rect1 = { x / 2,y / 2,x + lineWidth, y * 2 }; // left side bar
	D3DRECT rect2 = { x * 2, y / 2, x + lineWidth, y *2}; // right side bar
	D3DRECT rect3 = { x / 2, y * 2, x *2 , y + lineWidth }; // right side bar


	//weirdo
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, *color, 0, 0);
	//pDevice->Clear(1, &rect1, D3DCLEAR_TARGET, *color, 0, 0);
	//pDevice->Clear(1, &rect2, D3DCLEAR_TARGET, *color, 0, 0);
	//pDevice->Clear(1, &rect3, D3DCLEAR_TARGET, *color, 0, 0);

}

bool DXD::Draw::WorldToScreen(Vector3 pos, Vector3& screen, float matrix[16], int windowWidth, int windowHeight)
{
	Vector4 clipCoords;

	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3]; //first vertex row
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7]; //second vertex row
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11]; //third vertex row
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15]; //fourth vertex row


	if (clipCoords.w < 0.1f)
	{
		return false;
	}

	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;


	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	return true;
}

void DXD::Draw::DrawESP(PlayerMovement* localPlayer, ZombieZ* zombie, Vector3 screenCoords)
{
	D3DCOLOR color[3] = { 0,255,0 };

	if (zombie->Health < 50)
	{
		color[1] = 255;
		color[2] = 0;
	}
	else
	{
		color[1] = 0;
		color[2] = 255;
	}

	float distance = localPlayer->playerPosition.Distance(zombie->ZombPosition);


	float scale = (GAME_UNIT_MAGIC / distance) * (50 / VIRTUAL_SCREEN_WIDTH);
	float x = screenCoords.x - scale;
	float y = screenCoords.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	DrawOutline(x, y, width, height, 2.0f, color);
}