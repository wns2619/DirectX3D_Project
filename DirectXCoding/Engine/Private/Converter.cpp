#include "pch.h"
#include "Converter.h"
#include "Utils.h"
#include "FileUtils.h"
#include <filesystem>


Converter::Converter()
{
	_importer = make_shared<Assimp::Importer>();
}

Converter::Converter(const wstring& strFileName)
{
	_importer = make_shared<Assimp::Importer>();

	_strTest = strFileName;
}

Converter::~Converter()
{
}

void Converter::ReadAssetFile(wstring file)
{
	//wstring fileStr = _assetPath + file;

	auto p = std::filesystem::path(file);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		Utils::ToString(file),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);

}

void Converter::ExportModelData(wstring savePath)
{
	// _scene�� �ִ� ������ �ҷ��ͼ� �츮���� �� �����ͷ� ġȯ.

	wstring finalPath = _modelPath + savePath + L".mesh";
	{
		FILE* file;
		::fopen_s(&file, "../Vertices.csv", "w");

		for (shared_ptr<asBone>& bone : _bones)
		{
			string name = bone->name;
			::fprintf(file, "%d,%s\n", bone->index, bone->name.c_str());
		}

		::fprintf(file, "\n");

		for (shared_ptr<asMesh>& mesh : _meshes)
		{
			string name = mesh->name;
			::printf("%s\n", name.c_str());

			for (uint32 i = 0; i < mesh->vertices.size(); i++)
			{
				Vec3 p = mesh->vertices[i].position;
				XMUINT4 indices = mesh->vertices[i].blendIndices;
				Vec4 weights = mesh->vertices[i].blendWeights;

				::fprintf(file, "%f,%f,%f", p.x, p.y, p.z);
				::fprintf(file, "%f,%f,%f,%f", indices.x, indices.y, indices.z, indices.w);
				::fprintf(file, "%f,%f,%f,%f\n", weights.x, weights.y, weights.z, weights.w);
			}
		}

		::fclose(file);
	}


	//ReadModelData(_scene->mRootNode, -1, -1); // ����� ȣ��
	WriteModelFile(finalPath);
	// �츮�� �޸𸮿� ��� �ִ� ���� ���� ���� ���·� �����.

}

void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + L"Test" + L".xml";
	// xml ���Ϸ� �̾Ƴ��� fbx�ȿ� �ִ� ������� �� ���� Ȯ���� �� ���� jason�� ����ص� ������
	ReadMaterialData();
	WriteMaterialData(finalPath); // �츮�� ���ϴ� ���丮�� ���� 
}

void Converter::ImportModelData()
{
	ReadModelData(_scene->mRootNode, -1, -1);
	// TODO

}

void Converter::ImportMaterialData()
{
	ReadMaterialData();
}

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
	// ��������� ȣ���� ����.
	// ��������

	shared_ptr<asBone> bone = make_shared<asBone>();
	bone->index = index; // �Ľ��� ����
	bone->parent = parent;
	bone->name = node->mName.C_Str();
	
	// Relative Transform
	Matrix transform(node->mTransformation[0]);
	bone->Transform = transform.Transpose();
	
	// Local(Root) Transform
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = _bones[parent]->Transform; // Root���� ���ߵǴ� Ʈ�������� ��� �ִ� ����.

	bone->Transform = bone->Transform * matParent;
	_bones.push_back(bone);

	// Mesh
	ReadMeshData(node, index);

	// �ϳ��� ��忡 ���� ó���� �����ٸ� ����Լ� ����
	for (uint32 i = 0; i < node->mNumChildren; i++)
		ReadModelData(node->mChildren[i], _bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{
	if (node->mNumMeshes < 1)
		return;

	// as ���� �͵��� �츮�� �޸𸮻� ��� �ִ� ��������.
	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		uint32 index = node->mMeshes[i]; // �޽��� �������� ��ȣ.
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// Material Name
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();
		// � �޽��� �׸� ���� � ���׸����� �׸� ������ ��Ī ���� �� ����.

		const uint32 startVertex = mesh->vertices.size();

		for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			VertexType vertex;
			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));
			
			// uv
			if (srcMesh->HasTextureCoords(0))
				::memcpy(&vertex.texcoord, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

			// Normal
			if (srcMesh->HasNormals())
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; k++)
				mesh->indices.push_back(face.mIndices[k] + startVertex);
		}

	}

	_meshes.push_back(mesh);
}

void Converter::WriteModelFile(wstring finalPath)
{
	// Binaries ����
	auto path = filesystem::path(finalPath);

	// ������ ������ �����.
	filesystem::create_directory(path.parent_path());

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(finalPath, FileMode::Write);

	// Bone Data
	file->Write<uint32>(_bones.size());
	for (shared_ptr<asBone>& bone : _bones)
	{
		file->Write<int32>(bone->index);
		file->Write<string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->Transform);

		// Read �� �� �Ȱ��� ������� �ؾ���.
	}

	// Mesh Data
	file->Write<uint32>(_meshes.size());
	for (shared_ptr<asMesh>& meshData : _meshes)
	{
		file->Write<string>(meshData->name);
		file->Write<int32>(meshData->boneIndex);
		file->Write<string>(meshData->materialName);
		file->Write<int32>(meshData->materialIndex);

		// VertexData
		file->Write<uint32>(meshData->vertices.size());
		file->Write(&meshData->vertices[0], sizeof(VertexType) * meshData->vertices.size());

		// Index Data
		file->Write<uint32>(meshData->indices.size());
		file->Write(&meshData->indices[0], sizeof(uint32) * meshData->indices.size());
	}

}

void Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < _scene->mNumMaterials; i++)
	{
		// ������ ���¸� �޴´�.
		aiMaterial* srcMaterial = _scene->mMaterials[i];
		
		// �츮�� Ŀ����(�ް� ���� ������)���� ���� ���׸���.
		shared_ptr<asMaterial> material = make_shared<asMaterial>();

		material->name = srcMaterial->GetName().C_Str();
		
		aiColor3D color;
		// Ambient -> AI Material Key ���� �̿��ؼ� Color�� �������.
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Color(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w); // ����ŧ���� ����(����)

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = Color(color.r, color.g, color.b, 1.f);

		aiString file;

		// Diffuse Texture
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->diffuseFile = file.C_Str();

		// Specular Texture
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->specularFile = file.C_Str();

		// Normal Texture
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->normalFile = file.C_Str();

		_materials.push_back(material);
	}
}

void Converter::WriteMaterialData(wstring finalPath)
{
	// File ���丮�� ������ �����.
	auto path = filesystem::path(finalPath);
	filesystem::create_directory(path.parent_path());

	string folder = path.parent_path().string();

	// ���������� ��Ű�鼭 �����Ѵ�. tinyxml

	// tinyxml�� ��ť��Ʈ�� �ϳ� �����.
	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
	
	// xml �������� �����ſ��� -> ���� �ʿ������.
	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	// ������ Materials(Root)�� ��������Ŵ�.
	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	// �޸𸮿� ��� �ִ� ��� ���׸����� ��ȸ�ϸ鼭 ����
	for (shared_ptr<asMaterial> material : _materials)
	{
		// root ���� material�� ���� �Ӽ����� ����.
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		// material node���ٰ� �� �̸��� �ٿ��ִ� �۾��̴�.
		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		// �ں��Ʈ�� ������ ���� �߰�.
		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		// ��ǻ� ������ ���� �߰�
		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		// ����ŧ���� ������ ���� �߰�
		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		// Emissive�� ������ ���� �߰�
		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(Utils::ToString(finalPath).c_str());
}

string Converter::WriteTexture(string saveFolder, string file)
{
	string fileName = filesystem::path(file).filename().string();
	string folderName = filesystem::path(saveFolder).filename().string();
	
	const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
	if (srcTexture)
	{
		string pathStr = saveFolder + fileName;

		if (srcTexture->mHeight == 0) // ����Ʈ�� 0�̸� ���̳ʸ� ���� �����. 
		{
			shared_ptr<FileUtils> file = make_shared<FileUtils>();
			file->Open(Utils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else // �ƴϸ� �ٸ� �Լ��� �̿��ؼ� �����.
		{
			
		}
	}
	else
	{
		string originStr = (filesystem::path(_assetPath) / folderName / file).string();
		Utils::Replace(originStr, "\\", "/");

		string path = (filesystem::path(saveFolder) / fileName).string();
		Utils::Replace(path, "\\", "/");

		::CopyFileA(originStr.c_str(), path.c_str(), false);

	}

	return fileName;
}

