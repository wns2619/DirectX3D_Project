#pragma once

#include <process.h>

namespace Client
{
	enum class LEVEL { STATIC, LOGO, GAME, LOADING, LEVEL_END };

	const unsigned long g_iWinSizeX = 1280;
	const unsigned long g_iWinSizeY = 720;
}

extern HWND g_hWnd;

using namespace Client;
