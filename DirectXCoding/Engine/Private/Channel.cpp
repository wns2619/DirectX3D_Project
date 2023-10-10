#include "pch.h"
#include "Channel.h"
#include "Bone.h"
#include "Model.h"

Channel::Channel()
{
}

HRESULT Channel::Initialize(const class Model* pModel, const aiNodeAnim* pAIChannel)
{
    ::strcpy_s(_szName, pAIChannel->mNodeName.data);

    _channelDesc._boneIndex = pModel->GetBoneIndex(_szName);

    // IMGUI SAVE && LOAD
    {
        _channelDesc.mNumScalingKeys = pAIChannel->mNumScalingKeys;
        _channelDesc.mNumRotationKeys = pAIChannel->mNumRotationKeys;
        _channelDesc.mNumPositionKeys = pAIChannel->mNumPositionKeys;
    }

   
    // � ������ �ִϸ��̼��� ��忡 �� ���� ������ ��ĥ �� �Ǵ�.
    _channelDesc._numKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    _channelDesc._numKeyFrames = max(_channelDesc._numKeyFrames, pAIChannel->mNumPositionKeys);

    Vec3 scale;
    Vec4 rotation;
    Vec4 translation;

    /* m_iNumKeyFrames : Ư�� �ִϸ��̼� ������ Channel�� �ش��ϴ� ���� ��� ���º�ȯ�� �������°�?!  */
    for (uint32 i = 0; i < _channelDesc._numKeyFrames; ++i)
    {
        KEYFRAME keyFrame;
        ZeroMemory(&keyFrame, sizeof(keyFrame));

        if (pAIChannel->mNumScalingKeys > i)
        {
            ::memcpy(&scale, &pAIChannel->mScalingKeys[i].mValue, sizeof(Vec3));
            keyFrame.time = pAIChannel->mScalingKeys[i].mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            rotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            rotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            rotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            rotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            keyFrame.time = pAIChannel->mRotationKeys[i].mTime;
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            ::memcpy(&translation, &pAIChannel->mPositionKeys[i].mValue, sizeof(Vec3));
            translation.w = 1.f;
            keyFrame.time = pAIChannel->mPositionKeys[i].mTime;
        }

        /* KeyFrame : KeyFrame.fTime�� �� ���� ���ؾ��� KeyFrame.Scale, KeyFrame.Rotation, KeyFrame.Position�� ���� ���¸� ���夾����. */
        keyFrame.scale = scale;
        keyFrame.rotation = rotation;
        keyFrame.translation = translation;

        _keyFrames.push_back(keyFrame);
    }

    return S_OK;
}

void Channel::UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class Bone*>& Bones, _float trackPosition)
{
    // �ִϸ��̼��� ������ �� ����.
    if (0.0f == trackPosition)
        *pCurrentKeyFrame = 0;

    Vec3 scale;
    Vec4 rotation;
    Vec4 translation;

    // ������ Ű�����Ӱ� ��.
    KEYFRAME LastkeyFrame = _keyFrames.back();

    // Ʈ�� �������� ������ Ű �����Ӻ��� ũ�ų� ���ٸ�.
    if (trackPosition >= LastkeyFrame.time)
    {
        // ������ Ű�������� ũ��, ȸ��, �̵��� �״�� ����
        scale = LastkeyFrame.scale;
        rotation = LastkeyFrame.rotation;
        translation = LastkeyFrame.translation;
    }
    else
    {
        // Ʈ�� �������� ���� �������� �ð����� ũ�ų� ���ٸ� �������� �Ѿ�� ����.
        if (trackPosition >= _keyFrames[*pCurrentKeyFrame + 1].time)
            ++*pCurrentKeyFrame;

        _float ratio = (trackPosition - _keyFrames[*pCurrentKeyFrame].time) / (_keyFrames[*pCurrentKeyFrame + 1].time - _keyFrames[*pCurrentKeyFrame].time);
        // Ű ������ ���̸� ������ ���̴�. 0 ~ 1 ���̷� ����� �����̰�
        // ���� �����ӿ��� ���� �������� �� ������ ���� Ʈ�������ǰ� Ű �������� ����.
        // ex) �� �������� 10�̰�, ���� �������� 20�̶��    (15 - 10)  / ( 20 - 10 ) -> �̷������� 5 / 10 �� �ǹǷ� 1/2 ��ŭ �� ����.


        // ������ �����̼� Ʈ�������̼ǵ� �����Ѵ�.
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

    // ũ�� * ���� * �̵� ��� == Affine
    Matrix TransformationMatrix = ::XMMatrixAffineTransformation(::XMLoadFloat3(&scale),
        ::XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&rotation), ::XMLoadFloat4(&translation));

    // ���� ��� �ִ� ������ Affine Matrix ����.
    Bones[_channelDesc._boneIndex]->SetTransformationMatrix(TransformationMatrix);
}

Channel* Channel::Create(const class Model* pModel, const aiNodeAnim* pAIChannel)
{
    Channel* pInstance = new Channel();

    if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
    {
        MSG_BOX("Failed to Created : Channel");
        Safe_Release<Channel*>(pInstance);
    }

    return pInstance;
}

void Channel::Free()
{
    __super::Free();

    _keyFrames.clear();
}
