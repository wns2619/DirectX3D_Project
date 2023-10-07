#pragma once
#include "Defines.h"

using VertexType = VTXANIMMESH;


struct asBone
{
	string name;
	int32 index = -1;
	int32 parent = -1;
	Matrix Transform;
};

struct asMesh
{
	string name;
	aiMesh* mesh;
	vector<VertexType> vertices;
	vector<uint32> indices;

	int32 boneIndex; // 계층 구조에서 누구와 연관이 있는 지 본 인덱스로 관리한다.
	string materialName; 
};

struct asMaterial
{
	string name;
	Color ambient;
	Color diffuse;
	Color specular;
	Color emissive;

	string diffuseFile;
	string specularFile;
	string normalFile;
};