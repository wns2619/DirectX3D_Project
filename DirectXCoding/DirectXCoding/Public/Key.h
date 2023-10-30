#pragma once

#include "DynamicObject.h"


BEGIN(Client)

class Key final : public DynamicObject
{
private:
	explicit Key(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Key(const Key& rhs);
	virtual ~Key() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual HRESULT ReadyCollider()						override;


public:
	static Key* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END

