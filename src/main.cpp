#include "datatypes/LuaSignal.h"
#include "raylib.h"

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#include "CameraController.h"
#include "raymath.h"

#include "LuaScheduler.h"

#include "Game.h"
#include "services/Service.h"
#include "services/Workspace.h"
#include "services/Lighting.h"
#include "services/StarterGui.h"
#include "services/RunService.h"
#include "services/DebugVisualService.h"

#include "objects/BaseScript.h"
#include "objects/Part.h"

Game* gGame = nullptr;
Workspace* gWorkspace = nullptr;
Lighting* gLighting = nullptr;
StarterGui* gStarterGui = nullptr;
RunService* gRunService = nullptr;
DebugVisualService* gDebugVisualService = nullptr;
BaseScript* gMainScript = nullptr;

LuaScheduler gLuaScheduler;

#include "ScriptingAPI.h"
#include "Rendering.h"

#include "utils/rayutils.h"

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1600, 900, "Blockadia");
	SetTargetFPS(120);

	Camera3D camera;
	camera.position = {0, 5, -5};
	camera.target = {0, 5, 0};
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = {0, 1, 0};
	camera.fovy = 70;

	CameraController camController;
	camController.camera = &camera;

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	gLuaScheduler.L = L;

	

	gGame = new Game{};

	gWorkspace = new Workspace{}; gWorkspace->SetParent(gGame);
	gLighting = new Lighting{}; gLighting->SetParent(gGame);
	gStarterGui = new StarterGui{}; gStarterGui->SetParent(gGame);
	gRunService = new RunService{}; gRunService->SetParent(gGame);
	gDebugVisualService = new DebugVisualService{}; gDebugVisualService->SetParent(gGame);

	gMainScript = new BaseScript{}; gMainScript->SetParent(gWorkspace);
	gMainScript->Name = "Script";

	SetScriptingAPI(L);

	Part baseplate;
	baseplate.Size = {2048, 16, 2048};
	baseplate.Position = {0, -8, 0};
	baseplate.color = {99, 95, 98, 255};
	baseplate.SetParent(gWorkspace);

	std::string luauScript = LoadFileText("main.luau");
	gMainScript->Source = luauScript;

	StartScript(gLuaScheduler, gMainScript);

	ReadyRenderer();
	while(!WindowShouldClose()) {
		float frameTime = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLUE);
		
		gLuaScheduler.Step();
		camController.StepCamera();
		BeginMode3D(camera);
		
		DrawGrid(10, 1.0f);

		FireSignal(gRunService->RenderStepped, [&](lua_State* L) {
		    lua_pushnumber(L, frameTime);
		});

		RenderWorkspace();
		RenderDebugVisuals3D(gDebugVisualService);

		EndMode3D();
		
		RenderDebugVisuals2D(gDebugVisualService);
		gDebugVisualService->Clear();

		RenderGui();

		DrawTextOutlined(GetFPSText(), 10, 10, 20, 2, GetFPSTextColorEx(GREEN, YELLOW, RED), BLACK);

		EndDrawing();
	}

	UnreadyRenderer();

	gGame->Destroy();
	
	lua_close(L);
	CloseWindow();
	return 0;
}