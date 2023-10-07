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

	int32 boneIndex; // ���� �������� ������ ������ �ִ� �� �� �ε����� �����Ѵ�.
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