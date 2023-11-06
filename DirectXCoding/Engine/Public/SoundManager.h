#pragma once

#include "Base.h"

BEGIN(Engine)

class SoundManager : public Base
{
	DECLARE_SINGLETON(SoundManager)

private:
	SoundManager();
	virtual ~SoundManager() = default;

public:
	HRESULT ReadySound();
	HRESULT LoadSound();

public:
	void PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlaySoundLoop(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	FMOD_SYSTEM* _pSystem = nullptr;
	FMOD_CHANNEL* _pChannelArr[CHANNELID::SOUND_MAXCHANNEL];
	map<TCHAR*, FMOD_SOUND*> _mapSound;

private:
	virtual void Free() override;
};

END

