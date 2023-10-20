#pragma once

#include "DynamicObject.h"

class Drum final : public DynamicObject
{
private:
	explicit Drum(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Drum(const Drum& rhs);
	virtual ~Drum() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	static Drum* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};