#include "pch.h"
#include "InputManager.h"

IMPLEMENT_SINGLETON(InputManager)

Engine::InputManager::InputManager(void)
{

}

_bool InputManager::keyDown(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_byKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool InputManager::keyUp(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool InputManager::keyPressing(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] == m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool InputManager::mouseDown(DIMK eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool InputManager::mouseUp(DIMK eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_PreMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool InputManager::mousePressing(DIMK eMouseKeyID)
{
	if (m_PreMouseState.rgbButtons[eMouseKeyID] == m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;;
}

HRESULT InputManager::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void InputManager::Tick()
{
	memcpy(&m_preKeyState, &m_byKeyState, sizeof(_char) * 256);
	m_pKeyboard->GetDeviceState(256, m_byKeyState);
	memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

void InputManager::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);

	Safe_Release(m_pInputSDK);
}
