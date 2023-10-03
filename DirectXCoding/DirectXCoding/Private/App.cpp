#include "pch.h"
#include "App.h"

//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//extern App* app;

App::App()
{
	_name = L"Deppart ProtoType";
	_width = 1600;
	_height = 900;
	_fullScreen = false;
	_shouldQuit = false;
	_clipCursor = false;
}

App::~App()
{

}

_bool App::InitRawMouseDevice()
{
	RAWINPUTDEVICE rawDevice;
	rawDevice.usUsagePage = 0x01;
	rawDevice.usUsage = 0x02;
	rawDevice.dwFlags = 0;
	rawDevice.hwndTarget = NULL;
	if (RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice)) == FALSE)
		return false;


	return true;
}

_bool App::Initialize(const HINSTANCE hInstance, const LPWSTR lpCmdLine, const HWND hwnd, const int32 showCmd)
{
	//CreateWin32Window(hInstance, _name.c_str(), hwnd);


	return _bool();
}

void App::CreateWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window)
{

}

void App::setClipCursor(_bool clipCursor)
{
}

void App::setRenderToggle(_bool renderToggle)
{
}

void App::applicationLoop()
{
}

void App::Free()
{
}

//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//
//}