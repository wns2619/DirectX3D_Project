#include "pch.h"
#include "BodyCam.h"
#include "GameInstance.h"

BodyCam::BodyCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: PartObject(pDevice, pContext), _pCameraHelper(CameraHelper::GetInstance())
{
	Safe_AddRef<CameraHelper*>(_pCameraHelper);
}

BodyCam::BodyCam(const BodyCam& rhs)
	: PartObject(rhs)
	, _pCameraHelper(rhs._pCameraHelper)
{
	Safe_AddRef<CameraHelper*>(_pCameraHelper);
}

HRESULT BodyCam::InitializePrototype()
{
	return S_OK;
}

HRESULT BodyCam::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		BODYCAM_DESC* pPartDesc = static_cast<BODYCAM_DESC*>(pArg);

		m_SocketMatrix = pPartDesc->SocketMatrix;
		m_SocketPivotMatrix = pPartDesc->SocketPivot;
		
		//Vec4 _vEye, _vAt;
		//_float _fFov, _fAspect, _fNear, _fFar;

		_vEye = pPartDesc->vEye;
		_vAt = pPartDesc->vAt;

		_fFov = pPartDesc->fFov;
		_fAspect = pPartDesc->fAspect;
		_fNear = pPartDesc->fNear;
		_fFar = pPartDesc->fFar;


		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_transform->FixRotation(Vec4(0.f, 0.f, 1.f, 1.f), ::XMConvertToRadians(-180));
	::ShowCursor(FALSE);

	return S_OK;
}

void BodyCam::Tick(const _float& fTimeDelta)
{
	XMMATRIX worldMatrix = ::XMLoadFloat4x4(m_SocketMatrix) * ::XMLoadFloat4x4(&m_SocketPivotMatrix);

	Compute_RenderMatrix(_transform->GetWorldMatrix() * worldMatrix);

	_pCameraHelper->SetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW, _WorldMatrix.Invert());
	_pCameraHelper->SetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ,
		::XMMatrixPerspectiveFovLH(_fFov, _fAspect, _fNear, _fFar));

}

void BodyCam::LateTick(const _float& fTimeDelta)
{
}

HRESULT BodyCam::Render()
{
	return S_OK;
}

void BodyCam::StartCameraShake()
{
	if (_remainingShakeDuration <= 0.f)
	{
		_originalMatrix = _WorldMatrix;
		_remainingShakeDuration = _fShakeDuration;
	}
}

_float BodyCam::RandomFloatInRange(_float min, _float max)
{
	// C++에서 제공하는 난수 생성 엔진 사용
	std::random_device rd;  // 시드를 위한 장치
	std::mt19937 gen(rd()); // 메르센 트위스터 엔진 사용
	std::uniform_real_distribution<float> dist(min, max); // 주어진 범위 내의 균일한 분포

	// 범위 내에서 랜덤한 부동 소수점 숫자를 생성하여 반환
	return dist(gen);
}

XMMATRIX BodyCam::LerpMatrix(const XMMATRIX& matrix1, const XMMATRIX& matrix2, _float t)
{
	return XMMatrixMultiply(matrix1, XMMatrixScaling(1.0f - t, 1.0f - t, 1.0f - t)) +
		XMMatrixMultiply(matrix2, XMMatrixScaling(t, t, t));
}

HRESULT BodyCam::Ready_Components()
{
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 5.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;


	return S_OK;
}

BodyCam* BodyCam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	BodyCam* pInstance = new BodyCam(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed To Created : BodyCame");
		Safe_Release<BodyCam*>(pInstance);
	}

	return pInstance;
}

GameObject* BodyCam::Clone(void* pArg)
{
	BodyCam* pInstance = new BodyCam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : BodyCame");
		Safe_Release<BodyCam*>(pInstance);
	}

	return pInstance;
}

void BodyCam::Free()
{
	__super::Free();

	Safe_Release<CameraHelper*>(_pCameraHelper);
}
