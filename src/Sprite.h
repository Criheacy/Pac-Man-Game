#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <vector>

#include "LTexture.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	bool LoadTexture(SDL_Renderer* mainRenderer, std::string path, Uint8 setR = 0xFF, Uint8 setG = 0xFF, Uint8 setB = 0xFF);

	void SetTileSize(int width, int height);
	void SetColorModeTo(Uint8 r, Uint8 g, Uint8 b);
	void Render(int centerX, int centerY, double scale, double angle = 0.0);

	// Animation
	bool SetStaticTile(int tile, bool force = false);

	bool SetAnimation(int fromTile, int toTile, Uint32 timePerRound, bool infinityLoop = false);
	bool SetAnimation(std::vector<int> tileList, Uint32 timePerRound, bool infinityLoop = false);

	void UpdateNowTile();

	bool GetAnimationState();

	// Update last frame and stop animation
	bool StopAnimation();

	// Immediatly stop animation (without updating)
	bool TerminateAnimation();


private:
	LTexture* tTile;
	int nowTile;

	bool animationState;
	bool infinityLoop;

	Uint32 lastStartTime;
	Uint32 timePerRound;
	std::vector<int> tileList;

};