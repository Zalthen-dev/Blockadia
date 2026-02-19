#pragma once

static float signf(float x) {
    return (x > 0) - (x < 0);
}

static double lerp(double a, double b, double t) {
	return a + (b - a) * t;
}

static float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

static int lerpi(int a, int b, int t) {
	return a + (b - a) * t;
}
