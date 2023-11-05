#pragma once

#include "DynamicObject.h"

BEGIN(Client)

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
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

public:
	virtual HRESULT ReadyCollider()					override;

private:
	_bool _bGridDoorKey = false;


public:
	static GridDoor* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END