#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class CeilingChain final : public DynamicObject
{
private:
	explicit CeilingChain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit CeilingChain(const CeilingChain& rhs);
	virtual ~CeilingChain() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	static CeilingChain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END