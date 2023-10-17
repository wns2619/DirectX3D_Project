#include "pch.h"
#include "BinaryChannel.h"
#include "BinaryBone.h"
#include "BinaryModel.h"
#include "BinaryAnimation.h"

BinaryChannel::BinaryChannel()
{
}

HRESULT BinaryChannel::Initialize(const BinaryModel* pModel, const _char* channelName, const CHANNEL_DESC channelInfo, vector<KEYFRAME>& vecKey)
{
    ::strcpy_s(_szName, channelName);

    _channelDesc._boneIndex = pModel->GetBoneIndex(_szName);

    // IMGUI SAVE && LOAD
    {
        _channelDesc.mNumScalingKeys = channelInfo.mNumScalingKeys;
        _channelDesc.mNumRotationKeys = channelInfo.mNumRotationKeys;
        _channelDesc.mNumPositionKeys = channelInfo.mNumPositionKeys;
    }


    // 어떤 유형의 애니메이션이 노드에 더 많은 영향을 끼칠 지 판단.
    _channelDesc._numKeyFrames = max(channelInfo.mNumScalingKeys, channelInfo.mNumRotationKeys);
    _channelDesc._numKeyFrames = max(_channelDesc._numKeyFrames, channelInfo.mNumPositionKeys);

    _keyFrames = vecKey;

    return S_OK;
}

void BinaryChannel::UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class BinaryBone*>& Bones, _float trackPosition)
{
    // 애니메이션이 끝나면 재 시작.
    if (0.0f == trackPosition)
        *pCurrentKeyFrame = 0;

    KEYFRAME        FrontKeyFrame = _keyFrames.front();
    KEYFRAME		LastKeyFrame = _keyFrames.back();

    /* 마지막 뼈의 상태를 계속 유지하낟. */
    if (trackPosition >= LastKeyFrame.time)
    {
        // 마지막 키프레임의 크기, 회전, 이동을 그대로 유지
        *pCurrentKeyFrame = _keyFrames.size() - 1;
        _currentKeyFrame.scale = LastKeyFrame.scale;
        _currentKeyFrame.rotation = LastKeyFrame.rotation;
        _currentKeyFrame.translation = LastKeyFrame.translation;
        _currentKeyFrame.time = trackPosition;
    }
    else
    {
        // 트랙 포지션이 다음 프레임의 시간보다 크거나 같다면 프레임이 넘어온 것임.
        while (trackPosition >= _keyFrames[*pCurrentKeyFrame + 1].time)
            ++*pCurrentKeyFrame;

        _float ratio = (trackPosition - _keyFrames[*pCurrentKeyFrame].time) / (_keyFrames[*pCurrentKeyFrame + 1].time - _keyFrames[*pCurrentKeyFrame].time);
        // 키 프레임 사이를 보간한 값이다. 0 ~ 1 사이로 만드는 공식이고
        // 다음 프레임에서 현재 프레임을 뺀 값으로 들어온 트랙포지션과 키 프레임을 뺀다.
        // ex) 현 프레임이 10이고, 다음 프레임이 20이라면    (15 - 10)  / ( 20 - 10 ) -> 이런식으로 5 / 10 이 되므로 1/2 만큼 온 것임.


        // 스케일 로테이션 트렌슬레이션도 보간한다.
        Vec4 sourScale = ::XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame].scale);
        Vec4 destScale = ::XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame + 1].scale);
        ::XMStoreFloat3(&_currentKeyFrame.scale, ::XMVectorLerp(sourScale, destScale, ratio));

        Vec4 sourRotation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].rotation);
        Vec4 destRotation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].rotation);
        ::XMStoreFloat4(&_currentKeyFrame.rotation, ::XMQuaternionSlerp(sourRotation, destRotation, ratio));

        Vec4 sourTranslation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].translation);
        Vec4 destTranslation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].translation);
        ::XMStoreFloat4(&_currentKeyFrame.translation, ::XMVectorLerp(sourTranslation, destTranslation, ratio));

        _currentKeyFrame.time = trackPosition;
    }


    Matrix TransformationMatrix = ::XMMatrixAffineTransformation(::XMLoadFloat3(&_currentKeyFrame.scale),
        ::XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&_currentKeyFrame.rotation), ::XMLoadFloat4(&_currentKeyFrame.translation));

    // 

    // 모델이 들고 있는 뼈한테 Affine Matrix 세팅.
    Bones[_channelDesc._boneIndex]->SetTransformationMatrix(TransformationMatrix);
}

BinaryChannel* BinaryChannel::Create(const BinaryModel* pModel, const _char* channelName, const CHANNEL_DESC channelInfo, vector<KEYFRAME>& vecKey)
{
    BinaryChannel* pInstance = new BinaryChannel();

    if (FAILED(pInstance->Initialize(pModel, channelName, channelInfo, vecKey)))
    {
        MSG_BOX("Failed to Created : Channel");
        Safe_Release<BinaryChannel*>(pInstance);
    }

    return pInstance;
}

void BinaryChannel::Free()
{
    __super::Free();

    _keyFrames.clear();
}
