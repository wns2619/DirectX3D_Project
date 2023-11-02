#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

#include <windows.h>
#include "DirectXMath.h"
#include "SimpleMath.h"

namespace Engine
{
	using _bool = bool;
	using _byte = signed char;
	using _ubyte = unsigned char;


	using _char = char;
	using _tchar = wchar_t;

	using _short = signed short;
	using _ushort = unsigned short;

	using _float = float;
	using _double = double;

	using _long = signed long;
	using _ulong = unsigned long;

	using int8 = __int8;
	using int16 = __int16;
	using int32 = __int32;
	using int64 = __int64;
	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

	using Color = DirectX::XMFLOAT4;

	using Vec2 = DirectX::SimpleMath::Vector2;
	using Vec3 = DirectX::SimpleMath::Vector3;
	using Vec4 = DirectX::SimpleMath::Vector4;
	using Matrix = DirectX::SimpleMath::Matrix;
	using Quaternion = DirectX::SimpleMath::Quaternion;
	using Ray = DirectX::SimpleMath::Ray;

	enum class LAYER_TAG { LAYER_PLAYER, LAYER_MONSTER, LAYER_TERRAIN, 
		LAYER_CAMERA, LAYER_STATIC, LAYER_DYNAMIC, LAYER_PUZZLE, LAYER_ENVIRONMENT, LAYER_COLLIDER, LAYER_BULLET, LAYER_END, };

    enum TextureType
    {
        TextureType_NONE = 0,
        TextureType_DIFFUSE = 1,
        TextureType_SPECULAR = 2,
        TextureType_AMBIENT = 3,
        TextureType_EMISSIVE = 4,
        TextureType_HEIGHT = 5,
        TextureType_NORMALS = 6,
        TextureType_SHININESS = 7,
        TextureType_OPACITY = 8,
        TextureType_DISPLACEMENT = 9,
        TextureType_LIGHTMAP = 10,
        TextureType_REFLECTION = 11,
        TextureType_BASE_COLOR = 12,
        TextureType_NORMAL_CAMERA = 13,
        TextureType_EMISSION_COLOR = 14,
        TextureType_METALNESS = 15,
        TextureType_DIFFUSE_ROUGHNESS = 16,
        TextureType_AMBIENT_OCCLUSION = 17,
        TextureType_UNKNOWN = 18,


#ifndef SWIG
        TextureType_Force32Bit = INT_MAX
#endif
    };

#define TEXTURE_TYPE_MAX  TextureType_UNKNOWN

	enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
	enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

}

#endif // Engine_Typedef_h__
