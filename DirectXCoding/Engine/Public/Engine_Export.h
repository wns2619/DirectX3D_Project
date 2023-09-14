#pragma once

#include "TimeManager.h"
#include "GraphicsManager.h"

BEGIN(Engine)


inline _float	 GetTimeDelta(const wstring& timertag);
inline void		 SetTimeDelta(const wstring& timertag);
inline HRESULT	 Initialize(const wstring& timertag);

inline HRESULT	Initialize(HWND hwnd, GraphicsManager** graphis);
inline void		RenderBegin();
inline void		RenderEnd();

inline void		 Release_Engine();

#include "Engine_Export.inl"

END