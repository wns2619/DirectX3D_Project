#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Transform;
END

BEGIN(Client)

class PartObject abstract : public GameObject
{
public:
	typedef struct tagPartDesc
	{
		Transform* pParentTransform = nullptr;;
		_float* pLifeTime = nullptr;
	}PART_DESC;

protected:
	explicit PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit PartObject(const PartObject& rhs);

	virtual ~PartObject() = default;

public:
	Matrix* Get_SocketBonePtr(const char* pBoneName);
	Matrix Get_SocketPivotMatrix();

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

public:
	const _bool& IsObtainingLight() { return _bObtainLight; }
	const _bool& GetGun() { return _bObtainGun; }

public:
	void SetObtainLight(_bool obtain) { _bObtainLight = obtain; }
	void SetObtainGun(_bool obtain) { _bObtainGun = obtain; }

protected:
	Transform* _pParentTransform = { nullptr };
	Matrix	   _WorldMatrix;

protected:
	_bool _bObtainGun = false;
	_bool _bObtainLight = false;
	_float* _pLifeTime = nullptr;

protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	Shader* _shader = nullptr;
protected:
	HRESULT Compute_RenderMatrix(FXMMATRIX ChildMatrix);


public:
	static PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END