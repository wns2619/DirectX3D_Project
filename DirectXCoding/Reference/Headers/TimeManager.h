#pragma once

#include "Base.h"

BEGIN(Engine)

class Timer;

class TimeManager : public Base 
{
	DECLARE_SINGLETON(TimeManager)

private:
	TimeManager();
	virtual ~TimeManager() = default;

public:
	_float Compute_TimeDelta(const wstring& timeTag);

public:
	HRESULT	Add_Timer(const wstring& timertag);

private:
	Timer* Find_Timer(const wstring& timerTag) const;

private:
	map<const wstring, Timer*> _timers;

public:
	virtual void Free();
};

END