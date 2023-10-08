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

	// ���������� �����ϰų� ���� �����ϰ� ������ ������� ��� custom
public:
	void ReadAssetFile(wstring file); // ����(FBX) ������ �ҷ����� ��.
	void ExportModelData(wstring savePath, uint32 modelNumber);
	void ExportMaterialData(wstring savePath, uint32 modelNumber);


	void ImportModelData();
	void ImportMaterialData();



private: // scene �޸𸮿� ���ִ� �͵��� �о���� �غ�
	void ReadModelData(aiNode* node, int32 index, int32 parent); // ���� ���ϴ� Model Data�� �����Ѵ�.
	void ReadMeshData(aiNode* node, int32 bone); // ���� ���ϴ� Mesh Data�� �����Ѵ�.
	void WriteModelFile(wstring finalPath);

private: // scene �޸𸮿� ���ִ� �͵��� �о���� �غ�
	void ReadMaterialData(); // ���� ���ϴ� ���������� Material �����Ѵ�.
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