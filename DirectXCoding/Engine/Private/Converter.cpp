#include "pch.h"
#include "Converter.h"
#include "Utils.h"
#include <filesystem>


Converter::Converter()
{
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
	wstring fileStr = _assetPath + file;

	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		Utils::ToString(fileStr),
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
	ReadModelData(_scene->mRootNode, -1, -1); // 재귀적 호출

	WriteModelFile(finalPath);
	// 우리가 메모리에 들고 있던 것을 최종 파일 형태로 만든다.

}

void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + savePath + L".xml";
	// xml 파일로 뽑아내면 fbx안에 있던 내용들을 한 번에 확인할 수 있음 jason을 사용해도 괜찮음
	ReadMaterialData();
	WriteMaterialData(finalPath); // 우리가 원하는 디렉토리에 저장 

}

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
	// 재귀적으로 호출할 예정.
	
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{
}

void Converter::WriteModelFile(wstring finalPath)
{
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
	return "";
}

