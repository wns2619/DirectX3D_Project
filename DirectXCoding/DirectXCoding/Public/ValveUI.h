#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Transform;
class Texture;
class VIBufferNormalRect;
class Renderer;
END

BEGIN(Client)

class ValveUI final : public GameObject
{
public:
	struct PART_DESC
	{
		Transform* pParentTransform = nullptr;
		uint32 iID = 0;
	};
private:
	explicit ValveUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit ValveUI(const ValveUI& rhs);
	virtual ~ValveUI() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

private:
	Transform* _pParentTransform = nullptr;
	uint32 _iID;
	Matrix _WorldMatrix;
	XMMATRIX _mParentOriginalMatrix;

	_bool _bFirst = false;
private:
	Shader* _pShader = nullptr;
	Texture* _pTexture = nullptr;
	VIBufferNormalRect* _pVIBuffer = nullptr;
	Renderer* _pRenderer = nullptr;

private:
	HRESULT Compute_RenderMatrix(FXMMATRIX ChildMatrix);
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResource();

public:
	static ValveUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

