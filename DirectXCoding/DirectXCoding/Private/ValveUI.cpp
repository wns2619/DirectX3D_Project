#include "pch.h"
#include "ValveUI.h"
#include "GameInstance.h"

ValveUI::ValveUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: GameObject(pDevice, pDeviceContext, OBJECT_TYPE::UI)
{
}

ValveUI::ValveUI(const ValveUI& rhs)
	: GameObject(rhs)
{
}

HRESULT ValveUI::InitializePrototype()
{
	return S_OK;
}

HRESULT ValveUI::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = static_cast<PART_DESC*>(pArg);
		_pParentTransform = pPartDesc->pParentTransform;
		_iID = pPartDesc->iID;
		Safe_AddRef<Transform*>(_pParentTransform);
	}

	
	if (_iID != 186)
	{
		_transform->SetScaling(Vec3(10.f, 10.f, 10.f));
		_transform->FixRotation(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(180.f));
		_transform->SetState(Transform::STATE::POSITION, Vec4(0.f, 35.f, 10.f, 1.f));
	}
	else
	{
		_transform->SetScaling(Vec3(10.f, 10.f, 10.f));
		_transform->FixRotation(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(180.f));
		_transform->SetState(Transform::STATE::POSITION, Vec4(0.f, 10.f, 35.f, 1.f));
	}



	return S_OK;
}

void ValveUI::Tick(const _float& fTimeDelta)
{
	_transform->SetWorldMatrix(Matrix::Identity);

	_transform->SetScaling(Vec3(10.f, 10.f, 10.f));
	_transform->FixRotation(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(180.f));
	_transform->SetState(Transform::STATE::POSITION, Vec4(0.f, 35.f, 10.f, 1.f));

	Compute_RenderMatrix(_transform->GetWorldMatrixCaculator());
}

void ValveUI::LateTick(const _float& fTimeDelta)
{
	CameraHelper* pCamera = GET_INSTANCE(CameraHelper);

	Vec4 vCameraPos = pCamera->GetCameraPosition();

	RELEASE_INSTANCE(CameraHelper);

	_transform->SetWorldMatrix(_WorldMatrix);

	_transform->LookAt(vCameraPos);
	_transform->BillBoardRotate(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(180.f));

	_WorldMatrix = _transform->GetWorldMatrix();

	_pRenderer->AddRenderGroup(Renderer::RENDERGROUP::UI, this);
}

HRESULT ValveUI::Render()
{
	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	if (FAILED(_pShader->Begin(1)))
		return E_FAIL;

	if (FAILED(_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT ValveUI::Compute_RenderMatrix(FXMMATRIX ChildMatrix)
{
	if (false == _bFirst)
	{
		_mParentOriginalMatrix = _pParentTransform->GetWorldMatrixCaculator();
		_bFirst = true;
	}

	_WorldMatrix = ChildMatrix * _mParentOriginalMatrix;

	return S_OK;
}

HRESULT ValveUI::Ready_Components()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 iLevel = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		iLevel = static_cast<uint32>(LEVEL::EDIT);

	if(FAILED(__super::AddComponent(iLevel, TEXT("ProtoTypeComponentShaderVertexTextureData"), TEXT("Component_Shader"),
		reinterpret_cast<Component**>(&_pShader))))
		return E_FAIL;

	Transform::TRANSFORM_DESC transDesc;
	transDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);
	transDesc.speedPerSec = 2.f;

	if(FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferNormalRect"),
		TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_pVIBuffer))))
		return E_FAIL;

	if(FAILED(__super::AddComponent(iLevel, TEXT("ProtoTypeValveUI"), TEXT("ComponentTexture"),
		reinterpret_cast<Component**>(&_pTexture))))
		return E_FAIL;
	
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_pRenderer))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT ValveUI::Bind_ShaderResource()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(_pShader->BindMatrix("W", &_WorldMatrix)))
		return E_FAIL;
	
	if (FAILED(pGameInstance->BindTransformToShader(_pShader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(pGameInstance->BindTransformToShader(_pShader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(_pTexture->BindShaderResource(_pShader, "UITexture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

ValveUI* ValveUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	ValveUI* pInstance = new ValveUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to FAILED : ValveUI");
		Safe_Release<ValveUI*>(pInstance);
	}

	return pInstance;
}

GameObject* ValveUI::Clone(void* pArg)
{
	ValveUI* pInstance = new ValveUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create to Failed Cloned : ValveUI");
		Safe_Release<ValveUI*>(pInstance);
	}

	return pInstance;
}

void ValveUI::Free()
{
	__super::Free();

	Safe_Release<Texture*>(_pTexture);
	Safe_Release<Shader*>(_pShader);
	Safe_Release<VIBufferNormalRect*>(_pVIBuffer);
	Safe_Release<Transform*>(_pParentTransform);
	Safe_Release<Renderer*>(_pRenderer);
}
