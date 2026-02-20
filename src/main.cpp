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
#include "services/Workspace.h"
#include "services/Lighting.h"
#include "services/StarterGui.h"
#include "services/RunService.h"

#include "objects/BaseScript.h"

Game* gGame = nullptr;
Workspace* gWorkspace = nullptr;
Lighting* gLighting = nullptr;
StarterGui* gStarterGui = nullptr;
RunService* gRunService = nullptr;
BaseScript* gMainScript = nullptr;

LuaScheduler gLuaScheduler;

#include "ScriptingAPI.h"
#include "Rendering.h"

void DrawTextC(Font font, const char *text, int posX, int posY, int fontSize, Color color) {
	Vector2 position = { (float)posX, (float)posY };

	int defaultFontSize = 10;   // Default Font chars height in pixel
	if (fontSize < defaultFontSize) fontSize = defaultFontSize;
	int spacing = fontSize/defaultFontSize;
	
	DrawTextEx(font, text, position, fontSize, spacing, color);
}

void DrawFPSPro(Font font, int posX, int posY, int fontSize, Color good, Color mid, Color bad) {
	int fps = GetFPS();

	Color color = good;
	if (fps < 30) {
		if (fps < 15) {
			color = bad;
		} else {
			color = mid;
		}
	}

	DrawTextC(font, TextFormat("%2i FPS", fps), posX, posY, fontSize, color);
}

void DrawFPSEx(int posX, int posY, int fontSize, Color good, Color mid, Color bad) {
	DrawFPSPro(GetFontDefault(), posX, posY, fontSize, good, mid, bad);
}

void DrawFPSSize(int posX, int posY, int fontSize) {
	DrawFPSEx(posX, posY, fontSize, LIME, YELLOW, RED);
}

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

	gGame = new Game{};

	gWorkspace = new Workspace{}; gWorkspace->SetParent(gGame);
	gLighting = new Lighting{}; gLighting->SetParent(gGame);
	gStarterGui = new StarterGui{}; gStarterGui->SetParent(gGame);
	gRunService = new RunService{}; gRunService->SetParent(gGame);

	gMainScript = new BaseScript{}; gMainScript->SetParent(gWorkspace);
	gMainScript->Name = "Script";

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	gLuaScheduler.L = L;

	SetScriptingAPI(L);

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


		// TODO: push frameTime and fire with 1 argument once signals are fixed
		gRunService->RenderStepped.Fire(0);
		RenderWorkspace();

		EndMode3D();
		RenderGui();

		DrawFPSSize(10, 10, 20);
		DrawText("Who knows how many parts are there", 10, 30, 20, RAYWHITE);

		EndDrawing();
	}

	UnreadyRenderer();

	gGame->Destroy();
	
	lua_close(L);
	CloseWindow();
	return 0;
}