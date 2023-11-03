#include "pch.h"
#include "Monster.h"
#include "Surefire.h"
#include "BodyCam.h"
#include "GameInstance.h"
#include "BinaryNavi.h"
#include "BinaryAnimation.h"
#include "BinaryBone.h"
#include "MonsterWalk.h"

Monster::Monster(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: LandObject(device, deviceContext, OBJECT_TYPE::MONSTER)
{
	_objectType = OBJECT_TYPE::MONSTER;
	_modelName = "Monster";
	_modelPath = L"Monster";
}

Monster::Monster(const Monster& rhs)
	: LandObject(rhs)
{

}

HRESULT Monster::InitializePrototype()
{
	return S_OK;
}

HRESULT Monster::Initialize(void* pArg)
{
	
	//__super::Initialize(pArg);

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	State* pState = MonsterWalk::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::IDLE, pState);

	vector<BinaryAnimation*>& vecAnim = _binaryModel->GetBinaryAnimation();
	vector<BinaryBone*>& pVecBone = _binaryModel->GetBinaryBones();

	_pAnimator->AddAnimation(State::STATE::IDLE, vecAnim[0], &pVecBone, true);
	//_pAnimator->AddAnimation(State::STATE::RUN, vecAnim[1], &pVecBone, true);
	//_pAnimator->AddAnimation(State::STATE::SHOOT, vecAnim[2], &pVecBone);


	_pStateMachine->SetAnimator(_pAnimator);
	_pStateMachine->SetState(State::STATE::IDLE);
	//_transform->SetState(Transform::STATE::POSITION, ::XMVectorSet(11.f, 0.f, -45.5f, 1.f));


	return S_OK;
}

void Monster::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;

	_pStateMachine->UpdateStateMachine(fTimeDelta);


	//_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void Monster::LateTick(const _float& fTimeDelta)
{
	_pStateMachine->UpdateLateStateMachine(fTimeDelta);


	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Monster::Render()
{
	if (_enabled)
		return S_OK;

	if (FAILED(BindShaderResuorces()))
		return E_FAIL;


	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindBoneMatrices(_shader, i, "BoneMatrices")))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	//_pCollider->Render();
#endif // _DEBUG

#ifdef _DEBUG
	//_pNavigation->Render();
#endif // _DEBUG


	return S_OK;
}

void Monster::OnCollisionEnter(Collider* pOther)
{


}

void Monster::OnCollisionStay(Collider* pOther)
{
	

}

void Monster::OnCollisionExit(Collider* pOther)
{

}


HRESULT Monster::ReadyComponents()
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
		TEXT("ProtoTypeComponentAnimMesh"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeDanceMonster"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 2.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeStateMachine"),
		TEXT("ComponentStateMachine"), reinterpret_cast<Component**>(&_pStateMachine))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeAnimator"),
		TEXT("ComponentAnimator"), reinterpret_cast<Component**>(&_pAnimator))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);


	return S_OK;
}

HRESULT Monster::BindShaderResuorces()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_transform->BindShaderResources(_shader, "W")))
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


Monster* Monster::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Monster* pMonster = new Monster(device, deviceContext);

	if (FAILED(pMonster->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : Monster");
		Safe_Release<Monster*>(pMonster);
	}

	return pMonster;
}

GameObject* Monster::Clone(void* argument)
{
	Monster* pMonster = new Monster(*this);

	if (FAILED(pMonster->Initialize(argument)))
	{
		MSG_BOX("Failed to Cloned : Monster");
		Safe_Release<Monster*>(pMonster);
	}

	return pMonster;
}

void Monster::Free()
{
	__super::Free();

	Safe_Release<Shader*>(_shader);
	Safe_Release<Renderer*>(_render);
	Safe_Release<StateMachine*>(_pStateMachine);
	Safe_Release<Animator*>(_pAnimator);
}
