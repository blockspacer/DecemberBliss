#include "enttTest.h"
#include <imgui.h>
#include "LogGui.h"
#include "memory_editor.h"
#include "rayIncludes.h"
#include "MetricsGui/include/metrics_gui/metrics_gui.h"
#include <chrono>
#include <python.h>

//Generated by cmake using buildInfo.h.in template
#include "buildInfo.h"
#define BLISS_FULL_HEADER BLISS_TITLE " v" BLISS_VERSION " " BLISS_ARCH " " BLISS_BUILD_TYPE " " BLISS_BRANCH " " BLISS_COMMIT_HASH
#include "DevUI.h"

#include "main.h"

using namespace std::chrono;

//called by raylib as a callback, all logging functions are from raylib
void LogCallbackFunc(int logType, const char *text, va_list args);
static LogGui logGui;

int main()
{
    SetTraceLogCallback(LogCallbackFunc);
    SetTraceLogLevel(LOG_INFO);

    tryEntt();
    int screenWidth = 800;
    int screenHeight = 600;

    printf("Starting " BLISS_FULL_HEADER "...\n");
    InitWindow(screenWidth, screenHeight, BLISS_FULL_HEADER);

    TraceLog(LOG_INFO, "Loading fonts");
    Font fontRobotoMono = LoadFontEx("resources/fonts/RobotoMono-Regular.ttf", 28, nullptr, 255);
    Font fontRobotoMonoSm = LoadFontEx("resources/fonts/RobotoMono-Regular.ttf", 12, nullptr, 255);
    Font font = LoadFontEx("resources/fonts/Merriweather-Regular.ttf", 14, nullptr, 255);

//    SetTargetFPS(60);

    DevUIState devUIState;
    DevUIInit(&devUIState);

    TraceLog(LOG_INFO, "Loading textures");
    Texture2D santaTex = LoadTexture("resources/santa/Idle (1).png");

    bool exitWindow = false;

    TraceLog(LOG_INFO, "Initializing metric trackers");
    MetricsGuiMetric frameTimeMetric = MetricsGuiMetric(
            "Frame time", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric inputTimeMetric = MetricsGuiMetric(
            "Input time", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric simulationTimeMetric = MetricsGuiMetric(
            "Simulation", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric drawPrepTimeMetric = MetricsGuiMetric(
            "Draw Prep", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric renderTimeMetric = MetricsGuiMetric(
            "Main Render", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric endDrawingMetric = MetricsGuiMetric(
            "End Drawing (frame wait)", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric devUiTimeMetric = MetricsGuiMetric(
            "DevUI Draw Prep", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
    MetricsGuiMetric devUiRenderTimeMetric = MetricsGuiMetric(
            "DevUI Render", "us", MetricsGuiMetric::USE_SI_UNIT_PREFIX);

    MetricsGuiPlot plot;
//    plot.AddMetric(&frameTimeMetric);
    plot.AddMetric(&inputTimeMetric);
    plot.AddMetric(&simulationTimeMetric);
    plot.AddMetric(&drawPrepTimeMetric);
    plot.AddMetric(&renderTimeMetric);
    plot.AddMetric(&endDrawingMetric);
    plot.AddMetric(&devUiTimeMetric);
    plot.AddMetric(&devUiRenderTimeMetric);
    plot.mMinPlotHeight      = 250.0f;
    plot.mMinPlotWidth       = 535.0f;
    plot.mBarRounding        = 0.f;    // amount of rounding on bars
    plot.mRangeDampening     = 0.95f;  // weight of historic range on axis range [0,1]
    plot.mInlinePlotRowCount = 2;      // height of DrawList() inline plots, in text rows
    plot.mPlotRowCount       = 5;      // height of DrawHistory() plots, in text rows
    plot.mVBarMinWidth       = 1;      // min width of bar graph bar in pixels
    plot.mVBarGapWidth       = 0;      // width of bar graph inter-bar gap in pixels
    plot.mShowAverage        = false;  // draw horizontal line at series average
    plot.mShowInlineGraphs   = true;  // show history plot in DrawList()
    plot.mShowOnlyIfSelected = false;  // draw show selected metrics
    plot.mShowLegendDesc     = true;   // show series description in legend
    plot.mShowLegendColor    = true;   // use series color in legend
    plot.mShowLegendUnits    = true;   // show units in legend values
    plot.mShowLegendAverage  = true;  // show series average in legend
    plot.mShowLegendMin      = true;   // show plot y-axis minimum in legend
    plot.mShowLegendMax      = true;   // show plot y-axis maximum in legend
    plot.mBarGraph           = true;  // use bars to draw history
    plot.mStacked            = true;  // stack series when drawing history
    plot.mSharedAxis         = false;  // use first series' axis range
    plot.mFilterHistory      = true;   // allow single plot point to represent more than on history value
//    plot.m

    devUIState.plot = &plot;
    float frameTimeSum = 0.0f;

    TraceLog(LOG_INFO, "Starting simulation");
    while (!exitWindow)
    {
        //Input
        //----------------------------------------------------------------------------------
        time_point<steady_clock> inputStart = high_resolution_clock::now();
        microseconds inputElapsed;
        exitWindow = WindowShouldClose();

        if (!devUIState.igIO->WantCaptureMouse)
        {
            //only take mouse input in the game if imgui isn't catching it
            //clicks and input shouldn't go through a window
        }
        if (!devUIState.igIO->WantCaptureKeyboard)
        {
            //check keyboard inputs
        }
        if (!devUIState.igIO->WantTextInput)
        {
            //for on mobile & consoles
        }
        inputElapsed = duration_cast<microseconds>(high_resolution_clock::now() - inputStart);

        //Simulation
        //----------------------------------------------------------------------------------
        time_point<steady_clock> simulationStart = high_resolution_clock::now();
        microseconds simulationElapsed;
        simulationElapsed = duration_cast<microseconds>(high_resolution_clock::now() - simulationStart);

        // Draw prep
        //----------------------------------------------------------------------------------
        time_point<steady_clock> drawPrepStart = high_resolution_clock::now();
        BeginDrawing();
        ClearBackground(WHITE);

        DrawTexture(santaTex,
                    screenWidth / 2 - santaTex.width / 2,
                    screenHeight / 2 - santaTex.height / 2, WHITE);

        Vector2 textDim = MeasureTextEx(fontRobotoMono, BLISS_FULL_HEADER,
                                        (float)fontRobotoMono.baseSize, 0);
        Vector2 textPos;
        textPos.x = ((float)screenWidth - textDim.x) / 2.0f;
        textPos.y = ((float)screenHeight - textDim.y) / 2.0f;

        DrawTextEx(fontRobotoMono, BLISS_FULL_HEADER, textPos,
                   (float)fontRobotoMono.baseSize, 0, DARKGRAY);
        microseconds drawPrepElapsed = duration_cast<microseconds>(high_resolution_clock::now() - drawPrepStart);

        //----------------------------------------------------------------------------------
        //Dev UI

        time_point<steady_clock> devUiStart = high_resolution_clock::now();
        microseconds devUiElapsed;

        DevUINewFrame();
        DevUIDraw(&devUIState);

        ImVec2 vec2Zero = {0.0f, 0.0f};
        ImVec2 vec2One = {1.0f, 1.0f};
        ImVec4 vec4Zero = {0.0f, 0.0f, 0.0f, 0.0f};
        ImVec4 vec4One = {1.0f, 1.0f, 1.0f, 1.0f};
        ImVec2 size;
        size.x = (float)santaTex.width / 4.0f;
        size.y = (float)santaTex.height / 4.0f;
        if (ImGui::Begin("santa img", nullptr, 0)) {
            ImGui::Image((ImTextureID) &santaTex, size, vec2Zero, vec2One, vec4One, vec4Zero);

        }
        ImGui::End();

        logGui.Draw("Log");
        devUiElapsed = duration_cast<microseconds>(high_resolution_clock::now() - devUiStart);

        time_point<steady_clock> renderStart = high_resolution_clock::now();
        rlglDraw();
        microseconds renderElapsed = duration_cast<microseconds>(high_resolution_clock::now() - renderStart);
        time_point<steady_clock> devUiRenderStart = high_resolution_clock::now();
        DevUIRender();
        microseconds devUiRenderElapsed = duration_cast<microseconds>(high_resolution_clock::now() - devUiRenderStart);

        time_point<steady_clock> endDrawingStart = high_resolution_clock::now();
        EndDrawing();
        microseconds endDrawingElapsed = duration_cast<microseconds>(high_resolution_clock::now() - endDrawingStart);

        frameTimeSum += GetFrameTime();
        if (frameTimeSum > (1.0f/60.0f)) {
            frameTimeSum = 0.0f;
            //update metrics
            inputTimeMetric.AddNewValue((float) inputElapsed.count());
            simulationTimeMetric.AddNewValue((float) simulationElapsed.count());
            drawPrepTimeMetric.AddNewValue((float) drawPrepElapsed.count());
            renderTimeMetric.AddNewValue((float) renderElapsed.count());
            endDrawingMetric.AddNewValue((float) endDrawingElapsed.count());
            devUiTimeMetric.AddNewValue((float) devUiElapsed.count());
            devUiRenderTimeMetric.AddNewValue((float) devUiRenderElapsed.count());
//        frameTimeMetric.AddNewValue(GetFrameTime() * 1000000);
            plot.UpdateAxes();
        }
    }

    TraceLog(LOG_INFO, "Unloading fonts...");
    UnloadFont(fontRobotoMono);
    UnloadFont(fontRobotoMonoSm);
    UnloadFont(font);

    TraceLog(LOG_INFO, "Unloading textures...");
    UnloadTexture(santaTex);

    TraceLog(LOG_INFO, "Unloading devUi...");
    DevUIDestroy(&devUIState);

    CloseWindow();
    return 0;
}

#define LOG_LINE_BUFFER_SIZE 1024
char logLineBuffer[LOG_LINE_BUFFER_SIZE] = {};
void LogCallbackFunc(int logType, const char *text, va_list args)
{
    char logTypeStr[10] = {};
    switch (logType)
    {
        case LOG_TRACE: strcpy_s(logTypeStr,10, "TRACE: "); break;
        case LOG_DEBUG: strcpy_s(logTypeStr,10, "DEBUG: "); break;
        case LOG_INFO: strcpy_s(logTypeStr, 10,"INFO: "); break;
        case LOG_WARNING: strcpy_s(logTypeStr, 10, "WARNING: "); break;
        case LOG_ERROR: strcpy_s(logTypeStr, 10, "ERROR: "); break;
        case LOG_FATAL: strcpy_s(logTypeStr, 10, "FATAL: "); break;
        default: break;
    }
    logGui.AddLog("%s", logTypeStr);
    printf("%s", logTypeStr);

    vsprintf_s(logLineBuffer, LOG_LINE_BUFFER_SIZE, text, args);

    logGui.AddLog("%s", logLineBuffer);
    logGui.AddLog("\n");
    printf("%s", logLineBuffer);
    printf("\n");
}