#include "pch.h"
#include "BinaryAnimation.h"
#include "BinaryChannel.h"

BinaryAnimation::BinaryAnimation()
{
}

BinaryAnimation::BinaryAnimation(const BinaryAnimation& rhs)
	: _animationDesc(rhs._animationDesc)
	, _isFinished(rhs._isFinished)
	, _isLoop(rhs._isLoop)
	, _channels(rhs._channels)
	, _CurrentKeyFrame(rhs._CurrentKeyFrame)
{
	// ä���� �ִϸ��̼��� ���� �ִ� ������� �̱� ������ ���� ���縦 �� �ʿ䰡 ������ ���� ����� ó���ϰ�
// �ִϸ��̼� ��ü�� ���ο� �ּҸ� ���� �����.
	::strcpy_s(_szName, rhs._szName);

	for (auto& pChannel : _channels)
		Safe_AddRef<BinaryChannel*>(pChannel);
}

HRESULT BinaryAnimation::Initialize(const BinaryModel* pModel, const _char* animationName, ANIMATION_DESC animationInfo, vector<class BinaryChannel*>& vecChannel)
{
	// �ִϸ��̼��� �̸��� �����Ѵ� ex) idel, attack ���. 
	strcpy_s(_szName, animationName);

	// �ִϸ��̼� ����.
	_animationDesc._duration = static_cast<_float>(animationInfo._duration);

	// �ִϸ��̼� �ʴ� �ӵ�.
	_animationDesc._tickPerSecond = static_cast<_float>(animationInfo._tickPerSecond);

	// �ִϸ��̼��� ����ϴ� ���� ����.
	_animationDesc._numChannels = animationInfo._numChannels;

	_CurrentKeyFrame.resize(_animationDesc._numChannels);
	// �ִϸ��̼��� ����ϴ� ���� ������ŭ ������ ũ�⸦ �̸� �Ҵ�.

	for (auto& pChannel : vecChannel)
	{
		_channels.push_back(pChannel);
	}
	//_channels = vecChannel;

	return S_OK;
}

void BinaryAnimation::UpdateTransformationMatrix(vector<class BinaryBone*>& Bones, const _float& timeDelta, vector<class BinaryChannel*>& beforeChannel)
{
	if (true == _isFinished)
		return;

	_animationDesc._trackPosition += _animationDesc._tickPerSecond * timeDelta;

	if (_animationDesc._trackPosition >= _animationDesc._duration)
	{
		if (true == _isLoop)
			_animationDesc._trackPosition = 0.f;
		else
			_isFinished = true;
	}

	uint32 iNumChannel = 0;

	for (uint32 i = 0; i < _channels.size(); ++i)
	{
		KEYFRAME keyFrame = {};

		if (beforeChannel.size() != 0)
		{
			for (uint32 j = 0; j < beforeChannel.size(); ++j)
			{
				if (::strcmp(_channels[i]->GetChannelName(), beforeChannel[j]->GetChannelName()) == false)
					keyFrame = beforeChannel[j]->GetKeyFrame().back();
			}
		}

		_channels[i]->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++], Bones, _animationDesc._trackPosition, keyFrame);
	}

	//for (auto& pChannel : _channels)
	//	pChannel->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++], Bones, _animationDesc._trackPosition);
}

void BinaryAnimation::Reset()
{
	_animationDesc._trackPosition = 0.f;
	_isFinished = false;
	_isLoop = false;

	for (auto& pChannel : _CurrentKeyFrame)
		pChannel = 0;
}

BinaryAnimation* BinaryAnimation::Create(const BinaryModel* pModel, const _char* animationName, ANIMATION_DESC AnimationInfo, vector<class BinaryChannel * > & vecChannel)
{
	BinaryAnimation* pInstance = new BinaryAnimation();

	if (FAILED(pInstance->Initialize(pModel, animationName, AnimationInfo, vecChannel)))
	{
		MSG_BOX("Failed to Created : Animation");
		Safe_Release<BinaryAnimation*>(pInstance);
	}

	return pInstance;
}

BinaryAnimation* BinaryAnimation::Clone()
{
	return new BinaryAnimation(*this);
}

void BinaryAnimation::Free()
{
	__super::Free();

	for (auto& channeliter : _channels)
		Safe_Release<BinaryChannel*>(channeliter);

	_channels.clear();

	_CurrentKeyFrame.clear();
}
