#include "pch.h"

#include "imgui.h"
#include "imfilebrowser.h"
#include "imgui_impl_win32.h"  // IMGUI::SETUP
#include "imgui_impl_dx11.h"  // IMGUI::SETUP

#include "ImGuiManager.h"
#include "ImguiResourceHandler.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "EditorTerrain.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"
#include "Layer.h"
#include "Player.h"
#include "StaticObject.h"
#include "BinaryMesh.h"
#include "Cell.h"
#include "DynamicObject.h"

ImGui::FileBrowser g_fileDialog;

IMPLEMENT_SINGLETON(ImGuiManager)

ImGuiManager::ImGuiManager()
{

}

HRESULT ImGuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;

	g_fileDialog.SetTitle("Load Texture");
	g_fileDialog.SetTypeFilters({ ".png",".jpg",".jpeg",".tga",".dds",".DDS",".bmp" });


	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);

	_currentDirectoryName = _rootModelDirection;
	_currentDirectoryPath = _rootModelDirection;


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(device, deviceContext);

	GuiStyle();
	LoadModelList();

	return S_OK;
}

void ImGuiManager::Tick(const _float& timeDelta)
{
}

void ImGuiManager::LateTick(const _float& timeDelta)
{

}


HRESULT ImGuiManager::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuiStyle& imguiStyle = ImGui::GetStyle();

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);


	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_None;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_NoMove;

	{
		MainSection();
#ifdef _DEBUG
		if (_pNaviGation != nullptr)
		{
			if (false == _pNaviGation->GetCell().empty())
				_pNaviGation->Render();
		}
#endif // _DEBUG
	}

	 // Model Card Window
	{
		if (FAILED(ModelNameCardSection()))
			return E_FAIL;
	}


	/* Objects Space */
	{
		if (FAILED(ObjectsSection()))
			return E_FAIL;
	}


	/* Light Space */
	{
		if (FAILED(MainLightSection()))
			return E_FAIL;
	}

	/* Terrain */

	ImGui::Dummy(ImVec2(0.f, _splitterButtonPadding / 2.f));
	ImGui::Indent(5.f);
	ImGui::Text("Terrain");
	ImGui::Unindent(5.f);
	ImGui::Dummy(ImVec2(0.f, _splitterButtonPadding / 2.f));


	ImGui::BeginChild("Terrains", ImVec2(ImGui::GetWindowSize().x -
		(imguiStyle.ScrollbarSize / 1.5f), _TerrainSectionHeight), true);

	if (ImGui::CollapsingHeader("Terrain Panel", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// TODO
		static int i1 = 0;
		ImGui::SeparatorText("Terrain Width or Height");
		ImGui::DragInt("Terrain Width", &i1, 4, 0, INT_MAX);
		ImGui::DragInt("Terrain Height", &i1, 4, 0, INT_MAX);


		ImGui::Spacing();

		ImGui::SeparatorText("Terrain Create or Delete");

		if (ImGui::Button("Create"))
		{
			desc.numVerticesX = i1;
			desc.numVerticesZ = i1;

			if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), LAYER_TAG::LAYER_TERRAIN, TEXT("ProtoTypeGameObjectEditTerrain"), &desc)))
				return E_FAIL;

		
			_Isterrain = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{

		}

	}
	ImGui::EndChild();

	ImGui::End();
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void ImGuiManager::GuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.FrameBorderSize = 0.f;
	style.ChildBorderSize = 2.f;
	style.TabBorderSize = 0.f;
	style.PopupBorderSize = 0.f;

	style.WindowPadding = ImVec2(5, 5);
	style.WindowRounding = 2.f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 2.f;
	style.ItemSpacing = ImVec2(5, 5);
	style.ItemInnerSpacing = ImVec2(5, 5);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.f, 0.f, 0.1f, 0.5f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.1f, 0.3f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.15f, 0.2f, 1.f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.225f, 0.4f, 1.f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.05f, 0.075f, 0.1f, 1.f);

	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.f, 0.f, 0.f, 1.f);

	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.f, 0.f, 0.1f, 0.2f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.6f, 0.9f, 1.f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.f, 1.f / 255.f * 150.f, 1.f, 1.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.f, 1.f / 255.f * 130.f, 1.f, 1.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 1.f / 255.f * 130.f, 1.f, 0.6f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.225f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.05f, 0.075f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(.8f, .8f, .8f, 1.f);

}

void ImGuiManager::DrawSplitter(_bool split_vertically, _float thickness, _float* size0, _float* size1, _float min_size0, _float min_size1, _float size, _float buttonPadding)
{
	ImVec2 backup_pos = ImGui::GetCursorPos();
	if (split_vertically)
		ImGui::SetCursorPosY(backup_pos.y + *size0 + buttonPadding);
	else
		ImGui::SetCursorPosX(backup_pos.x + *size0 + buttonPadding);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.29f, 0.478f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // We don't draw while active/pressed because as we move the panes the splitter button will be 1 frame late
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.8f, 0.60f));
	ImGui::Button("##Splitter", ImVec2(!split_vertically ? thickness : size, split_vertically ? thickness : size));
	ImGui::PopStyleColor(3);

	ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

	if (ImGui::IsItemActive())
	{
		float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;

		// Minimum pane size
		if (mouse_delta < min_size0 - *size0)
			mouse_delta = min_size0 - *size0;
		if (mouse_delta > *size1 - min_size1)
			mouse_delta = *size1 - min_size1;

		// Apply resize
		*size0 += mouse_delta;
		*size1 -= mouse_delta;
	}
	ImGui::SetCursorPos(backup_pos);
}

void ImGuiManager::MouseMove()
{
	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);
	//::SetCursorPos(pt.x, pt.y);

	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_NoResize;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_NoMove;

	GameInstance* gameinstance = GET_INSTANCE(GameInstance);

	ImGui::Begin("Mouse Pose", NULL, windowFlags);
	ImGui::Text("Mouse Position X(%d), Y(%d)", pt.x, pt.y);

	Vec4 pos = Vec4(0.f, 0.f, 0.f, 1.f);
	_float distance = 0.f;

	static _bool IsTerrain = false;
	static _bool IsObjectPick = false;
	static _bool IsNavigation = false;

	_bool IsIntersects = false;

	if (ImGui::Button("Terrain Picking", ImVec2(140, 20)))
	{
		if (IsTerrain == true)
			IsTerrain = false;
		else
			IsTerrain = true;
	}

	ImGui::SameLine(150);

	if (ImGui::Button("Object Picking", ImVec2(140, 20)))
	{
		if (IsObjectPick == true)
			IsObjectPick = false;
		else
			IsObjectPick = true;
	}


	ImGui::SameLine(300);

	if (ImGui::Button("Navigation", ImVec2(135, 20)))
	{
		if (IsNavigation == true)
			IsNavigation = false;
		else
			IsNavigation = true;
	}


	ImGui::Spacing();


	if (true == IsTerrain)
	{
		if (_Isterrain)
		{
			EditorTerrain* terrain = static_cast<EditorTerrain*>(gameinstance->GetLayerObject(LAYER_TAG::LAYER_TERRAIN, OBJECT_TYPE::TERRAIN));

			if (nullptr == terrain)
				return;

			pos = terrain->TerrainPick();


			for (const auto& Selectiter : _SelectGameObject)
			{
				if (Selectiter.second.first == true)
				{
					if (gameinstance->Get_DIMouseState(DIMK::DIMK_LBUTTON))
						Selectiter.second.second->GetTransform()->SetState(Transform::STATE::POSITION, ::XMLoadFloat4(&pos));
				}
			}
		}


		ImGui::Text("Terrain Pos X : %.f", pos.x);
		ImGui::Spacing();

		ImGui::Text("Terrain Pos Y : %.f", pos.y);
		ImGui::Spacing();

		ImGui::Text("Terrain Pos Z : %.f", pos.z);
		ImGui::Spacing();

		ImGui::Text("Terrain Pos W : %.f", pos.w);
		ImGui::Spacing();
	}


	if (gameinstance->KeyPressing(DIK_P))
	{
		if (true == IsObjectPick)
		{
			map<const LAYER_TAG, Layer*>* EntireLayer = gameinstance->GetEntireObjectLayer();
			int32 LevelIndex = gameinstance->GetCurrentLevelIndex();


			Vec4 ObjectPos = Vec4(0.f, 0.f, 0.f, 1.f);


			for (auto& layeriter : EntireLayer[LevelIndex])
			{
				vector<GameObject*>* layerObjectList = layeriter.second->GetGameObject();

				if (true == IsIntersects)
					break;

				for (auto& pObj : *layerObjectList)
				{
					// TODO
					// picking에 pObj의 모델의 메쉬 넘기기.
					BinaryModel* ObjectMesh = nullptr;
					Transform* trans = nullptr;

					if (pObj->GetObjectType() == OBJECT_TYPE::PLAYER)
					{
						vector<GameObject*>& playerPart = dynamic_cast<Player*>(pObj)->GetPlyaerPart();
						ObjectMesh = playerPart[Player::PART::PART_BODY]->GetBinaryModelComponent();
					}
					else
						ObjectMesh = pObj->GetBinaryModelComponent();


					if (nullptr == ObjectMesh)
						continue;

					if (true == IsIntersects)
						break;



					vector<BinaryMesh*>* MeshList = ObjectMesh->GetMeshes();

					for (auto& meshiter : *MeshList)
					{
						IsIntersects = gameinstance->PickObject(pt, pObj->GetTransform(), meshiter, ObjectPos);

						if (true == IsIntersects)
							break;
					}

				}
			}


			ImGui::Text("Object Pos X : %.2f", ObjectPos.x);
			ImGui::Spacing();
			ImGui::Text("Object Pos Y : %.2f", ObjectPos.y);
			ImGui::Spacing();
			ImGui::Text("Object Pos Z : %.2f", ObjectPos.z);
			ImGui::Spacing();
			ImGui::Text("Object Pos W : %.2f", ObjectPos.w);
			ImGui::Spacing();



			for (const auto& Selectiter : _SelectGameObject)
			{
				if (Selectiter.second.first == true)
				{
					if (gameinstance->Get_DIMouseState(DIMK::DIMK_RBUTTON))
						Selectiter.second.second->GetTransform()->SetState(Transform::STATE::POSITION, ::XMLoadFloat4(&ObjectPos));
				}
			}
		}
	}


	if (true == IsNavigation)
	{

		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\previous_folder_icon.png"), ImVec2(20, 20)))
		{
			vector<Cell*>& NaviCells = _pNaviGation->GetCell();

			if (false == NaviCells.empty())
			{
				Safe_Release<Cell*>(NaviCells.back());
				NaviCells.pop_back();
			}
		}

		map<const LAYER_TAG, Layer*>* EntireLayer = gameinstance->GetEntireObjectLayer();
		int32 LevelIndex = gameinstance->GetCurrentLevelIndex();

		Vec4 NavigationPos = Vec4(0.f, 0.f, 0.f, 1.f);

		for (auto& layeriter : EntireLayer[LevelIndex])
		{
			vector<GameObject*>* layerObjectList = layeriter.second->GetGameObject();

			if (true == IsIntersects)
				break;

			for (auto& pObj : *layerObjectList)
			{
				// TODO
				// picking에 pObj의 모델의 메쉬 넘기기.
				if (pObj->GetModelName() == "2stBottom" || pObj->GetModelName() == "2stwall" || pObj->GetModelName() == "2stStair" || pObj->GetModelName() == "BasementBottom")
				{
					BinaryModel* ObjectMesh = pObj->GetBinaryModelComponent();

					if (true == IsIntersects)
						break;

					vector<BinaryMesh*>* MeshList = ObjectMesh->GetMeshes();

					for (auto& meshiter : *MeshList)
					{
						IsIntersects = gameinstance->PickObject(pt, pObj->GetTransform(), meshiter, NavigationPos);

						if (true == IsIntersects)
							break;
					}
				}
			}
		}


		ImGui::Text("NavigationPos Pos X : %.2f", NavigationPos.x);
		ImGui::Spacing();
		ImGui::Text("NavigationPos Pos Y : %.2f", NavigationPos.y);
		ImGui::Spacing();
		ImGui::Text("NavigationPos Pos Z : %.2f", NavigationPos.z);
		ImGui::Spacing();
		ImGui::Text("NavigationPos Pos W : %.2f", NavigationPos.w);
		ImGui::Spacing();

		if (_iPointCount > 2)
		{

			SortPoints();

			_pNaviGation->SetUp_Neighbors();
			Cell* pCell = Cell::Create(_device, _deviceContext, _vPoints, _pNaviGation->GetCell().size());
			if (pCell == nullptr)
			{
				MSG_BOX("Create To Failed Cell");
				return;
			}
			_pNaviGation->GetCell().push_back(pCell);

			_iPointCount = 0;
		}


		if (gameinstance->mouseDown(DIMK::DIMK_RBUTTON))
		{
			_vPoints[_iPointCount] = Vec3(NavigationPos.x, NavigationPos.y, NavigationPos.z);

			vector<Cell*>& NaviCells = _pNaviGation->GetCell();

			if (!NaviCells.empty())
			{
				for (uint32 i = 0; i < NaviCells.size(); ++i)
				{
					const Vec3* vPointA = NaviCells[i]->GetPoint(Cell::POINT_A);
					const Vec3* vPointB = NaviCells[i]->GetPoint(Cell::POINT_B);
					const Vec3* vPointC = NaviCells[i]->GetPoint(Cell::POINT_C);

					_float distanceToA = DistanceBetewwinPoints(_vPoints[_iPointCount], *vPointA);
					_float distanceToB = DistanceBetewwinPoints(_vPoints[_iPointCount], *vPointB);
					_float distanceToC = DistanceBetewwinPoints(_vPoints[_iPointCount], *vPointC);

					if (distanceToA < 0.2f)
						_vPoints[_iPointCount] = *vPointA;
					else if (distanceToB < 0.2f)
						_vPoints[_iPointCount] = *vPointB;
					else if (distanceToC < 0.2f)
						_vPoints[_iPointCount] = *vPointC;
				}
			}

			++_iPointCount;
		}
	}
	RELEASE_INSTANCE(GameInstance);

	ImGui::End();
}

void ImGuiManager::MainSection()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);


	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_NoResize;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_NoMove;

	{
		MouseMove();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.05f, 0.1f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(200.f, 0.f));
		ImGui::Begin("Main", NULL, windowFlags | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::SameLine(ImGui::GetWindowWidth() - 28);

		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"\\Icon\\save_white_18dp.png"), ImVec2(20, 20)))
		{
			wstring filepath = L"..\\Binaries\\Resources\\MapData\\";
			SceneSave(filepath);
			
			wstring Navipath = L"..\\Binaries\\Data\\";
			NavigationMeshSave(Navipath);
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 56);
		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\outline_refresh_white_18dp.png"), ImVec2(20, 20)))
		{

		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 84);
		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\free-icon-loading-6356659.png"), ImVec2(20, 20)))
		{
			wstring filepath = L"..\\Binaries\\Resources\\MapData\\";
			SceneLoad(filepath);

			_pNaviGation = dynamic_cast<BinaryNavi*>(gameInstance->GetComponent(static_cast<uint32>(LEVEL::EDIT), LAYER_TAG::LAYER_STATIC,
				TEXT("ComponentNavigation"), "2stBottom", 0));
		}

		ImGui::PopStyleVar();

		if (ImGui::CollapsingHeader("Settings"))
		{
			_bool mode = true;
			ImGui::Checkbox("Wireframe Mode", &mode);

			ImGui::PushItemWidth(-1);
			ImGui::PopItemWidth();
			ImGui::Checkbox("Window Resize", &_windowResizeFlag);
			ImGui::Checkbox("Window Move", &_windowMoveFlag);
			if (ImGui::CollapsingHeader("Camera"))
			{
				LAYER_TAG cameraLayer = LAYER_TAG::LAYER_CAMERA;
				vector<GameObject*>* gamevector = gameInstance->GetCurrentObjectList(cameraLayer);

				int32 objectsize = gameInstance->GetLayerObjectCount();
				for (size_t i = 0; i < objectsize; i++)
					if (gamevector != nullptr)
					{
						GameObject* gameObject = (*gamevector)[i];
						if (gameObject != nullptr)
							ImGui::Text(gameObject->GetModelNameId().c_str());

					}
			}
		}

		ImGui::End();
		ImGui::PopStyleColor();
	}

	RELEASE_INSTANCE(GameInstance);
}

HRESULT ImGuiManager::ModelNameCardSection()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_NoResize;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_NoMove;

	if (_modelNames.size())
	{
		ImGui::Begin("Models", NULL, windowFlags);
		ImGui::Columns(3, 0, false);

		for (size_t i = 0; i < _modelNames.size(); i++)
		{
			if (_modelNameHoveringState[i])
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.7f, 0.9f, 1.f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.15f, 0.2f, 1.f));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
			}


			// Model Card Begin
			ImGui::BeginChildFrame(static_cast<ImGuiID>(i + 1), ImVec2(110, 130));
			if (_modelNames[i].second)
			{

				if (_modelNames[i].first == "..")
					ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\previous_folder_icon.png"), ImVec2(100, 100));
				else
					ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\folder_icon.png"), ImVec2(100, 100));
			}
			else
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\model_icon.png"), ImVec2(100, 100));


			// Name

			if (_modelNames[i].first == "..")
				ImGui::Text("%s", "Back");
			else
				ImGui::Text("%s", _modelNames[i].first.c_str());



			ImGui::EndChildFrame();
			// model card End

			// Hover stryle pop
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// objects
			if (ImGui::IsItemClicked())
			{
				if (false == _modelNames[i].second)
				{
					size_t position = _currentDirectoryPath.find_first_of('\\/') + 1;
					string modelPath = _currentDirectoryPath + _modelNames[i].first;


					Matrix modelInitializMatrix = ::XMMatrixIdentity();
					modelInitializMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));

					// 여기까지

					const pair<LAYER_TAG, wstring>& findPrototypename = ImGuiResourceHandler::GetInstance()->FindProtoFilePath(modelPath);
					//const pair<const wstring, const wstring>& findProtoComName = ImGuiResourceHandler::GetInstance()->FindProtoComponentName(modelPath);


					ComponentNames comNames;
					{
						size_t namePosition = _modelNames[i].first.find_last_of(".");
						string useModelName;

						if (namePosition != std::string::npos && namePosition > 0)
							useModelName = _modelNames[i].first.substr(0, namePosition);

						comNames._protoTypeName = findPrototypename.second;
						//comNames._strModelComponentName = findProtoComName.first;
						//comNames._strShaderName = findProtoComName.second;
						comNames._strModelName = useModelName;
						Matrix ScaleMatrix;
						//comNames._saveWorldMatrix *= ScaleMatrix.CreateScale(Vec3(1.f, 1.f, 1.f));
					}
	


					if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), findPrototypename.first, findPrototypename.second, &comNames)))
					{
						RELEASE_INSTANCE(GameInstance);
						return E_FAIL;
					}

		/*			size_t dotPosition = modelPath.find_last_of(".");
					string fileExtension = "";

					if (dotPosition != std::string::npos && dotPosition > 0)
						fileExtension = modelPath.substr(0, dotPosition);*/

					//BinaryModelSave(modelPath, Utils::ToWString(fileExtension));
					//BinaryAnimModelSave(modelPath, Utils::ToWString(fileExtension));
		
				}
				else
				{
					if (_modelNames[i].first == "..")
					{
						_currentDirectoryPath.erase(_currentDirectoryPath.length() - _currentDirectoryName.length() - 1, _currentDirectoryName.length() + 2);
						size_t position = _currentDirectoryPath.find_last_of("\\/");
						_currentDirectoryName = _currentDirectoryPath.substr(0, position);
					}
					else
					{
						_currentDirectoryName = _modelNames[i].first;
						_currentDirectoryPath += _currentDirectoryName + "\\";
					}
					LoadModelList(_currentDirectoryPath);
					break;
				}
			}
			_modelNameHoveringState[i] = ImGui::IsItemHovered();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}



	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

HRESULT ImGuiManager::ObjectsSection()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_NoResize;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_NoMove;

	ImGuiStyle& imguiStyle = ImGui::GetStyle();


	ImGui::Begin("Objects", NULL, windowFlags);
	ImGui::SetWindowSize(ImVec2(_objectsWindowWidth, _windowContentHeight));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.29f, 0.478f, 0.8f));
	ImGui::BeginChildFrame(99499, ImVec2(ImGui::GetWindowSize().x - 10.f, 25.f));
	ImGui::Text("Game Objects");
	ImGui::EndChildFrame();
	ImGui::PopStyleColor();
	//
	ImGui::PushStyleColor(ImGuiCol_Border, imguiStyle.Colors[ImGuiCol_ChildBg]);
	DrawSplitter(true, _sectionSeperatorHeight, &_gameObjectSectionHeight, &_lightSectionHeight, 61.f, 61.f,
		ImGui::GetWindowSize().x - 10.f, _splitterButtonPadding);
	ImGui::BeginChild("Game Objects", ImVec2(ImGui::GetWindowSize().x - (imguiStyle.ScrollbarSize / 1.5f),
		_gameObjectSectionHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
	{
		// TODO : for문으로 gameobject 순회 
		int32 objectsize = gameInstance->GetLayerObjectCount();

		map<const LAYER_TAG, Layer*>* EntireLayer = gameInstance->GetEntireObjectLayer();
		int32 LevelIndex = gameInstance->GetEntireLevel();

		if (nullptr == EntireLayer)
			return E_FAIL;

		// 전부 순회? 

		for (auto& pair : EntireLayer[static_cast<uint32>(LEVEL::EDIT)])
		{
			if (pair.second == nullptr)
				continue;

			vector<GameObject*>* gamevector = pair.second->GetGameObject();
			if (nullptr == gamevector)
				continue;

			uint32 objectID = 0;

			for (auto& pObjList : *gamevector)
			{
				if (pObjList->GetObjectType() == OBJECT_TYPE::CAMERA)
					continue;

				objectID++;
				if (objectID > 1)
					ImGui::NewLine();

				ImGui::Text(pObjList->GetModelNameId().c_str());


				ImGui::PushID(pObjList->GetIdNumber());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 24);

				if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\baseline_delete_white_18dp.png"), ImVec2(20, 20)))
				{
					int32 a = 0;

					if (pObjList != nullptr)
					{
						if (FAILED(gameInstance->DeleteGameObject(static_cast<uint32>(LEVEL::EDIT), pair.second->GetLayerTag(), pObjList->GetIdNumber(), pObjList->GetModelName())))
							return E_FAIL;
					}


					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::PopStyleVar();


					GameObjectUpdate(objectID, pObjList);
				}

				ImGui::PopID();

			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void ImGuiManager::BinaryModelSave(const string& fpxPath, const wstring& binaryDirectory)
{
	Matrix ModelInitailizeMatrix = Matrix::Identity;
	ModelInitailizeMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));
	Model* pBinaryModel = Model::Create(_device, _deviceContext, Model::MODEL_TYPE::NONE, fpxPath, ModelInitailizeMatrix);

	if (nullptr != pBinaryModel)
	{
		auto path = filesystem::path(binaryDirectory);
		
		filesystem::create_directory(path.parent_path());

		shared_ptr<FileUtils> file = make_shared<FileUtils>();

		wstring finalPath = binaryDirectory + L".dat";

		file->Open(finalPath, FileMode::Write);

		uint32 iType = static_cast<uint32>(pBinaryModel->GetModelType());
		file->Write<uint32>(iType);
		// 모델 타입 저장.

		Matrix pivotMatrix = pBinaryModel->GetPivotMatrix();
		file->Write<Matrix>(pivotMatrix);
		// 모텔 사전 행렬 저장

		uint32 NumMeshes = pBinaryModel->GetNumMeshes();
		file->Write<uint32>(NumMeshes);
		// 메쉬 개수 -> 나중에 이 메쉬만큼 돌아야함.

		vector<Mesh*>* vecMesh = pBinaryModel->GetMeshes();
		for (auto& Meshiter : *vecMesh)
		{
			string MeshName = Meshiter->GetMeshName();
			file->Write<string>(MeshName);

			// 메쉬를 구성하고있는 버퍼를 어떤 형식으로 읽을건지 메인 BufferDesc
			VIBuffer::BUFFER_DESC viBufferDesc = *Meshiter->GetBufferDesc();
			file->Write<VIBuffer::BUFFER_DESC>(viBufferDesc);

			// 그리고 그 형식으로 이루어진 Mesh Desc
			Mesh::MESH_BUFFER_DESC meshBufferDesc = *Meshiter->GetMeshBufferDesc();
			file->Write<Mesh::MESH_BUFFER_DESC>(meshBufferDesc);

			uint32 meshIndex = Meshiter->GetMaterialIndex();
			file->Write<uint32>(meshIndex);

			
			uint32 Numvertices = Meshiter->GetBufferDesc()->_numvertices;
			file->Write<uint32>(Numvertices);

			VTXMESH* pVertices = Meshiter->GetVertexMeshBuffer();

			for (uint32 i = 0; i < Numvertices; i++)
			{
				file->Write<Vec3>(pVertices[i].position);
				file->Write<Vec3>(pVertices[i].normal);
				file->Write<Vec2>(pVertices[i].texcoord);
				file->Write<Vec3>(pVertices[i].tangent);
				file->Write<Vec3>(pVertices[i].bitangent);
			}
			// 그 정보로 구성된 버텍스로 이루어진 인덱스 
			_ulong* pIndices = Meshiter->GetIndicesMeshBuffer();

			const uint32 IndicesCount = Meshiter->GetBufferDesc()->_numIndices / 3;
			file->Write<uint32>(IndicesCount);

			uint32 numIndices = 0;

			for (uint32 i = 0; i < IndicesCount; ++i)
			{
				file->Write<_ulong>(pIndices[numIndices++]);
				file->Write<_ulong>(pIndices[numIndices++]);
				file->Write<_ulong>(pIndices[numIndices++]);
			}

		}

		// 마테리얼 개수 
		uint32 NumMaterialCount = pBinaryModel->GetMaterialCount();
		file->Write<uint32>(NumMaterialCount);

		// 이제 개수만큼 마테리얼 돌면서 텍스쳐 입히면 됨.
		vector<MESH_MATERIAL>* vecMaterial = pBinaryModel->GetMaterial();

		for (auto& Materialiter : *vecMaterial)
		{
			uint32 textureCount = Materialiter.textureCount;
			file->Write<uint32>(textureCount);

			for (uint32 i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			{
				if (Materialiter._texture[i] == nullptr)
					continue;

				// 텍스쳐의 타입
				uint32 textureType = i;
				file->Write<uint32>(textureType);
				
				wstring texturePath = Materialiter._texture[i]->GetTexturePath();
				file->Write<string>(Utils::ToString(texturePath));
			}
		}
	}
	Safe_Release<Model*>(pBinaryModel);

}

void ImGuiManager::BinaryAnimModelSave(const string& fpxPath, const wstring& binaryDirectory)
{
	Matrix ModelInitailizeMatrix = Matrix::Identity;
	ModelInitailizeMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));
	Model* pBinaryModel = Model::Create(_device, _deviceContext, Model::MODEL_TYPE::ANIM, fpxPath, ModelInitailizeMatrix);

	if (nullptr != pBinaryModel)
	{
		auto path = filesystem::path(binaryDirectory);

		filesystem::create_directory(path.parent_path());

		shared_ptr<FileUtils> file = make_shared<FileUtils>();

		wstring finalPath = binaryDirectory + L".dat";

		file->Open(finalPath, FileMode::Write);

		uint32 iType = static_cast<uint32>(pBinaryModel->GetModelType());
		file->Write<uint32>(iType);
		// 모델 타입 저장.

		//Matrix pivotMatrix = pBinaryModel->GetPivotMatrix();
		//file->Write<Matrix>(pivotMatrix);
		// 모텔 사전 행렬 저장

		// -> 애니메이션 모델은 사전행렬 설정하면 안 됨.


		// 본을 먼저 세팅.
		vector<Bone*> Bones = pBinaryModel->GetBones();
		file->Write<uint32>(Bones.size());
		for (auto& Boneiter : Bones)
		{
			// 뼈의 이름을 가지고온다.
			const _char* boneName = Boneiter->GetBoneName();
			file->Write<string>(boneName);

			// 본의 정보를 들고 있는 구조체.
			Bone::BONE_DESC boneDesc = Boneiter->GetBoneDesc();
			file->Write<Bone::BONE_DESC>(boneDesc);

		}



		uint32 NumMeshes = pBinaryModel->GetNumMeshes();
		file->Write<uint32>(NumMeshes);
		// 메쉬 개수 -> 나중에 이 메쉬만큼 돌아야함.

		vector<Mesh*>* vecMesh = pBinaryModel->GetMeshes();
		for (auto& Meshiter : *vecMesh)
		{
			string MeshName = Meshiter->GetMeshName();
			file->Write<string>(MeshName);

			// 메쉬를 구성하고있는 버퍼를 어떤 형식으로 읽을건지 메인 BufferDesc
			VIBuffer::BUFFER_DESC viBufferDesc = *Meshiter->GetBufferDesc();
			file->Write<VIBuffer::BUFFER_DESC>(viBufferDesc);

			// 그리고 그 형식으로 이루어진 Mesh Desc
			Mesh::MESH_BUFFER_DESC meshBufferDesc = *Meshiter->GetMeshBufferDesc();
			file->Write<Mesh::MESH_BUFFER_DESC>(meshBufferDesc);

			// 메쉬의 마테리얼 인덱스.
			uint32 meshIndex = Meshiter->GetMaterialIndex();
			file->Write<uint32>(meshIndex);

			// 애니메이션 메쉬의 버텍스 개수.
			uint32 Numvertices = Meshiter->GetBufferDesc()->_numvertices;
			file->Write<uint32>(Numvertices);

			// 애니메이션 버텍스를 가지고온다.
			VTXANIMMESH* pVertices = Meshiter->GetVertexAnimBuffer();

			for (uint32 i = 0; i < Numvertices; i++)
			{
				file->Write<Vec3>(pVertices[i].position);
				file->Write<Vec3>(pVertices[i].normal);
				file->Write<Vec2>(pVertices[i].texcoord);
				file->Write<Vec3>(pVertices[i].tangent);
				file->Write<Vec3>(pVertices[i].bitangent);
				file->Write<XMUINT4>(pVertices[i].blendIndices);
				file->Write<Vec4>(pVertices[i].blendWeights);
			}


			// 그 메쉬에 영향을 주는 뼈의 개수.
			uint32 effectBones = Meshiter->GetMeshEffectBones();
			file->Write<uint32>(effectBones);

			// 공통으로 사용되는 뼈대들을 보정하기 위한 매트릭스.
			vector<Matrix> vectorOffsetMatrix = Meshiter->vecGetOffsetMatrices();

			// 본 뼈 이름를 갖고있는 벡터를 가지고 온다.
			vector<string> boneNames = Meshiter->vecGetBoneName();

			// 메쉬의 영향을 주는 뼈의 개수만틈 오프셋과 몇 번 뼈의 인덱스인 지 확인.
			for (uint32 i = 0; i < effectBones; ++i)
			{
				// 영향을 받는 본의 개수만큼 오프셋매트릭스를 읽어들인다.
				Matrix BonesOffsetMatrix = vectorOffsetMatrix[i];
				file->Write<Matrix>(BonesOffsetMatrix);

				// 뼈 이름 정보를 저장해서 본 인덱스 몇 번 뼈의 인덱스를 확인한다.
				string szBoneName = boneNames[i];
				file->Write<string>(szBoneName);
			}



			// 그 정보로 구성된 버텍스로 이루어진 인덱스 
			_ulong* pIndices = Meshiter->GetIndicesMeshBuffer();

			const uint32 IndicesCount = Meshiter->GetBufferDesc()->_numIndices / 3;
			file->Write<uint32>(IndicesCount);

			uint32 numIndices = 0;

			for (uint32 i = 0; i < IndicesCount; ++i)
			{
				file->Write<_ulong>(pIndices[numIndices++]);
				file->Write<_ulong>(pIndices[numIndices++]);
				file->Write<_ulong>(pIndices[numIndices++]);
			}

		}

		// 마테리얼 개수 
		uint32 NumMaterialCount = pBinaryModel->GetMaterialCount();
		file->Write<uint32>(NumMaterialCount);

		// 이제 개수만큼 마테리얼 돌면서 텍스쳐 입히면 됨.
		vector<MESH_MATERIAL>* vecMaterial = pBinaryModel->GetMaterial();

		for (auto& Materialiter : *vecMaterial)
		{
			uint32 textureCount = Materialiter.textureCount;
			file->Write<uint32>(textureCount);

			for (uint32 i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			{
				if (Materialiter._texture[i] == nullptr)
					continue;

				// 텍스쳐의 타입
				uint32 textureType = i;
				file->Write<uint32>(textureType);

				wstring texturePath = Materialiter._texture[i]->GetTexturePath();
				file->Write<string>(Utils::ToString(texturePath));
			}
		}



		// Model Animation

		uint32 animationCount = pBinaryModel->GetNumAnimations();
		file->Write<uint32>(animationCount);

		vector<Animation*> modelAnimation = pBinaryModel->GetAnimation();
		for (auto& animiter : modelAnimation)
		{
			// pAIAniamtion이 담고있는 정보를 들고 있는 구조체.
			Animation::ANIMATION_DESC animDuration = animiter->GetAnimationDesc();
			file->Write<Animation::ANIMATION_DESC>(animDuration);
			// CurrenKeyFrame 사이즈는 어차피 디스크립션의 NumChannels임.

			_char* animationName = animiter->GetAnimationBoneName();
			file->Write<string>(animationName);

			vector<Channel*>& animationChannel = animiter->GetChannels();
			file->Write<uint32>(animationChannel.size());
			// 이 애니메이션 당 들고있는 채널 개수.
			for (auto& pChannel : animationChannel)
			{
				// 그 채널이 사용하는 뼈의 이름.
				string szName = pChannel->GetChannelName();
				file->Write<string>(szName);


				// 이 채널의 pAIChannel한테 넘겨받은 정보를 담은 구조체 
				// 돌 때는 numChannel 만큼 순회.
				Channel::CHANNEL_DESC channelDesc = pChannel->GetChannelDesc();
				file->Write<Channel::CHANNEL_DESC>(channelDesc);

				vector<KEYFRAME> keyFrame = pChannel->GetKeyFrame();
				file->Write<uint32>(keyFrame.size());
				for (auto& pKeyFrame : keyFrame)
				{

					// 벡터에 들어가있는 사이즈만큼 읽어 들인다.
					Vec3 Scale = pKeyFrame.scale;
					file->Write<Vec3>(Scale);

					Vec4 Rotation = pKeyFrame.rotation;
					file->Write<Vec4>(Rotation);

					Vec4 Translation = pKeyFrame.translation;
					file->Write<Vec4>(Translation);

					_float time = pKeyFrame.time;
					file->Write<_float>(time);
				}

			}
		}
		
	}



	Safe_Release<Model*>(pBinaryModel);
}


void ImGuiManager::NavigationMeshSave(const wstring& binaryDirectory)
{
	auto path = filesystem::path(binaryDirectory);

	filesystem::create_directory(path.parent_path());

	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring finalPath = binaryDirectory + L"Navigation.dat";

	file->Open(finalPath, FileMode::Write);

	vector<Cell*>& Cells = _pNaviGation->GetCell();

	uint32 Cellsize = Cells.size();
	file->Write<uint32>(Cellsize);

	Vec3 vPoint;

	for (auto& pCell : Cells)
	{
		// 셀의 A,B,C 선분 순서대로 저장.
		// 셀의 사이즈가 곧. 이웃을 결정한다.
		if (nullptr == pCell)
			continue;

		for (uint32 i = 0; i < Cell::POINTS::POINT_END; ++i)
		{
			vPoint = *pCell->GetPoint(static_cast<Cell::POINTS>(i));
			file->Write<Vec3>(vPoint);
		}
	}
}

void ImGuiManager::AddLightSection()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);


	for (size_t i = 0; i < 3; i++)
	{
		if (ImGui::Selectable(LightTypeName[i], false))
		{
			// new Light
			LIGHT_DESC lightDesc;
			LightHelper newLightHelper;

			if (LightTypeName[i] == "Directional")
			{

				::ZeroMemory(&lightDesc, sizeof(lightDesc));
				{
					lightDesc.Position = Vec4(0.f, 5.f, 0.f, 1.f);
					lightDesc.Diffuse = Vec3(1.f, 1.f, 1.f);
					lightDesc.intensity = 1.f;
					lightDesc.range = 4.f;
					lightDesc.type = LIGHT_DESC::DIRECTION;
					lightDesc.enabled = true;

					lightDesc.Direction = Vec3(1.f, -1.f, 1.f);
					lightDesc.Ambient = Vec4(1.f, 1.f, 1.f, 1.f);
					lightDesc.Specular = Vec4(1.f, 1.f, 1.f, 1.f);
				}
			}


			gameInstance->AddLight(lightDesc);
		}
	}

	RELEASE_INSTANCE(GameInstance);
}

HRESULT ImGuiManager::MainLightSection()
{

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	ImGuiStyle& imguiStyle = ImGui::GetStyle();

	vector<OtherLight*>* lightvector = gameInstance->getLightList();
	if (nullptr == lightvector)
		return S_OK;	


	ImGui::Dummy(ImVec2(0.0f, _splitterButtonPadding / 2.f));
	ImGui::Indent(5.f);
	ImGui::Text("Lighting");
	ImGui::Unindent(5.f);
	ImGui::Dummy(ImVec2(0.f, _splitterButtonPadding / 2.f));

	//DrawSplitter(true, _sectionSeperatorHeight, &_lightSectionHeight, &_TerrainSectionHeight, 61.f, 61.f,
	//	ImGui::GetWindowSize().x - 10.f, _splitterButtonPadding);
	ImGui::BeginChild("Lights", ImVec2(ImGui::GetWindowSize().x -
		(imguiStyle.ScrollbarSize / 1.5f), _lightSectionHeight), true);

	if (ImGui::CollapsingHeader("Lights Panel", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("LightList");
		ImGui::SameLine(ImGui::GetWindowWidth() - 123.f);
		ImGui::PushItemWidth(100.f);

		if (ImGui::BeginCombo("##Add Light", "Add Light"))
		{
			// TODO Light Add
			AddLightSection();

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::Separator();

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4f, 0.4f, 0.5f, 1.f));
		ImGui::Columns(2, "LightSettings", false);
		static _float colWidth = 105.f;
		ImGui::SetColumnWidth(0, colWidth);

		// TODO Light 

		uint32 lightsize = lightvector->size();

		for (size_t i = 0; i < lightsize; i++)
		{
			ImGui::Dummy(ImVec2(0.f, 3.f));

			string lightname;

			if ((*lightvector)[i] != nullptr)
			{
				if ((*lightvector)[i]->GetLightDesc()->type == LIGHT_DESC::DIRECTION)
					lightname = "Direction";
				else if ((*lightvector)[i]->GetLightDesc()->type == LIGHT_DESC::POINT)
					lightname = "Point";
				else
					lightname = "Spot";

				ImGui::Text((to_string(i) + ", " + lightname).c_str());
				ImGui::PushID(string("Light" + to_string(i)).c_str());

				ImGui::NextColumn();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - colWidth - 46);
				if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\baseline_delete_white_18dp.png"), ImVec2(20, 20)))
				{
					if (FAILED(gameInstance->DeleteLight(i, lightname)))
						return E_FAIL;
					else
						--lightsize;

					ImGui::PopStyleVar();
				}
				else
				{
					ImGui::PopStyleVar();

					ImGui::Separator();

					ImGui::NextColumn();
					ImGui::Text("Color");
					ImGui::NextColumn();

					ImGui::PushItemWidth(0.f);
					if (ImGui::ColorEdit3(string("##Color" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->Diffuse.x, ImGuiColorEditFlags_Float)) // 향 후 수정
					{

					}

					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Text("Intensity");
					ImGui::NextColumn();
					ImGui::PushItemWidth(0.f);


					if (ImGui::DragFloat(string("##Intensity" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->intensity, 0.01f, 0.025f, 2.f))
					{

					}
					ImGui::PopItemWidth();


					if ((*lightvector)[i]->GetLightDesc()->type != LIGHT_DESC::DIRECTION)
					{
						ImGui::NextColumn();
						ImGui::Text("Position");
						ImGui::NextColumn();
						ImGui::PushItemWidth(0.f);
						if (ImGui::DragFloat3(string("##Position" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->Position.x, 0.1f))
						{

						}
						ImGui::PopItemWidth();

						ImGui::NextColumn();
						ImGui::Text("Range");
						ImGui::NextColumn();
						ImGui::PushItemWidth(0.f);
						if (ImGui::DragFloat(string("##Range" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->range, 0.01f, 0.f, 100.f))
						{

						}
						ImGui::PopItemWidth();
					}

					if ((*lightvector)[i]->GetLightDesc()->type == LIGHT_DESC::SPOT)
					{
						ImGui::NextColumn();
						ImGui::Text("Direction");
						ImGui::NextColumn();
						ImGui::PushItemWidth(0.f);
						if (ImGui::DragFloat3(string("##Direction" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->rotationDeg.x, 1.f, -180.f, 180.f, "%.fdeg"))
						{
							XMVECTOR rotQuat = ::XMQuaternionRotationRollPitchYaw(
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.x),
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.y),
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.z));

							XMVECTOR rotQuatInverse = ::XMQuaternionInverse(rotQuat);
							XMVECTOR lightDir = ::XMQuaternionMultiply(rotQuat, ::XMVectorSet(0.f, 0.f, 1.f, 0.f));
							::XMStoreFloat3(&(*lightvector)[i]->GetLightDesc()->Direction, ::XMQuaternionMultiply(lightDir, rotQuatInverse));
						}
						ImGui::PopItemWidth();

						// Spot Anlges
						ImGui::NextColumn();
						ImGui::Text("Spot Angles");
						ImGui::NextColumn();
						ImGui::PushItemWidth(0.f);
						if (ImGui::DragFloat2(string("##Spot Angles" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->spotAngles.x, 0.001f, 0.025f, 2.f))
						{
							if ((*lightvector)[i]->GetLightDesc()->spotAngles.x > (*lightvector)[i]->GetLightDesc()->spotAngles.y)
								(*lightvector)[i]->GetLightDesc()->spotAngles.x = (*lightvector)[i]->GetLightDesc()->spotAngles.y;

							(*lightvector)[i]->GetLightDesc()->spotAngles.x = 1.f / (::cosf((*lightvector)[i]->GetLightDesc()->spotAngles.x) - ::cosf((*lightvector)[i]->GetLightDesc()->spotAngles.y));
							(*lightvector)[i]->GetLightDesc()->spotAngles.y = ::cosf((*lightvector)[i]->GetLightDesc()->spotAngles.y);
						}
						ImGui::PopItemWidth();
					}



					if ((*lightvector)[i]->GetLightDesc()->type == LIGHT_DESC::DIRECTION)
					{
						ImGui::NextColumn();
						ImGui::Text("Direction");
						ImGui::NextColumn();
						ImGui::PushItemWidth(0.f);
						if (ImGui::DragFloat3(string("##Direction" + to_string(i)).c_str(), &(*lightvector)[i]->GetLightDesc()->rotationDeg.x, 1.f, -180.f, 180.f, "%.fdeg"))
						{
							XMVECTOR rotQuat = ::XMQuaternionRotationRollPitchYaw(
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.x),
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.y),
								::XMConvertToRadians((*lightvector)[i]->GetLightDesc()->rotationDeg.z));

							XMVECTOR rotQuatInverse = ::XMQuaternionInverse(rotQuat);
							XMVECTOR lightDir = ::XMQuaternionMultiply(rotQuat, ::XMVectorSet(0.f, 0.f, 1.f, 0.f));
							::XMStoreFloat3(&(*lightvector)[i]->GetLightDesc()->Direction, ::XMQuaternionMultiply(lightDir, rotQuatInverse));
						}

						ImGui::PopItemWidth();


						//if() // Point
						// kind of Light TODO
					}
				}
				ImGui::NextColumn();
			}
			else
				continue;

			ImGui::PopID();
		}

		//

		ImGui::PopStyleColor();

		//

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(.8f, .8f, .8f, 1.f));
		ImGui::Separator();
		ImGui::PopStyleColor();

	
	}
	ImGui::EndChild();



	RELEASE_INSTANCE(GameInstance);
	return S_OK;
}

void ImGuiManager::GameObjectUpdate(uint32 objectID, GameObject* pObj)
{
	if (pObj != nullptr)
	{
		ImGui::PushID(pObj->GetModelNameId().c_str());
		string enabledLabel = "Enabled##" + to_string(pObj->GetIdNumber());
		string selectLabel = "Select##" + to_string(pObj->GetIdNumber());
		
		_bool* enable = pObj->GetEnabled();
		_bool* select = pObj->GetObjectSelect();
		if (ImGui::Checkbox(enabledLabel.c_str(), enable))
		{
		
		}
		
		ImGui::SameLine();
		
		if (ImGui::Checkbox(selectLabel.c_str(), select))
			_SelectGameObject.emplace(pObj->GetModelNameId(), make_pair(select, pObj));

		if (*select == false && !_SelectGameObject.empty())
		{
			auto iter = _SelectGameObject.find(pObj->GetModelNameId());

			if (iter != _SelectGameObject.end())
			{
				iter->second.first = false;
				iter->second.second = nullptr;
				_SelectGameObject.erase(iter);
			}
		}



		if (!*enable)
		{
		
			if (nullptr != pObj->GetTransform())
			{
				Vec3 Scaled = pObj->GetTransform()->GetScaled();
				XMVECTOR worldScale = ::XMLoadFloat3(&Scaled);
		
				XMMATRIX Position = pObj->GetTransform()->GetWorldMatrix();
				XMVECTOR worldPosition = Position.r[3];
		
				XMVECTOR worldRotation = pObj->GetTransform()->GetWorldRotation();
		
				//Vec3 Rotation = gameObject->GetTransform()->
		
				if (ImGui::CollapsingHeader("Movement"))
				{
					// TODO ratation은 고민 해봐야겠음.
					// 
		
					ImGui::DragFloat3("Scale", &worldScale.m128_f32[0], 0.001f, 0.001f, 20.f);
					ImGui::DragFloat3("Rotation", &worldRotation.m128_f32[0], 1.f);
					ImGui::DragFloat3("Position", &worldPosition.m128_f32[0], 0.1f);
		
		
					::XMStoreFloat3(&Scaled, worldScale);
					pObj->GetTransform()->SetScaling(Scaled);
					pObj->GetTransform()->SetState(Transform::STATE::POSITION, worldPosition);
					pObj->GetTransform()->FixRotation(worldRotation.m128_f32[0], worldRotation.m128_f32[1], worldRotation.m128_f32[2]);

				}
			}
			
		
			UpdateModelUI(objectID, pObj);
		}
		
		ImGui::PopID();
		
	}
}

void ImGuiManager::UpdateModelUI(uint32 objectID, GameObject* pObj)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);


	if (ImGui::CollapsingHeader("Mesh List"))
	{
		ImGui::PushID(string(::to_string(pObj->GetIdNumber()) + pObj->GetModelName()).c_str());

		if (nullptr != pObj->GetBinaryModelComponent())
		{

			if (ImGui::TreeNodeEx(string("PBR").c_str()))
			{
				UpdateMaterialUI(objectID, pObj);
				

				ImGui::TreePop();
			}
			
		}
		ImGui::PopID();
	}

	RELEASE_INSTANCE(GameInstance);
}

void ImGuiManager::UpdateMaterialUI(uint32 objectID, GameObject* pObj)
{
	// 여기까진 게임오브젝트의 리스트를 순회해서 마테리얼 정보랑 모델 정보를 갖고 와야 되고.
	BinaryModel* modelComponent = pObj->GetBinaryModelComponent();

	vector<MESH_MATERIAL>* vectorMesh = pObj->GetBinaryModelComponent()->GetMaterial();

	uint32 MeshIndex = pObj->GetBinaryModelComponent()->GetNumMeshes();

	// 이 밑에 부터는 메쉬가 따로 돌아야함. 그니까 vectorIndex는 여기까지만 사용하고 메쉬 인덱스를 다시 돌아야함.
	// 콤보박스에서 인덱스 지정해주고 그 인덱스번호에 있는 메쉬 텍스쳐 불러와서 텍스쳐 변경하면 될 듯.


	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_bstr_t nameStr(_tempName);

		ImGui::SameLine(ImGui::GetWindowWidth() - 160.f);
		ImGui::PushItemWidth(120.f);

		static int32 selectIndex = 0;
		static _bool IsComboOpen = false;


		if (ImGui::BeginCombo("##Mesh Number", IsComboOpen ? ("Mesh Number" + std::to_string(selectIndex)).c_str() : "Mesh Number"))
		{
			for (_char i = 0; i < MeshIndex; i++)
			{
				_char label[32];
				::snprintf(label, sizeof(label), "Mesh Number %d", i);
				_bool isSelected = (i == selectIndex);

				if (ImGui::Selectable(label, isSelected))
				{
					selectIndex = i;
					break;
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}

		if (ImGui::IsItemClicked())
			IsComboOpen = true;

		if (ImGui::IsItemDeactivatedAfterEdit())
			IsComboOpen = false;


		ImGui::PopItemWidth();
		ImGui::Separator();


		// Diffuse
		if (ImGui::TreeNodeEx((void*)"Diffuse", ImGuiTreeNodeFlags_DefaultOpen, "Diffuse"))
		{
			
			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.diffusePath.c_str());
				nameStr = _texturePath.diffusePath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			ImGui::BeginGroup();

			static _bool tempBool = true;
			if (ImGui::Checkbox(" ", &tempBool))
			{

			}
			ImGui::SameLine();
			ImGui::Indent(_checkBoxOffset);

			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_DIFFUSE;
			}

			ImGui::Unindent(_checkBoxOffset);
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - _imageSize - _offset - 80.f);
			static _float color[3] = { 1.f,1.f,1.f };
			if (ImGui::ColorEdit3("Color##2", color))
			{

			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();
			ImGui::Unindent(60.f);
			

			ImGui::TreePop();
		}


		// Normal
		if (ImGui::TreeNodeEx((void*)"Normal", ImGuiTreeNodeFlags_DefaultOpen, "Normal"))
		{
			
			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_NORMALS])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_NORMALS]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_NORMALS]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.normalPath.c_str());
				nameStr = _texturePath.normalPath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_NORMALS;
			}
			ImGui::Unindent(_offset);
			
			
			ImGui::TreePop();
		}

		// Metallic
		if (ImGui::TreeNodeEx((void*)"Metallic", ImGuiTreeNodeFlags_DefaultOpen, "Metallic"))
		{
			
			
			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_METALNESS])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_METALNESS]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_METALNESS]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.metallicPath.c_str());
				nameStr = _texturePath.metallicPath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			ImGui::BeginGroup();
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_METALNESS;
			}
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - _imageSize - _offset - 40.f);
			static _float metallic = 0.f;
			if (ImGui::DragFloat("##MetallicValue", &metallic, 0.01f, 0.f, 1.f))
			{

			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();
			ImGui::Unindent(_offset);
			
			ImGui::TreePop();
		}

		// Rougness
		if (ImGui::TreeNodeEx((void*)"Rougness", ImGuiTreeNodeFlags_DefaultOpen, "Rougness"))
		{
			
			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE_ROUGHNESS])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE_ROUGHNESS]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE_ROUGHNESS]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.roughnessPath.c_str());
				nameStr = _texturePath.roughnessPath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			ImGui::BeginGroup();
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_DIFFUSE_ROUGHNESS;
			}
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - _imageSize - _offset - 40.f);
			static _float Rougness = 0.f;
			if (ImGui::DragFloat("##RougnessValue", &Rougness, 0.01f, 0.f, 1.f))
			{

			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();
			ImGui::Unindent(_offset);
			
			ImGui::TreePop();
		}


		// Emissive
		if (ImGui::TreeNodeEx((void*)"Emissive", ImGuiTreeNodeFlags_DefaultOpen, "Emissive"))
		{

			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_EMISSIVE])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_EMISSIVE]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_EMISSIVE]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.emissivePath.c_str());
				nameStr = _texturePath.emissivePath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			ImGui::BeginGroup();
			static _bool tempBool = true;
			if (ImGui::Checkbox(" ", &tempBool))
			{

			}
			ImGui::SameLine();
			ImGui::Indent(_checkBoxOffset);
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_EMISSIVE;
			}
			ImGui::Unindent(_checkBoxOffset);
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - _imageSize - _offset - 40.f);
			static _float Emissive = 0.f;
			if (ImGui::DragFloat("##EmissiveStrength", &Emissive, 0.01f, 0.f, 100.f))
			{

			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();
			ImGui::Unindent(_offset);

			ImGui::TreePop();
		}
		

		// Amibent Occlusion
		if (ImGui::TreeNodeEx((void*)"Amibent Occlusion", ImGuiTreeNodeFlags_DefaultOpen, "Amibent Occlusion"))
		{

			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.ambientOcclusionPath.c_str());
				nameStr = _texturePath.ambientOcclusionPath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_AMBIENT_OCCLUSION;
			}
			ImGui::Unindent(_offset);
			ImGui::TreePop();
		}


		// Displacement
		if (ImGui::TreeNodeEx((void*)"Displacement", ImGuiTreeNodeFlags_DefaultOpen, "Displacement"))
		{

			if (!modelComponent->GetMaterial()->empty() && nullptr != (*vectorMesh)[selectIndex]._texture[TextureType_DISPLACEMENT])
			{
				ImGui::Image((*vectorMesh)[selectIndex]._texture[TextureType_DISPLACEMENT]->GetShaderResourceViews()[0], ImVec2(_imageSize, _imageSize));
				(*vectorMesh)[selectIndex]._texture[TextureType_DISPLACEMENT]->
					GetShaderResourceViews()[0]->GetPrivateData(WKPDID_D3DDebugObjectNameW, &_tempSize, (void*)_texturePath.displacmentPath.c_str());
				nameStr = _texturePath.displacmentPath.c_str();
			}
			else
			{
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon\\Empty_Texture.jpg"), ImVec2(_imageSize, _imageSize));
				nameStr = "Add Texture";
			}

			ImGui::SameLine();
			ImGui::Indent(_offset);
			if (ImGui::Button((const char*)nameStr))
			{
				g_fileDialog.Open();
				g_fileDialog.SetPwd(filesystem::current_path() / _fileguipath);
				_textureType = TextureType_DISPLACEMENT;
			}
			ImGui::Unindent(_offset);
			ImGui::TreePop();
		}

		ImGui::TreePop();

		g_fileDialog.Display();

		if (g_fileDialog.HasSelected())
		{
			string strPath = g_fileDialog.GetSelected().string();
			size_t pos = strPath.find("Textures");
			if(pos != string::npos)
				strPath.erase(0, pos);

			pos = strPath.find_last_of("\\/");
			if (pos != string::npos)
				strPath = strPath.substr(pos + 1);

			string fullpath = _rootTextureDirection + strPath;

			wstring path = ::charToWchar(fullpath);

			//wstring addpath = L".." + path;
			

			switch (_textureType)
			{
			case TextureType_NONE:
				break;
			case TextureType_DIFFUSE:
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.diffusePath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_SPECULAR:
				break;
			case TextureType_AMBIENT:
				break;
			case TextureType_EMISSIVE:
				(*vectorMesh)[selectIndex]._texture[TextureType_EMISSIVE] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_EMISSIVE]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.emissivePath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_HEIGHT:
				break;
			case TextureType_NORMALS:
				(*vectorMesh)[selectIndex]._texture[TextureType_NORMALS] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_NORMALS]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.normalPath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_SHININESS:
				break;
			case TextureType_OPACITY:
				break;
			case TextureType_DISPLACEMENT:
				(*vectorMesh)[selectIndex]._texture[TextureType_DISPLACEMENT] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_DISPLACEMENT]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.displacmentPath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_LIGHTMAP:
				break;
			case TextureType_REFLECTION:
				break;
			case TextureType_BASE_COLOR:
				break;
			case TextureType_NORMAL_CAMERA:
				break;
			case TextureType_EMISSION_COLOR:
				break;
			case TextureType_METALNESS:
				(*vectorMesh)[selectIndex]._texture[TextureType_METALNESS] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_METALNESS]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.metallicPath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_DIFFUSE_ROUGHNESS:
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE_ROUGHNESS] = Texture::Create(_device, _deviceContext, path);
				(*vectorMesh)[selectIndex]._texture[TextureType_DIFFUSE_ROUGHNESS]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				_texturePath.roughnessPath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_AMBIENT_OCCLUSION:
				//(*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION]->SelfDelete((*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION]);
				//(*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION] = Texture::Create(_device, _deviceContext, path);
				//(*vectorMesh)[selectIndex]._texture[TextureType_AMBIENT_OCCLUSION]->GetShaderResourceViews()[0]->SetPrivateData(WKPDID_D3DDebugObjectNameW, 64, path.c_str());
				//_texturePath.ambientOcclusionPath = wstring(strPath.begin(), strPath.end());
				break;
			case TextureType_UNKNOWN:
				break;
			case TextureType_Force32Bit:
				break;
			default:
				break;
			}
		}

		g_fileDialog.ClearSelected();
	}
}

void ImGuiManager::LoadModelList(string path)
{
	string name = "";
	string fileExtension = "";
	struct dirent* entry;
	DIR* dir = ::opendir(path.c_str());
	// opendir 인자값 디렉토리 경로, 성공시 디렉토리 스트림 포인터 반환하고 실패시 NULL 
	// DIR 구조체에 dirent가 있음.
	// 내부에 파일 경로 읽을 때 wdirent는 w_char 기반이고 dirent는 기본 char 임. 그러므로 dirent 사용한다.

	_modelNames.clear();
	_modelNameHoveringState.clear();

	vector<string> modelNames;
	if (dir != NULL)
	{
		// readdir은 디렉토리 스트림 포인터를 인자 값을 원함. opendir로 스트림 포인터를 얻었음.
		// 성공하면 엔트리 포인터(시작 포인터)를 얻고, 실패 시 NULL 반환 
		// 함수를 호출할 때마다 디렉터리 하위 파일들을 차례로 반환함. 더 이상 반환할 파일이 없을 떄 NULL 반환하므로 while문의 조건 식에 넣어서 사용.
		while ((entry = ::readdir(dir)) != NULL)
		{
 			name = entry->d_name;
			// 디렉토리 경로
			if (entry->d_type == DT_DIR && name != "." && ((path == _rootModelDirection && name != ".." || (path != _rootModelDirection))))
			{
				pair<string, _bool> directory(name, true);
				_modelNames.push_back(directory);
				_modelNameHoveringState.push_back(false);
			}
			else
			{
				size_t i = name.rfind('.', name.length());
				// rfind -> reverse find 문자열 뒷 자리부터 검색함. .fbx .obj 등 확장자를 찾을 때 뒤에부터 탐색하는 것이 더 빠름.
				if (i != string::npos)
				{
					// string::npos : -1을 가진 상수이고, find 함수 수행 시에 찾는 문자열이 없을 때 반환된다. 만약 못 찾았으면
					// string은 파인드 함수는 문자열을 찾지 못하면 npos를 리턴함.
					fileExtension = name.substr(i + 1, name.length() - 1);
					// substr(), str의 n번 째 index부터 k개의 문자를 부분 문자열로 반환함.
					if (fileExtension == "obj" || fileExtension == "DAT" || fileExtension == "dat")
					{
						modelNames.push_back(name);
					}
				}
			}
		}

		for (size_t i = 0; i < modelNames.size(); i++)
		{
			pair<string, _bool> modelFile(modelNames[i], false);
			_modelNames.push_back(modelFile);
			_modelNameHoveringState.push_back(false);
		}

		::closedir(dir);
	}
}

HRESULT ImGuiManager::SceneSave(wstring& filePath)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	auto path = filesystem::path(filePath);

	filesystem::create_directory(path.parent_path());

	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring finalPath = filePath + L"MapData.dat";

	file->Open(finalPath, FileMode::Write);

	map<const LAYER_TAG, Layer*>* EntireLayer = gameInstance->GetEntireObjectLayer();

	// 레이어 사이즈도 저장해야할듯?
	uint32 levelIndex = gameInstance->GetCurrentLevelIndex();
	file->Write<uint32>(levelIndex);

	uint32 totalLayerSize = EntireLayer[levelIndex].size();
	file->Write<uint32>(totalLayerSize);

	for (auto EntrieIter : EntireLayer[levelIndex])
	{
		// 오브젝트 매니저에 레이어 태그들 일단 저장.
		uint32 LayerTagType = static_cast<uint32>(EntrieIter.first);
		file->Write<uint32>(LayerTagType);

		//LAYER_PLAYER, LAYER_MONSTER, LAYER_TERRAIN, LAYER_CAMERA, LAYER_ENVIRONMENT, LAYER_END,

		vector<GameObject*>* LayerGameObjectList = EntrieIter.second->GetGameObject();
		uint32 GameObjectListSize = LayerGameObjectList->size();
		file->Write<uint32>(GameObjectListSize);


		switch (static_cast<LAYER_TAG>(LayerTagType))
		{
		case Engine::LAYER_TAG::LAYER_PLAYER:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				vector<GameObject*>& playerPart = static_cast<Player*>(LayerObjects)->GetPlyaerPart();

				//playerPart[Player::PART::PART_BODY]

			}
			break;
		case Engine::LAYER_TAG::LAYER_TERRAIN:
			break;
		case Engine::LAYER_TAG::LAYER_CAMERA:
			break;
		case Engine::LAYER_TAG::LAYER_STATIC:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				// 모델 타입.
				OBJECT_TYPE modelType = LayerObjects->GetObjectType();
				file->Write<uint32>(static_cast<uint32>(modelType));

				// 모델 이름.
				string modelName = LayerObjects->GetModelName();
				file->Write<string>(modelName);

				uint32 modelID = LayerObjects->GetIdNumber();
				file->Write<uint32>(modelID);

				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				StaticObject::STATE_DESC StaticComponentName = static_cast<StaticObject*>(LayerObjects)->GetStaticComponentsName();
				file->Write<string>(Utils::ToString(StaticComponentName._strModelComponentName));
				file->Write<string>(Utils::ToString(StaticComponentName._strShaderName));
				file->Write<string>(Utils::ToString(StaticComponentName._protoTypeTag));

				Matrix staticObjectWorldMarix = LayerObjects->GetTransform()->GetWorldMatrix();
				file->Write<Matrix>(staticObjectWorldMarix);
			}
			break;
		case Engine::LAYER_TAG::LAYER_DYNAMIC:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				// 모델 타입.
				OBJECT_TYPE modelType = LayerObjects->GetObjectType();
				file->Write<uint32>(static_cast<uint32>(modelType));

				// 모델 이름.
				string modelName = LayerObjects->GetModelName();
				file->Write<string>(modelName);

				uint32 modelID = LayerObjects->GetIdNumber();
				file->Write<uint32>(modelID);

				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				DynamicObject::STATE_DESC DynamicComponentName = static_cast<DynamicObject*>(LayerObjects)->GetDynamicComponentsName();
				file->Write<string>(Utils::ToString(DynamicComponentName._strModelComponentName));
				file->Write<string>(Utils::ToString(DynamicComponentName._strShaderName));
				file->Write<string>(Utils::ToString(DynamicComponentName._protoTypeTag));


				Matrix DynamicObjectWorldMarix = LayerObjects->GetTransform()->GetWorldMatrix();
				file->Write<Matrix>(DynamicObjectWorldMarix);
			}
			break;
		case Engine::LAYER_TAG::LAYER_PUZZLE:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				// 모델 타입.
				OBJECT_TYPE modelType = LayerObjects->GetObjectType();
				file->Write<uint32>(static_cast<uint32>(modelType));

				// 모델 이름.
				string modelName = LayerObjects->GetModelName();
				file->Write<string>(modelName);

				uint32 modelID = LayerObjects->GetIdNumber();
				file->Write<uint32>(modelID);

				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				DynamicObject::STATE_DESC puzzleComponentName = static_cast<DynamicObject*>(LayerObjects)->GetDynamicComponentsName();
				file->Write<string>(Utils::ToString(puzzleComponentName._strModelComponentName));
				file->Write<string>(Utils::ToString(puzzleComponentName._strShaderName));
				file->Write<string>(Utils::ToString(puzzleComponentName._protoTypeTag));

				Matrix staticObjectWorldMarix = LayerObjects->GetTransform()->GetWorldMatrix();
				file->Write<Matrix>(staticObjectWorldMarix);
			}
			break;
		case Engine::LAYER_TAG::LAYER_ENVIRONMENT:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				// 모델 타입.
				OBJECT_TYPE modelType = LayerObjects->GetObjectType();
				file->Write<uint32>(static_cast<uint32>(modelType));

				// 모델 이름.
				string modelName = LayerObjects->GetModelName();
				file->Write<string>(modelName);

				uint32 modelID = LayerObjects->GetIdNumber();
				file->Write<uint32>(modelID);

				// 스태틱마다 모델과 사용할 셰이더가 다르니까, 컴포넌트 모델 이름 + 컴포넌트 셰이더 이름 저장
				StaticObject::STATE_DESC StaticComponentName = static_cast<StaticObject*>(LayerObjects)->GetStaticComponentsName();
				file->Write<string>(Utils::ToString(StaticComponentName._strModelComponentName));
				file->Write<string>(Utils::ToString(StaticComponentName._strShaderName));
				file->Write<string>(Utils::ToString(StaticComponentName._protoTypeTag));

				Matrix staticObjectWorldMarix = LayerObjects->GetTransform()->GetWorldMatrix();
				file->Write<Matrix>(staticObjectWorldMarix);
			}
			break;
		case Engine::LAYER_TAG::LAYER_COLLIDER:
			for (auto& LayerObjects : *LayerGameObjectList)
			{
				OBJECT_TYPE modelType = LayerObjects->GetObjectType();
				file->Write<uint32>(static_cast<uint32>(modelType));

				Matrix colliderMatrix = LayerObjects->GetTransform()->GetWorldMatrix();
				file->Write<Matrix>(colliderMatrix);
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
}

HRESULT ImGuiManager::SceneLoad(wstring& filePath)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	shared_ptr<FileUtils> file = make_shared<FileUtils>();

	wstring finalPath = filePath + L"MapData.dat";

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

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), static_cast<LAYER_TAG>(LayerTagType), StaticComponentName._protoTypeName, &StaticComponentName)))
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

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), static_cast<LAYER_TAG>(LayerTagType), DynamicComponentName._protoTypeName, &DynamicComponentName)))
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

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), static_cast<LAYER_TAG>(LayerTagType), PuzzleComponentName._protoTypeName, &PuzzleComponentName)))
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

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), static_cast<LAYER_TAG>(LayerTagType), environmentComponentName._protoTypeName, &environmentComponentName)))
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

				Matrix staticObjectWorldMarix;
				file->Read<Matrix>(staticObjectWorldMarix);
				ColliderComponentName._saveWorldMatrix = staticObjectWorldMarix;

				if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), static_cast<LAYER_TAG>(LayerTagType), TEXT("ProtoTypeDoorCol"), &ColliderComponentName)))
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
}

void ImGuiManager::SortPoints()
{
	// 시계방향 공식을 이용.
	_float Value1 = _vPoints[0].x * _vPoints[1].z + _vPoints[1].x * _vPoints[2].z + _vPoints[2].x * _vPoints[0].z;
	_float Value2 = _vPoints[1].x * _vPoints[0].z + _vPoints[2].x * _vPoints[1].z + _vPoints[0].x * _vPoints[2].z;
	_float Result = Value1 - Value2;

	if (Result > 0)
	{
		Vec3 temp = _vPoints[0];
		Vec3 dest = _vPoints[2];

		_vPoints[0] = dest;
		_vPoints[2] = temp;
	}
}

_float ImGuiManager::DistanceBetewwinPoints(const Vec3& point1, const Vec3& point2)
{
	_float deltaX = point1.x - point2.x;
	_float deltaY = point1.y - point2.y;
	_float deltaZ = point1.z - point2.z;

	// 유클리드 거리 계산
	_float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

	return distance;
}

void ImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release<BinaryNavi*>(_pNaviGation);
	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}