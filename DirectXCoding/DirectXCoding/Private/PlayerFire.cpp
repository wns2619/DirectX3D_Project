#include "pch.h"
#include "PlayerFire.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"
#include "Player.h"
#include "BinaryAnimation.h"
#include "Bullet.h"
#include "BinaryBone.h"
#include "GameObject.h"
#include "BodyCam.h"

PlayerFire::PlayerFire(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT PlayerFire::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE PlayerFire::UpdateState(const _float& timeDelta)
{
	STATE eState = STATE::SHOOT;

	if (nullptr != _pOwner)
		eState = KeyInput(timeDelta);

	StateMachine* pStateMachine = dynamic_cast<Player*>(_pOwner)->GetStateMachine();
	if (nullptr == pStateMachine)
		return eState;


	if (pStateMachine->GetAnimator()->IsCurKeyFrame(1))
	{
		if (true == _IsShoot)
		{
			GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
			CameraHelper* pCamera = GET_INSTANCE(CameraHelper);

			Matrix CameraMatrix = pCamera->GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);

			BinaryModel* pModel = dynamic_cast<Player*>(_pOwner)->GetPlyaerPart()[Player::PART::PART_BODY]->GetBinaryModelComponent();
			if (pModel == nullptr)
				return STATE::SHOOT;

			Bullet::BULLET_DESC bullDesc;
			BinaryBone* pBone = pModel->GetBone("slider");
			Matrix mSliderMatrix = pBone->GetSliderPos();
			Matrix pivotMatrix = pModel->GetPivotMatrix();

			
			dynamic_cast<BodyCam*>(dynamic_cast<Player*>(_pOwner)->GetPlyaerPart()[Player::PART::PART_CAMERA])->StartCameraShake();


			Vec4 vSliderPos = Vec4(mSliderMatrix._41, mSliderMatrix._42, mSliderMatrix._43, mSliderMatrix._44);
			vSliderPos = ::XMVector3TransformCoord(vSliderPos, pivotMatrix);

			Matrix finalMatrix = _pOwner->GetTransform()->GetWorldMatrix();
			vSliderPos = ::XMVector3TransformCoord(vSliderPos, finalMatrix);


			bullDesc.vPos = vSliderPos;
			bullDesc.fBulletSpeed = 25.f;
			bullDesc.vDir = _pOwner->GetTransform()->GetState(Transform::STATE::LOOK);

			if (FAILED(pGameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_BULLET,
				TEXT("ProtoTypePlayerBullet"), &bullDesc)))
			{
				RELEASE_INSTANCE(GameInstance);
				RELEASE_INSTANCE(CameraHelper);
				return STATE::SHOOT;
			}

			_IsShoot = false;

			RELEASE_INSTANCE(GameInstance);
			RELEASE_INSTANCE(CameraHelper);
		}
	}
	else
		_IsShoot = true;


	return eState;
}

void PlayerFire::LateUpdateState(const _float& timeDelta)
{

}

void PlayerFire::RenderState()
{
	//std::cout << "Fire" << std::endl;

}

State::STATE PlayerFire::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::SHOOT;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	if (pGameInstance->KeyPressing(DIK_W))
	{
		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}
	// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}


	RELEASE_INSTANCE(GameInstance);

	return eState;
}

PlayerFire* PlayerFire::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	PlayerFire* pInstance = new PlayerFire(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : PlayerFire");
		Safe_Release<PlayerFire*>(pInstance);
	}

	return pInstance;
}

void PlayerFire::Free()
{
	__super::Free();
}
