#pragma once

#include "DynamicObject.h"

class Cage final : public DynamicObject
{
public:
	enum CAGE_TYPE { CAGE_1, CAGE_2, CAGE_3, CAGE_4, CAGE_END };

private:
	explicit Cage(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CAGE_TYPE eType);
	explicit Cage(const Cage& rhs);
	virtual ~Cage() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

private:
	CAGE_TYPE	_eType;

public:
	static Cage* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CAGE_TYPE eType);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};