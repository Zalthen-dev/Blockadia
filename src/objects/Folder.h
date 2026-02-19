#pragma once

#include "Instance.h"

struct Folder : Instance {
	std::string Name = "Folder";

	const char* ClassName() const override {
        return "Folder";
    }
};