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
}

#endif // Engine_Typedef_h__
