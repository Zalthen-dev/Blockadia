#pragma once

#include "Service.h"

struct StarterGui : Service {
    StarterGui() {
        Name = "StarterGui";
    }

    const char* ClassName() const override {
        return "StarterGui";
    }
};
