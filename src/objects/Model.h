#pragma once

#include "Instance.h"

struct ObjectModel : public Cloneable<ObjectModel, Instance> {
	std::string Name = "Model";

	const char* ClassName() const override {
		return "Model";
	}
};