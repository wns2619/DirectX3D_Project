#include "pch.h"
#include "StateMachine.h"
#include "State.h"
#include "Animator.h"
#include "BinaryAnimation.h"
#include "BinaryChannel.h"

StateMachine::StateMachine(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext, COMPONENT_TYPE::STATE_MACHINE)
{

}

StateMachine::StateMachine(const StateMachine& rhs)
    : Component(rhs)
{
}

HRESULT StateMachine::InitializePrototype()
{
    return S_OK;
}

HRESULT StateMachine::Initialize(void* argument)
{
    return S_OK;
}

void StateMachine::UpdateStateMachine(const _float& timeDelta)
{
    State::STATE eState = _pCurState->UpdateState(timeDelta);


    // 여기서 보간이 일어나니까.
    if (_pAnimator != nullptr)
        _pAnimator->UpdateAnimator(timeDelta, eState);

    if (_pAnimator->GetAnimation()->GetFinished())
        eState = State::IDLE;

   if(eState != _eCurState)
      SetState(eState);
}

void StateMachine::UpdateLateStateMachine(const _float& timeDelta)
{
    _pCurState->LateUpdateState(timeDelta);

    if (_pAnimator != nullptr)
        _pAnimator->LateUpdateAnimator(timeDelta);
}

HRESULT StateMachine::AddState(State::STATE eState, State* pState)
{
    if (nullptr == pState)
        return E_FAIL;

    _stateMap.insert({ eState, pState });

    return S_OK;
}



void StateMachine::SetState(State::STATE eState)
{
    auto iter = find_if(_stateMap.begin(), _stateMap.end(), [&](const pair<State::STATE, State*>& pair)
        { return eState == pair.first; });

    if (iter == _stateMap.end())
        return;

    _ePrevState = _eCurState;
    _pPrevState = _pCurState;
    _pCurState = iter->second;
    _pCurState->ChangeSetState();

    _eCurState = eState;

    if (true == _firstOn)
    {
        _pPrevState = iter->second;
        _firstOn = false;


        _pAnimator->SetAnimator(_eCurState);
    }




}

StateMachine* StateMachine::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    StateMachine* pInstance = new StateMachine(device, deviceContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : StateMachine");
        Safe_Release<StateMachine*>(pInstance);
    }

    return pInstance;
}

Component* StateMachine::Clone(void* pArg)
{
    StateMachine* pInstance = new StateMachine(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : StateMachine");
        Safe_Release<StateMachine*>(pInstance);
    }

    return pInstance;
}

void StateMachine::Free()
{
    __super::Free();

    for (auto& mapiter : _stateMap)
        Safe_Release<State*>(mapiter.second);

    _stateMap.clear();
}
