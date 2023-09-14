#include "pch.h"
#include "Timer.h"

Timer::Timer()
	: _timeDelta(0.f)
{
	ZeroMemory(&_frameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_fixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_lastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_cpuTick, sizeof(LARGE_INTEGER));

}

Timer::~Timer()
{
}

HRESULT Timer::Initialize()
{
	QueryPerformanceCounter(&_frameTime);
	QueryPerformanceCounter(&_fixTime);
	QueryPerformanceCounter(&_lastTime);

	QueryPerformanceFrequency(&_cpuTick);

	return S_OK;
}

_float Timer::Compute_TimeDelta()
{
	QueryPerformanceCounter(&_frameTime);

	if (_frameTime.QuadPart - _fixTime.QuadPart >= _cpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&_cpuTick);
		_fixTime = _frameTime;
	}

	_timeDelta = _float(_frameTime.QuadPart - _lastTime.QuadPart) / _cpuTick.QuadPart;

	_lastTime = _frameTime;

	return _timeDelta;
}

Timer* Timer::Create()
{
	Timer* pInstance = new Timer;
	
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Timer Create Failed");
		Safe_Release<Timer*>(pInstance);
		
		return nullptr;
	}

	return pInstance;
}

void Timer::Free()
{
}
