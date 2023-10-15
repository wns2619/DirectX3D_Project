#pragma once


// Channel�� Ư�� �ִϸ��̼��� ����ϴ� ���� �����̴�.

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Channel final : public Base
{
public:
	struct CHANNEL_DESC
	{
		uint32 mNumScalingKeys = 0;
		uint32 mNumRotationKeys = 0;
		uint32 mNumPositionKeys = 0;

		uint32 _numKeyFrames = 0; // ���� Ű ����.
		uint32 _boneIndex = 0;
	};


private:
	Channel();
	virtual ~Channel() = default;

public:
	HRESULT Initialize(const class Model* pModel, const aiNodeAnim* pAIChannel);
	void UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class Bone*>& Bones, _float trackPosition, KEYFRAME& beforeKeyFrame);
	void BlendChannel(uint32* pCurrentKeyFrame, KEYFRAME& prevLastKeyFrame, vector<class Bone*>& bones, _float blentTime);

public:
	_char* GetChannelName() { return _szName; }
	vector<KEYFRAME>& GetKeyFrame() { return _keyFrames; }
	CHANNEL_DESC& GetChannelDesc() { return _channelDesc; }

private:
	CHANNEL_DESC _channelDesc = {};

	_char _szName[MAX_PATH] = ""; // ���� �̸�.

	vector<KEYFRAME> _keyFrames;

public:
	static Channel* Create(const class Model* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END

