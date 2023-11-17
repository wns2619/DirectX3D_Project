#pragma once

#include "MonsterPartObject.h"

BEGIN(Client)

class MonsterAxe final : public MonsterPartObject
{
public:
	struct MONSTER_AXE : public MonsterPartObject::PART_DESC
	{
		Matrix* pSocketMatrix = nullptr;
		Matrix mSocketPivot;
	};


private:
	explicit MonsterAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit MonsterAxe(const MonsterAxe& rhs);
	virtual ~MonsterAxe() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	Matrix* _pSocketMatrix = nullptr;
	Matrix _mSocketPivotMatrix;

public:
	static MonsterAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

