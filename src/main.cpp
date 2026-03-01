#include "assets/ImageData.h"
#include "objects/Instance.h"
#include "objects/Sound.h"
#include "raylib.h"

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#include "core/CameraController.h"
#include "raymath.h"

#include "core/LuaScheduler.h"

#include "datatypes/LuaSignal.h"

#include "core/Game.h"
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

#include "core/ScriptingAPI.h"
#include "core/Rendering.h"

#include "utils/rayutils.h"

void UpdateDescendantSoundStreams(Instance* inst) {
	if (std::strcmp(inst->ClassName(), "Sound") == 0) {
		ObjectSound* sound = (ObjectSound*)inst;
		sound->Update();
	}

	if (inst->Children.size() > 0) {
		for (Instance* child : inst->Children) {
			UpdateDescendantSoundStreams(child);
		}
	}
}

struct LuauScriptData {
	std::string name;
	std::string content;
};

std::vector<LuauScriptData> luauScripts;

void CollectLuauScriptData() {
	FilePathList list = LoadDirectoryFilesEx(GetWorkingDirectory(), ".luau", false);
	for (int i = 0; i < list.count; i++) {
		std::string fileData = LoadFileText(list.paths[i]);
		luauScripts.push_back({GetFileName(list.paths[i]), fileData});
	}
}

static int RunLoaderMenu() {
	CollectLuauScriptData();
	if (luauScripts.size() <= 1) {
		return -1;
	}

	Vector2 optSize = {500, 50};
	int optOutline = 4;
	int padding = 20;

	int choice = 0;
	int selected = 0;
	int optionCount = luauScripts.size();

	Image cardImage = LoadImageFromMemory(".png", cardData, sizeof(cardData));
	Color* cardColors = LoadImageColors(cardImage);
	Color topbarColor = cardColors[cardImage.width - 1];
	UnloadImageColors(cardColors);

	Texture card = LoadTextureFromImage(cardImage);

	bool confirm = false;
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_DOWN)) selected = (selected + 1) % optionCount;
		if (IsKeyPressed(KEY_UP)) selected = (selected - 1 + optionCount) % optionCount;

		BeginDrawing();
		ClearBackground(DARKBLUE);

		DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE, DARKBLUE);
		
		DrawRectangle(0, 0, GetScreenWidth(), card.height, topbarColor);
		DrawTexture(card, 0, 0, WHITE);

		DrawText("Select a Luau file below to execute", padding, card.height + 10, 20, WHITE);
		DrawText("UP/DOWN or MOUSE to change selection, ENTER or CLICK to confirm", padding, card.height + 30, 20, WHITE);

		for (int i = 0; i < optionCount; i++) {
			LuauScriptData scriptData = luauScripts[i];

			Color bg = (i == selected) ? DARKGRAY : WHITE;
			Color fg = (i == selected) ? YELLOW : DARKBLUE;

			int x = padding;
			int y = (card.height + 70) + ((optSize.y + 5 + (optOutline*2)) * i);

			DrawRectangle(x - optOutline, y - optOutline, optSize.x + (optOutline*2), optSize.y + (optOutline*2), fg);
			DrawRectangle(x, y, optSize.x, optSize.y, bg);

			DrawText(TextFormat("%d.", i+1), x + 20, y + (optSize.y/2) - 10, 20, fg);
			DrawText(TextFormat("%s", scriptData.name.data()), x + 100, y + (optSize.y/2) - 10, 20, fg);

			Rectangle rect = {
				(float)x - optOutline,
				(float)y - optOutline,
				optSize.x + (optOutline*2),
				optSize.y + (optOutline*2),
			};
			
			if (IsMouseInRectangle(rect)) {
				selected = i;

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					confirm = true;
				}
			}
		}

		EndDrawing();

		if (IsKeyPressed(KEY_ENTER)) {
			confirm = true;
		}

		if (confirm) {
			choice = selected;
			break;
		}
	}

	UnloadTexture(card);

	return choice;
}

void SetupGame() {
	gLuaScheduler.L = luaL_newstate();
	luaL_openlibs(gLuaScheduler.L);

	gGame = new Game{};

	gWorkspace = new Workspace{}; gWorkspace->SetParent(gGame);
	gLighting = new Lighting{}; gLighting->SetParent(gGame);
	gStarterGui = new StarterGui{}; gStarterGui->SetParent(gGame);
	gRunService = new RunService{}; gRunService->SetParent(gGame);
	gDebugVisualService = new DebugVisualService{}; gDebugVisualService->SetParent(gGame);

	SetScriptingAPI(gLuaScheduler.L);
}

void ShutdownGame() {
	gGame->Destroy();

	lua_close(gLuaScheduler.L);
}

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1600, 900, "Blockadia");
	SetTargetFPS(60);

	

	Image icon = LoadImageFromMemory(".png", iconData, 534);
	SetWindowIcon(icon);

	int choice = -1;
	choice = RunLoaderMenu();

	// close early so no need for unnecessary stuff
	if (WindowShouldClose()) {
		CloseWindow();
		return 0;
	}

	InitAudioDevice();

	Camera3D camera;
	camera.position = {0, 5, -5};
	camera.target = {0, 5, 0};
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = {0, 1, 0};
	camera.fovy = 70;

	CameraController camController;
	camController.camera = &camera;

	SetupGame();

	if (choice != -1 && choice < luauScripts.size()) {
		LuauScriptData scriptData = luauScripts[choice];

		gMainScript = new BaseScript{}; gMainScript->SetParent(gWorkspace);
		gMainScript->Name = "Script";

		std::string luauScript = scriptData.content.data();
		gMainScript->Source = luauScript;

		StartScript(gLuaScheduler, gMainScript);
	}

	ReadyRenderer();
	while(!WindowShouldClose()) {
		float frameTime = GetFrameTime();

		BeginDrawing();
		ClearBackground(BLUE);
		
		gLuaScheduler.Step();
		UpdateDescendantSoundStreams(gGame);
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
	ShutdownGame();

	CloseAudioDevice();
	CloseWindow();
	return 0;
}