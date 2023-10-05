#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
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
