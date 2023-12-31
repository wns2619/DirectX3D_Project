#pragma once

#define WIN32_LEAN_AND_MEAN
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
#include <Include\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTex\DirectXTex.h>
#include <DirectXTex\DirectXTex.inl>
#include <DirectXTK\ScreenGrab.h>
#include <DirectXCollision.h>

#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\Effects.h>

#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>


// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//dirent
#include "dirent.h"
#include "tinyxml2.h"

using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <memory>
#include <iostream>
#include <queue>

#include <Windows.h>
#include <cmath>


using namespace std;

#include "Types.h"
#include "Macro.h"
#include "Struct.h"
#include "Function.h"
#include "Functor.h"
#include "Values.h"
#include "VertexData.h"
#include "MathHelper.h"
#include "MathUtilities.h"
#include "KeyCodes.h"

using namespace Engine;


#ifdef _DEBUG
#define MYDEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//allocations to be of _CLIENT_BLOCK type
#else
#define MYDEBUG_NEW
#endif // _DEBUG
/* MyApp.cpp
/* Compile options needed: /Zi /D_DEBUG /MLd
/* or use a
/* Default Workspace for a Console Application to
/* build a Debug version*/
#include "crtdbg.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

//#include "TimeManager.h"

//#define TIME	GET_SINGLE(TimeManager);