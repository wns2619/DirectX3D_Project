#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Animation final : public Base
{
public:
	struct ANIMATION_DESC
	{
		_float _duration = 0.f; // �ִϸ��̼� ����.
		_float _tickPerSecond = 0.f; // �ִϸ��̼��� �帣�� �ð�.
		_float _trackPosition = 0.f; // �ִϸ��̼��� ��� ��ġ. KeyFrame ������.
		uint32 _numChannels = 0; // ä�� ����.
	};


private:
	Animation();
	explicit Animation(const Animation& rhs);
	virtual ~Animation() = default;

public:
	void SetLoop(_bool isLoop) { _isLoop = isLoop; }

public:
	HRESULT Initialize(const class Model* pModel, const aiAnimation* pAIAnimation);
	void UpdateTransformationMatrix(vector<class Bone*>& Bones, const _float& timeDelta, vector<class Channel*>& beforChannel);
	void Reset();

public:
	_char* GetAnimationBoneName() { return _szName; }
	ANIMATION_DESC& GetAnimationDesc() { return _animationDesc; }
	vector<uint32>& GetCurrentKeyFrame() { return _CurrentKeyFrame; }
	vector<class Channel*>& GetChannels() { return _channels; }

public:
	void BlendAnimations(Animation* prevAnimation, _float blendFactor, vector<class Bone*>& Bones);

private:
	ANIMATION_DESC _animationDesc;

	_bool _isFinished = false; // �ִϸ��̼��� �����°�.
	_bool _isLoop = false; // ������ ���ƾ��ϴ� �ִϸ��̼��ΰ�.


	_char _szName[MAX_PATH] = ""; // �ִϸ��̼��� �����ؾ��ϴ� ���� ������.
	vector<class Channel*> _channels; // �ִϸ��̼��� ���� �ִ� ä�� ����.
	vector<uint32> _CurrentKeyFrame; // Ŭ�� ���縦 ���� ä���� current �������� �ִϸ��̼��� ���� ���� ����.
public:
	static Animation* Create(const class Model* pModel, const aiAnimation* pAIAnimation);
	Animation* Clone();
	virtual void Free() override;
};

END

