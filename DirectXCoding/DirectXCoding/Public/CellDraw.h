#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Renderer;
class Cell;
END

BEGIN(Client)

class CellDraw final : public GameObject
{
private:
	explicit CellDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit CellDraw(const CellDraw& rhs);
	virtual ~CellDraw() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render()							override;
	virtual void PriorityTick(const _float& timeDelta)	override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;

private:
	Renderer*	_pRenderer	= nullptr;
	Shader*		_pShader	= nullptr;
	Cell*		_pCell		= nullptr;


private:
	HRESULT	Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual GameObject* Clone(void* argument)			override;
	virtual void Free()									override;
	static CellDraw* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};

END

