#include "pch.h"

#include "imgui.h"  // IMGUI::SETUP
#include "imgui_impl_win32.h"  // IMGUI::SETUP
#include "imgui_impl_dx11.h"  // IMGUI::SETUP

#include "ImGuiManager.h"

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

	_bool map_tool_window = true;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	ImGui::ShowDemoWindow(&map_tool_window);


	if (map_tool_window)
	{
		ImGui::Begin("My Tool", &map_tool_window, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Close", "Ctrl+W")) {}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Object"))
		{
			if (ImGui::TreeNode("Test"))
			{
				ImGui::SeparatorText("Furniture");

				ImGui::Spacing();

				ImGui::BulletText("Inputs");

				ImGui::SeparatorText("Position Sliders");

				{
					static int i = 0;
					ImGui::SliderInt("Slider left or Right", &i, -1, 3);

					static float f1 = 0.123f, f2 = 0.f;
					ImGui::SliderFloat("Slider float", &f1, 0.f, 1.f, "Ratio = %.3f");
					ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

					static float angle = 0.0f;
					ImGui::SliderAngle("slider angle", &angle);

					enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
					static int elem = Element_Fire;
					const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
					const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
					ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
					ImGui::SameLine();
				}

				ImGui::TreePop();
			}
		}


		ImGui::End();
	}


	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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

void ImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
