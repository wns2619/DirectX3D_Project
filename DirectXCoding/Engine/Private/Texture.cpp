#include "pch.h"
#include "Texture.h"
#include "Shader.h"

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::TEXTURE)
{
}

Texture::Texture(const Texture& rhs)
	: Component(rhs)
	, _numTextures(rhs._numTextures)
	, _shaderResourceViews(rhs._shaderResourceViews)
{
	for (size_t i = 0; i < _numTextures; i++)
		Safe_AddRef<ID3D11ShaderResourceView*>(_shaderResourceViews[i]);
}

HRESULT Texture::InitializePrototype(const wstring& textureFilePath, uint32 numTextures)
{
	_numTextures = numTextures;

	_shaderResourceViews = new ID3D11ShaderResourceView*[numTextures];

	for (size_t i = 0; i < numTextures; i++)
	{
		_tchar ltextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(ltextureFilePath, textureFilePath.c_str(), i);

		_tchar lext[MAX_PATH] = TEXT("");

		_wsplitpath_s(ltextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, lext, MAX_PATH);

		_texturePath = ltextureFilePath;

		HRESULT hr = 0;
		ID3D11ShaderResourceView* resourceView = nullptr;

		if (false == lstrcmp(lext, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(_device, ltextureFilePath, nullptr, &resourceView);
		else if (false == lstrcmp(lext, TEXT(".tga")))
			return E_FAIL;
		else
			hr = CreateWICTextureFromFile(_device, ltextureFilePath, nullptr, &resourceView);

		if (FAILED(hr))
			return E_FAIL;

		_shaderResourceViews[i] = resourceView;

	}

	return S_OK;
}

HRESULT Texture::Initialize(void* argument)
{
	return S_OK;

	//Texture* changeTexture = (*vectorMesh)[vectorIndex]._texture[aiTextureType_EMISSIVE] = ImGuiResourceHandler::GetInstance()->GetResourceTexture(path.c_str());
}

HRESULT Texture::BindShaderResource(const Shader* shader, const _char* constantName, uint32 textureIndex)
{
	return shader->BindTexture(constantName, _shaderResourceViews[textureIndex]);
}

HRESULT Texture::BindShaderReosurces(const Shader* shader, const _char* constantName)
{
	return shader->BindTextures(constantName, _shaderResourceViews, _numTextures);
}

HRESULT Texture::ChangeShaderResourceView(ID3D11ShaderResourceView* changeResource, uint32 textureIndex, uint32 numTextures)
{
	if (nullptr == changeResource)
		return E_FAIL;


	return S_OK;
}

void Texture::SelfDelete(Texture* self)
{
	Safe_Release<Texture*>(self);
}

Texture* Texture::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& textureFilePath, uint32 numTextures)
{
 	Texture* texture = new Texture(device, deviceContext);

	if (FAILED(texture->InitializePrototype(textureFilePath, numTextures)))
	{
		MSG_BOX("Failed to Created : Texture");
		Safe_Release<Texture*>(texture);
		return nullptr;
	}


	return texture;
}

Component* Texture::Clone(void* argument)
{
	Texture* texture = new Texture(*this);

	if (FAILED(texture->Initialize(argument)))
	{
		MSG_BOX("Failed to Clone : Texture");
		Safe_Release<Texture*>(texture);
		return nullptr;
	}

	return texture;
}

void Texture::Free()
{
	__super::Free();

	for (size_t i = 0; i < _numTextures; i++)
		Safe_Release<ID3D11ShaderResourceView*>(_shaderResourceViews[i]);

	if (false == _isCloned)
		Safe_Delete_Array<ID3D11ShaderResourceView**>(_shaderResourceViews);
}
