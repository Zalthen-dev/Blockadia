#pragma once

#include <raylib.h>
#include <raymath.h>

#include <cmath>

static Vector3 SunDirFromClock(float clockHours, float maxElevationDeg = 60.0f, float azimuthDeg = 45.0f) {
    // Wrap time into [0, 24)
    float t = fmodf(clockHours, 24.0f);
    if (t < 0.0f) t += 24.0f;

    // Convert time to [-PI, PI]
    // 6 AM -> -PI/2
    // 12 PM -> 0
    // 6 PM -> +PI/2
    float dayAngle = (t - 12.0f) * (PI / 12.0f);

    // Elevation curve
    // sin() gives nice sunrise/sunset behavior
    float elevation = sinf(dayAngle);

    // Sun below horizon → clamp (night)
    elevation = fmaxf(elevation, -0.05f);

    float maxElevRad = maxElevationDeg * DEG2RAD;
    float elevRad = elevation * maxElevRad;

    float azRad = azimuthDeg * DEG2RAD;

    float ce = cosf(elevRad);
    float se = sinf(elevRad);
    float ca = cosf(azRad);
    float sa = sinf(azRad);

    // Direction FROM sun → scene
    Vector3 dir = {
        ce * ca,
        -se,
        ce * sa
    };

    return Vector3Normalize(dir);
}
