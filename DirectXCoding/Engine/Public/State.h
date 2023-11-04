#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL State : public Base 
{
public:
	enum STATE { SHOOT, RELOAD, IDLE, WALK, RUN , DANCE, STATE_END };
protected:
	explicit State();
	virtual ~State() = default;

public:
	virtual HRESULT ReadyState(class GameObject* pOwner) = 0;

	virtual STATE UpdateState(const _float& timeDelta) = 0;
	virtual void LateUpdateState(const _float& timeDelta) = 0;
	virtual void RenderState() = 0;

	virtual void ChangeSetState() = 0;

public:
	virtual STATE KeyInput(const _float& timeDelta) = 0;

public:
	virtual STATE GetState() { return _eState; }
	virtual STATE PrevState() { return _ePrevState; }


protected:
	class GameObject* _pOwner = nullptr;

	STATE _eState = STATE::STATE_END;
	STATE _ePrevState = STATE::STATE_END;

public:
	virtual void Free() override;
};

END

