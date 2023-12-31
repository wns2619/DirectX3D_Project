#pragma once

#include "GameObject.h"

BEGIN(Engine)

class Shader;
class Texture;
class Renderer;
class Transform;
class VIBufferTerrain;
class Navigation;
//class Light;

END


BEGIN(Client)

class Terrain final : public GameObject
{

private:
	explicit Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Terrain(const Terrain& rhs);
	virtual ~Terrain() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render()							override;
	virtual void PriorityTick(const _float& timeDelta)	override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;

private:
	Renderer* _renderComponent	= nullptr;
	Shader* _shader				= nullptr;
	Texture* _texture			= nullptr;
	VIBufferTerrain* _viBuffer	= nullptr;
	Navigation* _pNavigation	= nullptr;
	//Light* _light				= nullptr;

private:
	HRESULT	Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual GameObject* Clone(void* argument)			override;
	virtual void Free()									override;
	static Terrain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};

END

