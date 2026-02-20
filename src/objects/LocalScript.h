#pragma once

#include "BaseScript.h"

struct LocalScript : BaseScript {
	std::string Name = "LocalScript";

	const char* ClassName() const override {
        return "LocalScript";
    }
};