#include "pch.h"
#include "BinaryModel.h"
#include "Mesh.h"
#include "BinaryMesh.h"
#include "BinaryBone.h"
#include "BinaryAnimation.h"
#include "BinaryChannel.h"
#include "Texture.h"
#include "Utils.h"

BinaryModel::BinaryModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext, COMPONENT_TYPE::MODEL)
{
}

BinaryModel::BinaryModel(const BinaryModel& rhs)
	: Component(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, _numMaterial(rhs._numMaterial)
	, _materials(rhs._materials)
	, _ModelType(rhs._ModelType)
	, _numAnimations(rhs._numAnimations)
	, _pivotMatrix(rhs._pivotMatrix)
{

	for (auto& pAnimation : rhs._animations)
		_animations.push_back(pAnimation->Clone());

	for (auto& boneiter : rhs._bones)
		_bones.push_back(boneiter->Clone());

	for (auto& material : _materials)
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef<Texture*>(material._texture[i]);

	for (auto& mesh : m_Meshes)
		Safe_AddRef<BinaryMesh*>(mesh);
}

int32 BinaryModel::GetBoneIndex(const char* boneName) const
{
	uint32 boneIndex = 0;

	auto iter = find_if(_bones.begin(), _bones.end(), [&](BinaryBone* bone)
		{
			if (false == ::strcmp(bone->GetBoneName(), boneName))
				return true;

			++boneIndex;

			return false;
		});

	if (iter == _bones.end())
		return -1;

	return boneIndex;
}

Matrix* BinaryModel::GetBoneMatrix(const _char* pBoneName) const
{
	auto iter = find_if(_bones.begin(), _bones.end(), [&](BinaryBone* pBone)
		{
			if (false == ::strcmp(pBone->GetBoneName(), pBoneName))
				return true;

			return false;
		});

	return (*iter)->GetCombientTransformMatrixPoint();
}

BinaryBone* BinaryModel::GetBone(const _char* pNodeName) const
{
	auto	iter = find_if(_bones.begin(), _bones.end(), [&](BinaryBone* pNode)
		{
			return !strcmp(pNodeName, pNode->GetBoneName());
		});

	if (iter == _bones.end())
		return nullptr;

	return *iter;
}

HRESULT BinaryModel::InitializePrototype(MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	_pivotMatrix = pivotMat;

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(Utils::ToWString(pBinaryModelFilePath), FileMode::Read);


	//// 모델 타입 저장.
	uint32 iType;
	file->Read<uint32>(iType);
	_ModelType = static_cast<MODEL_TYPE>(iType);

	if (_ModelType == MODEL_TYPE::NONE)
		BinaryModelStatic(file, pBinaryModelFilePath, pivotMat);
	else
		BinaryModelDynamic(file, pBinaryModelFilePath);

	return S_OK;
}

HRESULT BinaryModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT BinaryModel::BindBoneMatrices(Shader* shader, uint32 meshIndex, const char* constantName)
{
	return m_Meshes[meshIndex]->BindBoneMatrices(shader, _bones, constantName, _pivotMatrix);
}

HRESULT BinaryModel::BindMaterialTexture(Shader* shader, const char* constantName, uint32 meshIndex, TextureType type)
{
	if (meshIndex >= m_iNumMeshes)
		return E_FAIL;

	uint32 iMaterialIndex = m_Meshes[meshIndex]->GetMaterialIndex();

	if (iMaterialIndex >= _numMaterial)
		return E_FAIL;


	return _materials[iMaterialIndex]._texture[type]->BindShaderResource(shader, constantName, 0);
}

HRESULT BinaryModel::PlayAnimation(const _float& timeDelta)
{
	if (true == _animationChange)
		ChangeAnimation(0.2f, timeDelta);
	if (false == _animationChange)
		_animations[_currentAnimIndex]->UpdateTransformationMatrix(_bones, timeDelta);
	

	for (auto& bone : _bones)
		bone->UpdateCombinedTransformMatrix(_bones);

	return S_OK;
}

HRESULT BinaryModel::Render(uint32 meshIndex)
{
	m_Meshes[meshIndex]->Render();

	return S_OK;
}

HRESULT BinaryModel::BinaryModelStatic(shared_ptr<FileUtils> file, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	Matrix pivotMatrix;
	file->Read<Matrix>(pivotMatrix);
	_pivotMatrix = pivotMatrix;
	//// 모텔 사전 행렬 저장

	uint32 NumMeshes;
	file->Read<uint32>(NumMeshes);
	m_iNumMeshes = NumMeshes;
	//// 메쉬 개수 -> 나중에 이 메쉬만큼 돌아야함.

	for (uint32 i = 0; i < NumMeshes; ++i)
	{
		string MeshName;
		file->Read(MeshName);

		// 메쉬를 구성하고있는 버퍼를 어떤 형식으로 읽을건지 메인 BufferDesc
		VIBuffer::BUFFER_DESC viBufferDesc;
		file->Read<VIBuffer::BUFFER_DESC>(viBufferDesc);

		// 그리고 그 형식으로 이루어진 Mesh Desc
		BinaryMesh::MESH_BUFFER_DESC meshBufferDesc;
		file->Read<BinaryMesh::MESH_BUFFER_DESC>(meshBufferDesc);

		// 메쉬 인덱스 
		uint32 meshIndex;
		file->Read<uint32>(meshIndex);

		// 그 정보로 구성된 버텍스 
		uint32 Numvertices;
		file->Read<uint32>(Numvertices);

		VTXMESH* pVertices = new VTXMESH[Numvertices];
		ZeroMemory(pVertices, sizeof(VTXMESH));

		for (uint32 i = 0; i < Numvertices; i++)
		{
			file->Read<Vec3>(pVertices[i].position);
			file->Read<Vec3>(pVertices[i].normal);
			file->Read<Vec2>(pVertices[i].texcoord);
			file->Read<Vec3>(pVertices[i].tangent);
			file->Read<Vec3>(pVertices[i].bitangent);
		}
		// 그 정보로 구성된 버텍스로 이루어진 인덱스 


		uint32 IndicesCount = 0;
		file->Read<uint32>(IndicesCount);

		_ulong* pIndices = new _ulong[IndicesCount * 3];
		ZeroMemory(pIndices, sizeof(pIndices));

		uint32 numIndices = 0;


		for (uint32 i = 0; i < IndicesCount; ++i)
		{
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
		}

		BinaryMesh* newBinaryMesh =
			BinaryMesh::Create(_device, _deviceContext, _ModelType, MeshName, viBufferDesc, meshBufferDesc, meshIndex, pVertices, pIndices, pivotMatrix);

		//if (nullptr == newBinaryMesh)
		//	return E_FAIL;

		m_Meshes.push_back(newBinaryMesh);

		Safe_Delete_Array<VTXMESH*>(pVertices);
		Safe_Delete_Array<_ulong*>(pIndices);
	}

	// 마테리얼 개수 
	uint32 NumMaterialCount;
	file->Read<uint32>(NumMaterialCount);
	_numMaterial = NumMaterialCount;


	for (uint32 i = 0; i < NumMaterialCount; ++i)
	{
		uint32 textureCount;
		file->Read<uint32>(textureCount);

		MESH_MATERIAL meshMaterials;
		::ZeroMemory(&meshMaterials, sizeof(meshMaterials));

		for (uint32 j = 0; j < textureCount; ++j)
		{

			uint32 textureType;
			file->Read<uint32>(textureType);

			string texturePath;
			file->Read(texturePath);

			Texture* texture = Texture::Create(_device, _deviceContext, Utils::ToWString(texturePath));
			if (texture == nullptr)
				return E_FAIL;

			meshMaterials._texture[textureType] = texture;
		}
		_materials.push_back(meshMaterials);
	}

	return S_OK;
}

HRESULT BinaryModel::BinaryModelDynamic(shared_ptr<FileUtils> file, const string& pBinaryModelFilePath)
{
	// 본 세팅.
	uint32 BoneCount;
	file->Read<uint32>(BoneCount);

	for (uint32 i = 0; i < BoneCount; ++i)
	{
		// 해당 뼈의 이름
		string boneName;
		file->Read(boneName);
		
		// 뼈의 모든 정보가 들어있는 구조체.
		BinaryBone::BONE_DESC binaryBoneDesc;
		file->Read<BinaryBone::BONE_DESC>(binaryBoneDesc);

		// 재귀적으로 순회 할 필요 없음. 어차피 벡터에 들어간 순서대로 세팅이 되어있을 것으로 예상.
		BinaryBone* bianryBone = BinaryBone::Cretae(binaryBoneDesc, boneName.c_str(), binaryBoneDesc._parentBoneIndex);

		_bones.push_back(bianryBone);
	}

	

	uint32 NumMeshes;
	file->Read<uint32>(NumMeshes);
		m_iNumMeshes = NumMeshes;
	//// 메쉬 개수 -> 나중에 이 메쉬만큼 돌아야함.

	for (uint32 i = 0; i < NumMeshes; ++i)
	{
		string MeshName;
		file->Read(MeshName);

		// 메쉬를 구성하고있는 버퍼를 어떤 형식으로 읽을건지 메인 BufferDesc
		VIBuffer::BUFFER_DESC viBufferDesc;
		file->Read<VIBuffer::BUFFER_DESC>(viBufferDesc);

		// 그리고 그 형식으로 이루어진 Mesh Desc
		BinaryMesh::MESH_BUFFER_DESC meshBufferDesc;
		file->Read<BinaryMesh::MESH_BUFFER_DESC>(meshBufferDesc);

		// 메쉬 인덱스 
		uint32 meshIndex;
		file->Read<uint32>(meshIndex);

		// 그 정보로 구성된 버텍스 
		uint32 Numvertices;
		file->Read<uint32>(Numvertices);

		VTXANIMMESH* pVertices = new VTXANIMMESH[Numvertices];
		ZeroMemory(pVertices, sizeof(VTXANIMMESH));

		for (uint32 i = 0; i < Numvertices; i++)
		{
			file->Read<Vec3>(pVertices[i].position);
			file->Read<Vec3>(pVertices[i].normal);
			file->Read<Vec2>(pVertices[i].texcoord);
			file->Read<Vec3>(pVertices[i].tangent);
			file->Read<Vec3>(pVertices[i].bitangent);
			file->Read<XMUINT4>(pVertices[i].blendIndices);
			file->Read<Vec4>(pVertices[i].blendWeights);
		}
		// 그 정보로 구성된 버텍스로 이루어진 인덱스 

		// 메쉬에 영향을 주는 뼈의 개수를 받아옴.
		uint32 effectBones;
		file->Read<uint32>(effectBones);

		vector<uint32> vecBoneIndex;
		vecBoneIndex.reserve(effectBones);
		vector<Matrix> offsetMatrix;
		offsetMatrix.reserve(effectBones);

		// 그 뼈의 개수만큼 뼈의 정보를 오프셋과 뼈의 인덱스 확인.
		for (uint32 i = 0; i < effectBones; ++i)
		{
			// 본의 오프셋 매트릭스.
			Matrix bonesOffsetMatrix;
			file->Read<Matrix>(bonesOffsetMatrix);
			offsetMatrix.push_back(bonesOffsetMatrix);

			// 이 메쉬의 영향을 끼치는 본의 이름
			string szBoneName;
			file->Read(szBoneName);

			// 이 메쉬의 영향을 주는 뼈의 인덱스 저장.
			int32 boneIndex = GetBoneIndex(szBoneName.c_str());
			vecBoneIndex.push_back(boneIndex);
		}


		uint32 IndicesCount = 0;
		file->Read<uint32>(IndicesCount);

		_ulong* pIndices = new _ulong[IndicesCount * 3];
		ZeroMemory(pIndices, sizeof(pIndices));

		uint32 numIndices = 0;


		for (uint32 i = 0; i < IndicesCount; ++i)
		{
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
		}

		BinaryMesh* newBinaryMesh = BinaryMesh::Create(_device, _deviceContext, _ModelType, 
			MeshName, viBufferDesc, meshBufferDesc, meshIndex, pVertices, pIndices, vecBoneIndex, offsetMatrix, effectBones);

		if (nullptr == newBinaryMesh)
			return E_FAIL;

		m_Meshes.push_back(newBinaryMesh);

		Safe_Delete_Array<VTXANIMMESH*>(pVertices);
		Safe_Delete_Array<_ulong*>(pIndices);
	}

	// 마테리얼 개수 
	uint32 NumMaterialCount;
	file->Read<uint32>(NumMaterialCount);
	_numMaterial = NumMaterialCount;


	for (uint32 i = 0; i < NumMaterialCount; ++i)
	{
		uint32 textureCount;
		file->Read<uint32>(textureCount);

		MESH_MATERIAL meshMaterials;
		::ZeroMemory(&meshMaterials, sizeof(meshMaterials));

		for (uint32 j = 0; j < textureCount; ++j)
		{

			uint32 textureType;
			file->Read<uint32>(textureType);

			string texturePath;
			file->Read(texturePath);

			Texture* texture = Texture::Create(_device, _deviceContext, Utils::ToWString(texturePath));
			if (texture == nullptr)
				return E_FAIL;

			meshMaterials._texture[textureType] = texture;
		}
		_materials.push_back(meshMaterials);
	}


	// Animation
	uint32 animationCount;
   	file->Read<uint32>(animationCount);
	_numAnimations = animationCount;

	for (uint32 i = 0; i < _numAnimations; ++i)
	{
		BinaryAnimation::ANIMATION_DESC animationDesc;
		file->Read<BinaryAnimation::ANIMATION_DESC>(animationDesc);

		string animationName;
		file->Read(animationName);

		uint32 ChannelCount;
		file->Read<uint32>(ChannelCount);

		vector<BinaryChannel*> vecChannel;
		vecChannel.reserve(ChannelCount);

		// 채널의 개수만큼..
		for (uint32 j = 0; j < ChannelCount; ++j)
		{
			string szName;
			file->Read(szName);

			BinaryChannel::CHANNEL_DESC channelDesc;
			file->Read<BinaryChannel::CHANNEL_DESC>(channelDesc);

			uint32 FrameCount;
			file->Read<uint32>(FrameCount);


			vector<KEYFRAME> readKeyFrame;
			readKeyFrame.reserve(FrameCount);

			for (uint32 k = 0; k < FrameCount; ++k)
			{
				KEYFRAME keyFrame;
				file->Read<Vec3>(keyFrame.scale);
				file->Read<Vec4>(keyFrame.rotation);
				file->Read<Vec4>(keyFrame.translation);
				file->Read<_float>(keyFrame.time);

				readKeyFrame.push_back(keyFrame);
			}

			BinaryChannel* pBinaryChannel = BinaryChannel::Create(this, szName.c_str(), channelDesc, readKeyFrame);
			if (nullptr == pBinaryChannel)
				return E_FAIL;

			vecChannel.push_back(pBinaryChannel);
		}


		BinaryAnimation* pAnimation = BinaryAnimation::Create(this, animationName.c_str(), animationDesc, vecChannel);
		_animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT BinaryModel::SetFirstAnimation(uint32 first, _bool loop)
{
	if (first >= _numAnimations &&
		first == _currentAnimIndex)
		return S_OK;

	_currentAnimIndex = first;

	_animations[_currentAnimIndex]->Reset();
	_animations[_currentAnimIndex]->SetLoop(loop);
	

	return S_OK;
}

HRESULT BinaryModel::SetAnimation(uint32 next, _bool loop)
{
	if (next >= _numAnimations &&
		next == _currentAnimIndex)
		return S_OK;

	_animationChange = true;
	_nextAnimationIndex = next;
	_ChangeTrackPosition = 0.f;
	
	_curChannels = _animations[_currentAnimIndex]->GetChannels();
	_nextChannels = _animations[_nextAnimationIndex]->GetChannels();

	return S_OK;
}

HRESULT BinaryModel::ChangeAnimation(_float duration, const _float& timeDelta)
{
	_ChangeTrackPosition += timeDelta;

	Vec3 vScale;
	Vec4 vRotation;
	Vec4 vTranslation;

	for (auto& pCurChannel : _curChannels)
	{
		for (auto& pNextChannel : _nextChannels)
		{
			if (pCurChannel->GetChannelDesc()._boneIndex == pNextChannel->GetChannelDesc()._boneIndex)
			{
				KEYFRAME curKeyFrame = pCurChannel->GetCurrentKeyFrame();
				KEYFRAME nextKeyFrame = pNextChannel->GetKeyFrame().front();

				while (_ChangeTrackPosition >= duration)
				{
					_animationChange = false;
					_currentAnimIndex = _nextAnimationIndex;
					_animations[_currentAnimIndex]->Reset();
					_animations[_currentAnimIndex]->SetLoop(_nextAnimationLoop);

					return S_OK;
				}

				_float ratio = (_ChangeTrackPosition - 0.f) / duration;

				// 스케일 로테이션 트렌슬레이션도 보간한다.
				Vec4 sourScale = ::XMLoadFloat3(&curKeyFrame.scale);
				Vec4 destScale = ::XMLoadFloat3(&nextKeyFrame.scale);
				::XMStoreFloat3(&vScale, ::XMVectorLerp(sourScale, destScale, ratio));

				Vec4 sourRotation = ::XMLoadFloat4(&curKeyFrame.rotation);
				Vec4 destRotation = ::XMLoadFloat4(&nextKeyFrame.rotation);
				::XMStoreFloat4(&vRotation, ::XMQuaternionSlerp(sourRotation, destRotation, ratio));

				Vec4 sourTranslation = ::XMLoadFloat4(&curKeyFrame.translation);
				Vec4 destTranslation = ::XMLoadFloat4(&nextKeyFrame.translation);
				::XMStoreFloat4(&vTranslation, ::XMVectorLerp(sourTranslation, destTranslation, ratio));


				Matrix TransformationMatrix = ::XMMatrixAffineTransformation(::XMLoadFloat3(&vScale),
					::XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), ::XMLoadFloat4(&vTranslation));

				// 

				// 모델이 들고 있는 뼈한테 Affine Matrix 세팅.
				_bones[pCurChannel->GetChannelDesc()._boneIndex]->SetTransformationMatrix(TransformationMatrix);
				//Bones[_channelDesc._boneIndex]->SetTransformationMatrix(TransformationMatrix);

			}
		}
	}


	return S_OK;
}

BinaryModel* BinaryModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	BinaryModel* pInstance = new BinaryModel(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(type, pBinaryModelFilePath, pivotMat)))
	{
		MSG_BOX("Failed to Created : Model");
		Safe_Release<BinaryModel*>(pInstance);
	}

	return pInstance;
}

Component* BinaryModel::Clone(void* pArg)
{
	BinaryModel* pInstance = new BinaryModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Model");
		Safe_Release<BinaryModel*>(pInstance);
	}

	return pInstance;
}

void BinaryModel::Free()
{
	__super::Free();


	for (auto& pAnimation : _animations)
		Safe_Release<BinaryAnimation*>(pAnimation);

	_animations.clear();

	for (auto& material : _materials)
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release<Texture*>(material._texture[i]);

	_materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& bone : _bones)
		Safe_Release<BinaryBone*>(bone);

	_bones.clear();

}
