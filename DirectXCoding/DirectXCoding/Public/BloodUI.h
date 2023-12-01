#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Renderer;
class Shader;
class VIBufferRect;
class Texture;
class Transform;
END


BEGIN(Client)

class BloodUI final : public GameObject
{
private:
	explicit BloodUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BloodUI(const BloodUI& rhs);
	virtual ~BloodUI() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual HRESULT Render()						override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;

public:
	void SetHit(_bool hit) { _bHitBullet = hit; }
	void SetResetTime(_float time) { _fLifeTime = time; }

private:
	Renderer* _pRenderer = nullptr;
	Shader* _pShader = nullptr;
	VIBufferRect* _pVIBuffer[2] = { nullptr , nullptr };
	Texture* _pTexture[2] = { nullptr, nullptr };
	Transform* _pTransform = nullptr;
private:
	_float _fX = 0.f, _fY = 0.f, _fSizeX = 0.f, _fSizeY = 0.f;
	Matrix _mViewMatrix[2];
	Matrix _mProjMatrix[2];

	_float _fLifeTime = 0.f;
	_bool _bHitBullet = false;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources(uint32 Iindex);

public:
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
	static BloodUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
};

END

