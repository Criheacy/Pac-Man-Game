#include "LTexture.h"

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mRenderer = NULL;

	mWidth = 0;
	mHeight = 0;

	mTileWidth = 0;
	mTileHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	Free();
}

SDL_Surface* ChangeColor(SDL_Surface* surface, Uint8 sourceR, Uint8 sourceG, Uint8 sourceB,
	Uint8 setR, Uint8 setG, Uint8 setB)
{
	SDL_LockSurface(surface);

	Uint32* s = (Uint32*)surface->pixels;

	int width = surface->w;
	int height = surface->h;

	long lPitch = surface->pitch >> 2;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Uint32 pixelColor = (Uint32)s[lPitch * i + j];
			if ((pixelColor & 0xFFFFFF) == ((Uint32)(sourceR) | (Uint32)(sourceG << 8) | (Uint32)(sourceB << 16)))
			{
				s[lPitch * i + j] = (((pixelColor & 0xFF000000)) | ((Uint32)(setR) | (Uint32)(setG << 8) | (Uint32)(setB << 16)));
			}
		}
	}
	SDL_UnlockSurface(surface);
	
	return surface;
}

bool LTexture::LoadFromFile(SDL_Renderer* mainRenderer, std::string path,
	Uint8 fromR, Uint8 fromG, Uint8 fromB,
	Uint8 setR, Uint8 setG, Uint8 setB)
{
	//Get rid of preexisting texture
	Free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		if (fromR != 0xFF || fromG != 0xFF || fromB != 0xFF ||
			setR != 0xFF || setG != 0xFF || setB != 0xFF)
		{
			loadedSurface = ChangeColor(loadedSurface, fromR, fromG, fromB, setR, setG, setB);
		}

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(mainRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mRenderer = mainRenderer;
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::LoadFromRenderedText(SDL_Renderer* mainRenderer, std::string textureText, TTF_Font* mainFont, SDL_Color textColor)
{
	//Get rid of preexisting texture
	Free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(mainFont, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(mainRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;

			mRenderer = mainRenderer;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}

bool LTexture::SetColorModeTo(Uint8 r, Uint8 g, Uint8 b)
{
//	printf("Color Mode Settled!\n");
	if (!SDL_SetTextureColorMod(mTexture, r, g, b))
	{
		return true;
	}
	return false;
}


void LTexture::Free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::Render(int centerX, int centerY, SDL_Rect* clip, double scale)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { centerX - int(mWidth * scale / 2),
							centerY - int(mHeight * scale / 2),
							int(mWidth * scale), int(mHeight * scale) };

	//Render to screen
	SDL_RenderCopy(mRenderer, mTexture, clip, &renderQuad);
}

// Render certain tile in texture
void LTexture::RenderTile(int centerX, int centerY, int tileNumber, double scale, double angle)
{
	SDL_Rect clipQuad = { mTileWidth * (tileNumber % (mWidth / mTileWidth)),
						  mTileHeight * (tileNumber / (mWidth / mTileWidth)),
						  mTileWidth, mTileHeight };

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { centerX - int(mTileWidth * scale / 2),
							centerY - int(mTileHeight * scale / 2),
							int(mTileWidth * scale), int(mTileHeight * scale) };

	//Render to screen
	SDL_RenderCopyEx(mRenderer, mTexture, &clipQuad, &renderQuad, angle, NULL, SDL_FLIP_NONE);
}

void LTexture::SetTileSize(int width, int height)
{
	mTileWidth = width;
	mTileHeight = height;
}

int LTexture::GetWidth()
{
	return mWidth;
}

int LTexture::GetHeight()
{
	return mHeight;
}