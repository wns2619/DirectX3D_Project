#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Model;
class Shader;
class Renderer;
class Transform;
END

BEGIN(Client)

class Player final : public GameObject
{
private:
	explicit Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Player(const Player& rhs);
	virtual ~Player() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

public:
	_bool Intersects(POINT pt);

private:
	Renderer* _render		= nullptr;
	Shader* _shader			= nullptr;
	//Light* _light			= nullptr;

private:
	HRESULT ReadyComponents();
	HRESULT BindShaderResuorces();

public:
	static Player* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END

