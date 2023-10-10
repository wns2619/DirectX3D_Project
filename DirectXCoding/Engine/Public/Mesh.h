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
	uint32 _numBones = 0; // 메시를 구성하는 정점들에게 영향을 주는 뼈의 개수.
	vector<uint32> _bones; // 메시는 어떤 뼈들의 영향을 받는가. 뼈의 인덱스는 모델 클래스가 들고 있는 뼈 배열의 인덱스랑 같다.
	vector<Matrix> _offsetMatrices; // 실제 뼈들의 상태를 메쉬에 맞게끔 추가적으로 변형해야한다.

private:
	HRESULT ReadyVertexBufferNoneAnim(const aiMesh* mesh, FXMMATRIX pivotMat);
	HRESULT ReadyVertexBufferAnim(const aiMesh* mesh, const Model* model);
public:
	static Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
