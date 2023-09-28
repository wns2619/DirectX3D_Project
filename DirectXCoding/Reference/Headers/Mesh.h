#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
	explicit Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Mesh(const Mesh& rhs);
	virtual ~Mesh() = default;

public:
	uint32 GetMaterialIndex() const { return _materialIndex; }

public:
	virtual HRESULT InitializePrototype(const aiMesh* pAIMesh, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

private:
	uint32 _materialIndex = 0;

public:
	static Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, FXMMATRIX pivotMat);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
