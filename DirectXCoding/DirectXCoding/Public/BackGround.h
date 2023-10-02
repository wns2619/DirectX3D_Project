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

class BackGround final : public GameObject
{
private:
	explicit BackGround(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit BackGround(const GameObject& rhs);
	virtual ~BackGround() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render()						override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;

private:
	Renderer* _renderComponent	= nullptr;
	Shader* _shader				= nullptr;
	Texture* _texture			= nullptr;
	VIBufferRect* _viBuffer		= nullptr;

private:
	_float _x = 0, _y = 0, _sizeX = 0, _sizeY = 0;
	Matrix _viewMatrix = ::XMMatrixIdentity(), _projMatrix = ::XMMatrixIdentity();

private:
	HRESULT	Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual GameObject* Clone(void* argument)		override;
	virtual void Free()								override;
	static BackGround* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};

END
