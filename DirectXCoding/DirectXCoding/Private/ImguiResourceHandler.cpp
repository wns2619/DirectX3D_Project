#include "pch.h"
#include "ImguiResourceHandler.h"

IMPLEMENT_SINGLETON(ImGuiResourceHandler)

ImGuiResourceHandler::ImGuiResourceHandler()
{
}

HRESULT ImGuiResourceHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    Safe_AddRef<ID3D11Device*>(device);
    Safe_AddRef<ID3D11DeviceContext*>(deviceContext);

    _device = device;
    _deviceContext = deviceContext;

    return S_OK;
}

void ImGuiResourceHandler::Tick(const _float& timeDelta)
{
}

void ImGuiResourceHandler::LateTick(const _float& timeDelta)
{
}

HRESULT ImGuiResourceHandler::Render()
{
    return S_OK;
}

ID3D11ShaderResourceView* ImGuiResourceHandler::GetResourceTexture(const WCHAR* texturePath, _bool isCubeMap)
{
    wstring path(texturePath);
    if (path.find(rootTexturePath) == 0)
        path.erase(0, rootTexturePath.length());

    if (!_imguitextures.count(path.c_str()))
    {
        if (_device != nullptr)
            CreateTextureFormFile(path.c_str(), isCubeMap);
    

        return _imguitextures[path.c_str()];
    }
    else
        return _imguitextures[path.c_str()];

}

_bool ImGuiResourceHandler::CreateTextureFormFile(const WCHAR* texturePath, _bool isCubeMap)
{
    HRESULT hr;
    _bool couldLoad = true;
    _imguitextures[texturePath] = nullptr;
    wstring path = texturePath;
    path = rootTexturePath + path;


    size_t i = path.rfind('.', path.length()); // 문자열, 문자의 역방향으로 발생위치 찾음. .jpg .png .dds .tga 등 
    wstring fileExtension = path.substr(i + 1, path.length() - i); // 점 다음 문자부터 시작하는 인덱스 + 마지막 점 이후 문자열의 길이를 나타냄
    if (fileExtension == L"dds" || fileExtension == L"DDS")
        hr = ::CreateDDSTextureFromFile(_device, _deviceContext, path.c_str(), nullptr, &_imguitextures[texturePath]);
    else if (fileExtension == L"hdr")
    {
        DirectX::TexMetadata metadata;
        DirectX::ScratchImage scratchImage;
        hr = ::LoadFromHDRFile(path.c_str(), &metadata, scratchImage);
        assert(SUCCEEDED(hr) && "Error, Failed to load HDR Texture file!");

        hr = ::CreateShaderResourceView(_device, scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, &_imguitextures[texturePath]);
    }
    else if (fileExtension == L"tga" || fileExtension == L"TGA")
    {
        DirectX::TexMetadata metaData;
        DirectX::ScratchImage scratchImage;
        DirectX::LoadFromTGAFile(path.c_str(), &metaData, scratchImage);
        DirectX::ScratchImage mipChain;

        hr = ::GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
        assert(SUCCEEDED(hr) && "Error, Failed to generate mipmaps for TGA Texture file!");
    }
    else
        hr = ::CreateWICTextureFromFile(_device, _deviceContext, path.c_str(), nullptr, &_imguitextures[texturePath]);

    if (FAILED(hr))
    {
        ::OutputDebugString(L"Failed to Load : ");
        ::OutputDebugString(texturePath);
        ::OutputDebugString(L"'\n");
        path = rootTexturePath + L"Empty_Texture.jpg";
        hr = ::CreateWICTextureFromFile(_device, _deviceContext, path.c_str(), nullptr, &_imguitextures[texturePath]);
        couldLoad = false;

 
    }
    else
    {
        ::OutputDebugString(L"Texture loaded : ");
        ::OutputDebugString(texturePath);
        ::OutputDebugString(L"'\n");
    }

    return couldLoad;
}

void ImGuiResourceHandler::Free()
{
    __super::Free();

    for (auto& iter : _imguitextures)
        Safe_Release<ID3D11ShaderResourceView*>(iter.second);

    Safe_Release<ID3D11Device*>(_device);
    Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}
