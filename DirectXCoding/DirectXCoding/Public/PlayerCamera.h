#pragma once

#include "Camera.h"

BEGIN(Engine)


class PlayerCamera final : public Camera
{
private:
	explicit PlayerCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit PlayerCamera(const PlayerCamera& rhs);
	virtual ~PlayerCamera();

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* argument)		override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;

private:
	HRESULT	Ready_Components();

public:
	virtual GameObject* Clone(void* argument)		override;
	virtual void Free()								override;
	static PlayerCamera* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};

END;

