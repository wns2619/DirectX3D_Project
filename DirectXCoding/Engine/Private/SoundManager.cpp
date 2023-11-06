#include "pch.h"
#include "SoundManager.h"

IMPLEMENT_SINGLETON(SoundManager)

SoundManager::SoundManager()
{
}

HRESULT SoundManager::ReadySound()
{
	FMOD_System_Create(&_pSystem, FMOD_VERSION);

	FMOD_System_Init(_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSound();

	return S_OK;
}

HRESULT SoundManager::LoadSound()
{

	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd;

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	long long handle = _findfirst("..\\Binaries\\Resources\\SoundResource\\Use\\*.*", &fd);

	if (handle == -1)
		return E_FAIL;

	int iResult = 0;

	char szCurPath[128] = "..\\Binaries\\Resources\\SoundResource\\Use\\";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			_mapSound.emplace(pSoundKey, pSound);

			//Safe_Delete_Array<TCHAR*>(pSoundKey);
		}
		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(_pSystem);

	_findclose(handle);
}

void SoundManager::PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(_mapSound.begin(), _mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == _mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(_pSystem, iter->second, NULL, FALSE, &_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(_pChannelArr[eID], fVolume);

	FMOD_System_Update(_pSystem);
}

void SoundManager::PlaySoundLoop(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(_mapSound.begin(), _mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == _mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;


	if (FMOD_Channel_IsPlaying(_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(_pSystem, iter->second, NULL, FALSE, &_pChannelArr[eID]);
	}

	FMOD_Channel_SetMode(_pChannelArr[eID], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(_pChannelArr[eID], fVolume);
	FMOD_System_Update(_pSystem);
}

void SoundManager::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(_mapSound.begin(), _mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == _mapSound.end())
		return;

	FMOD_System_PlaySound(_pSystem, iter->second, NULL, FALSE, &_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(_pSystem);
}

void SoundManager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(_pChannelArr[eID]);
}

void SoundManager::StopAll()
{
	for (int i = 0; i < SOUND_MAXCHANNEL; ++i)
		FMOD_Channel_Stop(_pChannelArr[i]);
}

void SoundManager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(_pChannelArr[eID], fVolume);

	FMOD_System_Update(_pSystem);
}

void SoundManager::Free()
{
	for (auto& Mypair : _mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	_mapSound.clear();

	FMOD_System_Release(_pSystem);
	FMOD_System_Close(_pSystem);
}
