#pragma once

#include "GameObject.h"

BEGIN(Client)

class ValveBody final : public GameObject
{
private:
	explicit ValveBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit ValveBody(const ValveBody& rhs);
	virtual ~ValveBody() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

private:
	HRESULT Ready_Components();
	HRESULT BindShaderResources();

public:
	static ValveBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

