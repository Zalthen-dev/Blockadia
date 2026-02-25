#pragma once

inline static float 	signf(float x) { return (x > 0) - (x < 0); }

inline static double 	lerp(double a, double b, double t) { return a + (b - a) * t; }
inline static float 	lerp(float a, float b, float t) { return a + (b - a) * t; }
inline static int 		lerp(int a, int b, int t) { return a + (b - a) * t; }

inline static int 		roundceil(float x) { return (int)(x + 1.0f); }
//inline static int 		round(float x) { return (int)(x + 0.5f); }
inline static int 		roundfloor(float x) { return (int)x; }
