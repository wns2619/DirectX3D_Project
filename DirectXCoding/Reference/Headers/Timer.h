#pragma once

#include "Base.h"

BEGIN(Engine)

class Timer : public Base
{
private:
	Timer();
	virtual ~Timer();

public:
	_float	GetTimeDelta() { return _timeDelta; }
	
public:
	HRESULT	Initialize();
	_float	Compute_TimeDelta();

private:
	LARGE_INTEGER	_frameTime;
	LARGE_INTEGER	_fixTime;
	LARGE_INTEGER	_lastTime;
	LARGE_INTEGER	_cpuTick;

	_float			_timeDelta;

public:
	static Timer* Create();

private:
	virtual void Free() override;
};

END