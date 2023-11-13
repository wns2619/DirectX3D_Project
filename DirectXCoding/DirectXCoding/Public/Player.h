#pragma once

#include "LandObject.h"

BEGIN(Engine)
class Renderer;
class Transform;
class BinaryNavi;
class StateMachine;
class Animator;
class Collider;
class OtherLight;
END

BEGIN(Client)

class Player final : public LandObject
{
public:
	enum PART { PART_BODY, PART_CAMERA, PLAYER_LIGHT, PART_END };

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
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

public:
	void KeyInput(const _float& timeDelta);
	void TrigerBoxEvent(Collider* pOther);
	void ScareCamera();
public:
	_bool GetObtainKey() { return _bObtatinKey; }
	_bool GetIsJoom() { return _bIsJoom; }
public:
	void SetInteration(_bool interaction) { _bInteraction = interaction; }
	void SetObtainKey(_bool obtain) { _bObtatinKey = obtain; }
	void SetScare(_bool scare) { _bScare = scare; }
public:
	vector<class GameObject*>& GetPlyaerPart() { return m_pPlayerPart; }
	StateMachine* GetStateMachine() { return _pStateMachine; }
	_bool GetInteraction() { return _bInteraction; }
private:
	vector<class GameObject*> m_pPlayerPart;



private:
	Renderer* _render				= nullptr;
	Shader* _shader					= nullptr;
	StateMachine* _pStateMachine	= nullptr;
	Animator* _pAnimator			= nullptr;

	uint32 animationcount = 0;


private:
	_bool _bInteraction = false;
	_bool _bObtatinKey = false;
	_bool _bIsJoom = false;

private:
	_bool _bScare = false;
	uint32 _iPeriod = 0;
	int32 _iPulseTick = 11;
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

