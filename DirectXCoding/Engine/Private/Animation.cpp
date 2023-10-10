#include "pch.h"
#include "Animation.h"
#include "Channel.h"

Animation::Animation()
{

}

Animation::Animation(const Animation& rhs)
	: _duration(rhs._duration)
	, _tickPerSecond(rhs._tickPerSecond)
	, _trackPosition(rhs._trackPosition)
	, _isFinished(rhs._isFinished)
	, _isLoop(rhs._isLoop)
	, _numChannels(rhs._numChannels)
	, _channels(rhs._channels)
	, _CurrentKeyFrame(rhs._CurrentKeyFrame)
{

	// ä���� �ִϸ��̼��� ���� �ִ� ������� �̱� ������ ���� ���縦 �� �ʿ䰡 ������ ���� ����� ó���ϰ�
	// �ִϸ��̼� ��ü�� ���ο� �ּҸ� ���� �����.
	::strcpy_s(_szName, rhs._szName);

	for (auto& pChannel : _channels)
		Safe_AddRef<Channel*>(pChannel);
}

HRESULT Animation::Initialize(const class Model* pModel, const aiAnimation* pAIAnimation)
{
	// �ִϸ��̼��� �̸��� �����Ѵ� ex) idel, attack ���. 
	strcpy_s(_szName, pAIAnimation->mName.data);

	// �ִϸ��̼� ����.
	_duration = static_cast<_float>(pAIAnimation->mDuration);

	// �ִϸ��̼� �ʴ� �ӵ�.
	_tickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);

	// �ִϸ��̼��� ����ϴ� ���� ����.
	_numChannels = pAIAnimation->mNumChannels;

	_CurrentKeyFrame.resize(_numChannels);
	// �ִϸ��̼��� ����ϴ� ���� ������ŭ ������ ũ�⸦ �̸� �Ҵ�.

	for (uint32 i = 0; i < _numChannels; ++i)
	{
		// �� ������ ���鼭 ������ ����.
		// �� ���� �� �ִϸ��̼� �ȿ��� � ���� ������ �����°��� �̸� �����س������� �Ѵ�.
		Channel* pChannel = Channel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		_channels.push_back(pChannel);
	}

	return S_OK;
}

void Animation::UpdateTransformationMatrix(vector<class Bone*>& Bones, const _float& timeDelta)
{
	if (true == _isFinished)
		return;

	_trackPosition += _tickPerSecond * timeDelta;

	if (_trackPosition >= _duration)
	{
		if (true == _isLoop)
			_trackPosition = 0.f;
		else
			_isFinished = true;
	}

	uint32 iNumChannel = 0;

	for (auto& pChannel : _channels)
		pChannel->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++], Bones, _trackPosition);

	// ä���� Ű �������� �ִϸ��̼ǿ��� �����Ѵ�. 
}

void Animation::Reset()
{
	_trackPosition = 0.f;
	_isFinished = false;
	_isLoop = false;

	for (auto& pChannel : _CurrentKeyFrame)
		pChannel = 0;
	// ������ �� ���͸� ���鼭 ���� �������� 0���� ����� �ȴ�.
}

Animation* Animation::Create(const class Model* pModel, const aiAnimation* pAIAnimation)
{
	Animation* pInstance = new Animation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		MSG_BOX("Failed to Created : Animation");
		Safe_Release<Animation*>(pInstance);
	}

	return pInstance;
}

Animation* Animation::Clone()
{
	return new Animation(*this);
}

void Animation::Free()
{
	__super::Free();

	for (auto& channeliter : _channels)
		Safe_Release<Channel*>(channeliter);

	_channels.clear();

	_CurrentKeyFrame.clear();
}
