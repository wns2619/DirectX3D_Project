#pragma once

#include "Base.h"
#include "Client_Define.h"

BEGIN(Client)

class App : public Base
{
	DECLARE_SINGLETON(App)
private:
	App();
	virtual ~App();
private:
	// window
	wstring _name;
	_bool _shouldQuit;
	_bool _clipCursor;

	// Settings
	int32 _width;
	int32 _height;
	_bool _fullScreen = false;
	//


	// Functions
	_bool InitRawMouseDevice();

public:
	_bool Initialize(const HINSTANCE hInstance, const LPWSTR lpCmdLine, const HWND hwnd, const int32 showCmd);
	void CreateWin32Window(const HINSTANCE hInstance, const wchar_t* windowTitle, HWND& _d3d11Window);

	void setClipCursor(_bool clipCursor);
	void setRenderToggle(_bool renderToggle);

	// Loop
	void applicationLoop();

public:
	virtual void Free() override;
};


END

