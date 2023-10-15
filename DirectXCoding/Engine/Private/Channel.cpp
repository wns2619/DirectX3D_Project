#include "pch.h"
#include "Channel.h"
#include "Bone.h"
#include "Model.h"

Channel::Channel()
{
}

HRESULT Channel::Initialize(const class Model* pModel, const aiNodeAnim* pAIChannel)
{

    strcpy_s(_szName, pAIChannel->mNodeName.data);

    _channelDesc._boneIndex = pModel->GetBoneIndex(_szName);

    _channelDesc._numKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    _channelDesc._numKeyFrames = max(_channelDesc._numKeyFrames, pAIChannel->mNumPositionKeys);

    Vec3		vScale;
    Vec4		vRotation;
    Vec4		vTranslation;

    /* m_iNumKeyFrames : Ư�� �ִϸ��̼� ������ Channel�� �ش��ϴ� ���� ��� ���º�ȯ�� �������°�?!  */
    for (size_t i = 0; i < _channelDesc._numKeyFrames; i++)
    {
        KEYFRAME		KeyFrame;
        ZeroMemory(&KeyFrame, sizeof KeyFrame);

        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(Vec3));
            KeyFrame.time = pAIChannel->mScalingKeys[i].mTime;
        }

        if (pAIChannel->mNumRotationKeys > i)
        {
            /*memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.time = pAIChannel->mRotationKeys[i].mTime;
        }

        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(Vec3));
            vTranslation.w = 1.f;
            KeyFrame.time = pAIChannel->mPositionKeys[i].mTime;
        }

        /* KeyFrame : KeyFrame.fTime�� �� ���� ���ؾ��� KeyFrame.Scale, KeyFrame.Rotation, KeyFrame.Position�� ���� ���¸� ���夾����. */
        KeyFrame.scale = vScale;
        KeyFrame.rotation = vRotation;
        KeyFrame.translation = vTranslation;

        _keyFrames.push_back(KeyFrame);
    }

     return S_OK;
}

void Channel::UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class Bone*>& Bones, _float trackPosition, KEYFRAME& beforeKeyFrame)
{

    /* �ִϸ��̼� �ѹ� ������ �����. */
    if (0.0f == trackPosition)
        *pCurrentKeyFrame = 0;

    Vec3		vScale;
    Vec4		vRotation;
    Vec4		vTranslation;

    KEYFRAME        FrontKeyFrame = _keyFrames.front();
    KEYFRAME		LastKeyFrame = _keyFrames.back();

    /* ������ ���� ���¸� ��� �����ϳ�. */

    if (beforeKeyFrame.time != 0.f)
    {
        _float fTransitionTime = min(trackPosition, 0.2f);
        _float fRatio = fTransitionTime / 0.2f;

        Vec4		vSourScale = XMLoadFloat3(&beforeKeyFrame.scale);
        Vec4		vDestScale = XMLoadFloat3(&FrontKeyFrame.scale);
        XMStoreFloat3(&vScale, XMVectorLerp(vSourScale, vDestScale, fRatio));

        Vec4		vSourRotation = XMLoadFloat4(&beforeKeyFrame.rotation);
        Vec4		vDestRotation = XMLoadFloat4(&FrontKeyFrame.rotation);
        XMStoreFloat4(&vRotation, XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio));

        Vec4		vSourTranslation = XMLoadFloat4(&beforeKeyFrame.translation);
        Vec4		vDestTranslation = XMLoadFloat4(&FrontKeyFrame.translation);
        XMStoreFloat4(&vTranslation, XMVectorLerp(vSourTranslation, vDestTranslation, fRatio));
    }
    else if (trackPosition >= LastKeyFrame.time)
    {
        *pCurrentKeyFrame = _keyFrames.size() - 1;
        vScale = LastKeyFrame.scale;
        vRotation = LastKeyFrame.rotation;
        vTranslation = Vec4(LastKeyFrame.translation.x, LastKeyFrame.translation.y, LastKeyFrame.translation.z, 1.f);
    }
    /* Ư�� Ű�����Ӱ� Ű������ ���̿� �ִ�. */
    else
    {
        while (trackPosition >= _keyFrames[*pCurrentKeyFrame + 1].time)
            ++*pCurrentKeyFrame;

        _float		fRatio = (trackPosition - _keyFrames[*pCurrentKeyFrame].time) / (_keyFrames[*pCurrentKeyFrame + 1].time - _keyFrames[*pCurrentKeyFrame].time);

        Vec4		vSourScale = XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame].scale);
        Vec4		vDestScale = XMLoadFloat3(&_keyFrames[*pCurrentKeyFrame + 1].scale);
        XMStoreFloat3(&vScale, XMVectorLerp(vSourScale, vDestScale, fRatio));

        Vec4		vSourRotation = XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].rotation);
        Vec4		vDestRotation = XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].rotation);
        XMStoreFloat4(&vRotation, XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio));

        Vec4		vSourTranslation = XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame].translation);
        Vec4		vDestTranslation = XMLoadFloat4(&_keyFrames[*pCurrentKeyFrame + 1].translation);
        XMStoreFloat4(&vTranslation, XMVectorLerp(vSourTranslation, vDestTranslation, fRatio));
    }

    /* �������(ũ����)�� �����. */
    Matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat4(&vTranslation));

    Bones[_channelDesc._boneIndex]->SetTransformationMatrix(TransformationMatrix);

}

void Channel::BlendChannel(uint32* pCurrentKeyFrame, KEYFRAME& prevLastKeyFrame, vector<class Bone*>& bones, _float blentTime)
{

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
