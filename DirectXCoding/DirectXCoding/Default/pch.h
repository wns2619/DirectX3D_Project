// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "framework.h"

#include "Client_Define.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>
#include <dinput.h>
#include "comdef.h"

#include <DirectXMath.h>
#include <Include\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTex\DirectXTex.h>
#include <DirectXTex\DirectXTex.inl>

#include "dirent.h"
#include "StringUtilities.h"

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
