#include "pch.h"

#include "imgui.h"  // IMGUI::SETUP
#include "imgui_impl_win32.h"  // IMGUI::SETUP
#include "imgui_impl_dx11.h"  // IMGUI::SETUP

#include "ImGuiManager.h"
#include "ImguiResourceHandler.h"
#include "GameInstance.h"

#include "GameObject.h"
#include "EditorTerrain.h"



IMPLEMENT_SINGLETON(ImGuiManager)

ImGuiManager::ImGuiManager()
{

}

HRESULT ImGuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;


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
		windowFlags |= ImGuiWindowFlags_None;

	/* Main Space */
	{
		MouseMove();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.05f, 0.1f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(200.f, 0.f));
		ImGui::Begin("Main", NULL, windowFlags | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::SameLine(ImGui::GetWindowWidth() - 28);

		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"/Icon/save_white_18dp.png"), ImVec2(20, 20)))
		{

		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 56);
		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/outline_refresh_white_18dp.png"), ImVec2(20, 20)))
		{

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

			}
		}

		ImGui::End();
		ImGui::PopStyleColor();
	}

	 // Model Card Window
	if(_modelNames.size())
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
					ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/previous_folder_icon.png"), ImVec2(100, 100));
				else
					ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/folder_icon.png"), ImVec2(100, 100));
			}
			else
				ImGui::Image(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/model_icon.png"), ImVec2(100, 100));


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

					// ������ �������� Ŭ�� ���� �� �ش� FBX�� �ҷ������� �ش� FBX�� ������ �� �� ��ġ�� Model���� ���� ������ �ҷ�������.
					size_t position = _currentDirectoryPath.find_first_of('\\/') + 1;
					string modelPath = _currentDirectoryPath + _modelNames[i].first;
					//modelPath.erase(0, position);

					// �� ��θ� �޾ƿ��� �ڵ�. ��θ� �޾Ƽ� ��� �� ���ΰ�. Loading���� �̸� �ҷ��� ������. �ƴϸ� ���� ������ �����ǰ� ���� ������.
					// ������ �� ��θ� ��ȯ�ϴϱ�. �� ��ζ� ProtoType ��ζ� ���ؼ� ���ӿ�����Ʈ�� ��������.
					// �׷��ٸ� ObjectManager�� �ִ� prototype find�� ����ؼ�. �� �� �ڿ� �����ϸ� �� �� ������.

					// �ҷ����� EDIT�� �����. �ٸ� ������ �ҷ� �� ���� ���̾� ��ġ�� �ٲ��ָ� ��.
					Matrix modelInitializMatrix = ::XMMatrixIdentity();
					modelInitializMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));

					// ���� �� �����ؾ� �Ǵϱ� IMGUI���� ProtoType�� ����� AddObject���� ���� �����Ű�°� ���� �� ����.
					// Model ������ ������ Helper Ŭ������ ���� ���ο��� ProtoType ����� �� ��� Clone���� ������.
					// �׷��� ���� ���� ���� Layer ��ġ + ProtoTypeTag

					wstring findPrototypename = ImGuiResourceHandler::GetInstance()->FindProtoFilePath(modelPath);

					if(FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), _editlayerTag, findPrototypename)))
						return E_FAIL;

					

					//if(FAILED())


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


	/* Objects Space */

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
		// TODO : for������ gameobject ��ȸ 
		int32 objectsize = gameInstance->GetLayerObjectCount();
		for (size_t i = 0; i < objectsize; i++)
		{
			if (i > 0)
				ImGui::NewLine();

			vector<GameObject*>* gamevector = gameInstance->GetCurrentObjectList(_editlayerTag);

			if (gamevector != nullptr)
			{
				GameObject* gameObject = (*gamevector)[i];
				if (gameObject != nullptr)
				{
					ImGui::Text(gameObject->GetModelNameId().c_str());
				}
			}

			ImGui::PushID("gameobject Uptate" + i);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 24);

			if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/baseline_delete_white_18dp.png"), ImVec2(20, 20)))
			{
				if (_selectedIndex = (int32)i)
				{
					_selectedIndex = -1;
				}

				if (gamevector != nullptr)
				{
					GameObject* gameObject = (*gamevector)[i];
					Safe_Release<GameObject*>(gameObject);
					gamevector->erase(gamevector->begin() + i);
				}

				ImGui::PopStyleVar();
			}
			else
			{
				ImGui::PopStyleVar();



			}

			ImGui::PopID();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	/* Light Space */

	ImGui::Dummy(ImVec2(0.0f, _splitterButtonPadding / 2.f));
	ImGui::Indent(5.f);
	ImGui::Text("Lighting");
	ImGui::Unindent(5.f);
	ImGui::Dummy(ImVec2(0.f, _splitterButtonPadding / 2.f));

	DrawSplitter(true, _sectionSeperatorHeight, &_lightSectionHeight, &_TerrainSectionHeight, 61.f, 61.f,
		ImGui::GetWindowSize().x - 10.f, _splitterButtonPadding);
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
		}

		ImGui::PopItemWidth();
		ImGui::Separator();

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4f, 0.4f, 0.5f, 1.f));
		ImGui::Columns(2, "LightSettings", false);
		static _float colWidth = 105.f;
		ImGui::SetColumnWidth(0, colWidth);


		//

		// TODO Light 
		const wstring example = L"PointLight";
		const int i = 0;
		ImGui::Dummy(ImVec2(0.f, 3.f));
		ImGui::Text("Test");
		ImGui::PushID(string("Light" + to_string(i)).c_str());

		ImGui::NextColumn();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - colWidth - 46);
		if (ImGui::ImageButton(ImGuiResourceHandler::GetInstance()->GetResourceTexture(L"Icon/baseline_delete_white_18dp.png"), ImVec2(20, 20)))
		{
			// TODO

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
			XMFLOAT3 testColor1 = XMFLOAT3(1.f, 1.f, 1.f);
			if (ImGui::ColorEdit3(string("##Color" + to_string(i)).c_str(), &testColor1.x, ImGuiColorEditFlags_Float)) // �� �� ����
			{
				// TODO 
				// ���� update
			}
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Intensity");
			ImGui::NextColumn();
			ImGui::PushItemWidth(0.f);

			_float testIntensity = 1.f;
			if (ImGui::DragFloat(string("##Intensity" + to_string(i)).c_str(), &testIntensity, 0.01f, 0.f, 200.f))
			{

			}
			ImGui::PopItemWidth();

			//if() // Directional

			//if() // Spot
			 
			//if() // Point
			// kind of Light TODO



		}

		//
		ImGui::PopID();
		ImGui::PopStyleColor();

		//

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(.8f, .8f, .8f, 1.f));
		ImGui::Separator();
		ImGui::PopStyleColor();
	}


	ImGui::EndChild();

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

			if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), _editlayerTag, TEXT("ProtoTypeGameObjectEditTerrain"), &desc)))
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

	GameInstance* gameinstance = GET_INSTANCE(GameInstance);

	ImGui::Begin("Mouse Pose");
	ImGui::Text("Mouse Position X(%d), Y(%d)", pt.x, pt.y);

	Vec3 pos = Vec3(0.f, 0.f, 0.f);
	_float distance = 0.f;

	if (_Isterrain)
	{
		EditorTerrain* terrain = static_cast<EditorTerrain*>(gameinstance->GetLayerObject(TEXT("LayerEditTerrain"), OBJECT_TYPE::TERRAIN));

		terrain->TerrainPick(pos, distance);
	}

	ImGui::Spacing();

	ImGui::Text("Terrain Pos X : %.f", pos.x);
	ImGui::Text("Terrain Pos Y : %.f", pos.y);
	ImGui::Text("Terrain Pos Z : %.f", pos.z);




	RELEASE_INSTANCE(GameInstance);

	ImGui::End();
}

void ImGuiManager::GameObjectUpdate(int32 vectorIndex)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	vector<GameObject*>* gamevector = gameInstance->GetCurrentObjectList(_editlayerTag);

	if (gamevector != nullptr)
	{
		GameObject* gameObject = (*gamevector)[vectorIndex];
		if (gameObject != nullptr)
		{
			ImGui::PushID(gameObject->GetModelNameId().c_str());
			string enabledLabel = "Enabled##" + to_string(gameObject->GetIdNumber());

			if(ImGui::Checkbox(enabledLabel.c_str(), ))
		}
	}

	
	RELEASE_INSTANCE(GameInstance);
}

void ImGuiManager::LoadModelList(string path)
{
	string name = "";
	string fileExtension = "";
	struct dirent* entry;
	DIR* dir = ::opendir(path.c_str());
	// opendir ���ڰ� ���丮 ���, ������ ���丮 ��Ʈ�� ������ ��ȯ�ϰ� ���н� NULL 
	// DIR ����ü�� dirent�� ����.
	// ���ο� ���� ��� ���� �� wdirent�� w_char ����̰� dirent�� �⺻ char ��. �׷��Ƿ� dirent ����Ѵ�.

	_modelNames.clear();
	_modelNameHoveringState.clear();

	vector<string> modelNames;
	if (dir != NULL)
	{
		// readdir�� ���丮 ��Ʈ�� �����͸� ���� ���� ����. opendir�� ��Ʈ�� �����͸� �����.
		// �����ϸ� ��Ʈ�� ������(���� ������)�� ���, ���� �� NULL ��ȯ 
		// �Լ��� ȣ���� ������ ���͸� ���� ���ϵ��� ���ʷ� ��ȯ��. �� �̻� ��ȯ�� ������ ���� �� NULL ��ȯ�ϹǷ� while���� ���� �Ŀ� �־ ���.
		while ((entry = ::readdir(dir)) != NULL)
		{
 			name = entry->d_name;
			// ���丮 ���
			if (entry->d_type == DT_DIR && name != "." && ((path == _rootModelDirection && name != ".." || (path != _rootModelDirection))))
			{
				pair<string, _bool> directory(name, true);
				_modelNames.push_back(directory);
				_modelNameHoveringState.push_back(false);
			}
			else
			{
				size_t i = name.rfind('.', name.length());
				// rfind -> reverse find ���ڿ� �� �ڸ����� �˻���. .fbx .obj �� Ȯ���ڸ� ã�� �� �ڿ����� Ž���ϴ� ���� �� ����.
				if (i != string::npos)
				{
					// string::npos : -1�� ���� ����̰�, find �Լ� ���� �ÿ� ã�� ���ڿ��� ���� �� ��ȯ�ȴ�. ���� �� ã������
					// string�� ���ε� �Լ��� ���ڿ��� ã�� ���ϸ� npos�� ������.
					fileExtension = name.substr(i + 1, name.length() - 1);
					// substr(), str�� n�� ° index���� k���� ���ڸ� �κ� ���ڿ��� ��ȯ��.
					if (fileExtension == "obj" || fileExtension == "FBX" || fileExtension == "fbx")
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

void ImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();



	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}