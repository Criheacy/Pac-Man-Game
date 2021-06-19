#pragma once
#include "LTexture.h"

class LFont
{
public:
	LFont();
	~LFont();

	bool LoadTexture(SDL_Renderer* mainRenderer, std::string fontPath, Uint8 r = 0xFF, Uint8 g = 0xFF, Uint8 b = 0xFF);

	void SetOrigin(int x, int y);
	void SetUnitSize(int uWidth, int uHeight);
	void SetTileSize(int tWidth, int tHeight);

	std::pair<int, int> ConvertToPosition(int x, int y);

	void RenderChar(char ch, int centerPixelX, int centerPixelY, double scale);
	void RenderString(std::string text, int centerPixelX, int centerPixelY, double scale);
	void RenderStringAt(std::string text, int centerCoordX, int centerCoordY, double scale);

private:
	SDL_Renderer* mRenderer;
	LTexture* tFont;

	int originX;
	int originY;

	// Tile size (tile size in raw texture)
	int tWidth;
	int tHeight;

	// Unit size (unit ground coordinate size -> pixel)
	int uWidth;
	int uHeight;
};