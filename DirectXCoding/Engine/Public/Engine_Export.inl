#pragma once
#include "Engine_Export.h"

inline _float	 GetTimeDelta(const wstring& timertag)
{
	return TimeManager::GetInstance()->GetTimeDelta(timertag);
}
inline void		 SetTimeDelta(const wstring& timertag)
{
	TimeManager::GetInstance()->SetTimeDelta(timertag);
}
inline HRESULT	 Initialize(const wstring& timertag)
{
	return TimeManager::GetInstance()->Initialize(timertag);
}

inline HRESULT	Initialize(HWND hwnd, GraphicsManager** graphis)
{
	return GraphicsManager::GetInstance()->Initialize(hwnd, graphis);
}
inline void RenderBegin()
{
	GraphicsManager::GetInstance()->RenderBegin();
}
inline void RenderEnd()
{
	GraphicsManager::GetInstance()->RenderEnd();
}

inline void		 Release_Engine()
{
	TimeManager::GetInstance()->DestroyInstance();
	GraphicsManager::GetInstance()->DestroyInstance();
}