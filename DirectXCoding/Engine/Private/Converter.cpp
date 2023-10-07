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
	// _scene에 있는 정보를 불러와서 우리만에 모델 데이터로 치환.

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


	//ReadModelData(_scene->mRootNode, -1, -1); // 재귀적 호출
	WriteModelFile(finalPath);
	// 우리가 메모리에 들고 있던 것을 최종 파일 형태로 만든다.

}

void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + L"Test" + L".xml";
	// xml 파일로 뽑아내면 fbx안에 있던 내용들을 한 번에 확인할 수 있음 jason을 사용해도 괜찮음
	ReadMaterialData();
	WriteMaterialData(finalPath); // 우리가 원하는 디렉토리에 저장 
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
	// 재귀적으로 호출할 예정.
	// 계층구조

	shared_ptr<asBone> bone = make_shared<asBone>();
	bone->index = index; // 파싱한 순서
	bone->parent = parent;
	bone->name = node->mName.C_Str();
	
	// Relative Transform
	Matrix transform(node->mTransformation[0]);
	bone->Transform = transform.Transpose();
	
	// Local(Root) Transform
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = _bones[parent]->Transform; // Root까지 가야되는 트랜스폼을 들고 있는 것임.

	bone->Transform = bone->Transform * matParent;
	_bones.push_back(bone);

	// Mesh
	ReadMeshData(node, index);

	// 하나의 노드에 대한 처리가 끝났다면 재귀함수 시작
	for (uint32 i = 0; i < node->mNumChildren; i++)
		ReadModelData(node->mChildren[i], _bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{
	if (node->mNumMeshes < 1)
		return;

	// as 붙은 것들은 우리가 메모리상에 들고 있는 정보들임.
	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		uint32 index = node->mMeshes[i]; // 메쉬의 실질적인 번호.
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// Material Name
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();
		// 어떤 메쉬를 그릴 때는 어떤 마테리얼을 그릴 것인지 매칭 해줄 수 있음.

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
	// Binaries 파일
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
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

		// Read 할 때 똑같은 순서대로 해야함.
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
		// 원본의 형태를 받는다.
		aiMaterial* srcMaterial = _scene->mMaterials[i];
		
		// 우리가 커스텀(받고 싶은 정보만)으로 받을 마테리얼.
		shared_ptr<asMaterial> material = make_shared<asMaterial>();

		material->name = srcMaterial->GetName().C_Str();
		
		aiColor3D color;
		// Ambient -> AI Material Key 값을 이용해서 Color를 가지고옴.
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Color(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w); // 스펙큘러의 세기(범위)

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
	// File 디렉토리가 없으면 만든다.
	auto path = filesystem::path(finalPath);
	filesystem::create_directory(path.parent_path());

	string folder = path.parent_path().string();

	// 계층구조를 지키면서 저장한다. tinyxml

	// tinyxml로 도큐먼트를 하나 만든다.
	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();
	
	// xml 포맷으로 읽을거예요 -> 굳이 필요없긴함.
	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	// 문서에 Materials(Root)가 만들어진거다.
	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	// 메모리에 들고 있는 모든 마테리얼을 순회하면서 저장
	for (shared_ptr<asMaterial> material : _materials)
	{
		// root 밑의 material을 만들어서 속성들을 저장.
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		// material node에다가 이 이름을 붙여주는 작업이다.
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

		// 앰비언트의 저장할 색상 추가.
		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		// 디퓨즈에 저장할 색상 추가
		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		// 스펙큘러에 저장할 색상 추가
		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		// Emissive에 저장할 색상 추가
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

		if (srcTexture->mHeight == 0) // 헤이트가 0이면 바이너리 모드로 만든다. 
		{
			shared_ptr<FileUtils> file = make_shared<FileUtils>();
			file->Open(Utils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else // 아니면 다른 함수를 이용해서 만든다.
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

