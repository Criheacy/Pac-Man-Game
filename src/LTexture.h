#pragma once
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class LTexture
{
public:
	LTexture();
	~LTexture();

	//Loads image at specified path
	bool LoadFromFile(SDL_Renderer* mainRenderer, std::string path,
		Uint8 fromR = 0xFF, Uint8 fromG = 0xFF, Uint8 fromB = 0xFF,
		Uint8 setR = 0xFF, Uint8 setG = 0xFF, Uint8 setB = 0xFF);

	bool LoadFromRenderedText(SDL_Renderer* mainRenderer, std::string textureText, TTF_Font* mainFont, SDL_Color textColor);

	bool SetColorModeTo(Uint8 r, Uint8 g, Uint8 b);

	//Deallocates texture
	void Free();

	//Renders texture at given point
	void Render(int centerX, int centerY, SDL_Rect* clip = NULL, double scale = 1.0);
	void RenderTile(int centerX, int centerY, int tileNumber, double scale = 1.0, double angle = 0.0);

	void SetTileSize(int width, int height);

	int GetWidth();
	int GetHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	SDL_Renderer* mRenderer;

	int mTileWidth;
	int mTileHeight;

	//Image dimensions
	int mWidth;
	int mHeight;
};
