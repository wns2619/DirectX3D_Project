#include "pch.h"
#include "TimeManager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(TimeManager)

TimeManager::TimeManager()
{

}

_float TimeManager::Compute_TimeDelta(const wstring& timeTag)
{
	Timer* pTimer = Find_Timer(timeTag);

	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Compute_TimeDelta();
}

HRESULT TimeManager::Add_Timer(const wstring& timertag)
{
	Timer* pTimer = Find_Timer(timertag);

	if (nullptr != pTimer)
		return E_FAIL;

	_timers.insert(make_pair(timertag, Timer::Create()));
	
	return S_OK;
}

Timer* TimeManager::Find_Timer(const wstring& timerTag) const
{
	auto iter = _timers.find(timerTag);

	if (iter != _timers.end())
		return iter->second;

	return nullptr;
}


void TimeManager::Free()
{
	__super::Free();

	for (auto& pair : _timers)
		Safe_Release<Timer*>(pair.second);

	_timers.clear();
}
