#pragma once

#include "Base.h"


BEGIN(Engine)


class ENGINE_DLL BinaryChannel final : public Base
{
public:
	struct CHANNEL_DESC
	{
		uint32 mNumScalingKeys = 0;
		uint32 mNumRotationKeys = 0;
		uint32 mNumPositionKeys = 0;

		uint32 _numKeyFrames = 0; // 최종 키 개수.
		uint32 _boneIndex = 0;
	};

private:
	BinaryChannel();
	virtual ~BinaryChannel() = default;

public:
	HRESULT Initialize(const class BinaryModel* pModel, const _char* channelName, const CHANNEL_DESC channelInfo, vector<KEYFRAME>& vecKey);
	void UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class BinaryBone*>& Bones, _float trackPosition);

public:
	const _char* GetChannelName() { return _szName; }
	vector<KEYFRAME>& GetKeyFrame() { return _keyFrames; }
	CHANNEL_DESC& GetChannelDesc() { return _channelDesc; }

	KEYFRAME& GetCurrentKeyFrame() { return _currentKeyFrame; }

private:
	CHANNEL_DESC _channelDesc = {};

	_char _szName[MAX_PATH] = ""; // 뼈의 이름.

	vector<KEYFRAME> _keyFrames;
	KEYFRAME _currentKeyFrame;

public:
	static BinaryChannel* Create(const class BinaryModel* pModel, const _char* channelName, const CHANNEL_DESC channelInfo, vector<KEYFRAME>& vecKey);
	virtual void Free() override;
};

END

