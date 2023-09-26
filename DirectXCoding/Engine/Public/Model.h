#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Model final : public Component
{
private:
	Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Model(const Model& rhs);
	virtual ~Model() = default;

public:
	virtual HRESULT InitializePrototype(const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Render();

private: /* .fbx������ ��� �о��ִ� ��Ȱ */
	Assimp::Importer			m_Importer;

	/* m_Importer�� �о��� �����͵��� �����Ѵ�. */
	const aiScene* m_pAIScene = { nullptr };

private:
	uint32					m_iNumMeshes = { 0 };
	vector<class Mesh*>		m_Meshes;

private:
	HRESULT ReadyMeshes();

public:
	static Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

