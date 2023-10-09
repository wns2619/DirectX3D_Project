#pragma once

#include "VIBuffer.h"
#include "BinaryModel.h"
#include "FileUtils.h"

BEGIN(Engine)

class ENGINE_DLL BinaryMesh final : public VIBuffer
{
public:
	struct MESH_BUFFER_DESC
	{
		D3D11_BUFFER_DESC _vertexBufferDesc;
		D3D11_BUFFER_DESC _indexBufferDesc;
	};
private:
	explicit BinaryMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit BinaryMesh(const BinaryMesh& rhs);
	virtual ~BinaryMesh() = default;

public:
	uint32 GetMaterialIndex() const { return _materialIndex; }

public:
	virtual HRESULT InitializePrototype(BinaryModel::MODEL_TYPE type, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, VTXMESH* vertices,
		_ulong* Indices, uint32 MeshIndex, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindBoneMatrices(class Shader* shader, const vector<class Bone*>& bones, const char* constantName);


private:
	uint32 _boneIndex;

	MESH_BUFFER_DESC _MeshBufferDesc;

	VTXMESH* _pVertices = nullptr;
	_ulong* _pIndices = nullptr;

private:
	uint32 _materialIndex = 0;

private:
	uint32 _numBones = 0; // �޽ø� �����ϴ� �����鿡�� ������ �ִ� ���� ����.
	vector<uint32> _bones; // �޽ô� � ������ ������ �޴°�. ���� �ε����� �� Ŭ������ ��� �ִ� �� �迭�� �ε����� ����.
	vector<Matrix> _offsetMatrices; // ���� ������ ���¸� �޽��� �°Բ� �߰������� �����ؾ��Ѵ�.

private:
	//HRESULT ReadyVertexBufferNoneAnim(const aiBinaryMesh* BinaryMesh, FXMMATRIX pivotMat);
	//HRESULT ReadyVertexBufferAnim(const aiBinaryMesh* BinaryMesh, const Model* model);
public:
	static BinaryMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		BinaryModel::MODEL_TYPE type, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, VTXMESH* vertices, 
		_ulong* Indices, FXMMATRIX pivotMat);


	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END