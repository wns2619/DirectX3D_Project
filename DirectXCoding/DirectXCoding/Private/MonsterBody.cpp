#include "pch.h"
#include "MonsterBody.h"
#include "GameInstance.h"

MonsterBody::MonsterBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: MonsterPartObject(pDevice, pDeviceContext)
{
}

MonsterBody::MonsterBody(const MonsterBody& rhs)
	: MonsterPartObject(rhs)
{
}

HRESULT MonsterBody::InitializePrototype()
{
	return S_OK;
}

HRESULT MonsterBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		_pDissolveTime = static_cast<MonsterPartObject::PART_DESC*>(pArg)->pDissolveTime;

	return S_OK;
}

void MonsterBody::Tick(const _float& fTimeDelta)
{
	Compute_RenderMatrix(_transform->GetWorldMatrix());
}

void MonsterBody::LateTick(const _float& fTimeDelta)
{
}

HRESULT MonsterBody::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{

		if (FAILED(_binaryModel->BindBoneMatrices(_pShader, i, "BoneMatrices")))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_pShader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_pShader, "NormalMap", i, TextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(_pShader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT MonsterBody::Ready_Components()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME),
		TEXT("ProtoTypeComponentAnimMesh"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_pShader))))
		return E_FAIL;

	/* Transform Component */

	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 5.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(40.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDanceMonster"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDissolveTexture"),
		TEXT("ComponentTexture"), reinterpret_cast<Component**>(&_pTexture))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT MonsterBody::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;

	if (FAILED(_pTexture->BindShaderResource(_pShader, "NoiseMap", 0)))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_pShader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_pShader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(_pShader->BindRawValue("_time", _pDissolveTime, sizeof(_float))))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

MonsterBody* MonsterBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	MonsterBody* pInstance = new MonsterBody(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : MonsterBody");
		Safe_Release<MonsterBody*>(pInstance);
	}

	return pInstance;
}

GameObject* MonsterBody::Clone(void* pArg)
{
	MonsterBody* pInstance = new MonsterBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : MonsterBody");
		Safe_Release<MonsterBody*>(pInstance);
	}

	return pInstance;
}

void MonsterBody::Free()
{
	__super::Free();

}
