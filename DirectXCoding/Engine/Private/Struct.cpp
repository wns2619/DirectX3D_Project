#include "pch.h"
#include "Defines.h"


namespace Engine
{
	const D3D11_INPUT_ELEMENT_DESC VertexTextureData::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};


	const D3D11_INPUT_ELEMENT_DESC VertexTextureNormalData::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC VertexPosTexFrustumIndex::Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "FRUSTUM_INDEX", 0, DXGI_FORMAT_R32_UINT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC VertexPosCol::Elements[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_INPUT_ELEMENT_DESC VertexPosNormTexTan::Elements[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const D3D11_INPUT_ELEMENT_DESC VTXMESH::Elements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, /* ��ġ���Ϳ� ���� ����. */
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, /* �븻���Ϳ� ���� ����. */
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },	/* �ؽ����� ���Ϳ� ���� ����. */
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },	/* �ؽ����� ���Ϳ� ���� ����. */
	};


	const D3D11_INPUT_ELEMENT_DESC VertexParticle::Elements[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION",	0, DXGI_FORMAT_R32_FLOAT,			0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "AGE",		0, DXGI_FORMAT_R32_FLOAT,			0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TYPE",		0, DXGI_FORMAT_R32_UINT,			0, 40,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MAXID",		0, DXGI_FORMAT_R32_UINT,			0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};




	const ENGINE_DLL D3D11_SO_DECLARATION_ENTRY VertexParticleSoDecl[] =
	{
		{ 0, "POSITION", 0, 0, 3, 0 },
		{ 0, "VELOCITY", 0, 0, 3, 0 },
		{ 0, "SIZE", 0, 0, 2, 0 },
		{ 0, "ROTATION", 0, 0, 1, 0 },
		{ 0, "AGE", 0, 0, 1, 0 },
		{ 0, "TYPE", 0, 0, 1, 0 },
		{ 0, "MAXID", 0, 0, 1, 0 },
	};

}