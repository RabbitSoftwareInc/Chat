#pragma once
#include "../util/util.h"

class c_gui
{
public:
	ID3D11Device* g_pd3dDevice = nullptr;
	ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	bool g_SwapChainOccluded = false;
	UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
	ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

	bool g_DeviceLost = false;

	void initialize();
};

extern c_gui* g_gui;