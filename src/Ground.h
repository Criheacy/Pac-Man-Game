#pragma once
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <queue>

#include "LTexture.h"
#include "Settings.h"

class Ground
{
public:
	Ground();
	~Ground();
	
	bool LoadTexture(SDL_Renderer* mainRenderer);
	bool LoadMap(std::string path, int mapWidth, int mapHeight);
	bool AnalizeMap();
	bool AllowcateFood();
	void Render(int centerX, int centerY, double scale = 1);

	bool WinStateCheck();
	bool SetSpecialFoodType(int foodType);
	bool AllowcateSpecialFood();

	std::pair<int, int> GetNearValidPoint(int x, int y);
	int HowToGo(int fromX, int fromY, int goalX, int goalY);

	bool GetWall(int x, int y);
	bool SetWall(int x, int y, bool value);

	int GetFood(int x, int y);
	bool SetFood(int x, int y, int value);

	int EatSpecialFoodAt(int x, int y);
	void ClearSpecialFood();

	int GetCorner(int x, int y);
	bool SetCorner(int x, int y, int value);

private:
	LTexture* tWall;
	LTexture* tFood;
	int mWidth;
	int mHeight;
	int foodNumber;

	Uint32 lastAllowcateFoodTime;

	int specialFoodType;
	int specialFoodX;
	int specialFoodY;
	bool hasSpecialFood;
	int allowcateSpecialFoodCount;
	std::vector<int> specialFoodPoint;

	int specialFoodTime;
	Uint32 lastSpecialFoodTime;

	std::vector<bool> map;
	std::vector<int> foodMap;
	std::vector<int> cornerMap;
};
