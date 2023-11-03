#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL BinaryAnimation final : public Base
{
public:
	struct ANIMATION_DESC
	{
		_float _duration = 0.f; // 애니메이션 길이.
		_float _tickPerSecond = 0.f; // 애니메이션이 흐르는 시간.
		_float _trackPosition = 0.f; // 애니메이션의 재생 위치. KeyFrame 구간들.
		uint32 _numChannels = 0; // 채널 개수.
	};

private:
	BinaryAnimation();
	explicit BinaryAnimation(const BinaryAnimation& rhs);
	virtual ~BinaryAnimation() = default;

public:
	void SetLoop(_bool isLoop) { _isLoop = isLoop; }
	void SetAnimationSpeed(_float fSpped) { _fAnimationSpeed = fSpped; }

public:
	HRESULT Initialize(const class BinaryModel* pModel, const _char* animationName, const ANIMATION_DESC pAIAnimation, vector<class BinaryChannel*>& vecChannel);
	void UpdateTransformationMatrix(vector<class BinaryBone*>& Bones, const _float& timeDelta);
	void Reset();

public:
	_char* GetAnimationBoneName() { return _szName; }
	ANIMATION_DESC& GetAnimationDesc() { return _animationDesc; }
	vector<uint32>& GetCurrentKeyFrame() { return _CurrentKeyFrame; }
	vector<class BinaryChannel*>& GetChannels() { return _channels; }
	_bool GetFinished() { return _isFinished; }

private:
	ANIMATION_DESC _animationDesc;

	_bool _isFinished = false; // 애니메이션이 끝났는가.
	_bool _isLoop = false; // 루프를 돌아야하는 애니메이션인가.


	_char _szName[MAX_PATH] = ""; // 애니메이션이 구동해야하는 뼈의 정보들.
	vector<class BinaryChannel*> _channels; // 애니메이션이 갖고 있는 채널 개수.
	vector<uint32> _CurrentKeyFrame; // 클론 복사를 위해 채널의 current 프레임을 애니메이션이 직접 갖고 있음.

	_float _fAnimationSpeed = 1.f;
public:
	static BinaryAnimation* Create(const class BinaryModel* pModel, const _char* animationName, ANIMATION_DESC AnimationInfo, vector<class BinaryChannel*>& vecChannel);
	BinaryAnimation* Clone();
	virtual void Free() override;
};

END

