#pragma once

#include "Component.h"
#include "AsTypes.h"

BEGIN(Engine)

class ENGINE_DLL Model final : public Component
{
public:
	enum class MODEL_TYPE { NONE, ANIM, TYPE_END };
private:
	Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Model(const Model& rhs);
	virtual ~Model() = default;

public:
	uint32 GetNumMeshes() const { return m_iNumMeshes; }
	int32 GetBoneIndex(const char* boneName) const;
	Matrix* GetBoneMatrix(const _char* pBoneName) const;

public:
	virtual HRESULT InitializePrototype(MODEL_TYPE type, const string& pModelFilePath, FXMMATRIX pivotMat);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT SetUp_Animation(_bool isLoop, uint32 animationIndex);
	HRESULT BindBoneMatrices(class Shader* shader, uint32 meshIndex, const char* constantName);
	HRESULT BindMaterialTexture(class Shader* shader, const char* constantName, uint32 meshIndex, aiTextureType type);
	HRESULT PlayAnimation(const _float& timeDelta);
	HRESULT Render(uint32 meshIndex);


public: // IMGUI

	void SetModelPath(const string& modelPath) { _modelPath = modelPath; }
	const string& GetModelPath() { return _modelPath; }

	vector<class Bone*>& GetBones() { return _bones; }
	vector<class Mesh*>* GetMeshes() { return &m_Meshes; }
	vector<MESH_MATERIAL>* GetMaterial() { return &_materials; }
	vector<class Animation*>& GetAnimation() { return _animations; }
	MODEL_TYPE GetModelType() { return _ModelType; }
	Matrix GetPivotMatrix() { return _pivotMatrix; }
	uint32 GetMaterialCount() { return _numMaterial; }
	uint32 GetNumAnimations() { return _numAnimations; }
private: /* .fbx파일을 열어서 읽어주는 역활 */
	Assimp::Importer			m_Importer;

	/* m_Importer가 읽어준 데이터들을 보관한다. */
	const aiScene* m_pAIScene = { nullptr };

private: // Mesh
	uint32					m_iNumMeshes = { 0 };
	vector<class Mesh*>		m_Meshes; // 메쉬정보를 들고있으니까 여기다가 넣어야함.

	Matrix					_pivotMatrix;

private: // Material
	uint32					_numMaterial = 0;
	vector<MESH_MATERIAL>	_materials;

private: // Bone
	vector<class Bone*>		_bones;

private: // Animation
	uint32 _currenAnimIndex = 0;
	uint32 _prevAnimIndex = 0;
	uint32 _numAnimations = 0;
	vector<class Animation*> _animations;
	vector<class Channel*> _beforeChannel;
	_float _transitionTimeElapsed = 0.2f;
	_float _transitionDuration = 1.0f;
	_bool _transitioningAnimation = false;


private: // IMGUI
	MODEL_TYPE _ModelType = MODEL_TYPE::TYPE_END;


	// Converter
	class Converter* _converter = nullptr;
	string _modelPath = "";
	string _texturePath = "";
	
	shared_ptr<ModelBone> _rootBone;
	vector<shared_ptr<ModelBone>> _modelBones;







	wstring _modelRootfilePath = L"..\\Binaries\\Resources\\MyModels\\";
	wstring _modelName;

private:
	HRESULT ReadyMeshes(MODEL_TYPE type);
	HRESULT ReadyMaterial(const string& modelFilePath);
	HRESULT ReadyBones(const aiNode* node, int32 parentNodeIndex);
	HRESULT ReadyAnimations();

private:
	_float CacluateBlendFactor();

public:
	static Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE type, const string& pModelFilePath, FXMMATRIX pivotMat = ::XMMatrixIdentity());
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END

