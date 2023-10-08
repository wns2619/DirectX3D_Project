#pragma once
#include "AsTypes.h"
#include "Base.h"

BEGIN(Engine)

class Converter
{
public:
	Converter();
	explicit Converter(ID3D11Device* devivce, ID3D11DeviceContext* deviceContext);
	~Converter();

	// 개별적으로 추출하거나 전부 추출하고 싶으면 마음대로 골라서 custom
public:
	void ReadAssetFile(wstring file); // 에셋(FBX) 파일을 불러오는 것.
	void ExportModelData(wstring savePath, uint32 modelNumber);
	void ExportMaterialData(wstring savePath, uint32 modelNumber);


	void ImportModelData();
	void ImportMaterialData();



private: // scene 메모리에 들어가있는 것들을 읽어들일 준비
	void ReadModelData(aiNode* node, int32 index, int32 parent); // 내가 원하는 Model Data만 저장한다.
	void ReadMeshData(aiNode* node, int32 bone); // 내가 원하는 Mesh Data만 저장한다.
	void WriteModelFile(wstring finalPath);

private: // scene 메모리에 들어가있는 것들을 읽어들일 준비
	void ReadMaterialData(); // 내가 원하는 정보만으로 Material 저장한다.
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);

public:
	vector<shared_ptr<asBone>>* GetAsBones() { return &_bones; }
	vector<shared_ptr<asMesh>>* GetAsMeshes() { return &_meshes; }
	vector<shared_ptr<asMaterial>>* GetAsMaterials() { return &_materials; }


public:
	wstring _assetPath = L"..\\Binaries\\Resources\\MyModels\\";
	wstring _modelPath = L"..\\Binaries\\Resources\\MyModels\\";
	wstring _texturePath = L"..\\Binaries\\Resources\\MyModels\\";
	wstring _strTest = L"";
private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

private:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;


	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceConetext = nullptr;
};

END