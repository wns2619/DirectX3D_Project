#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class BreakDoor final : public DynamicObject
{
private:
	explicit BreakDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit BreakDoor(const BreakDoor& rhs);
	virtual ~BreakDoor() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

	virtual void OnAssistCollisionEnter(class  Collider* pOther);
	virtual void OnAssistCollisionStay(class Collider* pOther);
	virtual void OnAssistCollisionExit(class Collider* pOther);

public:
	const _bool& GetIsBreak() { return _bIsBreak; }
	const Vec4& GetPrevPosition() { return _vPrevPosition; }
public:
	virtual HRESULT ReadyCollider()					override;

private:
	_bool _bIsBreak = false;
	Vec4 _vPrevPosition;

private:
	_bool _bChageMoment = false;
	_float _fTime = 0.f;
private:
	vector<class GameObject*> _pDoorArt;

	HRESULT ReadyDoorArt();

public:
	static BreakDoor* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END