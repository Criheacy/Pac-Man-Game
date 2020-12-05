#include "LFont.h"

LFont::LFont()
{
	mRenderer = NULL;
	tFont = NULL;
	originX = 0;
	originY = 0;
	uWidth = 0;
	uHeight = 0;
	tWidth = 0;
	tHeight = 0;
}

LFont::~LFont()
{
	mRenderer = NULL;
	if (tFont != NULL)
	{
		delete tFont;
		tFont = NULL;
	}
	originX = 0;
	originY = 0;
}

bool LFont::LoadTexture(SDL_Renderer* mainRenderer, std::string fontPath, Uint8 r, Uint8 g, Uint8 b)
{
	if (tFont != NULL)
	{
		delete tFont;
		tFont = NULL;
	}
	tFont = new LTexture();
	return tFont->LoadFromFile(mainRenderer, fontPath, 0xFF, 0xFF, 0xFF, r, g, b);
}

void LFont::SetOrigin(int x, int y)
{
	originX = x;
	originY = y;
}

void LFont::SetUnitSize(int uWidth, int uHeight)
{
	this->uWidth = uWidth;
	this->uHeight = uHeight;
}

void LFont::SetTileSize(int tWidth, int tHeight)
{
	if (tFont == NULL) return;
	this->tWidth = tWidth;
	this->tHeight = tHeight;
	tFont->SetTileSize(tWidth, tHeight);
}

std::pair<int, int> LFont::ConvertToPosition(int x, int y)
{
	int centerX = originX + int(x * uWidth);
	int centerY = originY + int(y * uHeight);
	return std::make_pair(centerX, centerY);
}

void LFont::RenderChar(char ch, int centerPixelX, int centerPixelY, double scale)
{
	if (tFont == NULL) return;
	if (int(ch) >= 0x00 && int(ch) <= 0x7F)
	{
		tFont->RenderTile(centerPixelX, centerPixelY, int(ch), scale);
	}
}

void LFont::RenderString(std::string text, int centerPixelX, int centerPixelY, double scale)
{
	if (tFont == NULL) return;
	int len = strlen(text.c_str());
	int mX = int(((len + 1.0) / 2.0) * tWidth);
	for (int i = 0; i < len; i++)
	{
		RenderChar(text[i], centerPixelX + int((- mX + i * tWidth) * scale), centerPixelY, scale);
	}
}

void LFont::RenderStringAt(std::string text, int centerCoordX, int centerCoordY, double scale)
{
	std::pair<int, int> convertedPos = ConvertToPosition(centerCoordX, centerCoordY);

	RenderString(text, convertedPos.first, convertedPos.second, scale);
}