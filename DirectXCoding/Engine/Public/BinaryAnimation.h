#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL BinaryAnimation final : public Base
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

	_bool _isFinished = false; // �ִϸ��̼��� �����°�.
	_bool _isLoop = false; // ������ ���ƾ��ϴ� �ִϸ��̼��ΰ�.


	_char _szName[MAX_PATH] = ""; // �ִϸ��̼��� �����ؾ��ϴ� ���� ������.
	vector<class BinaryChannel*> _channels; // �ִϸ��̼��� ���� �ִ� ä�� ����.
	vector<uint32> _CurrentKeyFrame; // Ŭ�� ���縦 ���� ä���� current �������� �ִϸ��̼��� ���� ���� ����.

	_float _fAnimationSpeed = 1.f;
public:
	static BinaryAnimation* Create(const class BinaryModel* pModel, const _char* animationName, ANIMATION_DESC AnimationInfo, vector<class BinaryChannel*>& vecChannel);
	BinaryAnimation* Clone();
	virtual void Free() override;
};

END

