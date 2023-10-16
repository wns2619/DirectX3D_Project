#include "pch.h"
#include "BinaryAnimation.h"
#include "BinaryChannel.h"
#include "BinaryModel.h"

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
	// 채널은 애니메이션이 갖고 있는 구성요소 이기 때문에 깊은 복사를 할 필요가 없으니 얕은 복사로 처리하고
// 애니메이션 자체만 새로운 주소를 갖게 만든다.
	::strcpy_s(_szName, rhs._szName);

	for (auto& pChannel : _channels)
		Safe_AddRef<BinaryChannel*>(pChannel);
}

HRESULT BinaryAnimation::Initialize(const BinaryModel* pModel, const _char* animationName, ANIMATION_DESC animationInfo, vector<class BinaryChannel*>& vecChannel)
{
	// 애니메이션의 이름을 저장한다 ex) idel, attack 등등. 
	strcpy_s(_szName, animationName);

	// 애니메이션 길이.
	_animationDesc._duration = static_cast<_float>(animationInfo._duration);

	// 애니메이션 초당 속도.
	_animationDesc._tickPerSecond = static_cast<_float>(animationInfo._tickPerSecond);

	// 애니메이션이 사용하는 뼈의 개수.
	_animationDesc._numChannels = animationInfo._numChannels;

	_CurrentKeyFrame.resize(_animationDesc._numChannels);
	// 애니메이션이 사용하는 뼈의 개수만큼 프레임 크기를 미리 할당.

	for (auto& pChannel : vecChannel)
		_channels.push_back(pChannel);


	for (uint32 i = 0; i < _channels.size(); ++i)
		// 이 애니메이션이 사용하는 채널중에 가장 큰 프레임 개수를 구하는 듯?
		_maxFrameCount = ::max(_maxFrameCount, static_cast<uint32>(_channels[i]->GetKeyFrame().size()));


	//_channels = vecChannel;

	return S_OK;
}

void BinaryAnimation::UpdateTransformationMatrix(vector<class BinaryBone*>& Bones, const _float& timeDelta, vector<BinaryChannel*>& pNextChannel)
{
	if (true == _isFinished)
		return;


	if (pNextChannel.size() != 0 && _animationDesc._trackPosition >= 0.5f)
	{
		pNextChannel.clear();
		_animationDesc._trackPosition = 0.f;
		for (auto& iCurrentKeyFrame : _CurrentKeyFrame) { iCurrentKeyFrame = 0; }
	}

	_animationDesc._trackPosition += _animationDesc._tickPerSecond * timeDelta;

	if (_animationDesc._trackPosition >= _animationDesc._duration)
	{
		if (true == _isLoop)
			_animationDesc._trackPosition = 0.f;
		else
			_isFinished = true;
	}

	uint32 iNumChannel = 0;

	// 포인터로 보내서 이미 커런트 키프레임이 바뀌었음. 본은 모델한테 받아왔음.


	for (uint32 i = 0; i < _channels.size(); ++i)
	{
		KEYFRAME keyFrame = {};


		if (pNextChannel.size() != 0)
		{
			for (uint32 j = 0; j < pNextChannel.size(); ++j)
			{
				if (::strcmp(_channels[i]->GetChannelName(), pNextChannel[j]->GetChannelName()) == false)
					keyFrame = pNextChannel[j]->GetKeyFrame().back();
			}
		}

		_channels[i]->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++], Bones, _animationDesc._trackPosition, keyFrame);
	}
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
