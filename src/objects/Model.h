#pragma once

#include "Instance.h"

struct Model : Instance {
	std::string Name = "Model";

	const char* ClassName() const override {
        return "Model";
    }
};