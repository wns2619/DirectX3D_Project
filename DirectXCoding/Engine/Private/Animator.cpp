#include "pch.h"
#include "Animator.h"
#include "BinaryAnimation.h"
#include "State.h"
#include "BinaryBone.h"
#include "BinaryChannel.h"

Animator::Animator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext, COMPONENT_TYPE::ANIMATOR)
{
}

Animator::Animator(const Animator& rhs)
    : Component(rhs)
{

}

HRESULT Animator::Initialize(void* pArg)
{
    return S_OK;
}   

HRESULT Animator::InitializePrototype()
{
    return S_OK;
}

void Animator::UpdateAnimator(const _float& timeDelta, State::STATE eState)
{
    if (nullptr == _pCurAnimation)
        return;
    
    if (_eCurState != eState)
    {
        _eCurState = eState;
        _fChangeTrackPosition = 0.f;
        _bAnimationChange = true;
    }

    if (true == _bAnimationChange)
        ChangeAnimation(0.2f, timeDelta, eState);

    if(false == _bAnimationChange)
        _pCurAnimation->UpdateTransformationMatrix(*_pBinaryBone, timeDelta);


    for (auto& bone : *_pBinaryBone)
        bone->UpdateCombinedTransformMatrix(*_pBinaryBone);

}

void Animator::LateUpdateAnimator(const _float& timeDelta)
{
}

void Animator::SetAnimator(State::STATE eState)
{
    auto iter = find_if(_mapAnimation.begin(), _mapAnimation.end(),
        [&](const pair<State::STATE, BinaryAnimation*>& pair)
        {
            return eState == pair.first;
        });

    if (iter == _mapAnimation.end())
        return;


    _ePrevState = _eCurState;
    _eCurState = eState;

    _pCurAnimation = iter->second;
    _pCurAnimation->Reset();

    if (_bFirstSetting)
    {
        _ePrevState = eState;
        _bFirstSetting = false;

        auto iter = _mapAnimation.find(State::IDLE);
        _pPrevAnimation = (*iter).second;
    }


    //if (_ePrevState != _eCurState)
    //{
    //    _fChangeTrackPosition = 0.f;
    //    _bAnimationChange = true;
    //}

}

HRESULT Animator::AddAnimation(State::STATE eState, BinaryAnimation* pAnimation, 
    vector<class BinaryBone*>* pBone, _bool loop, _float perSpeed)
{
    if (nullptr == pAnimation)
        return E_FAIL;

    _mapAnimation.insert({ eState, pAnimation });
    _pBinaryBone = pBone;
    pAnimation->SetLoop(loop);
    pAnimation->SetAnimationSpeed(perSpeed);

    return S_OK;
}

HRESULT Animator::ChangeAnimation(_float duration, const _float& timeDelta, State::STATE eState)
{
    _fChangeTrackPosition += timeDelta;

    Vec3 vScale;
    Vec4 vRotation;
    Vec4 vTranslation;

    for (auto& pCurChannel : _pCurAnimation->GetChannels())
    {
        for (auto& pNextChannel : _pPrevAnimation->GetChannels())
        {
            if (pCurChannel->GetChannelDesc()._boneIndex == pNextChannel->GetChannelDesc()._boneIndex)
            {
                KEYFRAME curKeyFrame = pCurChannel->GetCurrentKeyFrame();
                KEYFRAME nextKeyFrame = pNextChannel->GetKeyFrame().front();

                while (_fChangeTrackPosition >= duration)
                {
                    _bAnimationChange = false;
                    SetAnimator(eState);
                    //_pCurAnimation->Reset();
                    return S_OK;
                }

                _float ratio = (_fChangeTrackPosition - 0.f) / duration;

                // 스케일 로테이션 트렌슬레이션도 보간한다.
                Vec4 sourScale = ::XMLoadFloat3(&curKeyFrame.scale);
                Vec4 destScale = ::XMLoadFloat3(&nextKeyFrame.scale);
                ::XMStoreFloat3(&vScale, ::XMVectorLerp(sourScale, destScale, ratio));

                Vec4 sourRotation = ::XMLoadFloat4(&curKeyFrame.rotation);
                Vec4 destRotation = ::XMLoadFloat4(&nextKeyFrame.rotation);
                ::XMStoreFloat4(&vRotation, ::XMQuaternionSlerp(sourRotation, destRotation, ratio));

                Vec4 sourTranslation = ::XMLoadFloat4(&curKeyFrame.translation);
                Vec4 destTranslation = ::XMLoadFloat4(&nextKeyFrame.translation);
                ::XMStoreFloat4(&vTranslation, ::XMVectorLerp(sourTranslation, destTranslation, ratio));


                Matrix TransformationMatrix = ::XMMatrixAffineTransformation(::XMLoadFloat3(&vScale),
                    ::XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), ::XMLoadFloat4(&vTranslation));

                (*_pBinaryBone)[pCurChannel->GetChannelDesc()._boneIndex]->SetTransformationMatrix(TransformationMatrix);
            }
        }
    }

    return S_OK;
}

_bool Animator::IsCurKeyFrame(uint32 iIndex)
{
    vector<uint32> pCurrentKeyFrame = _pCurAnimation->GetCurrentKeyFrame();

    for (auto& pCurKey : pCurrentKeyFrame)
    {
        if (iIndex <= pCurKey)
            return true;
    }

    return false;
}

Animator* Animator::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    Animator* pInstance = new Animator(device, deviceContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : Animator");
        Safe_Release<Animator*>(pInstance);
    }

    return pInstance;
}

Component* Animator::Clone(void* pArg)
{
    Animator* pInstance = new Animator(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Animator");
        Safe_Release<Animator*>(pInstance);
    }

    return pInstance;
}

void Animator::Free()
{
    __super::Free();


    _mapAnimation.clear();
}
