#pragma once

#include <cstdio>
#include <raylib.h>
#include "GuiObject.h"

struct Frame : public Cloneable<Frame, GuiObject> {
	std::string Name = "Frame";

	const char* ClassName() const override {
		return "Frame";
	}
};