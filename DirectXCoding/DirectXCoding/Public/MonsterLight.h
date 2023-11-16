#pragma once

#include "MonsterPartObject.h"

BEGIN(Engine)
class OtherLight;
END

BEGIN(Client)

class MonsterLight final : public MonsterPartObject
{
public:
	struct MONSTER_FLASH : public MonsterPartObject::PART_DESC
	{
		Matrix* pSocketMatrix = nullptr;
		Matrix mSocketPivot;
	};

private:
	explicit MonsterLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit MonsterLight(const MonsterLight& rhs);
	virtual ~MonsterLight() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	OtherLight* GetOwnLight() { return _pLight; }

private:
	Matrix* _pSocketMatrix;
	Matrix _mSocketPivotMatrix;

private:
	HRESULT ReadyComponents();

private:
	OtherLight* _pLight = nullptr;

public:
	static MonsterLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

