#include "pch.h"
#include "BloodUI.h"
#include "GameInstance.h"
#include "Player.h"

BloodUI::BloodUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : GameObject(pDevice, pDeviceContext, OBJECT_TYPE::UI)
{
    _modelName = "BloodUI";
}

BloodUI::BloodUI(const BloodUI& rhs)
    : GameObject(rhs)
{
}

HRESULT BloodUI::InitializePrototype()
{


    return S_OK;
}

HRESULT BloodUI::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    _fSizeX = g_iWinSizeX;
    _fSizeY = g_iWinSizeY;

    _fX = _fSizeX * 0.5f;
    _fY = _fSizeY * 0.5f;

    _transform->SetScaling(Vec3(_fSizeX * 0.5f, _fSizeY * 0.5f, 1.f));
    _transform->SetState(Transform::STATE::POSITION, Vec4(_fX - _fSizeX * 0.2, -_fY + _fSizeY * 0.78f, 0.f, 1.f));

    _mProjMatrix[0] = _mProjMatrix[0].CreateOrthographic(_fSizeX, _fSizeY, 0.f, 1.f);

    _pTransform->SetScaling(Vec3(_fSizeX * 0.3f, _fSizeY * 0.3f, 1.f));
    _pTransform->SetState(Transform::STATE::POSITION, Vec4(_fX - _fSizeX * 0.9f, -_fY + _fSizeY * 0.18f, 0.f, 1.f));

    _mProjMatrix[1] = _mProjMatrix[1].CreateOrthographic(_fSizeX, _fSizeY, 0.f, 1.f);

    return S_OK;
}

HRESULT BloodUI::Render()
{


    if (true == _bHitBullet)
    {
        for (uint32 i = 0; i < 2; ++i)
        {
            if (FAILED(Bind_ShaderResources(i)))
                return E_FAIL;

            if (FAILED(_pShader->Begin(1)))
                return E_FAIL;

            if (FAILED(_pVIBuffer[i]->Render()))
                return E_FAIL;
        }
    }


    return S_OK;
}

void BloodUI::Tick(const _float& fTimeDelta)
{
    if (true == _bHitBullet)
        _fLifeTime += fTimeDelta;

    if (_fLifeTime > 4.5f)
    {
     /*   _fLifeTime = 0.f;
        _bHitBullet = false;*/
    }
}

void BloodUI::LateTick(const _float& fTimeDelta)
{
    _pRenderer->AddRenderGroup(Renderer::RENDERGROUP::UI, this);
}

HRESULT BloodUI::Ready_Components()
{
    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
        TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_pRenderer))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
        TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_pVIBuffer[0]))))
        return E_FAIL;


    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
        TEXT("ComponentVIBuffer2"), reinterpret_cast<Component**>(&_pVIBuffer[1]))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
        TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
        TEXT("ComponentTransform2"), reinterpret_cast<Component**>(&_pTransform))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeShaderRect"),
        TEXT("ComponentShader"), reinterpret_cast<Component**>(&_pShader))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBlood1"), TEXT("ComponentTexture"),
        reinterpret_cast<Component**>(&_pTexture[0]))))
        return E_FAIL;

    if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBlood2"), TEXT("ComponentTexture2"),
        reinterpret_cast<Component**>(&_pTexture[1]))))
        return E_FAIL;

    return S_OK;
}

HRESULT BloodUI::Bind_ShaderResources(uint32 Iindex)
{

    if (Iindex == 0)
    {
        if (FAILED(_transform->BindShaderResources(_pShader, "W")))
            return E_FAIL;
    }
    else
        if (FAILED(_pTransform->BindShaderResources(_pShader, "W")))
            return E_FAIL;
 
    if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix[Iindex])))
        return E_FAIL;
    if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix[Iindex])))
        return E_FAIL;
    if (FAILED(_pShader->BindRawValue("fTime", &_fLifeTime, sizeof(_float))))
        return E_FAIL;

    string strTextureName = "UITextureArray0";

    if (FAILED(_pTexture[Iindex]->BindShaderReosurces(_pShader, strTextureName.c_str())))
        return E_FAIL;
   
    return S_OK;
}

GameObject* BloodUI::Clone(void* pArg)
{
    BloodUI* pInstance = new BloodUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : BloodUI");
        Safe_Release<BloodUI*>(pInstance);
    }

    return pInstance;
}

void BloodUI::Free()
{
    __super::Free();

    Safe_Release<Renderer*>(_pRenderer);
    Safe_Release<Shader*>(_pShader);

    for (uint32 i = 0; i < 2; ++i)
    {
        Safe_Release<Texture*>(_pTexture[i]);
        Safe_Release<VIBufferRect*>(_pVIBuffer[i]);
    }

    Safe_Release<Transform*>(_pTransform);
}

BloodUI* BloodUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    BloodUI* pInstance = new BloodUI(pDevice, pDeviceContext);

    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to BloodUI");
        Safe_Release<BloodUI*>(pInstance);
    }

    return pInstance;
}
