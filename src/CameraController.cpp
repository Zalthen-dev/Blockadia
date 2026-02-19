#include "CameraController.h"
#include <raymath.h>
#include <rcamera.h>

void CameraMove(Camera *camera, Vector3 moveDelta) {
	camera->position += moveDelta;
	camera->target += moveDelta;
}

Vector3 CameraController::GetForward() {
	return Vector3Normalize(Vector3Subtract(camera->position, camera->target));
}

Vector3 CameraController::GetRight() {
	Vector3 forward = Vector3Normalize(Vector3Subtract(camera->position, camera->target));
	return Vector3Normalize(Vector3CrossProduct({0, 1, 0}, forward));
}

Vector3 CameraController::GetUpward() {
	Vector3 forward = Vector3Normalize(Vector3Subtract(camera->position, camera->target));
	Vector3 right = Vector3Normalize(Vector3CrossProduct({0, 1, 0}, forward));
	return Vector3Normalize(Vector3CrossProduct(forward, right));
}

void CameraController::StepCamera() {
	float delta = GetFrameTime();

	float cameraMoveSpeed = speed * delta;
    float cameraRotateSpeed = 0.004 * rotateSpeed;

	if (IsKeyDown(KEY_LEFT_SHIFT)) {
		cameraMoveSpeed *= speedShiftMultiplier;
	}

	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
		Vector2 mouseDelta = GetMouseDelta();
		CameraYaw(camera, -mouseDelta.x*cameraRotateSpeed, false);
        CameraPitch(camera, -mouseDelta.y*cameraRotateSpeed, false, false, false);
	}

	float mouseWheelDelta = GetMouseWheelMove();
	if (mouseWheelDelta != 0) {
		Ray ray = GetScreenToWorldRay(GetMousePosition(), *camera);
		CameraMove(camera, ray.direction*mouseWheelDelta*scrollSpeed);
	}

	Vector3 forward = Vector3Normalize(Vector3Subtract(camera->position, camera->target));
	Vector3 right = Vector3Normalize(Vector3CrossProduct({0, 1, 0}, forward));
	Vector3 up = Vector3Normalize(Vector3CrossProduct(forward, right));

	if (IsKeyDown(KEY_W)) CameraMoveForward(camera, cameraMoveSpeed, false);
    if (IsKeyDown(KEY_A)) CameraMoveRight(camera, -cameraMoveSpeed, false);
    if (IsKeyDown(KEY_S)) CameraMoveForward(camera, -cameraMoveSpeed, false);
    if (IsKeyDown(KEY_D)) CameraMoveRight(camera, cameraMoveSpeed, false);
	if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_E)) CameraMove(camera, up*cameraMoveSpeed);
	if (IsKeyDown(KEY_Q)) CameraMove(camera, up*-cameraMoveSpeed);
}