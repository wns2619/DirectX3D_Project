#include "pch.h"

#include "imgui.h"  // IMGUI::SETUP
#include "imgui_impl_win32.h"  // IMGUI::SETUP
#include "imgui_impl_dx11.h"  // IMGUI::SETUP

#include "ImGuiManager.h"
#include "ImguiResourceHandler.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(ImGuiManager)

ImGuiManager::ImGuiManager()
{

}

HRESULT ImGuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Safe_AddRef<ID3D11Device*>(device);
	Safe_AddRef<ID3D11DeviceContext*>(deviceContext);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(device, deviceContext);

	ImGuiResourceHandler::GetInstance()->Initialize(device, deviceContext);

	GuiStyle();

	Safe_Release<ID3D11Device*>(device);
	Safe_Release<ID3D11DeviceContext*>(deviceContext);

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

	_bool map_tool_window = true;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	ImGui::ShowDemoWindow(&map_tool_window);

	ImGuiWindowFlags windowFlags = 0;
	if (!_windowResizeFlag)
		windowFlags |= ImGuiWindowFlags_NoResize;
	if (!_windowMoveFlag)
		windowFlags |= ImGuiWindowFlags_None;

	/* Main Space */
	{

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
		ImGui::PopStyleColor();
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
		// TODO : for문으로 gameobject 순회 
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
			if (ImGui::ColorEdit3(string("##Color" + to_string(i)).c_str(), &testColor1.x, ImGuiColorEditFlags_Float)) // 향 후 수정
			{
				// TODO 
				// 빛의 update
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
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 1.f / 255.f * 130.f, 1.f, 0.8f);
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

void ImGuiManager::Free()
{
	ImGuiResourceHandler::GetInstance()->DestroyInstance();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}



//if(ImGui::ImageButton())

//if (ImGui::BeginMenuBar())
//{
//	if (ImGui::BeginMenu("Menu"))
//	{
//		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
//		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
//		if (ImGui::MenuItem("Close", "Ctrl+W")) {}

//		ImGui::EndMenu();
//	}

//	ImGui::EndMenuBar();
//}

//ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
//ImGui::Spacing();

//if (ImGui::CollapsingHeader("Object"))
//{
//	if (ImGui::TreeNode("Test"))
//	{
//		ImGui::SeparatorText("Furniture");

//		ImGui::Spacing();

//		ImGui::BulletText("Inputs");

//		ImGui::SeparatorText("Position Sliders");

//		{
//			static int i = 0;
//			ImGui::SliderInt("Slider left or Right", &i, -1, 3);

//			static float f1 = 0.123f, f2 = 0.f;
//			ImGui::SliderFloat("Slider float", &f1, 0.f, 1.f, "Ratio = %.3f");
//			ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

//			static float angle = 0.0f;
//			ImGui::SliderAngle("slider angle", &angle);

//			enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
//			static int elem = Element_Fire;
//			const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
//			const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
//			ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
//			ImGui::SameLine();
//		}

//		ImGui::TreePop();
//	}
//}