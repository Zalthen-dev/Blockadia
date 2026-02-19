#pragma once

#include <raylib.h>
#include <raymath.h>

#include "objects/Instance.h"

void ReadyRenderer();
void UnreadyRenderer();
void EnsureRendererIsReady();

void RenderInstance(Instance* inst, const Matrix& parentTransform);
void RenderWorkspace();
void RenderGui();