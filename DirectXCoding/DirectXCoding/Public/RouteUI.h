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

class RouteUI final : public GameObject
{
private:
	explicit RouteUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit RouteUI(const GameObject& rhs);
	virtual ~RouteUI() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual HRESULT Render()						override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;

private:
	Renderer* _pRenderer = nullptr;
	Shader* _pShader = nullptr;
	VIBufferRect* _pVIBuffer = nullptr;
	Texture* _pTexture = nullptr;

private:
	_float _fX = 0.f, _fY = 0.f, _fSizeX = 0.f, _fSizeY = 0.f;
	Matrix _mViewMatrix = Matrix::Identity, _mProjMatrix = Matrix::Identity;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
	static RouteUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
};

END

