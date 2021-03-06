#include "DevUI.h"
#include "ImGuiIntegration.h"
#include "rayIncludes.h"

void DrawResourceCounter(DevUIState *devUIState);

void DevUIInit(DevUIState *devUIState)
{
	devUIState->guiContext = ImGui::CreateContext(nullptr);
	devUIState->igIO = &ImGui::GetIO();
	devUIState->show_demo_window = true;
	devUIState->isResourceCounterOpen = false;

	ImGuiInitialize();
}

void DevUINewFrame()
{
	BeginImGui();
}

// Updates the DevUI state and prepares all of the draw calls for the DevUI
void DevUIDraw(DevUIState *devUIState)
{
	DrawResourceCounter(devUIState);
}

void DevUIRender()
{
	EndImGui();
}

void DevUIDestroy(DevUIState *devUIState)
{
	DestroyImGui();
    ImGui::DestroyContext(devUIState->guiContext);
}

//Draw the FPS counter window
void DrawResourceCounter(DevUIState *devUIState)
{
	ImGui::SetNextWindowPos({ 10, 10 }, ImGuiCond_Appearing, { 0, 0 });
	ImGui::SetNextWindowBgAlpha(0.40f);

    ImGuiWindowFlags windowFlags = //ImGuiWindowFlags_NoDecoration |
                                   ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoNav |
                                   ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoSavedSettings |
                                   ImGuiWindowFlags_NoResize;

    if (devUIState->isResourceCounterOpen)
    {
//        ImGui::SetNextWindowSize({780, 290});
        ImGui::SetNextWindowBgAlpha(1.0f);
    }
    else {
        //sizing is switched to auto resize above
        ImGui::SetNextWindowBgAlpha(0.40f);
    }

	ImGui::Begin("Metrics", nullptr, windowFlags);

    ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "FPS: %.2f", devUIState->igIO->Framerate);
    devUIState->isResourceCounterOpen =
            devUIState->isResourceCounterOpen ^ (ImGui::IsWindowHovered(0) & ImGui::IsMouseClicked(0, false) & ImGui::IsItemClicked(0));
	if (devUIState->isResourceCounterOpen)
	{
		ImGui::Separator();
//		ImGui::Text("Frame times (last 120 frames)");
//		ImGui::PlotLines("##", frameTimes, 120, arrayOffset,
//					nullptr, 0.0f, max, {240, 40}, 4);
        devUIState->plot->DrawHistory();
//        devUIState->plot->DrawList();
	}
    ImGui::End();
}