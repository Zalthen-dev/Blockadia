#pragma once
#include <raylib.h>

class CameraController {

public:
	Camera *camera;

	float speed = 5.f;
	float speedShiftMultiplier = 0.5f;
	float mouseSensitivity = 1.f;
	float rotateSpeed = 1.f;
	float scrollSpeed = 1.f;

	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetUpward();
	void StepCamera();
};