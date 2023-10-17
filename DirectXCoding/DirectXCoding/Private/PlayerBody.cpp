#include "pch.h"
#include "PlayerBody.h"
#include "GameInstance.h"

PlayerBody::PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: PartObject(pDevice, pContext)
{

}

PlayerBody::PlayerBody(const PartObject& rhs)
	: PartObject(rhs)
{

}

Matrix* PlayerBody::Get_SocketBonePtr(const char* pBoneName)
{
	if (nullptr == _binaryModel)
		return nullptr;

	return _binaryModel->GetBoneMatrix(pBoneName);
	// 소켓을 붙일 뼈의 매트릭스 정보.
}

Matrix PlayerBody::Get_SocketPivotMatrix()
{
	if (nullptr == _binaryModel)
		return Matrix();

	return _binaryModel->GetPivotMatrix();
}

void PlayerBody::Set_AnimationIndex(uint32 iAnimIndex, _bool isLoop)
{
	_binaryModel->SetAnimation(iAnimIndex, isLoop);
}

void PlayerBody::StartAnimation(uint32 startIndex, _bool isLoop)
{
	_binaryModel->SetFirstAnimation(startIndex, isLoop);
}

HRESULT PlayerBody::InitializePrototype()
{
	return S_OK;
}

HRESULT PlayerBody::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//_binaryModel->SetUp_Animation(true, 0);
	//_binaryModel->SetFirstAnimation(2, true);

	return S_OK;
}

void PlayerBody::Tick(const _float& fTimeDelta)
{
	_binaryModel->PlayAnimation(fTimeDelta);


	Compute_RenderMatrix(_transform->GetWorldMatrix());
}

void PlayerBody::LateTick(const _float& fTimeDelta)
{
}

HRESULT PlayerBody::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindBoneMatrices(_shader, i, "BoneMatrices")))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT PlayerBody::Ready_Components()
{

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Shader Component */
	if (FAILED(__super::AddComponent(level,
		TEXT("ProtoTypeComponentAnimMesh"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* Transform Component */
	
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 10.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeModelPlayer"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT PlayerBody::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_shader->BindMatrix("W", &_WorldMatrix)))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	const LIGHT_DESC* lightdesc = gameInstance->GetLightDesc(0);

	if (FAILED(_shader->BindRawValue("GlobalLight", lightdesc, sizeof(LIGHT_DESC))))
		return E_FAIL;

	MESH_MATERIAL materialDesc;
	::ZeroMemory(&materialDesc, sizeof(materialDesc));
	materialDesc.ambient = Vec4(0.8);
	materialDesc.diffuse = Vec4(1.f);
	materialDesc.specular = Vec4(1.f);

	if (FAILED(_shader->BindRawValue("Material", &materialDesc, 80)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

PlayerBody* PlayerBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	PlayerBody* pInstance = new PlayerBody(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : Player Body");
		Safe_Release<PlayerBody*>(pInstance);
	}

	return pInstance;
}

GameObject* PlayerBody::Clone(void* pArg)
{
	PlayerBody* pInstance = new PlayerBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : Player Body");
		Safe_Release<PlayerBody*>(pInstance);
	}

	return pInstance;
}

void PlayerBody::Free()
{
	__super::Free();

	Safe_Release<Transform*>(_transform);
	Safe_Release<Shader*>(_shader);
}
