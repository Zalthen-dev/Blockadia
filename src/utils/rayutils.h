#pragma once

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

inline Color GetFPSTextColorEx(Color good, Color mid, Color bad) {
	int fps = GetFPS();
	Color color = good;

	if (fps < 30) {
		if (fps < 15) {
			color = bad;
		} else {
			color = mid;
		}
	}

	return color;
}

inline Color GetFPSTextColor() {
	return GetFPSTextColorEx(LIME, YELLOW, RED);
}

inline const char* GetFPSTextEx(const char* format) {
	return TextFormat(format, GetFPS());
}

inline const char* GetFPSText() {
	return GetFPSTextEx("%2i FPS");
}

inline void DrawTextC(Font font, const char *text, int posX, int posY, int fontSize, Color color) {
	Vector2 position = { (float)posX, (float)posY };

	int defaultFontSize = 10;   // Default Font chars height in pixel
	if (fontSize < defaultFontSize) fontSize = defaultFontSize;
	int spacing = fontSize/defaultFontSize;
	
	DrawTextEx(font, text, position, fontSize, spacing, color);
}

inline void DrawFPSPro(Font font, int posX, int posY, int fontSize, Color good, Color mid, Color bad) {
	int fps = GetFPS();

	Color color = good;
	if (fps < 30) {
		if (fps < 15) {
			color = bad;
		} else {
			color = mid;
		}
	}

	DrawTextC(font, TextFormat("%2i FPS", fps), posX, posY, fontSize, color);
}

inline void DrawFPSEx(int posX, int posY, int fontSize, Color good, Color mid, Color bad) {
	DrawFPSPro(GetFontDefault(), posX, posY, fontSize, good, mid, bad);
}

inline void DrawFPSSize(int posX, int posY, int fontSize) {
	DrawFPSEx(posX, posY, fontSize, LIME, YELLOW, RED);
}

inline void DrawTextOutlined(const char *text, int posX, int posY, int fontSize, int outlineSize, Color textColor, Color outlineColor) {
	DrawText(text, posX - outlineSize, posY, fontSize, outlineColor);
	DrawText(text, posX + outlineSize, posY, fontSize, outlineColor);
	DrawText(text, posX, posY - outlineSize, fontSize, outlineColor);
	DrawText(text, posX, posY + outlineSize, fontSize, outlineColor);

	DrawText(text, posX - outlineSize, posY - outlineSize, fontSize, outlineColor);
	DrawText(text, posX + outlineSize, posY - outlineSize, fontSize, outlineColor);
	DrawText(text, posX - outlineSize, posY + outlineSize, fontSize, outlineColor);
	DrawText(text, posX + outlineSize, posY + outlineSize, fontSize, outlineColor);

	DrawText(text, posX, posY, fontSize, textColor);
}

inline bool IsMouseInRectangle(Rectangle rect) {
	Vector2 mousePos = GetMousePosition();

	return (
		(mousePos.x >= rect.x && mousePos.y >= rect.y) && 
		(mousePos.x <= rect.x + rect.width && mousePos.y <= rect.y + rect.height)
	);
}

static void DrawCornerFan(Vector2 center, float radius, float startAngleDeg, float endAngleDeg, int segments, Vector2 pivot, float rotation) {
	float cosr = cosf(rotation*DEG2RAD);
	float sinr = sinf(rotation*DEG2RAD);

	if (startAngleDeg == endAngleDeg) return;
	if (radius <= 0.0f) return;

	if (endAngleDeg < startAngleDeg) {
		float tmp = startAngleDeg;
		startAngleDeg = endAngleDeg;
		endAngleDeg = tmp;
	}

	float step = (endAngleDeg - startAngleDeg) / segments;
	float angle = startAngleDeg;

	for (int i = 0; i < segments; i++) {
		float a0 = DEG2RAD * angle;
		float a1 = DEG2RAD * (angle + step);

		Vector2 p0 = {center.x + cosf(a0) * radius, center.y + sinf(a0) * radius};
		Vector2 p1 = {center.x + cosf(a1) * radius, center.y + sinf(a1) * radius};

		Vector2 wc = {
			pivot.x + center.x * cosr - center.y * sinr,
			pivot.y + center.x * sinr + center.y * cosr
		};

		Vector2 wp0 = {
			pivot.x + p0.x * cosr - p0.y * sinr,
			pivot.y + p0.x * sinr + p0.y * cosr
		};

		Vector2 wp1 = {
			pivot.x + p1.x * cosr - p1.y * sinr,
			pivot.y + p1.x * sinr + p1.y * cosr
		};

		rlVertex2f(wc.x, wc.y);
		rlVertex2f(wp1.x, wp1.y);
		rlVertex2f(wp0.x, wp0.y);

		angle += step;
    }
}

static int roundedRectangleSegments = 16;

// Rotation is in Degrees
// Origin is not a percentage
inline void advDrawRoundedRotatedRectangle(Rectangle rect, Vector2 origin, float rotation, float rounding, Color color) {
	float sinRotation = sinf(rotation*DEG2RAD);
	float cosRotation = cosf(rotation*DEG2RAD);

	int segments = roundedRectangleSegments;
	if (segments < 1) segments = 1;

	#define ROTATE(v) (Vector2){ pivot.x + v.x * cosRotation - v.y * sinRotation, pivot.y + v.x * sinRotation + v.y * cosRotation }

	Vector2 anchorPoint = {
		rect.width * origin.x,
		rect.height * origin.y
	};

	Vector2 pivot = {
		rect.x + anchorPoint.x,
		rect.y + anchorPoint.y
	};

	Vector2 topLeft = {rect.x, rect.y};
	Vector2 topRight = {rect.x + rect.width, rect.y};
	Vector2 bottomLeft = {rect.x, rect.y + rect.height};
	Vector2 bottomRight = {rect.x + rect.width, rect.y + rect.height};
	
	if (fmod(rotation, 360) != 0) {
		topLeft = (Vector2){-anchorPoint.x, -anchorPoint.y};
		topRight = (Vector2){rect.width - anchorPoint.x, -anchorPoint.y};
		bottomLeft = (Vector2){-anchorPoint.x, rect.height - anchorPoint.y};
		bottomRight = (Vector2){rect.width - anchorPoint.x, rect.height - anchorPoint.y};

		topLeft = ROTATE(topLeft);
		topRight = ROTATE(topRight);
		bottomLeft = ROTATE(bottomLeft);
		bottomRight = ROTATE(bottomRight);
	}

	rlBegin(RL_TRIANGLES);
		if (rounding <= 0) {
			rlColor4ub(color.r, color.g, color.b, color.a);

			rlVertex2f(topLeft.x, topLeft.y);
			rlVertex2f(bottomLeft.x, bottomLeft.y);
			rlVertex2f(bottomRight.x, bottomRight.y);

			rlVertex2f(topLeft.x, topLeft.y);
			rlVertex2f(bottomRight.x, bottomRight.y);
			rlVertex2f(topRight.x, topRight.y);
		} else {
			rounding = Clamp(rounding, 0.0f, 1.0f);

			float minSize = rect.width < rect.height ? rect.width : rect.height;
			float radius = rounding * minSize * 0.5f;

			float l = -anchorPoint.x;
			float r = rect.width - anchorPoint.x;
			float t = -anchorPoint.y;
			float b = rect.height - anchorPoint.y;

			float il = l + radius;
			float ir = r - radius;
			float it = t + radius;
			float ib = b - radius;

			Vector2 cTL = {-anchorPoint.x + radius, -anchorPoint.y + radius};
			Vector2 cTR = {rect.width - anchorPoint.x - radius, -anchorPoint.y + radius};
			Vector2 cBR = {rect.width - anchorPoint.x - radius, rect.height - anchorPoint.y - radius};
			Vector2 cBL = {-anchorPoint.x + radius, rect.height - anchorPoint.y - radius};

			// Center Rectangle
			Vector2 mTL = ROTATE(cTL);
			Vector2 mTR = ROTATE(cTR);
			Vector2 mBR = ROTATE(cBR);
			Vector2 mBL = ROTATE(cBL);

			rlColor4ub(color.r, color.g, color.b, color.a);
			rlVertex2f(mTL.x, mTL.y);
			rlVertex2f(mBL.x, mBL.y);
			rlVertex2f(mBR.x, mBR.y);

			rlVertex2f(mTL.x, mTL.y);
			rlVertex2f(mBR.x, mBR.y);
			rlVertex2f(mTR.x, mTR.y);

			// Top Rectangle
			Vector2 tl = ROTATE(((Vector2){il, t}));
			Vector2 tr = ROTATE(((Vector2){ir, t}));
			Vector2 br = ROTATE(((Vector2){ir, it}));
			Vector2 bl = ROTATE(((Vector2){il, it}));

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(bl.x, bl.y);
			rlVertex2f(br.x, br.y);

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(br.x, br.y);
			rlVertex2f(tr.x, tr.y);

			// Bottom Rectangle
			tl = ROTATE(((Vector2){il, ib}));
			tr = ROTATE(((Vector2){ir, ib}));
			br = ROTATE(((Vector2){ir, b}));
			bl = ROTATE(((Vector2){il, b}));

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(bl.x, bl.y);
			rlVertex2f(br.x, br.y);

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(br.x, br.y);
			rlVertex2f(tr.x, tr.y);

			// Left Rectangle
			tl = ROTATE(((Vector2){l, it}));
			tr = ROTATE(((Vector2){il, it}));
			br = ROTATE(((Vector2){il, ib}));
			bl = ROTATE(((Vector2){l, ib}));

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(bl.x, bl.y);
			rlVertex2f(br.x, br.y);

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(br.x, br.y);
			rlVertex2f(tr.x, tr.y);

			// Right Rectangle
			tl = ROTATE(((Vector2){ir, it}));
			tr = ROTATE(((Vector2){r, it}));
			br = ROTATE(((Vector2){r, ib}));
			bl = ROTATE(((Vector2){ir, ib}));

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(bl.x, bl.y);
			rlVertex2f(br.x, br.y);

			rlVertex2f(tl.x, tl.y);
			rlVertex2f(br.x, br.y);
			rlVertex2f(tr.x, tr.y);

			// Rounded corners
			DrawCornerFan(cTL, radius, 180.0f, 270.0f, segments, pivot, rotation);
			DrawCornerFan(cTR, radius, 270.0f, 360.0f, segments, pivot, rotation);
			DrawCornerFan(cBR, radius,   0.0f,  90.0f, segments, pivot, rotation);
			DrawCornerFan(cBL, radius,  90.0f, 180.0f, segments, pivot, rotation);
		}
	rlEnd();

	#undef ROTATE
}

// Rotation is in Degrees
// Origin is not a percentage
inline void advDrawRotatedRectangle(Rectangle rect, Vector2 origin, float rotation, Color color) {
	advDrawRoundedRotatedRectangle(rect, origin, rotation, 0, color);
}

inline void advDrawRectangle(Rectangle rect, Color color) {
	advDrawRoundedRotatedRectangle(rect, Vector2Zero(), 0, 0, color);
}