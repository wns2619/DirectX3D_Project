#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Transform;
class Texture;
END

BEGIN(Client)

class MonsterPartObject abstract : public GameObject
{
public:
	struct PART_DESC
	{
		Transform* pParentTransform = nullptr;
		_float* pDissolveTime = nullptr;
	};

protected:
	explicit MonsterPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit MonsterPartObject(const MonsterPartObject& rhs);
	virtual ~MonsterPartObject() = default;

public:
	Matrix* GetSocketBoneMatrix(const _char* pBoneName);
	Matrix GetSocketPivotMatrix();

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

protected:
	Transform* _pParentTransform = nullptr;
	Matrix _mWorldMatrix;
	_float* _pDissolveTime = nullptr;

protected:
	Shader* _pShader = nullptr;
	Texture* _pTexture = nullptr;

protected:
	HRESULT	Compute_RenderMatrix(FXMMATRIX ChildeMatrix);

public:
	static MonsterPartObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual GameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END