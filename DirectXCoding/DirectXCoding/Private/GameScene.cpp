#include "pch.h"
#include "GameScene.h"

#include "GameInstance.h"
#include "PlayerCamera.h"
#include "LandObject.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Player.h"
#include "PlayerBody.h"

GameScene::GameScene(ID3D11Device* _device, ID3D11DeviceContext* _deviceConetxt)
    : Level(_device, _deviceConetxt)
{

}

GameScene::~GameScene()
{

}

HRESULT GameScene::Initialize()
{
	if (FAILED(ReadyMyMap()))
		return E_FAIL;

    //if (FAILED(ReadyLayerTerrain(LAYER_TAG::LAYER_STATIC)))
    //    return E_FAIL;

	if (FAILED(ReadyLight()))
		return E_FAIL;

    if (FAILED(ReadyLandObjects(LAYER_TAG::LAYER_STATIC)))
        return E_FAIL;

	if (FAILED(ReadyUI(LAYER_TAG::LAYER_UI)))
		return E_FAIL;

	//if (FAILED(ReadyLayerParticle(LAYER_TAG::LAYER_PARTICLE)))
	//	return E_FAIL;

	//if (FAILED(ReadyLayerCamera(LAYER_TAG::LAYER_CAMERA)))
	//	return E_FAIL;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	pGameInstance->PlayBGM(TEXT("gool.wav"), 0.3f);

	RELEASE_INSTANCE(GameInstance);

    return S_OK;
}

HRESULT GameScene::Tick(const _float& timeDelta)
{
    return S_OK;
}

HRESULT GameScene::LateTick(const _float& timeDelata)
{
    SetWindowText(g_hWnd, TEXT("This is GameLevel"));

    return S_OK;
}

HRESULT GameScene::ReadyMyMap()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring finalPath = L"..\\Binaries\\Resources\\MapData\\MapData.dat";

	file->Open(finalPath, FileMode::Read);

	map<const LAYER_TAG, Layer*>* EntireLayer = gameInstance->GetEntireObjectLayer();

	// 레이어 사이즈도 저장해야할듯?
	uint32 levelIndex;
	file->Read<uint32>(levelIndex);

	uint32 totalLayerSize;
	file->Read<uint32>(totalLayerSize);

	for (uint32 i = 0; i < totalLayerSize; ++i)
	{
		uint32 LayerTagType;
		file->Read<uint32>(LayerTagType);

		//LAYER_PLAYER, LAYER_MONSTER, LAYER_TERRAIN, LAYER_CAMERA, LAYER_ENVIRONMENT, LAYER_END,

		uint32 GameObjectListSize;
		file->Read<uint32>(GameObjectListSize);

		switch (static_cast<LAYER_TAG>(LayerTagType))
		{
		case Engine::LAYER_TAG::LAYER_PLAYER:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				//TODO
			}
			break;
		case Engine::LAYER_TAG::LAYER_TERRAIN:
			break;
		case Engine::LAYER_TAG::LAYER_CAMERA:
			break;
		case Engine::LAYER_TAG::LAYER_STATIC:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				// 모델 타입.
				uint32 modelType;
				file->Read<uint32>(modelType);


				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames StaticComponentName;

				// 모델 이름.
				string modelName;
				file->Read(modelName);
				StaticComponentName._strModelName = modelName;

				uint32 modelID;
				file->Read<uint32>(modelID);
				StaticComponentName._modelID = modelID;

				string modelComponentName;
				file->Read(modelComponentName);
				StaticComponentName._strModelComponentName = Utils::ToWString(modelComponentName);

				string modelShaderName;
				file->Read(modelShaderName);
				StaticComponentName._strShaderName = Utils::ToWString(modelShaderName);

				string prototypeModelName;
				file->Read(prototypeModelName);
				StaticComponentName._protoTypeName = Utils::ToWString(prototypeModelName);

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				StaticComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), StaticComponentName._protoTypeName, &StaticComponentName)))
					return E_FAIL;

			}
			break;
		case Engine::LAYER_TAG::LAYER_DYNAMIC:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				// 모델 타입.
				uint32 modelType;
				file->Read<uint32>(modelType);


				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames DynamicComponentName;

				// 모델 이름.
				string modelName;
				file->Read(modelName);
				DynamicComponentName._strModelName = modelName;

				uint32 modelID;
				file->Read<uint32>(modelID);
				DynamicComponentName._modelID = modelID;

				string modelComponentName;
				file->Read(modelComponentName);
				DynamicComponentName._strModelComponentName = Utils::ToWString(modelComponentName);

				string modelShaderName;
				file->Read(modelShaderName);
				DynamicComponentName._strShaderName = Utils::ToWString(modelShaderName);

				string prototypeModelName;
				file->Read(prototypeModelName);
				DynamicComponentName._protoTypeName = Utils::ToWString(prototypeModelName);

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				DynamicComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), DynamicComponentName._protoTypeName, &DynamicComponentName)))
					return E_FAIL;

			}
			break;
		case Engine::LAYER_TAG::LAYER_PUZZLE:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				// 모델 타입.
				uint32 modelType;
				file->Read<uint32>(modelType);


				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames PuzzleComponentName;

				// 모델 이름.
				string modelName;
				file->Read(modelName);
				PuzzleComponentName._strModelName = modelName;

				uint32 modelID;
				file->Read<uint32>(modelID);
				PuzzleComponentName._modelID = modelID;

				string modelComponentName;
				file->Read(modelComponentName);
				PuzzleComponentName._strModelComponentName = Utils::ToWString(modelComponentName);

				string modelShaderName;
				file->Read(modelShaderName);
				PuzzleComponentName._strShaderName = Utils::ToWString(modelShaderName);

				string prototypeModelName;
				file->Read(prototypeModelName);
				PuzzleComponentName._protoTypeName = Utils::ToWString(prototypeModelName);

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				PuzzleComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), PuzzleComponentName._protoTypeName, &PuzzleComponentName)))
					return E_FAIL;

			}
			break;
		case LAYER_TAG::LAYER_ENVIRONMENT:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				// 모델 타입.
				uint32 modelType;
				file->Read<uint32>(modelType);


				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames environmentComponentName;

				// 모델 이름.
				string modelName;
				file->Read(modelName);
				environmentComponentName._strModelName = modelName;

				uint32 modelID;
				file->Read<uint32>(modelID);
				environmentComponentName._modelID = modelID;

				string modelComponentName;
				file->Read(modelComponentName);
				environmentComponentName._strModelComponentName = Utils::ToWString(modelComponentName);

				string modelShaderName;
				file->Read(modelShaderName);
				environmentComponentName._strShaderName = Utils::ToWString(modelShaderName);

				string prototypeModelName;
				file->Read(prototypeModelName);
				environmentComponentName._protoTypeName = Utils::ToWString(prototypeModelName);

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				environmentComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), environmentComponentName._protoTypeName, &environmentComponentName)))
					return E_FAIL;

			}
			break;
		case Engine::LAYER_TAG::LAYER_COLLIDER:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				uint32 modelType;
				file->Read<uint32>(modelType);
				//// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames ColliderComponentName;

				uint32 modelID;
				file->Read<uint32>(modelID);
				ColliderComponentName._modelID = modelID;

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				ColliderComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), TEXT("ProtoTypeDoorCol"), &ColliderComponentName)))
					return E_FAIL;

			}
			break;
		case Engine::LAYER_TAG::LAYER_TRIGER:
			for (uint32 j = 0; j < GameObjectListSize; ++j)
			{
				uint32 modelType;
				file->Read<uint32>(modelType);
				//// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				ComponentNames ColliderComponentName;


				uint32 modelID;
				file->Read<uint32>(modelID);
				ColliderComponentName._modelID = modelID;

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				ColliderComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), static_cast<LAYER_TAG>(LayerTagType), TEXT("ProtoTypeTrigerBox"), &ColliderComponentName)))
					return E_FAIL;
			}
			break;
		case Engine::LAYER_TAG::LAYER_END:
			break;
		default:
			break;
		}

	}

	RELEASE_INSTANCE(GameInstance);
	return S_OK;

    return S_OK;
}

HRESULT GameScene::ReadyLandObjects(const LAYER_TAG layerTag)
{
	LandObject::LANDOBJET_DESC LandObjectDesc = {};

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Transform* pTransform = static_cast<Transform*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ComponentTransform"), "2stBottom", 0));
	BinaryNavi* pNavi = static_cast<BinaryNavi*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ComponentNavigation"), "2stBottom", 0));

	// Navigation이 만들어질 때. 버퍼의 Cell의 정보를 들고있다면..그냥 pNavi의 버퍼를 가지고오면 될 듯..?

	vector<Cell*>& vecCells = pNavi->GetCell();

	LandObjectDesc.pCells = &vecCells;
	LandObjectDesc.pCellTransform = pTransform;

	RELEASE_INSTANCE(GameInstance);


	if (FAILED(pGameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_PLAYER, TEXT("ProtoTypeGameObjectPlayer"), &LandObjectDesc)))
		return E_FAIL;

	ReadyLayerMonster(LAYER_TAG::LAYER_MONSTER, &LandObjectDesc);

	return S_OK;
}

HRESULT GameScene::ReadyLayerTerrain(const LAYER_TAG layerTag)
{

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	ComponentNames comNames;

	comNames._protoTypeName = TEXT("ProtoTypeStaticObject");
	comNames._strModelComponentName = TEXT("ProtoType2stBottom");
	comNames._strShaderName = TEXT("ProtoTypeComponentDefaultMeshShader");
	comNames._strModelName = "2stBottom";
	Matrix ScaleMatrix;
	comNames._saveWorldMatrix *= ScaleMatrix.CreateScale(Vec3(0.1, 0.1f, 0.1f));



	if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), layerTag, TEXT("ProtoTypeStaticObject"), &comNames)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
	return S_OK;

}

HRESULT GameScene::ReadyLayerCamera(const LAYER_TAG layerTag)
{
    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    PlayerCamera::PLAYERCAMERA_DESC cameraDesc;
    ZeroMemory(&cameraDesc, sizeof(cameraDesc));
    {
        cameraDesc._mouseSensitive = 0.1f;
        cameraDesc._eye = Vec4(0.f, 10.f, -8.f, 1.f);
        cameraDesc._at = Vec4(0.f, 0.f, 0.f, 1.f);
        cameraDesc._fovy = ::XMConvertToRadians(60.f);
        cameraDesc._aspect = g_iWinSizeX / static_cast<_float>(g_iWinSizeY);
        cameraDesc._near = 0.2f;
        cameraDesc._far = 300.f;
        cameraDesc.speedPerSec = 5.f;
        cameraDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);
    }

    if(FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeGameObjectCamera"),
        &cameraDesc)))
        return E_FAIL;

    Safe_Release<GameInstance*>(gameInstance);

    return S_OK;
}

HRESULT GameScene::ReadyLayerPlayer(const LAYER_TAG layerTag, void* pArg)
{

    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeGameObjectPlayer"), pArg)))
        return E_FAIL;
   
    Safe_Release<GameInstance*>(gameInstance);


    return S_OK;
}

HRESULT GameScene::ReadyLayerMonster(const LAYER_TAG layerTag, void* pArg)
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeDanceMonster"), pArg)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

HRESULT GameScene::ReadyLayerParticle(const LAYER_TAG layerTag)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(pGameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeFireParticle"))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT GameScene::ReadyLight()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring finalPath = L"..\\Binaries\\Data\\Light.dat";

	file->Open(finalPath, FileMode::Read);


	uint32 iLightSize;
	file->Read<uint32>(iLightSize);

	for (uint32 i = 0; i < iLightSize; ++i)
	{
		LIGHT_DESC lightDesc;

		uint32 iLightType;
		file->Read<uint32>(iLightType);

		lightDesc.type = iLightType;

		switch (lightDesc.type)
		{
		case LIGHT_DESC::TYPE::DIRECTION:
			file->Read<Vec4>(lightDesc.Position);
			file->Read<Vec4>(lightDesc.Diffuse);
			file->Read<Vec4>(lightDesc.vAmbientLowerColor);
			file->Read<Vec4>(lightDesc.vAmbientUpperColor);
			file->Read<Vec4>(lightDesc.Specular);
			file->Read<Vec3>(lightDesc.Direction);
			file->Read<_float>(lightDesc.fSpecIntensity);
			file->Read<_float>(lightDesc.fSpecExp);
			file->Read<_bool>(lightDesc.PlayerLight);
			file->Read<_bool>(lightDesc.bEnable);
			file->Read<_bool>(lightDesc.bSelect);
			file->Read<_bool>(lightDesc.MonsterLight);
			break;
		case LIGHT_DESC::TYPE::POINT:
			file->Read<Vec4>(lightDesc.Position);
			file->Read<Vec4>(lightDesc.Diffuse);
			file->Read<Vec4>(lightDesc.vAmbientLowerColor);
			file->Read<Vec4>(lightDesc.vAmbientUpperColor);
			file->Read<Vec4>(lightDesc.Specular);
			file->Read<Vec3>(lightDesc.Direction);
			file->Read<_float>(lightDesc.fSpecIntensity);
			file->Read<_float>(lightDesc.fSpecExp);
			file->Read<_float>(lightDesc.pointLightRangeRcp);
			file->Read<_bool>(lightDesc.PlayerLight);
			file->Read<_bool>(lightDesc.bEnable);
			file->Read<_bool>(lightDesc.bSelect);
			file->Read<_bool>(lightDesc.MonsterLight);
			break;
		case LIGHT_DESC::TYPE::SPOT:
			file->Read<Vec4>(lightDesc.Position);
			file->Read<Vec4>(lightDesc.Diffuse);
			file->Read<Vec4>(lightDesc.vAmbientLowerColor);
			file->Read<Vec4>(lightDesc.vAmbientUpperColor);
			file->Read<Vec4>(lightDesc.Specular);
			file->Read<Vec3>(lightDesc.Direction);
			file->Read<_float>(lightDesc.fSpecIntensity);
			file->Read<_float>(lightDesc.fSpecExp);
			file->Read<_float>(lightDesc.fSpotLightRangeRcp);
			file->Read<_float>(lightDesc.fSpotCosOuterCone);
			file->Read<_float>(lightDesc.fSpotInnerConeRcp);
			file->Read<_bool>(lightDesc.PlayerLight);
			file->Read<_bool>(lightDesc.bEnable);
			file->Read<_bool>(lightDesc.bSelect);
			file->Read<_bool>(lightDesc.MonsterLight);
			break;
		}

		gameInstance->AddLight(lightDesc);
	}


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT GameScene::ReadyUI(const LAYER_TAG layerTag)
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeRouteUI"))))
		return E_FAIL;

	if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeBlood1UI"))))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

GameScene* GameScene::Create(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    GameScene* scene = new GameScene(_device, _deviceContext);

    if (FAILED(scene->Initialize()))
    {
        MSG_BOX("Failed Create GameScene");
        Safe_Release<GameScene*>(scene);
        return nullptr;
    }

    return scene;
}

void GameScene::Free()
{
    __super::Free();
}
