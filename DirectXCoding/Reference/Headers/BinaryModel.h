#pragma once

#include "Component.h"
#include "AsTypes.h"

BEGIN(Engine)

class ENGINE_DLL BinaryModel final : public Component
{
public:
	enum class Model_TYPE { NONE, ANIM, TYPE_END };
private:
	BinaryModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	BinaryModel(const BinaryModel& rhs);
	virtual ~BinaryModel() = default;

public:
	uint32 GetNumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT InitializePrototype(Model_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindBoneMatrices(class Shader* shader, uint32 meshIndex, const char* constantName);
	HRESULT BindMaterialTexture(class Shader* shader, const char* constantName, uint32 meshIndex, aiTextureType type);
	HRESULT PlayAnimation(const _float& timeDelta);
	HRESULT Render(uint32 meshIndex);

public:
	vector<class Mesh*>* GetMeshes() { return &m_Meshes; }
	vector<MESH_MATERIAL>* GetMaterial() { return &_materials; }
	Model_TYPE GetBinaryModelType() { return _ModelType; }
	Matrix GetPivotMatrix() { return _pivotMatrix; }
	uint32 GetMaterialCount() { return _numMaterial; }


private:
	uint32					m_iNumMeshes = { 0 };
	vector<class Mesh*>		m_Meshes; // 메쉬정보를 들고있으니까 여기다가 넣어야함.

private:
	uint32					_numMaterial = 0;
	vector<MESH_MATERIAL>	_materials;

	Matrix					_pivotMatrix;

private:
	vector<class Bone*>		_bones;

private: // IMGUI
	Model_TYPE _ModelType = Model_TYPE::TYPE_END;

	wstring _ModelRootfilePath = L"..\\Binaries\\Resources\\MyBinaryModels\\";
	wstring _ModelName;


public:
	static BinaryModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		Model_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

