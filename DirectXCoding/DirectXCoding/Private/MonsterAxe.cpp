#include "pch.h"
#include "MonsterAxe.h"
#include "GameInstance.h"


MonsterAxe::MonsterAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: MonsterPartObject(pDevice, pDeviceContext)
{
}

MonsterAxe::MonsterAxe(const MonsterAxe& rhs)
	: MonsterPartObject(rhs)
{
}

HRESULT MonsterAxe::InitializePrototype()
{


	return S_OK;
}

HRESULT MonsterAxe::Initialize(void* pArg)
{

	if (nullptr != pArg)
	{
		MONSTER_AXE* pPartDesc = static_cast<MONSTER_AXE*>(pArg);
		_mSocketPivotMatrix = pPartDesc->mSocketPivot;
		_pSocketMatrix = pPartDesc->pSocketMatrix;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		if (nullptr != pArg)
			_pDissolveTime = static_cast<MonsterPartObject::PART_DESC*>(pArg)->pDissolveTime;
	}


	if (FAILED(Ready_Components()))
		return E_FAIL;


	_transform->FixRotation(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(90.f));
	_transform->FixRotation(0.f, -90.f, 180.f);

	Vec4 vPos = _transform->GetState(Transform::STATE::POSITION);
	vPos.y += 0.1f;
	vPos.z -= 0.02f;
	_transform->SetState(Transform::STATE::POSITION, vPos);

	return S_OK;
}

void MonsterAxe::Tick(const _float& fTimeDelta)
{
	XMMATRIX worldMatrix = (*_pSocketMatrix) * _mSocketPivotMatrix;

	worldMatrix.r[0] = XMVector3Normalize(worldMatrix.r[0]);
	worldMatrix.r[1] = XMVector3Normalize(worldMatrix.r[1]);
	worldMatrix.r[2] = XMVector3Normalize(worldMatrix.r[2]);

	Compute_RenderMatrix(_transform->GetWorldMatrix() * worldMatrix);
}

void MonsterAxe::LateTick(const _float& fTimeDelta)
{
}

HRESULT MonsterAxe::Render()
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

HRESULT MonsterAxe::Ready_Components()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME),
		TEXT("ProtoTypeComponentDefaultMeshShader"),
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
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAxe"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDissolveTexture"),
		TEXT("ComponentTexture"), reinterpret_cast<Component**>(&_pTexture))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT MonsterAxe::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_pShader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_pShader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

MonsterAxe* MonsterAxe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	MonsterAxe* pInstance = new MonsterAxe(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : MonsterAxe");
		Safe_Release<MonsterAxe*>(pInstance);
	}

	return pInstance;
}

GameObject* MonsterAxe::Clone(void* pArg)
{
	MonsterAxe* pInstance = new MonsterAxe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : MonsterAxe");
		Safe_Release<MonsterAxe*>(pInstance);
	}

	return pInstance;
}

void MonsterAxe::Free()
{
	__super::Free();
}
