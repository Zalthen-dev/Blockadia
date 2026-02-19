#pragma once

#include "Service.h"

struct Workspace : Service {
    Workspace() {
        Name = "Workspace";
    }

    const char* ClassName() const override {
        return "Workspace";
    }
};
