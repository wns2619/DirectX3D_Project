#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Animation final : public Base
{
private:
	Animation();
	explicit Animation(const Animation& rhs);
	virtual ~Animation() = default;

public:
	void SetLoop(_bool isLoop) { _isLoop = isLoop; }

public:
	HRESULT Initialize(const class Model* pModel, const aiAnimation* pAIAnimation);
	void UpdateTransformationMatrix(vector<class Bone*>& Bones, const _float& timeDelta);
	void Reset();

private:
	_float _duration = 0.f; // 애니메이션 길이.
	_float _tickPerSecond = 0.f; // 애니메이션이 흐르는 시간.
	_float _trackPosition = 0.f; // 애니메이션의 재생 위치. KeyFrame 구간들.

	_bool _isFinished = false; // 애니메이션이 끝났는가.
	_bool _isLoop = false; // 루프를 돌아야하는 애니메이션인가.


	_char _szName[MAX_PATH] = ""; // 애니메이션이 구동해야하는 뼈의 정보들.
	uint32 _numChannels = 0; // 채널 개수.
	vector<class Channel*> _channels;
	vector<uint32> _CurrentKeyFrame; // 클론 복사를 위해 채널의 current 프레임을 애니메이션이 직접 갖고 있음.
public:
	static Animation* Create(const class Model* pModel, const aiAnimation* pAIAnimation);
	Animation* Clone();
	virtual void Free() override;
};

END

