#include "pch.h"
#include "Surefire.h"
#include "GameInstance.h"

Surefire::Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext, OBJECT_TYPE::PLAYER)
{
	_modelName = "Surefire";
}

Surefire::Surefire(const GameObject& rhs)
	: GameObject(rhs)
{

}

HRESULT Surefire::InitializePrototype()
{
	return S_OK;
}

HRESULT Surefire::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = static_cast<PART_DESC*>(pArg);
		m_pParentTransform = pPartDesc->pParentTransform;
		Safe_AddRef<Transform*>(m_pParentTransform);

		m_SocketMatrix = pPartDesc->SocetMatrix;
		m_SocketPivotMatrix = pPartDesc->SocketPivot;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_transform->SetScaling(Vec3(0.0001f, 0.0001f, 0.00005f));
	_transform->SetState(Transform::STATE::POSITION, Vec4(0.f, -0.07f, 0.015f, 1.f));
	_transform->FixRotation(Vec3(1.f, 0.f, 0.f), ::XMConvertToRadians(45));

	return S_OK;
}

void Surefire::Tick(const _float& fTimeDelta)
{

	m_WorldMatrix = _transform->GetWorldMatrix() * (*m_SocketMatrix) * m_SocketPivotMatrix * m_pParentTransform->GetWorldMatrix();
}

void Surefire::LateTick(const _float& fTimeDelta)
{
}

HRESULT Surefire::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	uint32 numMeshes = _model->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_model->BindMaterialTexture(m_pShaderCom, "DiffuseMap", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(_model->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT Surefire::Ready_Components()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Shader Component */
	if (FAILED(__super::AddComponent(level,
		TEXT("ProtoTypeComponentDefaultMeshShader"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&m_pShaderCom))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 10.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeModelsurefire"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_model))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Surefire::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(m_pShaderCom->BindMatrix("W", &m_WorldMatrix)))
		return E_FAIL;


	if (FAILED(gameInstance->BindTransformToShader(m_pShaderCom, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(m_pShaderCom, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	const LIGHT_DESC* lightdesc = gameInstance->GetLightDesc(0);

	if (FAILED(m_pShaderCom->BindRawValue("GlobalLight", lightdesc, sizeof(LIGHT_DESC))))
		return E_FAIL;

	MESH_MATERIAL materialDesc;
	::ZeroMemory(&materialDesc, sizeof(materialDesc));
	materialDesc.ambient = Vec4(0.8);
	materialDesc.diffuse = Vec4(1.f);
	materialDesc.specular = Vec4(1.f);

	if (FAILED(m_pShaderCom->BindRawValue("Material", &materialDesc, 80)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

Surefire* Surefire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Surefire* pInstance = new Surefire(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : Surefire");
		Safe_Release<Surefire*>(pInstance);
	}

	return pInstance;
}

GameObject* Surefire::Clone(void* pArg)
{
	Surefire* pInstance = new Surefire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : Surefire");
		Safe_Release<Surefire*>(pInstance);
	}

	return pInstance;
}

void Surefire::Free()
{
	__super::Free();

	Safe_Release<Shader*>(m_pShaderCom);
	Safe_Release<Transform*>(m_pParentTransform);
}
