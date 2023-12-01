#include "pch.h"
#include "FireEffect.h"
#include "PlayerBody.h"
#include "Player.h"
#include "BinaryBone.h"

#include "GameInstance.h"

FireEffect::FireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : GameObject(pDevice, pDeviceContext, OBJECT_TYPE::PARTICLE)
{
}

FireEffect::FireEffect(const FireEffect& rhs)
    : GameObject(rhs)
{
}

HRESULT FireEffect::InitializePrototype()
{
    return S_OK;
}

HRESULT FireEffect::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT FireEffect::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _pVIBuffer->Render();

    return S_OK;
}

void FireEffect::Tick(const _float& fTimeDelta)
{
    _pVIBuffer->Update(fTimeDelta);

    _fLifeTime += fTimeDelta;

    if (_fLifeTime >= 0.01f)
    {
        GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

        pGameInstance->DeleteObject(this);

        RELEASE_INSTANCE(GameInstance);

        _fLifeTime = 0.f;
    }
}

void FireEffect::LateTick(const _float& fTimeDelta)
{
    GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

    Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObject(LAYER_TAG::LAYER_PLAYER).front());
    PlayerBody* pBody = static_cast<PlayerBody*>(pPlayer->GetPlyaerPart()[Player::PART::PART_BODY]);

    BinaryBone* pBone = pBody->GetBinaryModelComponent()->GetBone("slider");
    Matrix mSliderMatrix = pBone->GetSliderPos();
    Matrix pivotMatrix = pBody->GetBinaryModelComponent()->GetPivotMatrix();

    Vec4 vSliderPos = Vec4(mSliderMatrix._41 + 0.03f, mSliderMatrix._42 + 0.05f, mSliderMatrix._43, mSliderMatrix._44);
    vSliderPos = ::XMVector3TransformCoord(vSliderPos, pivotMatrix);

    Matrix finalMatrix = pPlayer->GetTransform()->GetWorldMatrix();
    vSliderPos = ::XMVector3TransformCoord(vSliderPos, finalMatrix);

    _transform->SetState(Transform::STATE::POSITION, vSliderPos);

    RELEASE_INSTANCE(GameInstance);

    _pRenderer->AddRenderGroup(Renderer::RENDERGROUP::BLEND, this);
}

HRESULT FireEffect::Ready_Components()
{
    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
        TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_pRenderer))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
        TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePointInstance"),
        TEXT("ComponentShader"), reinterpret_cast<Component**>(&_pShader))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeGunFire"),
        TEXT("ComponentTexture"), reinterpret_cast<Component**>(&_pTexture))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePointBuffer"),
        TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_pVIBuffer))))
        return E_FAIL;

    return S_OK;
}

HRESULT FireEffect::Bind_ShaderResources()
{
    if (FAILED(_transform->BindShaderResources(_pShader, "W")))
        return E_FAIL;

    GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

    if (FAILED(pGameInstance->BindTransformToShader(_pShader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
        return E_FAIL;
    if (FAILED(pGameInstance->BindTransformToShader(_pShader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
        return E_FAIL;

    Vec4 vCameraPos = pGameInstance->GetCameraCaculator();

    if (FAILED(_pShader->BindRawValue("CameraPos", &vCameraPos, sizeof(Vec4))))
        return E_FAIL;

    RELEASE_INSTANCE(GameInstance);

    if (FAILED(_pTexture->BindShaderResource(_pShader, "gTexture", 0)))
        return E_FAIL;

    _pShader->Begin(0);
    
    return S_OK;
}

FireEffect* FireEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    FireEffect* pInstance = new FireEffect(pDevice, pDeviceContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create : FireEffect");
        Safe_Release<FireEffect*>(pInstance);
    }

    return pInstance;
}

GameObject* FireEffect::Clone(void* pArg)
{
    FireEffect* pInstance = new FireEffect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : FireEffect");
        Safe_Release<FireEffect*>(pInstance);
    }

    return pInstance;
}

void FireEffect::Free()
{
    __super::Free();

    Safe_Release<Renderer*>(_pRenderer);
    Safe_Release<Texture*>(_pTexture);
    Safe_Release<Shader*>(_pShader);
    Safe_Release<VIBuffer_PointInstance*>(_pVIBuffer);
}
