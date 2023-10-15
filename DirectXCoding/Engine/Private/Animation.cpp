#include "pch.h"
#include "Animation.h"
#include "Channel.h"

Animation::Animation()
{

}

Animation::Animation(const Animation& rhs)
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
		Safe_AddRef<Channel*>(pChannel);
}

HRESULT Animation::Initialize(const class Model* pModel, const aiAnimation* pAIAnimation)
{
	// 애니메이션의 이름을 저장한다 ex) idel, attack 등등. 
	strcpy_s(_szName, pAIAnimation->mName.data);

	// 애니메이션 길이.
	_animationDesc._duration = static_cast<_float>(pAIAnimation->mDuration);

	// 애니메이션 초당 속도.
	_animationDesc._tickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
	_animationDesc._tickPerSecond /= 1.5;

	// 애니메이션이 사용하는 뼈의 개수.
	_animationDesc._numChannels = pAIAnimation->mNumChannels;

	_CurrentKeyFrame.resize(_animationDesc._numChannels);
	// 애니메이션이 사용하는 뼈의 개수만큼 프레임 크기를 미리 할당.

	for (uint32 i = 0; i < _animationDesc._numChannels; ++i)
	{
		// 각 뼈들을 돌면서 정보를 생성.
		// 이 뼈가 이 애니메이션 안에서 몇ㅊ 개의 동작을 가지는가를 미리 저장해놓으려고 한다.
		Channel* pChannel = Channel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		_channels.push_back(pChannel);
	}

	return S_OK;
}

void Animation::UpdateTransformationMatrix(vector<class Bone*>& Bones, const _float& timeDelta, vector<class Channel*>& beforChannel)
{
	if (true == _isFinished)
		return;


	if (beforChannel.size() != 0 && _animationDesc._trackPosition >= 0.2f)
	{
		beforChannel.clear();
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


	for (uint32 i = 0; i < _channels.size(); ++i)
	{
		KEYFRAME keyFrame = {};

		if (beforChannel.size() != 0)
		{
			for (uint32 j = 0; j < beforChannel.size(); ++j)
			{
				if (::strcmp(_channels[i]->GetChannelName(), beforChannel[j]->GetChannelName()) == false)
					keyFrame = beforChannel[j]->GetKeyFrame().back();
			}
		}

		_channels[i]->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++],Bones, _animationDesc._trackPosition, keyFrame);
	}


	//for (auto& pChannel : _channels)
	//	pChannel->UpdateTransformationMatrix(&_CurrentKeyFrame[iNumChannel++], Bones, _animationDesc._trackPosition);

	// 채널을 키 프레임을 애니메이션에서 관리한다. 
}

void Animation::Reset()
{
	_animationDesc._trackPosition = 0.f;
	_isFinished = false;
	_isLoop = false;

	for (auto& pChannel : _CurrentKeyFrame)
		pChannel = 0;
	// 리셋할 땐 벡터를 돌면서 전부 프레임을 0으로 만들면 된다.
}

void Animation::BlendAnimations(Animation* prevAnimation, _float blendFactor, vector<class Bone*>& Bones)
{
	if (true == _isFinished)
		return;

	vector<Channel*>& vecPrevChannel = prevAnimation->GetChannels();

	if (vecPrevChannel.size() != 0 && _animationDesc._trackPosition >= 0.2f)
	{
		vecPrevChannel.clear();
		_animationDesc._tickPerSecond = 0.f;
		for (auto& currentKeyFrame : _CurrentKeyFrame) { currentKeyFrame = 0; }
	}

	_animationDesc._trackPosition += _animationDesc._tickPerSecond * blendFactor;

	if (_animationDesc._trackPosition >= _animationDesc._duration)
	{
		if (true == _isLoop)
			_animationDesc._trackPosition = 0.f;
		else
			_isFinished = true;
	}


	// 이전 채널과, 현재 채널
	uint32 iNumChannel = 0;
	
	for (uint32 i = 0; i < _channels.size(); ++i)
	{
		KEYFRAME keyFrame = {};

		if (vecPrevChannel.size() != 0)
		{
			for (uint32 j = 0; j < vecPrevChannel.size(); ++j)
			{
				if (::strcmp(_channels[i]->GetChannelName(), _channels[i]->GetChannelName()) == false)
					keyFrame = vecPrevChannel[j]->GetKeyFrame().back();
			}
		}

		_channels[i]->BlendChannel(&_CurrentKeyFrame[iNumChannel++], keyFrame, Bones, blendFactor);
	}


	//for (auto& pChannel : _channels)
	//	_channels.front()->BlendChannel(&_CurrentKeyFrame[iNumChannel++], prevLastKeyFrame, Bones, blendFactor);


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
