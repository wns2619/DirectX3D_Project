#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#pragma warning(disable : 26495)

enum class LayoutType { POS_NOR_TEX_TAN, POS_NOR_TEX, POS_TEX_FINDEX, POS_TEX, POS_COL, POS, PARTICLE, NONE };


namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		HWND	_hwnd;
		WINMODE	_winMode;
		unsigned int	iWinSizeX, iWinSizeY;
	}GRAPHIC_DESC;


	/* VertexType Struct */
	struct ENGINE_DLL VertexPos
	{
		Vec3 position;

		static const uint32 VertexPosElementCount = 1;
	};


	struct ENGINE_DLL VertexPosTexFrustumIndex
	{
		Vec3 position;
		Vec2 uv;
		uint32 frustumIndex;

		static const uint32 VertexPosTexFrustumIndexElementCount = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[VertexPosTexFrustumIndexElementCount];
	};

	struct VertexPosCol
	{
		Vec3 position;
		Vec4 color;

		static const unsigned int VertexPosColElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[VertexPosColElementCount];
	};


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

	struct ENGINE_DLL VertexParticle
	{
		XMFLOAT3 position;
		XMFLOAT3 velocity;
		XMFLOAT2 size;
		float rotation; // radiance
		float age;
		UINT type;
		UINT maxId;

		static const uint32 numElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC Elements[numElements];
	};

	struct MESH_MATERIAL
	{
		class Texture* _texture[AI_TEXTURE_TYPE_MAX];
	};


	struct ENGINE_DLL VTXMESH
	{
		Vec3		position;
		Vec3		normal;
		Vec2		texcoord;
		Vec3		tangent;
		Vec3		bitangent;

		static const unsigned int				iNumElements = 5;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	};

	struct ENGINE_DLL VTXANIMMESH
	{
		Vec3		position;
		Vec3		normal;
		Vec2		texcoord;
		Vec3		tangent;
		Vec3		bitangent;

		/* ������ ���¸� �޾ƾ��� ������ �ε��� */
		XMUINT4		blendIndices;
		Vec4		blendWeights;

		static const unsigned int				iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	};

	struct ENGINE_DLL TexturePaths
	{
		wstring diffusePath = L"";
		wstring specularPath = L"";
		wstring normalPath = L"";
		wstring displacementPath = L"";
	};

};




#endif // Engine_Struct_h__
