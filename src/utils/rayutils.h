#pragma once

#include <raylib.h>

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