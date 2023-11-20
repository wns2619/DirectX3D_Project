#include "pch.h"
#include "DynamicObject.h"

#include "GameInstance.h"
#include "BoundingOBB.h"
DynamicObject::DynamicObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DYNAMIC_TYPE eType)
	: GameObject(device, deviceContext, OBJECT_TYPE::DYNAMIC), _eDynamicType(eType)
{
	// Empty였다가 
}

DynamicObject::DynamicObject(const DynamicObject& rhs)
	: GameObject(rhs)
{
}

HRESULT DynamicObject::InitializePrototype()
{
	return S_OK;
}

HRESULT DynamicObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		// TODO
		// 복사할 떄 사용할 쉐이더와 모델 이름을 던진다.
		ComponentNames* names = static_cast<ComponentNames*>(pArg);
		_comNames._protoTypeTag = names->_protoTypeName;
		_comNames._strModelComponentName = names->_strModelComponentName;
		_modelName = names->_strModelName;
		_comNames._strShaderName = names->_strShaderName;
		_id = names->_modelID;

		if (_modelName == "Water")
			_comNames._strShaderName = L"ProtoTypeComponentDefaultMeshShader";
	}

	if (FAILED(ReadyComponents()))
		return E_FAIL;


	return S_OK;
}

void DynamicObject::Tick(const _float& timeDelta)
{
}

void DynamicObject::LateTick(const _float& timeDelta)
{

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (true == pGameInstance->IsIn_FrustumWorld(_transform->GetState(Transform::STATE::POSITION), 2.f))
	{
		if (!_enabled)
			_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
	}


	RELEASE_INSTANCE(GameInstance);
}

HRESULT DynamicObject::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(BindShaderResource()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "NormalMap", i, TextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

_bool DynamicObject::CheckOBBOverlap(class Collider* pOther)
{
	ContainmentType containment = static_cast<BoundingOBB*>(_pCollider->GetBounding())->
		GetBounding()->Contains(*static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding());


	if (containment == INTERSECTS)
	{
		XMVECTOR collisionDepth = ::XMVectorSubtract(::XMLoadFloat3(&static_cast<BoundingOBB*>(_pCollider->GetBounding())->
			GetBounding()->Center), ::XMLoadFloat3(&static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Center));

		Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
		vPos.z -= collisionDepth.m128_f32[2];

		pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	}

	return true;
}

void DynamicObject::CheckAABBOverlap(Collider* pOther)
{

}

void DynamicObject::CheckSphereOverlap(Collider* pOther)
{

}

HRESULT DynamicObject::ReadyComponents()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(level,
		_comNames._strShaderName,
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, _comNames._strModelComponentName,
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT DynamicObject::BindShaderResource()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;


	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

void DynamicObject::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
}
