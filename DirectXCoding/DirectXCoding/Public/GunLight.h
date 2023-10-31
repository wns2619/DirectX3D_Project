#pragma once

#include "DynamicObject.h"


BEGIN(Client)

class GunLight final : public DynamicObject
{
private:
	explicit GunLight(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit GunLight(const GunLight& rhs);
	virtual ~GunLight() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual HRESULT ReadyCollider()						override;


public:
	static GunLight* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END

