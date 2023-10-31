#pragma once

#include "GameObject.h"

BEGIN(Engine)

class UI abstract : public GameObject
{
protected:
	explicit UI(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit UI(const UI& rhs);
	virtual ~UI() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT	Initialize(void* argument);
	virtual void PriorityTick(const _float& timeDelta);
	virtual void Tick(const _float& timeDelta);
	virtual void LateTick(const _float& timeDelta);
	virtual HRESULT Render();

private:

public:
	virtual GameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

