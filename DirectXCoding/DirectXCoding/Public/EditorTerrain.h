#pragma once

#include "GameObject.h"

BEGIN(Engine)

class Shader;
class Renderer;
class Transform;
class VIBufferTerrain;

END

BEGIN(Client)

class EditorTerrain final : public GameObject
{
private:
	explicit EditorTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit EditorTerrain(const EditorTerrain& rhs);
	virtual ~EditorTerrain() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render()						override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;

	Vec4 TerrainPick();

private:
	Renderer* _renderComponent = nullptr;
	Shader* _shader = nullptr;
	VIBufferTerrain* _viBuffer = nullptr;

private:
	uint32 _NumVerticesX = 0;
	uint32 _NumVerticesZ = 0;

private:
	HRESULT	Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual GameObject* Clone(void* argument)			override;
	virtual void Free()									override;
	static EditorTerrain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
};


END

