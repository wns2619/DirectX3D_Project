#include "pch.h"
#include "Bullet.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"

Bullet::Bullet(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::BULLET)
{
}

Bullet::Bullet(const Bullet& rhs)
	: DynamicObject(rhs)
{

}

HRESULT Bullet::InitializePrototype()
{
	return S_OK;
}

HRESULT Bullet::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		BULLET_DESC* pBulletDesc = static_cast<BULLET_DESC*>(pArg);
		::memcpy(&_BulletDesc, pBulletDesc, sizeof(BULLET_DESC));
	}


	if (FAILED(ReadyCollider()))
		return E_FAIL;

	_modelName = "PlayerBullet";


	_transform->SetState(Transform::STATE::POSITION, _BulletDesc.vPos);


	_BulletDesc.vDir.Normalize();

	_transform->SetState(Transform::STATE::LOOK, _BulletDesc.vDir);

	return S_OK;
}

void Bullet::Tick(const _float& timeDelta)
{
	if (true == _IsDead)
		return;

	_transform->Forward(timeDelta);

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void Bullet::LateTick(const _float& timeDelta)
{
	if (true == _IsDead)
		return;

	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Bullet::Render()
{
	if (_enabled)
		return S_OK;

	if (true == _IsDead)
		return S_OK;

	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT Bullet::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(level,
		TEXT("ProtoTypeComponentDefaultMeshShader"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypePlayerBullet"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;


	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		sphereDesc.fRadius = 0.1f;
		sphereDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeSphereCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
		return E_FAIL;

	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = _BulletDesc.fBulletSpeed;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

Bullet* Bullet::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Bullet* pInstance = new Bullet(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : Bullet");
		Safe_Release<Bullet*>(pInstance);
	}

	return pInstance;
}

GameObject* Bullet::Clone(void* pArg)
{
	Bullet* pInstance = new Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : Bullet");
		Safe_Release<Bullet*>(pInstance);
	}

	return pInstance;
}

void Bullet::Free()
{
	__super::Free();
}
