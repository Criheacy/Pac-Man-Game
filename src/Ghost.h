#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Sprite.h"
#include "Player.h"
#include "Ground.h"
#include "Settings.h"

class Ghost
{
public:
	Ghost();
	~Ghost();

	bool LoadTexture(SDL_Renderer* mainRenderer, Uint8 r = 0xFF, Uint8 g = 0xFF, Uint8 b = 0xFF);
	bool LoadPlayer(Player* mainPlayer);

	void StartAliveAnimation();

	void StartMove(Ground* mGround, int towards);
	void InitializeInfo(double chasingVelocity, double fleeingVelocity, double rebornVelocity,
		int fleeingTime, int scatteringTime, int preparingTime);
	void Freeze();

	void UpdateState();
	void SetState(int state);

	void SetTowards(int towards);
	int GetRandomTowards();
	void UpdateNextTowards();
	virtual int GetChasingTowards();
	virtual int GetFleeingTowards();

	void BumpIntoPlayer(bool player_immune = false);

	void UpdatePosition();
	void UpdateNextPosition();

	std::pair<double, double> GetPosition();
	int GetTowards();
	double GetVelocity();

	void Render(int centerX, int centerY, double scale, double angle = 0.0);
	void RenderAtPoint(double xCoord, double yCoord, double scale);
	void RenderAtPosition(double scale);

	// Set XY-coordinate of (0, 0) position
	void SetOriginPoint(int x, int y);

	// Get how many ghosts are eaten by player since last called
	int GetGhostEaten(bool clear = true);

	// For debugging
	virtual void DrawGizmos();

protected:
	Sprite* sGhost;
	Player* sPlayer;
	Ground* mGround;

	int originX;
	int originY;

	int rebornX;
	int rebornY;

	int homeX;
	int homeY;

	// 0->preparing 1->chasing 2->scattering 3->frightening 4->be_eaten
	int state;

	// true->moving, false->freeze
	bool motionState;

	double positionX;
	double positionY;

	int lastPositionX;
	int lastPositionY;

	int nextPositionX;
	int nextPositionY;

	double chasingVelocity;
	double fleeingVelocity;
	double rebornVelocity;

	Uint32 lastPositionTime;

	Uint32 lastPrepareTime;
	int preparingTime;

	Uint32 lastChaseTime;
	int chasingTime;

	Uint32 lastScatterTime;
	int scatteringTime;

	Uint32 lastFleeTime;
	int fleeingTime;

	Uint32 lastRebornTime;
	int reborningTime;

	// 0->up, 1->left, 2->down, 3->right
	int towards;
	bool towardsAnimationLock;

	// when reborning ghost reaches center position
	bool rebornFlag;

	int ghostEatenStack;
};