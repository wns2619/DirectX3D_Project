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
	uint32 GetNumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT InitializePrototype(const string& pModelFilePath, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindMaterialTexture(class Shader* shader, const char* constantName, uint32 meshIndex, aiTextureType type);
	HRESULT Render(uint32 meshIndex);

public: // IMGUI
	void SetModelPath(const string& modelPath) { _modelPath = modelPath; }
	const string& GetModelPath() { return _modelPath; }

	vector<class Mesh*>* GetMeshes() { return &m_Meshes; }
	vector<MESH_MATERIAL>* GetMaterial() { return &_materials; }
private: /* .fbx파일을 열어서 읽어주는 역활 */
	Assimp::Importer			m_Importer;

	/* m_Importer가 읽어준 데이터들을 보관한다. */
	const aiScene* m_pAIScene = { nullptr };

private:
	uint32					m_iNumMeshes = { 0 };
	vector<class Mesh*>		m_Meshes; // 메쉬정보를 들고있으니까 여기다가 넣어야함.

private:
	uint32					_numMaterial = 0;
	vector<MESH_MATERIAL>	_materials;
	
	Matrix					_pivotMatrix;

private: // IMGUI
	string _modelPath = "";

private:
	HRESULT ReadyMeshes();
	HRESULT ReadyMaterial(const string& modelFilePath);

public:
	static Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& pModelFilePath, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

