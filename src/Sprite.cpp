#include "Sprite.h"

#include <cmath>

Sprite::Sprite()
{
	tTile = new LTexture();
	nowTile = 0;

	animationState = false;
	infinityLoop = false;

	lastStartTime = 0;
	timePerRound = 0;

	tileList.clear();
}

Sprite::~Sprite()
{
	if (tTile != NULL)
	{
		tTile->Free();
		tTile = NULL;
	}
	nowTile = 0;
	animationState = false;
	lastStartTime = 0;
	timePerRound = 0;

	tileList.clear();
}

bool Sprite::LoadTexture(SDL_Renderer* mainRenderer, std::string path, Uint8 setR, Uint8 setG, Uint8 setB)
{
	if (tTile == NULL)
	{
		tTile = new LTexture();
	}
	return tTile->LoadFromFile(mainRenderer, path, 0x80, 0x80, 0x80, setR, setG, setB);
}

void Sprite::SetColorModeTo(Uint8 r, Uint8 g, Uint8 b)
{
	if (tTile != NULL)
	{
		tTile->SetColorModeTo(r, g, b);
	}
}

void Sprite::SetTileSize(int width, int height)
{
	if (tTile != NULL)
	{
		tTile->SetTileSize(width, height);
	}
}

void Sprite::Render(int centerX, int centerY, double scale, double angle)
{
	UpdateNowTile();
	if (tTile != NULL)
	{
		tTile->RenderTile(centerX, centerY, nowTile, scale, angle);
	}
}

bool Sprite::SetStaticTile(int tile, bool force)
{
	if (animationState)
	{
		if (force)
		{
			TerminateAnimation();
		}
		else
		{
			return false;
		}
	}
	nowTile = tile;
	return true;
}

bool Sprite::SetAnimation(int fromTile, int toTile, Uint32 timePerRound, bool infinityLoop)
{
	std::vector<int> _tileList;

	_tileList.clear();
	if (fromTile > toTile)
	{
		// Invalid tile number
		throw "Invalid Tile List";
		return false;
	}
	for (int i = fromTile; i <= toTile; i++)
	{
		_tileList.push_back(i);
	}

	return SetAnimation(_tileList, timePerRound, infinityLoop);
}

bool Sprite::SetAnimation(std::vector<int> tileList, Uint32 timePerRound, bool infinityLoop)
{
	if (tileList == this->tileList)
	{
		return false;
	}
	// Terminate previous animation
	if (animationState)
	{
		TerminateAnimation();
	}

	animationState = true;
	this->lastStartTime = SDL_GetTicks();
	this->infinityLoop = infinityLoop;
	this->timePerRound = timePerRound;
	this->tileList.clear();
	this->tileList.assign(tileList.begin(), tileList.end());
	return true;
}

void Sprite::UpdateNowTile()
{
	if (!animationState) 
		return;
	Uint32 nowTime = SDL_GetTicks();
	double rate = (nowTime - lastStartTime) * 1.0 / timePerRound;

	if (rate >= 1)
	{
		if (infinityLoop)
		{
			lastStartTime += timePerRound;
		}
		else
		{
			nowTile = tileList[tileList.size() - 1];
			TerminateAnimation();
		}
	}
	else
	{
		nowTile = tileList[int(floor(rate * tileList.size()))];
	}
}

bool Sprite::GetAnimationState()
{
	return animationState;
}

bool Sprite::StopAnimation()
{
	if (animationState)
	{
		UpdateNowTile();
		animationState = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool Sprite::TerminateAnimation()
{
	if (animationState)
	{
		animationState = false;
		return true;
	}
	else
	{
		return false;
	}
}