#pragma once
#include "Ghost.h"

// Red : RU
// targets pac-man himself
class Shadow : public Ghost
{
public:
	Shadow();

	int GetChasingTowards() override;
	int GetFleeingTowards() override;

	void DrawGizmos() override;
};

// Pink : LU
// targets 4 tiles in front of pac-man
class Speedy : public Ghost
{
public:
	Speedy();

	int GetChasingTowards() override;
	int GetFleeingTowards() override;

	void DrawGizmos() override;
};

// Blue : RD
// draw a line from red ghost (Shadow) to a spot two tiles in front of pac-man
// and extending the line with same length
// targets ending position of the line
class BashFul : public Ghost
{
public:
	BashFul();

	void SetShadowGhost(Shadow* shadow);

	int GetChasingTowards() override;
	int GetFleeingTowards() override;

	void DrawGizmos() override;

private:
	Shadow* mShadow;
};

// Orange : LD
// targets pac-man unless the distance to him is less than 8 tiles
// which leads it to left-down home
class Pokey : public Ghost
{
public:
	Pokey();

	int GetChasingTowards() override;
	int GetFleeingTowards() override;

	void DrawGizmos() override;
};