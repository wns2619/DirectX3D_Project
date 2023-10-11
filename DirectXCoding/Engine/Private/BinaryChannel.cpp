#include "pch.h"
#include "BinaryChannel.h"
#include "BinaryBone.h"
#include "BinaryModel.h"

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

    Vec3 scale;
    Vec4 rotation;
    Vec4 translation;


    _keyFrames = vecKey;

    ///* m_iNumKeyFrames : 특정 애니메이션 내에서 Channel에 해당하는 뼈가 몇개의 상태변환을 가져가는가?!  */
    //for (uint32 i = 0; i < _channelDesc._numKeyFrames; ++i)
    //{
    //    KEYFRAME keyFrame;
    //    ZeroMemory(&keyFrame, sizeof(keyFrame));

    //    if (channelinfo.mNumScalingKeys > i)
    //    {
    //        ::memcpy(&scale, &channelinfo.mScalingKeys[i].mValue, sizeof(Vec3));
    //        keyFrame.time = channelinfo.mScalingKeys[i].mTime;
    //    }

    //    if (channelinfo.mNumRotationKeys > i)
    //    {
    //        rotation.x = pAIChannel->mRotationKeys[i].mValue.x;
    //        rotation.y = pAIChannel->mRotationKeys[i].mValue.y;
    //        rotation.z = pAIChannel->mRotationKeys[i].mValue.z;
    //        rotation.w = pAIChannel->mRotationKeys[i].mValue.w;
    //        keyFrame.time = pAIChannel->mRotationKeys[i].mTime;
    //    }

    //    if (pAIChannel->mNumPositionKeys > i)
    //    {
    //        ::memcpy(&translation, &pAIChannel->mPositionKeys[i].mValue, sizeof(Vec3));
    //        translation.w = 1.f;
    //        keyFrame.time = pAIChannel->mPositionKeys[i].mTime;
    //    }

    //    /* KeyFrame : KeyFrame.fTime에 이 뼈가 취해야할 KeyFrame.Scale, KeyFrame.Rotation, KeyFrame.Position에 대한 상태를 저장ㅎ나다. */
    //    keyFrame.scale = scale;
    //    keyFrame.rotation = rotation;
    //    keyFrame.translation = translation;

    //    _keyFrames.push_back(keyFrame);
    //}

    return S_OK;
}

void BinaryChannel::UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class BinaryBone*>& Bones, _float trackPosition)
{
    // 애니메이션이 끝나면 재 시작.
    if (0.0f == trackPosition)
        *pCurrentKeyFrame = 0;

    Vec3 scale;
    Vec4 rotation;
    Vec4 translation;

    // 마지막 키프레임과 비교.
    KEYFRAME LastkeyFrame = _keyFrames.back();

    // 트랙 포지션이 마지막 키 프레임보다 크거나 같다면.
    if (trackPosition >= LastkeyFrame.time)
    {
        // 마지막 키프레임의 크기, 회전, 이동을 그대로 유지
        scale = LastkeyFrame.scale;
        rotation = LastkeyFrame.rotation;
        translation = LastkeyFrame.translation;
    }
    else
    {
        // 트랙 포지션이 다음 프레임의 시간보다 크거나 같다면 프레임이 넘어온 것임.
        if (trackPosition >= _keyFrames[*pCurrentKeyFrame + 1].time)
            ++*pCurrentKeyFrame;

        _float ratio = (trackPosition - _keyFrames[*pCurrentKeyFrame].time) / (_keyFrames[*pCurrentKeyFrame + 1].time - _keyFrames[*pCurrentKeyFrame].time);
        // 키 프레임 사이를 보간한 값이다. 0 ~ 1 사이로 만드는 공식이고
        // 다음 프레임에서 현재 프레임을 뺀 값으로 들어온 트랙포지션과 키 프레임을 뺀다.
        // ex) 현 프레임이 10이고, 다음 프레임이 20이라면    (15 - 10)  / ( 20 - 10 ) -> 이런식으로 5 / 10 이 되므로 1/2 만큼 온 것임.


        // 스케일 로테이션 트렌슬레이션도 보간한다.
        Vec4 sourScale = ::XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame].scale);
        Vec4 destScale = ::XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame + 1].scale);
        ::XMStoreFloat3(&scale, ::XMVectorLerp(sourScale, destScale, ratio));

        Vec4 sourRotation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].rotation);
        Vec4 destRotation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].rotation);
        ::XMStoreFloat4(&rotation, ::XMQuaternionSlerp(sourRotation, destRotation, ratio));

        Vec4 sourTranslation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].translation);
        Vec4 destTranslation = ::XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].translation);
        ::XMStoreFloat4(&translation, ::XMVectorLerp(sourTranslation, destTranslation, ratio));
    }

    // 크기 * 자전 * 이동 행렬 == Affine
    Matrix TransformationMatrix = ::XMMatrixAffineTransformation(::XMLoadFloat3(&scale),
        ::XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&rotation), ::XMLoadFloat4(&translation));

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
