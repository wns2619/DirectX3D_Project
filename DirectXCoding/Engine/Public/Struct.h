#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN,  WINMODE_END };

		HWND	_hwnd;
		WINMODE	_winMode;
		unsigned int	iWinSizeX, iWinSizeY;
	}GRAPHIC_DESC;


	struct VertexData
	{
		Vec3 position = { 0,0,0 };
	};

	struct ENGINE_DLL VertexTextureData
	{
		Vec3 position = { 0,0,0, };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];

	};

	struct VertexColorData
	{
		Vec3 position = { 0,0,0 };
		Color color = { 0,0,0,0 };
	};

	struct ENGINE_DLL VertexTextureNormalData
	{
		Vec3 position = { 0,0,0 };
		Vec3 normal = { 0,0,0 };
		Vec2 uv = { 0,0 };

		static const uint32 numElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};
	

}


#endif // Engine_Struct_h__
