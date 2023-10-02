#pragma once

#include "Component.h"

BEGIN(Engine)

class Shader;

class ENGINE_DLL Texture final : public Component
{
private:
	explicit Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Texture(const Texture& rhs);
	virtual ~Texture() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& textureFilePath, uint32 numTextures);
	virtual HRESULT	Initialize(void* argument) override;

public:
	HRESULT	BindShaderResource(const Shader* shader, const _char* constantName, uint32 textureIndex);
	HRESULT BindShaderReosurces(const Shader* shader, const _char* constantName);

	// ImGuI
	HRESULT ChangeShaderResourceView(ID3D11ShaderResourceView* changeResource, uint32 textureIndex, uint32 numTextures = 1);
	ID3D11ShaderResourceView** GetShaderResourceViews() { return _shaderResourceViews; }
	void SelfDelete(Texture* self);
private:
	ID3D11ShaderResourceView** _shaderResourceViews = nullptr;
	uint32 _numTextures = 0;

public:
	static Texture* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const wstring& textureFilePath, uint32 numTextures = 1);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END