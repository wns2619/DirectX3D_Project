#pragma once

#include "DynamicObject.h"

class GridDoor final : public DynamicObject
{
private:
	explicit GridDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit GridDoor(const GridDoor& rhs);
	virtual ~GridDoor() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	static GridDoor* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};