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
	HRESULT InitializePrototype(BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, void* vertices,
		_ulong* Indices, uint32 MeshIndex, vector<uint32>& boneIndex, vector<Matrix>& offsetMatrix, uint32 NumberBone, FXMMATRIX pivotMat = ::XMMatrixIdentity());

	HRESULT InitializePrototype(BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, void* vertices,
		_ulong* Indices, uint32 MeshIndex, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindBoneMatrices(class Shader* shader, const vector<class BinaryBone*>& bones, const char* constantName, FXMMATRIX pivotMatrix);


private:
	MESH_BUFFER_DESC _MeshBufferDesc;

private:
	uint32 _materialIndex = 0;

private:
	uint32 _numBones = 0; // �޽ø� �����ϴ� �����鿡�� ������ �ִ� ���� ����.
	vector<uint32> _bones; // �޽ô� � ������ ������ �޴°�. ���� �ε����� �� Ŭ������ ��� �ִ� �� �迭�� �ε����� ����.
	vector<Matrix> _offsetMatrices; // ���� ������ ���¸� �޽��� �°Բ� �߰������� �����ؾ��Ѵ�.

	_char _szName[MAX_PATH] = "";
private:
	//HRESULT ReadyVertexBufferNoneAnim(const aiBinaryMesh* BinaryMesh, FXMMATRIX pivotMat);
	//HRESULT ReadyVertexBufferAnim(const aiBinaryMesh* BinaryMesh, const Model* model);
public:
	static BinaryMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, void* vertices,
		_ulong* Indices, vector<uint32>& boneIndex, vector<Matrix>& offsetMatrix, uint32 NumberBone, FXMMATRIX pivotMat = ::XMMatrixIdentity());

	static BinaryMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, void* vertices,
		_ulong* Indices, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

