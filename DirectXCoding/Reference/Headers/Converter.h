#pragma once
#include "AsTypes.h"
#include "Base.h"

BEGIN(Engine)

class Converter
{
public:
	explicit Converter();
	explicit Converter(const wstring& strFileName);
	~Converter();

	// ���������� �����ϰų� ���� �����ϰ� ������ ������� ��� custom
public:
	void ReadAssetFile(wstring file); // ����(FBX) ������ �ҷ����� ��.
	void ExportModelData(wstring savePath);
	void ExportMaterialData(wstring savePath);


private: // scene �޸𸮿� ���ִ� �͵��� �о���� �غ�
	void ReadModelData(aiNode* node, int32 index, int32 parent); // ���� ���ϴ� Model Data�� �����Ѵ�.
	void ReadMeshData(aiNode* node, int32 bone); // ���� ���ϴ� Mesh Data�� �����Ѵ�.
	void WriteModelFile(wstring finalPath);

private: // scene �޸𸮿� ���ִ� �͵��� �о���� �غ�
	void ReadMaterialData(); // ���� ���ϴ� ���������� Material �����Ѵ�.
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);


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
};

END