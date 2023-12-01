#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Renderer;
class Shader;
class VIBuffer_PointInstance;
class Texture;
class Transform;
END


BEGIN(Client)

class FireEffect final : public GameObject
{
private:
	explicit FireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit FireEffect(const FireEffect& rhs);
	virtual ~FireEffect() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual HRESULT Render()						override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;

private:
	Renderer* _pRenderer = nullptr;
	Shader* _pShader = nullptr;
	VIBuffer_PointInstance* _pVIBuffer = nullptr;
	Texture* _pTexture = nullptr;


private:
	_float _fLifeTime = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static FireEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END