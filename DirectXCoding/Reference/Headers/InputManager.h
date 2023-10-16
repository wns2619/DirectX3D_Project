#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL InputManager : public Base
{
	DECLARE_SINGLETON(InputManager)
private:
	InputManager();
	virtual ~InputManager() = default;

public:
	_char Get_DIKState(_ubyte eKeyID) {
		return m_byKeyState[eKeyID];
	}

	_char Get_DIMKeyState(DIMK eMouseKeyID) {
		return m_MouseState.rgbButtons[eMouseKeyID];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_MouseState)[eMouseMoveID];
	}

	_bool keyDown(_ubyte eKeyID);
	_bool keyUp(_ubyte eKeyID);
	_bool keyPressing(_ubyte eKeyID);

	_bool mouseDown(DIMK eMouseKeyID);
	_bool mouseUp(DIMK eMouseKeyID);
	_bool mousePressing(DIMK eMouseKeyID);


public:
	HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
	void Tick();

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:
	_char				m_preKeyState[256] = { 0 };
	_char				m_byKeyState[256] = { 0 };
	DIMOUSESTATE		m_PreMouseState;
	DIMOUSESTATE		m_MouseState;

public:
	virtual void Free();

};
END
#endif // InputDev_h__
