#pragma once

#include <raylib.h>
#include "GuiObject.h"

struct Frame : GuiObject {
	std::string Name = "Frame";

	const char* ClassName() const override {
        return "Frame";
    }
};