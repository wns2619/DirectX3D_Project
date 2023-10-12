#include "pch.h"
#include "PlayerBody.h"
#include "GameInstance.h"

PlayerBody::PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext, OBJECT_TYPE::PLAYER)
{

}

PlayerBody::PlayerBody(const GameObject& rhs)
	: GameObject(rhs)
{

}

Matrix* PlayerBody::Get_SocketBonePtr(const char* pBoneName)
{
	if (nullptr == _model)
		return nullptr;

	return _model->GetBoneMatrix(pBoneName);
	// 소켓을 붙일 뼈의 매트릭스 정보.
}

Matrix PlayerBody::Get_SocketPivotMatrix()
{
	if (nullptr == _model)
		return Matrix();

	return _model->GetPivotMatrix();
}

void PlayerBody::Set_AnimationIndex(_bool isLoop, uint32 iAnimIndex)
{


	_model->SetUp_Animation(isLoop, iAnimIndex);
}

HRESULT PlayerBody::InitializePrototype()
{
	return S_OK;
}

HRESULT PlayerBody::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = static_cast<PART_DESC*>(pArg);
		m_pParentTransform = pPartDesc->pParentTransform;
		Safe_AddRef<Transform*>(m_pParentTransform);
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_model->SetUp_Animation(true, 3);

	return S_OK;
}

void PlayerBody::Tick(const _float& fTimeDelta)
{
	_model->PlayAnimation(fTimeDelta);

	m_WorldMatrix = _transform->GetWorldMatrix() * m_pParentTransform->GetWorldMatrix();
	// 본인 월드 * 소켓뼈의 부모행렬 
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

	uint32 numMeshes = _model->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_model->BindBoneMatrices(m_pShaderCom, i, "BoneMatrices")))
			return E_FAIL;

		if (FAILED(_model->BindMaterialTexture(m_pShaderCom, "DiffuseMap", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(_model->Render(i)))
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
	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeModelPlayer"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_model))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT PlayerBody::Bind_ShaderResources()
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

	Safe_Release<Shader*>(m_pShaderCom);
	Safe_Release<Transform*>(m_pParentTransform);
}
