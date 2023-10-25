#pragma once

#include "Component.h"
#include "State.h"

BEGIN(Engine)


class ENGINE_DLL StateMachine final : public Component
{
private:
	explicit StateMachine(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit StateMachine(const StateMachine& rhs);
	virtual ~StateMachine() = default;

public:
	virtual HRESULT	InitializePrototype();
	virtual HRESULT Initialize(void* argument);

public:
	void UpdateStateMachine(const _float& timeDelta);
	void UpdateLateStateMachine(const _float& timeDelta);


public:
	// 상태를 맵에 추가
	HRESULT AddState(State::STATE eState, State* pState);

public:
	void SetState(State::STATE eState);
	void SetAnimator(class Animator* pAnimator) { _pAnimator = pAnimator; }

public:
	virtual class Animator* GetAnimator() { return _pAnimator; }
protected:
	map<State::STATE, State*> _stateMap;
	
	State* _pCurState = nullptr;
	State* _pPrevState = nullptr;

	State::STATE _eCurState = State::STATE::STATE_END;
	State::STATE _ePrevState = State::STATE::STATE_END;

	_bool _firstOn = true;

	_float time = 0.f;

	class Animator* _pAnimator = nullptr;

public:
	static StateMachine* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};


END

