#include "pch.h"
#include "WallPainting.h"

#include "GameInstance.h"
#include "BreakDoor.h"
WallPainting::WallPainting(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: StaticObject(device, deviceContext)
{
	// Empty¿´´Ù°¡ 
}

WallPainting::WallPainting(const WallPainting& rhs)
	: StaticObject(rhs)
{
}

HRESULT WallPainting::InitializePrototype()
{
	return S_OK;
}

HRESULT WallPainting::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	DoorTargetSet();

	return S_OK;
}

void WallPainting::Tick(const _float& timeDelta)
{	
	if (nullptr != _pTargetObject)
	{
		if (true == dynamic_cast<BreakDoor*>(_pTargetObject)->GetIsBreak())
		{

			if (false == dynamic_cast<BreakDoor*>(_pTargetObject)->GetIsOpen())
			{
				Matrix MyWorld = _transform->GetWorldMatrix();
				XMVECTOR TargetRotation = _pTargetObject->GetTransform()->GetWorldRotation();

				XMVECTOR right = _transform->GetState(Transform::STATE::RIGHT);
				XMVECTOR up = _transform->GetState(Transform::STATE::UP);
				XMVECTOR look = _transform->GetState(Transform::STATE::LOOK);

				XMVECTOR vMyPosition = _transform->GetState(Transform::STATE::POSITION);
				XMVECTOR vTargetPosition =
					::XMLoadFloat3(&dynamic_cast<Bounding_Sphere*>
						(_pTargetObject->GetAssistCollider()->GetBounding())->GetBounding()->Center);

				XMVECTOR relativePosition = vMyPosition - vTargetPosition;

				Vec4 targetMoment = vTargetPosition - 
					::XMLoadFloat4(&dynamic_cast<BreakDoor*>(_pTargetObject)->GetPrevPosition());
				targetMoment.Normalize();

				vMyPosition += targetMoment * timeDelta * 1.2f;


				right = ::XMVector3Rotate(right, TargetRotation);
				up = ::XMVector3Rotate(up, TargetRotation);
				look = ::XMVector3Rotate(look, TargetRotation);

				_transform->SetState(Transform::STATE::POSITION, vMyPosition);

				_transform->SetState(Transform::STATE::RIGHT, right);
				_transform->SetState(Transform::STATE::UP, up);
				_transform->SetState(Transform::STATE::LOOK, look);
			}

		}
	}
}

void WallPainting::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT WallPainting::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;

	Color vColor = Color(1.f,1.f,1.f,1.f);

	if (_modelName == "EvilGraffiti" || _modelName == "ShootGraffiti")
		vColor = Color(0.5f, 0.f, 0.f, 1.f);
	else if (_modelName == "HumanGraffiti")
		vColor = Color(0.f, 0.9f, 0.f, 1.f);
	else if (_modelName == "WayGraffiti")
		vColor = Color(0.1, 0.6f, 0.f, 1.f);

	_shader->BindRawValue("NewColor", &vColor, sizeof(Color));

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


	return S_OK;
}

void WallPainting::DoorTargetSet()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	vector<GameObject*>* pGameList = pGameInstance->GetCurrentObjectList(LAYER_TAG::LAYER_DYNAMIC);

	if (_id == 212)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 172)
					return true;

				return false;
			});

		_pTargetObject = *iter;
	}
	else if (_id == 213)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 183)
					return true;

				return false;
			});

		_pTargetObject = *iter;
	}

	RELEASE_INSTANCE(GameInstance);
}


WallPainting* WallPainting::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	WallPainting* Instance = new WallPainting(device, deviceContext);

	if (FAILED(Instance->InitializePrototype()))
	{
		MSG_BOX("Create Failed WallPainting");
		Safe_Release<WallPainting*>(Instance);
	}

	return Instance;
}

GameObject* WallPainting::Clone(void* argument)
{
	WallPainting* Instance = new WallPainting(*this);

	if (FAILED(Instance->Initialize(argument)))
	{
		MSG_BOX("Create Failed WallPainting");
		Safe_Release<WallPainting*>(Instance);
	}

	return Instance;
}

void WallPainting::Free()
{
	__super::Free();
}
