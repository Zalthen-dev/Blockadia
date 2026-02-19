#include "Rendering.h"

#include <cstring>
#include <rlgl.h>

#include "assets/GLSLShaders.h"

#include "datatypes/LuaVector2.h"
#include "datatypes/LuaUDim.h"

#include "objects/GuiObject.h"
#include "objects/Instance.h"
#include "objects/Part.h"
#include "objects/ScreenGui.h"
#include "raylib.h"
#include "services/Workspace.h"
#include "services/Lighting.h"
#include "services/StarterGui.h"

#include "utils/VecMath.h"

// config options

static float vMinRenderDistance = 0.05f;
static float vMaxRenderDistance = 50000.f;

static float vShadowRange = 500.f;
static int vShadowResolution = 1024;
static const int vShadowMapCascadeCount = 3;
static float vOutlineStrength = 0.f;

// end config options

extern Workspace* gWorkspace;
extern Lighting* gLighting;
extern StarterGui* gStarterGui;

bool rendererReady = false;
static RenderTexture shadowMaps[vShadowMapCascadeCount];
static Material material;
static Mesh meshCube;
static Mesh meshBall;
static Shader gBasicShader;
static Shader gDepthShader;

// helpers gui

static Vector2 ComputeAbsoluteSize(const LuaUDim2& size, const Vector2& parentSize) {
    return {
        size.x.scale * parentSize.x + size.x.offset,
        size.y.scale * parentSize.y + size.y.offset
    };
}

static Vector2 ComputeAbsolutePosition(const LuaUDim2& pos,const Vector2& parentSize,const Vector2& absSize,const LuaVector2& anchor) {
    Vector2 p = {pos.x.scale * parentSize.x + pos.x.offset, pos.y.scale * parentSize.y + pos.y.offset};

    // Apply anchor
    p.x -= absSize.x * anchor.x;
    p.y -= absSize.y * anchor.y;

    return p;
}

// helpers rendering

void CreateShadowMaps() {
	for (int i = 0; i < vShadowMapCascadeCount; i++) {
		shadowMaps[i] = LoadRenderTexture(vShadowResolution, vShadowResolution);
	}
}

void ClearShadowMaps() {
	for (int i = 0; i < 3; i++) {
		UnloadRenderTexture(shadowMaps[i]);
	}
}

void ReadyRenderer() {
	rlSetClipPlanes(vMinRenderDistance, vMaxRenderDistance);

	meshCube = GenMeshCube(1, 1, 1);
	meshBall = GenMeshSphere(0.5, 16, 16);

	material = LoadMaterialDefault();
	material.maps[MATERIAL_MAP_ALBEDO].color = WHITE;

	gBasicShader = LoadShaderFromMemory(GLSL_BASIC_VERT, GLSL_BASIC_FRAG);
	gBasicShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(gBasicShader, "viewPos");

	material.shader = gBasicShader;

	CreateShadowMaps();
	rendererReady = true;
}

void UnreadyRenderer() {
	rendererReady = false;
	ClearShadowMaps();
	UnloadMesh(meshCube);
	//UnloadMaterial(material);
	if (gBasicShader.id) UnloadShader(gBasicShader);
}

void EnsureRendererIsReady() {
	if (!rendererReady) ReadyRenderer();
}

void RenderInstance(Instance* inst, const Matrix& parentTransform) {
    Matrix local = parentTransform;

    if (Part* part = dynamic_cast<Part*>(inst)) {
        Matrix transform = MatrixScale(
            part->Size.x,
            part->Size.y,
            part->Size.z
        );

        transform = MatrixMultiply(
            transform,
            MatrixRotateXYZ((Vector3){
                part->Rotation.x * DEG2RAD,
                part->Rotation.y * DEG2RAD,
                part->Rotation.z * DEG2RAD
            })
        );

        transform = MatrixMultiply(
            transform,
            MatrixTranslate(
                part->Position.x,
                part->Position.y,
                part->Position.z
            )
        );

        local = MatrixMultiply(parentTransform, transform);

		material.maps[MATERIAL_MAP_ALBEDO].color = part->color;
        DrawMesh(meshCube, material, local);
    }

    for (Instance* child : inst->Children) {
        RenderInstance(child, local);
    }
}

void RenderWorkspace() {
	Vector3 sunDir = SunDirFromClock(gLighting->ClockTime);
	Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
	Vector3 ambient = {0.5f, 0.5f, 0.5f};
	
	BeginShaderMode(gBasicShader);
	SetShaderValue(gBasicShader, GetShaderLocation(gBasicShader, "lightDir"), &sunDir, SHADER_UNIFORM_VEC3);
	SetShaderValue(gBasicShader, GetShaderLocation(gBasicShader, "lightColor"), &lightColor, SHADER_UNIFORM_VEC3);
	SetShaderValue(gBasicShader, GetShaderLocation(gBasicShader, "ambientColor"), &ambient, SHADER_UNIFORM_VEC3);
	
	RenderInstance(gWorkspace, MatrixIdentity());

	EndShaderMode();
}

void RenderGuiObject(GuiObject* object, const Vector2& parentPos, const Vector2& parentSize) {
    if (!object->Visible)
        return;

    // Compute layout
    Vector2 absSize = ComputeAbsoluteSize(object->Size, parentSize);
    Vector2 absPos  = ComputeAbsolutePosition(object->Position, parentSize, absSize, object->AnchorPoint);

    // Final position in screen space
    Vector2 drawPos = {
        parentPos.x + absPos.x,
        parentPos.y + absPos.y
    };

    // Draw background
    float alpha = 1.0f - object->BackgroundTransparency;
    Color color = {
        (unsigned char)(object->BackgroundColor.x * 255),
        (unsigned char)(object->BackgroundColor.y * 255),
        (unsigned char)(object->BackgroundColor.z * 255),
        (unsigned char)(alpha * 255)
    };

    DrawRectangle(
        (int)drawPos.x,
        (int)drawPos.y,
        (int)absSize.x,
        (int)absSize.y,
        color
    );

    // Render children
    for (Instance* childInst : object->Children) {
        GuiObject* child = dynamic_cast<GuiObject*>(childInst);
        if (child && child->Visible) {
            RenderGuiObject(child, drawPos, absSize);
        }
    }
}

void RenderScreenGui(ScreenGui* screenGui) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Vector2 rootPos  = { 0, 0 };
    Vector2 rootSize = { (float)screenW, (float)screenH };

    for (Instance* inst : screenGui->Children) {
        GuiObject* obj = dynamic_cast<GuiObject*>(inst);
        if (obj && obj->Visible) {
            RenderGuiObject(obj, rootPos, rootSize);
        }
    }
}

void RenderGui() {
    for (Instance* inst : gStarterGui->Children) {
        ScreenGui* gui = dynamic_cast<ScreenGui*>(inst);
        if (gui && gui->Enabled) {
            RenderScreenGui(gui);
        }
    }
}