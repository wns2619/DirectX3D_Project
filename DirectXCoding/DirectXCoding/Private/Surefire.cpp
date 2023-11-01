#include "pch.h"
#include "Surefire.h"
#include "GameInstance.h"

Surefire::Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: PartObject(pDevice, pContext)
{
	_modelName = "Surefire";
}

Surefire::Surefire(const Surefire& rhs)
	: PartObject(rhs)
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
		FLASHLIGHT_DESC* pPartDesc = static_cast<FLASHLIGHT_DESC*>(pArg);

		m_SocketPivotMatrix = pPartDesc->SocketPivot;
		m_SocketMatrix = pPartDesc->SocketMatrix;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_transform->SetScaling(Vec3(0.0001f, 0.0001f, 0.00005f));
	_transform->FixRotation(Vec3(1.f, 0.f, 0.f), ::XMConvertToRadians(45));
	_transform->SetState(Transform::STATE::POSITION, Vec4(0.f, -0.07f, 0.015f, 1.f));

	return S_OK;
}

void Surefire::Tick(const _float& fTimeDelta)
{
	XMMATRIX worldMatrix = ::XMLoadFloat4x4(m_SocketMatrix) * ::XMLoadFloat4x4(&m_SocketPivotMatrix);

	worldMatrix.r[0] = XMVector3Normalize(worldMatrix.r[0]);
	worldMatrix.r[1] = XMVector3Normalize(worldMatrix.r[1]);
	worldMatrix.r[2] = XMVector3Normalize(worldMatrix.r[2]);

	Compute_RenderMatrix(_transform->GetWorldMatrix() * worldMatrix);
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


	//if (true == _bObtainLight)
	//{
		uint32 numMeshes = _binaryModel->GetNumMeshes();

		for (size_t i = 0; i < numMeshes; i++)
		{
			if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(_shader->Begin(1)))
				return E_FAIL;

			if (FAILED(_binaryModel->Render(i)))
				return E_FAIL;
		}
	//}



	return S_OK;
}

HRESULT Surefire::Ready_Components()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT),
		TEXT("ProtoTypeComponentDefaultMeshShader"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 5.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeModelsurefire"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Surefire::Bind_ShaderResources()
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

	Safe_Release<Transform*>(_transform);
	Safe_Release<Shader*>(_shader);
}
