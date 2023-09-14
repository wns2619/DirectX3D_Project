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
	_bool demo = true;
	ImGui::ShowDemoWindow(&demo);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void ImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
