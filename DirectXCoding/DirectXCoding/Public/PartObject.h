#pragma once

#include "GameObject.h"

class PartObject abstract : public GameObject
{
public:
	typedef struct tagPartDesc
	{
		Transform* pParentTransform;
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

public:
	void SetObtainLight(_bool obtain) { _bObtainLight = obtain; }
	void SetObtainGun(_bool obtain) { _bObtainGun = obtain; }

protected:
	Transform* _pParentTransform = { nullptr };
	Matrix	   _WorldMatrix;

protected:
	_bool _bObtainGun = false;
	_bool _bObtainLight = false;

protected: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
	Shader* _shader = nullptr;
protected:
	HRESULT Compute_RenderMatrix(FXMMATRIX ChildMatrix);


public:
	static PartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

