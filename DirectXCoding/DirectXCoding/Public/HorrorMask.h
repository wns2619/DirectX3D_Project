#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class HorrorMask final : public DynamicObject
{
private:
	explicit HorrorMask(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit HorrorMask(const HorrorMask& rhs);
	virtual ~HorrorMask() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	virtual HRESULT ReadyCollider()					override;

public:
	static HorrorMask* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END