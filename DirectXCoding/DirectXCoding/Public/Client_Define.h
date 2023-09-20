#pragma once

#include <process.h>

namespace Client
{
	enum class LEVEL { STATIC, LOGO, GAME, EDIT, LOADING, LEVEL_END };

	const unsigned long g_iWinSizeX = 1280;
	const unsigned long g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace Client;
