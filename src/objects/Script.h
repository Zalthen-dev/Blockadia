#pragma once

#include "BaseScript.h"

struct Script : public Cloneable<Script, BaseScript> {
	std::string Name = "Script";

	const char* ClassName() const override {
		return "Script";
	}
};