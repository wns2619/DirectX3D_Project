#include "pch.h"
#include "StaticObject.h"

#include "GameInstance.h"
StaticObject::StaticObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::STATIC)
{
	// Empty였다가 
}

StaticObject::StaticObject(const StaticObject& rhs)
	: GameObject(rhs)
{
}

HRESULT StaticObject::InitializePrototype()
{
	return S_OK;
}

HRESULT StaticObject::Initialize(void* pArg)
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
	}

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	//_transform->SetScaling(Vec3(0.01f, 0.01f, 0.01f));
	
	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	//_transform->SetScaling(Vec3(0.01f, 0.01f, 0.01f));

	//_transform->SetWorldMatrix()
	//_transform->FixRotation(Vec3(0.f, 1.f, 0.f), ::XMConvertToRadians(90.f));
	return S_OK;
}

void StaticObject::PriorityTick(const _float& timeDelta)
{
	// 스태틱 오브젝트 중에 모델 네임이 바닥일 때만 업데이트.
	if(_modelName == "2stBottom")
		_pNavigation->Update(Matrix::Identity);
}

void StaticObject::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	//if (_modelName == "Ventilator")
	//{

	//	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	//	_float fVolume = 0.f;
	//	_float fDistance = 5.f;

	//	LerpSoundPlayer(fVolume, fDistance, 2.f, pGameInstance);

	//	if (fDistance <= 5.f)
	//		pGameInstance->PlaySoundLoop(TEXT("condicioner.wav"), SOUND_ENVIRONMENT, fVolume);

	//	RELEASE_INSTANCE(GameInstance);

	//}
}

void StaticObject::LateTick(const _float& timeDelta)
{

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (true == pGameInstance->IsIn_FrustumWorld(_transform->GetState(Transform::STATE::POSITION), 2.f))
	{

		if (_id != 2 && _id != 3 && _id != 5 && _id != 85 && _id != 86 && _id != 150 && _id != 20 && _id != 35
			&& _id != 54 && _id != 56 && _id != 57 && _id != 89 && _id != 108 && _id != 11)
		{
			if (!_enabled)
				_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
		}



#ifdef _DEBUG
		if (_modelName == "2stBottom")
			_render->AddDebug(_pNavigation);
#endif // _DEBUG

	}

	if(_id == 2 || _id == 3 || _id == 5 || _id == 85 || _id == 86 || _id == 150 || _id == 20 || _id == 35 || _id == 54
		|| _id == 56 || _id == 57 || _id == 89 || _id == 108 || _id == 11)
	{
		if (!_enabled)
			_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
	}

	RELEASE_INSTANCE(GameInstance);

}

HRESULT StaticObject::Render()
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

void StaticObject::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 1.f;
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _transform->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = static_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

HRESULT StaticObject::ReadyComponents()
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


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, _comNames._strModelComponentName,
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;

	if (_modelName == "2stBottom"  || _modelName == "BasementBottom")
	{
		if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeNavigation"),
			TEXT("ComponentNavigation"), reinterpret_cast<Component**>(&_pNavigation))))
			return E_FAIL;
	}


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT StaticObject::BindShaderResource()
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

StaticObject* StaticObject::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	StaticObject* Instance = new StaticObject(device, deviceContext);

	if (FAILED(Instance->InitializePrototype()))
	{
		MSG_BOX("Create Failed StaticObject");
		Safe_Release<StaticObject*>(Instance);
	}

	return Instance;
}

GameObject* StaticObject::Clone(void* argument)
{
	StaticObject* Instance = new StaticObject(*this);

	if (FAILED(Instance->Initialize(argument)))
	{
		MSG_BOX("Create Failed StaticObject");
		Safe_Release<StaticObject*>(Instance);
	}

	return Instance;
}

void StaticObject::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
	Safe_Release<BinaryNavi*>(_pNavigation);
	Safe_Release<VIBufferCell*>(_pVIBuffer);
}
