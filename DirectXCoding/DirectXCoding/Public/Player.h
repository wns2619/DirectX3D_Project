#pragma once

#include "LandObject.h"

BEGIN(Engine)
class Renderer;
class Transform;
class BinaryNavi;
END

BEGIN(Client)

class Player final : public LandObject
{
public:
	enum PART { PART_BODY, PART_SURFIRE, PART_END };

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
	void KeyInput(const _float& timeDelta);

public:
	vector<class GameObject*>& GetPlyaerPart() { return m_pPlayerPart; }

private:
	vector<class GameObject*> m_pPlayerPart;

private:
	Renderer* _render			= nullptr;
	Shader* _shader				= nullptr;
	BinaryNavi* _pNavigation	= nullptr;

	uint32 animationcount = 0;
private:
	HRESULT ReadyComponents();
	HRESULT BindShaderResuorces();
	HRESULT ReadyPlayerPart();

public:
	static Player* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END

