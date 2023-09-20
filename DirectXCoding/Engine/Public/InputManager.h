#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL InputManager : public Base
{
	DECLARE_SINGLETON(InputManager)

public:
	enum MOUSEKEYSTATE { MKS_LBUTTON, MKS_RBUTTON, MKS_WHEELBUTTON, MKS_XBUTTON, MKS_END };
	enum MOUSEMOVESTATE { MMS_X, MMS_Y, MMS_WHEEL, MMS_END };

private:
	InputManager(void);
	virtual ~InputManager(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)			{ return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	{ return m_tMouseState.rgbButtons[eMouse]; 	}
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	
public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void	Tick(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	_byte					m_byKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	DIMOUSESTATE			m_tMouseState;	

public:
	virtual void	Free(void);

};
END
#endif // InputDev_h__
