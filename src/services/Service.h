#pragma once

#include "objects/Instance.h"

struct Service : Instance {
    Service() {
        Name = "Service";
		ParentingLocked = true;
    }

    const char* ClassName() const override {
        return "Service";
    }
};
