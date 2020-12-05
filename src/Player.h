#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <utility>

// #include "Ghost.h"
#include "Ground.h"
#include "Sprite.h"

class Ghost;

class Player
{
public:
	Player();
	~Player();

	bool LoadTexture(SDL_Renderer* mainRenderer);

	void SetGhosts(std::vector<Ghost*> ghosts);

	void StartAliveAnimation();
	void Death();

	void StartMove(Ground* mGround, int x, int y, int towards);
	void SetVelocity(double velocity);
	void Freeze();

	void EatFoodAt(int x, int y);

	bool TryTowards(int towards);
	void UpdateState();
	void UpdatePosition();
	void UpdateNextPosition(bool inRecursion = false);

	std::pair<double, double> GetPosition();
	int GetTowards();
	int GetAliveState();

	void Render(int centerX, int centerY, double scale, double angle = 0.0);
	void RenderAtPoint(double xCoord, double yCoord, double scale);
	void RenderAtPosition(double scale);

	// Set XY-coordinate of (0, 0) position
	void SetOriginPoint(int x, int y);

	void ClearNextTowardsStack();

	bool HandleControl(int newTowards);

	int GetFoodEatenScore(bool clear = true);
	int GetSpecialFoodEatenScore(bool clear = true);
	bool GetPowerPelletEaten(bool clear = true);

private:
	Sprite* sPlayer;
	Ground* mGround;
	std::vector<Ghost*> ghostList;

	int originX;
	int originY;

	// 0->alive, 1->death_delay, 2->fading, 3->disappear
	int aliveState;
	Uint32 deathTime;

	// true->moving, false->freeze
	bool motionState;
	bool blocked;

	double motionVelocity;

	double positionX;
	double positionY;

	int lastPositionX;
	int lastPositionY;

	int nextPositionX;
	int nextPositionY;

	Uint32 lastPositionTime;
	Uint32 lastNextControlTime;

	// 0->up, 1->left, 2->down, 3->right
	int towards;
	int nextTowards;
	std::vector<int> nextTowardsStack;
	int nextTowardsMaxSize;

	bool changeTowardsAtLastPosition;

	int foodEatenScoreStack;
	int specialFoodEatenScoreStack;
	bool powerPelletEatenStack;
};

