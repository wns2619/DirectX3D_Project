#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#pragma warning(disable : 26495)

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN,  WINMODE_END };

		HWND	_hwnd;
		WINMODE	_winMode;
		unsigned int	iWinSizeX, iWinSizeY;
	}GRAPHIC_DESC;


	struct ENGINE_DLL VertexTextureData
	{
		Vec3 position = { 0,0,0, };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];

	};


	struct ENGINE_DLL VertexTextureNormalData
	{
		Vec3 position = { 0,0,0 };
		Vec3 normal = { 0,0,0 };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};
	
	struct DirectionalLight
	{
		DirectionalLight() { ::ZeroMemory(this, sizeof(this)); }

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;
		Vec3 Direction;
		_float pad;
	};

	struct PointLight
	{
		PointLight() { ::ZeroMemory(this, sizeof(this)); }

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;

		Vec3 Position;
		_float Range;

		Vec3 Attenuation;
		_float Pad;
	};

	struct SpotLight
	{
		SpotLight() { ::ZeroMemory(this, sizeof(this)); }

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;

		Vec3 Position;
		_float Range;

		Vec3 Direction;
		_float Spot;

		Vec3 Attenuation;
		_float Pad;
	};

	struct ENGINE_DLL Material
	{
		Material() { ::ZeroMemory(this, sizeof(this)); }

		Vec4 Ambient;
		Vec4 Diffuse;
		Vec4 Specular;
		Vec4 Reflect;
	};

	struct ENGINE_DLL LightGroup
	{
		DirectionalLight dirLight;
		PointLight pointLight;
		SpotLight spotLight;
	};


}


#endif // Engine_Struct_h__
