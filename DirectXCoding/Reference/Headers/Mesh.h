#pragma once

#include "VIBuffer.h"
#include "Model.h"
#include "FileUtils.h"

BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
public:
	struct MESH_BUFFER_DESC
	{
		D3D11_BUFFER_DESC _vertexBufferDesc;
		D3D11_BUFFER_DESC _indexBufferDesc;
	};
private:
	explicit Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Mesh(const Mesh& rhs);
	virtual ~Mesh() = default;

public:
	uint32 GetMaterialIndex() const { return _materialIndex; }


public:
	virtual HRESULT InitializePrototype(Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindBoneMatrices(class Shader* shader, const vector<class Bone*>& bones, const char* constantName);


public:
	MESH_BUFFER_DESC* GetMeshBufferDesc() { return &_MeshBufferDesc; }
	VTXMESH* GetVertexMeshBuffer() { return _pVertices; }
	VTXANIMMESH* GetVertexAnimBuffer() { return _pAnimVertex; }
	_ulong* GetIndicesMeshBuffer() { return _pIndices; }

	uint32 GetBlendWeights() { return _blendWeights; }
	uint32 GetMeshEffectBones() { return _numBones; }
	string& GetBoneName() { return _szBoneName; }

	vector<uint32>& vecGetMeshEffectBones() { return _bones; }
	vector<Matrix>& vecGetOffsetMatrices() { return _offsetMatrices; }

private: // IMGUI LOAD && SAVE
	MESH_BUFFER_DESC _MeshBufferDesc;
	VTXANIMMESH* _pAnimVertex = nullptr;
	VTXMESH* _pVertices = nullptr;
	_ulong* _pIndices = nullptr;


	string _szBoneName;
	uint32 _blendWeights = 0;

private:
	uint32 _materialIndex = 0;

private:
	uint32 _numBones = 0; // �޽ø� �����ϴ� �����鿡�� ������ �ִ� ���� ����.
	vector<uint32> _bones; // �޽ô� � ������ ������ �޴°�. ���� �ε����� �� Ŭ������ ��� �ִ� �� �迭�� �ε����� ����.
	vector<Matrix> _offsetMatrices; // ���� ������ ���¸� �޽��� �°Բ� �߰������� �����ؾ��Ѵ�.

private:
	HRESULT ReadyVertexBufferNoneAnim(const aiMesh* mesh, FXMMATRIX pivotMat);
	HRESULT ReadyVertexBufferAnim(const aiMesh* mesh, const Model* model);
public:
	static Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
