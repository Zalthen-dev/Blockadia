#pragma once

#include <cstdio>
#include <raylib.h>
#include "GuiObject.h"

struct Frame : GuiObject {
	std::string Name = "Frame";

	const char* ClassName() const override {
        return "Frame";
    }

	void Draw(Rectangle rect, Color color) override {
		rect.x += rect.width*0.5f;
		rect.y += rect.height*0.5f;

		DrawRectanglePro(rect, {rect.width*0.5f, rect.height*0.5f}, Rotation, color);
	}
};