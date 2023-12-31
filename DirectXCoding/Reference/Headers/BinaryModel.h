#pragma once

#include "Component.h"
#include "AsTypes.h"
#include "FileUtils.h"

BEGIN(Engine)

class ENGINE_DLL BinaryModel final : public Component
{
public:
	enum class MODEL_TYPE { NONE, ANIM, TYPE_END };
private:
	BinaryModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	BinaryModel(const BinaryModel& rhs);
	virtual ~BinaryModel() = default;

public:
	uint32 GetNumMeshes() const { return m_iNumMeshes; }
	int32 GetBoneIndex(const char* boneName) const;
	Matrix* GetBoneMatrix(const _char* pBoneName) const;
	class BinaryBone* GetBone(const _char* pNodeName) const;

public:
	virtual HRESULT InitializePrototype(MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT BindBoneMatrices(class Shader* shader, uint32 meshIndex, const char* constantName);
	HRESULT BindMaterialTexture(class Shader* shader, const char* constantName, uint32 meshIndex, TextureType type);
	HRESULT Render(uint32 meshIndex);

public:
	HRESULT BinaryModelStatic(shared_ptr<FileUtils> file, const string& pBinaryModelFilePath, FXMMATRIX pivotMat);
	HRESULT BinaryModelDynamic(shared_ptr<FileUtils> file, const string& pBinaryModelFilePath);

public:
	vector<class BinaryMesh*>* GetMeshes() { return &m_Meshes; }
	vector<MESH_MATERIAL>* GetMaterial() { return &_materials; }
	MODEL_TYPE GetBinaryModelType() { return _ModelType; }
	Matrix GetPivotMatrix() { return _pivotMatrix; }
	uint32 GetMaterialCount() { return _numMaterial; }

	vector<class BinaryAnimation*>& GetBinaryAnimation() { return _animations; }
	vector<class BinaryBone*>& GetBinaryBones() { return _bones; }
private:
	uint32							m_iNumMeshes = { 0 };
	vector<class BinaryMesh*>		m_Meshes; // 메쉬정보를 들고있으니까 여기다가 넣어야함.

private:
	uint32					_numMaterial = 0;
	vector<MESH_MATERIAL>	_materials;

	Matrix					_pivotMatrix;

private:
	vector<class BinaryBone*>		_bones;

private:
	uint32 _numAnimations = 0;
	vector<class BinaryAnimation*> _animations;
	BinaryAnimation* _nextAnimation = nullptr;
private: // IMGUI
	MODEL_TYPE _ModelType = MODEL_TYPE::TYPE_END;

	wstring _ModelRootfilePath = L"..\\Binaries\\Resources\\MyBinaryModels\\";
	wstring _ModelName;


public:
	static BinaryModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, 
		MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

