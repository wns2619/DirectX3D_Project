#pragma once

#include "LandObject.h"

BEGIN(Engine)
class Renderer;
class Transform;
class Texture;
class BinaryNavi;
class StateMachine;
class Animator;
class Collider;
END

BEGIN(Client)

class Monster final : public LandObject
{
public:
	enum MONSTER_PART { PART_BODY, PART_LIGHT, PART_MONSTER, PART_END };
	enum MONSTER_TEXTURE { MONSTER_DIFFUSE, MONSTER_NORMAL, MONSTER_TEXTURE_END, };
public:
	struct STATE_DESC
	{
		wstring _strShaderName;
		wstring _strModelComponentName;
		wstring _protoTypeTag;
	};
private:
	explicit Monster(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Monster(const Monster& rhs);
	virtual ~Monster() = default;

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

	virtual void OnAssistCollisionEnter(class  Collider* pOther);
	virtual void OnAssistCollisionStay(class Collider* pOther);
	virtual void OnAssistCollisionExit(class Collider* pOther);

public:
	StateMachine* GetStateMachine() { return _pStateMachine; }

private:
	void TrigerBoxEvent(Collider* pOther);
	

public:
	_bool GetOnWater() { return _bOnWater; }
	_bool GetRunOnWater() { return _bRunOnWater; }
	uint32 GetMonsterID() { return _iMonsterID; }
	const int32& GetLife() { return _iLife; }
	_float& GetLifeTime() { return _fLifeTime; }
	_float& GetSurpriseTime() { return _fSurpriseTime; }
	const _bool& GetDeadDelay() { return _bDeadDelay; }


	void SetOnWater(_bool water) { _bOnWater = water; }
	void SetLife(int32 life) { _iLife = life; }
public: // A*
	void SetTarget(class GameObject* pGameObject) { _pTargetObject = pGameObject; }
	class GameObject* GetTargetObject() { return _pTargetObject; }

	Vec4& GetDestination() { return _vDestination; }
	list<Cell*>& GetBestList() { return _bestList; }

	vector<class GameObject*>& GetMonsterPart() { return _pMonsterPart; }

	void MoveAstar(const _float& fTimeDelta);
	void TargetStare(XMVECTOR vGoal, const _float& fTimeDelta);
private:
	Texture* _pTexture[MONSTER_TEXTURE_END] = { nullptr, nullptr };
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;
	StateMachine* _pStateMachine = nullptr;
	Animator* _pAnimator = nullptr;

	uint32 animationcount = 0;

	STATE_DESC _comNames;

private: // Stat
	int32 _iLife = 3;

private:
	_bool _bDeadDelay = false;

	_float _fSurpriseTime = 0.f;
	_float _fLifeTime = 0.f;

	// Basement
	_bool _bOnWater = false;
	_bool _bRunOnWater = false;


	static uint32 _iMonsterCount;
	uint32 _iMonsterID;

	vector<class GameObject*> _pMonsterPart;

private: // A*
	list<Cell*> _bestList;
	Vec4 _vDestination;
	GameObject* _pTargetObject = nullptr;

	
private:
	HRESULT ReadyComponents();
	HRESULT BindShaderResuorces();
	HRESULT ReadyMonsterPart();

public:
	static Monster* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END

