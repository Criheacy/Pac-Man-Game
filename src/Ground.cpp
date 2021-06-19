#include "Ground.h"
#include "Gizmo.h"

extern Gizmo* gizmo;

Ground::Ground()
{
	tWall = NULL;
	tFood = NULL;
	mWidth = 0;
	mHeight = 0;
	foodNumber = 0;

	lastAllowcateFoodTime = 0;
	specialFoodType = 0;
	hasSpecialFood = false;
	allowcateSpecialFoodCount = 0;
	specialFoodTime = 15000;
	lastSpecialFoodTime = 0;

	specialFoodX = 10;
	specialFoodY = 15;

	specialFoodPoint.clear();
	specialFoodPoint.push_back(100);
	specialFoodPoint.push_back(300);
	specialFoodPoint.push_back(500);
	specialFoodPoint.push_back(700);
	specialFoodPoint.push_back(1000);
	specialFoodPoint.push_back(2000);
	specialFoodPoint.push_back(3000);
	specialFoodPoint.push_back(5000);

	map.clear();
	cornerMap.clear();
}

Ground::~Ground()
{
	if (tWall != NULL)
	{
		tWall->Free();
		tWall = NULL;
	}
	if (tFood != NULL)
	{
		tFood->Free();
		tFood = NULL;
	}
	mWidth = 0;
	mHeight = 0;
	map.clear();
}

bool Ground::LoadTexture(SDL_Renderer* mainRenderer)
{
	tWall = NULL;
	tFood = NULL;

	tWall = new LTexture();
	if (!tWall->LoadFromFile(mainRenderer, "res/image/wall.png"))
		return false;
	tWall->SetTileSize(WALL_WIDTH, WALL_HEIGHT);

	tFood = new LTexture();
	if (!tFood->LoadFromFile(mainRenderer, "res/image/food.png"))
		return false;
	tFood->SetTileSize(FOOD_WIDTH, FOOD_HEIGHT);

	return true;
}

bool Ground::LoadMap(std::string path, int mapWidth, int mapHeight)
{
	std::ifstream mapFile(path.c_str());
	if (mapFile.fail())
	{
		printf("Unable to load map file!\n");
		return false;
	}
	int totalTile = mapWidth * mapHeight;

	map.resize(totalTile);

	mWidth = mapWidth;
	mHeight = mapHeight;
	int tileType = 0;
	for (int j = 0; j < mHeight; j++)
	{
		for (int i = 0; i < mWidth; i++)
		{
			mapFile >> tileType;

			if (mapFile.fail())
			{
				printf("Error loading map: Unexpected end of file!\n");
				mapFile.close();
				return false;
			}

			if (tileType != 0)
			{
				// is wall
				SetWall(i, j, true);
			}
			else
			{
				// not a wall
				SetWall(i, j, false);
			}
		}
	}
	mapFile.close();
	return (AnalizeMap() && AllowcateFood());
}

bool Ground::AnalizeMap()
{
	cornerMap.clear();
	cornerMap.resize(mWidth * mHeight);

	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			if (GetWall(i, j))
			{
				SetCorner(i, j, -1);
			}
			else if (abs(i - (mWidth - 1.0) / 2) < 2.2 && abs(j - (mHeight - 1.0) / 2) < 1.8)
			{
				SetCorner(i, j, 5);
			}
			else
			{
				int totalCorner = 0;
				if (!GetWall(i, j - 1)) totalCorner++;
				if (!GetWall(i - 1, j)) totalCorner++;
				if (!GetWall(i, j + 1)) totalCorner++;
				if (!GetWall(i + 1, j)) totalCorner++;
				SetCorner(i, j, totalCorner);
			}
		}
	}

	return true;
}

bool Ground::AllowcateFood()
{
	foodNumber = 0;
	foodMap.clear();
	foodMap.resize(mWidth * mHeight);

	lastAllowcateFoodTime = SDL_GetTicks();

	double midX = (mWidth - 1) / 2;
	double midY = (mHeight - 1) / 2;

	int startX = 0;
	int startY = 0;

	std::pair<int, int> startPos = GetNearValidPoint(startX, startY);
	startX = startPos.first;
	startY = startPos.second;

	std::queue<std::pair<int, int> > que;
	std::map<std::pair<int, int>, bool> inque;

	que.push(std::make_pair(startX, startY));

	std::pair<int, int> _now;
	int _x; int _y;
	int dx[] = { 0,-1,0,1 };
	int dy[] = { -1,0,1,0 };

	while (!que.empty())
	{
		_now = que.front();
		que.pop();
		_x = _now.first;
		_y = _now.second;

		if (!GetWall(_x, _y) && 
			((abs(_x - midX) >= 4.2 || abs(_y - midY) >= 4.2)
			&& (abs(_x - midX) <= 5.2 || abs(_y - midY) >= 1.2)))
		{
			SetFood(_x, _y, 1);
		}
		for (int i = 0; i < 4; i++)
		{
			if (_x + dx[i] >= 0 && _x + dx[i] < mWidth && _y + dy[i] >= 0
				&& _y + dy[i] < mHeight && !GetWall(_x + dx[i], _y + dy[i])
				&& !inque[std::make_pair(_x + dx[i], _y + dy[i])])
			{
				que.push(std::make_pair(_x + dx[i], _y + dy[i]));
				inque[std::make_pair(_x + dx[i], _y + dy[i])] = true;
			}
		}
	}

	SetFood(1, 3, 2);
	SetFood(19, 3, 2);

	SetFood(1, 19, 2);
	SetFood(19, 19, 2);

	return true;
}

void Ground::Render(int centerX, int centerY, double scale)
{
	AllowcateSpecialFood();

	int LU_X = centerX - int(mWidth * scale * WALL_WIDTH / 2);		// left-up x
	int LU_Y = centerY - int(mHeight * scale * WALL_HEIGHT / 2);	// left-up y

	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			// Draw Debug corners
			/*
			if (GetCorner(i, j) > 2)
			{
				gizmo->SetGizmoColor(0xFF, 0x00, 0x00);
				gizmo->DebugRect(i, j, 8, false);
			}
			*/
			if (GetWall(i, j))
			{
				int tileCode = 0;
				if (GetWall(i, j - 1)) tileCode |= 1;
				if (GetWall(i - 1, j)) tileCode |= 2;
				if (GetWall(i, j + 1)) tileCode |= 4;
				if (GetWall(i + 1, j)) tileCode |= 8;

				if (GetWall(i - 1, j - 1) && GetWall(i + 1, j - 1) && (tileCode & 2) && (tileCode & 8)) tileCode &= ~1;
				if (GetWall(i - 1, j - 1) && GetWall(i - 1, j + 1) && (tileCode & 1) && (tileCode & 4)) tileCode &= ~2;
				if (GetWall(i + 1, j + 1) && GetWall(i - 1, j + 1) && (tileCode & 2) && (tileCode & 8)) tileCode &= ~4;
				if (GetWall(i + 1, j + 1) && GetWall(i + 1, j - 1) && (tileCode & 1) && (tileCode & 4)) tileCode &= ~8;

				// From 0 to mWidth-1, fix value is +0.5
				tWall->RenderTile(LU_X + int((i + 0.5) * WALL_WIDTH * scale),
					LU_Y + int((j + 0.5) * WALL_HEIGHT * scale), tileCode, scale);
			}
			else if (GetFood(i, j) == 1)
			{
				tFood->RenderTile(LU_X + int((i + 0.5) * WALL_WIDTH * scale),
					LU_Y + int((j + 0.5) * WALL_HEIGHT * scale), 0x00, scale);
			}
			else if (GetFood(i, j) == 2)
			{
				if (SDL_GetTicks() / 200 % 2 == 0)
				{
					tFood->RenderTile(LU_X + int((i + 0.5) * WALL_WIDTH * scale),
						LU_Y + int((j + 0.5) * WALL_HEIGHT * scale), 0x01, scale);
				}
			}

			if (specialFoodX == i && specialFoodY == j && hasSpecialFood)
			{
				tFood->RenderTile(LU_X + int((i + 0.5) * WALL_WIDTH * scale),
					LU_Y + int((j + 0.5) * WALL_HEIGHT * scale), 0x04 + specialFoodType, scale);
			}
		}
	}
}

bool Ground::WinStateCheck()
{
	return (foodNumber == 0);
}

bool Ground::SetSpecialFoodType(int foodType)
{
	if (foodType >= 0 && foodType < int(specialFoodPoint.size()))
	{
		allowcateSpecialFoodCount = 0;
		specialFoodType = foodType;
		specialFoodTime = 15000 - specialFoodType * 1000;
		return true;
	}
	else
	{
		specialFoodType = 0;
		return false;
	}
}

bool Ground::AllowcateSpecialFood()
{
	Uint32 nowTicks = SDL_GetTicks();
	if (int(nowTicks - lastAllowcateFoodTime >= 10000)
		&& allowcateSpecialFoodCount < 1)
	{
		if (GetFood(specialFoodX, specialFoodY) == 0)
		{
			hasSpecialFood = true;
			lastSpecialFoodTime = nowTicks;
		}
		allowcateSpecialFoodCount++;
		return true;
	}
	if (int(nowTicks - lastAllowcateFoodTime >= 30000)
		&& allowcateSpecialFoodCount < 2
		&& GetFood(specialFoodX, specialFoodY) == 0)
	{
		if (GetFood(specialFoodX, specialFoodY) == 0)
		{
			hasSpecialFood = true;
			lastSpecialFoodTime = nowTicks;
		}
		allowcateSpecialFoodCount++;
		return true;
	}
	if (int(nowTicks - lastAllowcateFoodTime >= 50000)
		&& allowcateSpecialFoodCount < 3
		&& GetFood(specialFoodX, specialFoodY) == 0
		&& specialFoodType <= 5)
	{
		if (GetFood(specialFoodX, specialFoodY) == 0)
		{
			hasSpecialFood = true;
			lastSpecialFoodTime = nowTicks;
		}
		allowcateSpecialFoodCount++;
		return true;
	}

	if (hasSpecialFood && int(nowTicks - lastSpecialFoodTime) > specialFoodTime)
	{
		hasSpecialFood = false;
	}
	return false;
}

std::pair<int, int> Ground::GetNearValidPoint(int x, int y)
{
	if (x < 0) x = 0;
	else if (x >= MAP_WIDTH) x = MAP_WIDTH - 1;
	if (y < 0) y = 0;
	else if (y >= MAP_HEIGHT) y = MAP_HEIGHT - 1;

	std::queue<std::pair<int, int> > que;
	std::map<std::pair<int, int>, bool> inque;

	que.push(std::make_pair(x, y));

	std::pair<int, int> _now;
	int _x;
	int _y;

	while (!que.empty())
	{
		_now = que.front();
		que.pop();
		_x = _now.first;
		_y = _now.second;

		int dx[] = { 0,-1,0,1 };
		int dy[] = { -1,0,1,0 };

		if (!GetWall(_x, _y))
		{
			return std::make_pair(_x, _y);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				if (_x + dx[i] >= 0 && _x + dx[i] < mWidth && _y + dy[i] >= 0
					&& _y + dy[i] < mHeight && !inque[std::make_pair(_x + dx[i], _y + dy[i])])
				{
					que.push(std::make_pair(_x + dx[i], _y + dy[i]));
					inque[std::make_pair(_x + dx[i], _y + dy[i])] = true;
				}
			}
		}
	}
	return std::make_pair(-1, -1);
}

int Ground::HowToGo(int fromX, int fromY, int goalX, int goalY)
{
	std::pair<int, int> _coord;
	_coord = GetNearValidPoint(fromX, fromY);
	fromX = _coord.first; fromY = _coord.second;
	_coord = GetNearValidPoint(goalX, goalY);
	goalX = _coord.first; goalY = _coord.second;

	if (fromX == goalX && fromY == goalY)
	{
		return 4;
	}

	std::queue<std::pair<int, int> > que;
	std::queue<int> towardsQue;
	std::map<std::pair<int, int>, bool> inque;

	que.push(std::make_pair(fromX, fromY));
	towardsQue.push(-1);

	std::pair<int, int> _now;
	int _x; int _y;
	int nextX = 0; int nextY = 0;
	int _nowTowards;

	while (!que.empty())
	{
		_now = que.front();
		_nowTowards = towardsQue.front();
		que.pop();
		towardsQue.pop();
		_x = _now.first;
		_y = _now.second;

		int dx[] = { 0,-1,0,1 };
		int dy[] = { -1,0,1,0 };

		if (goalX == _x && goalY == _y)
		{
			return _nowTowards;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				nextX = (_x + dx[i] + mWidth) % mWidth;
				nextY = (_y + dy[i] + mHeight) % mHeight;
				if (!GetWall(nextX, nextY) && !inque[std::make_pair(nextX, nextY)])
				{
					que.push(std::make_pair(nextX, nextY));
					inque[std::make_pair(nextX, nextY)] = true;
					if (_nowTowards == -1)
					{
						towardsQue.push(i);
					}
					else
					{
						towardsQue.push(_nowTowards);
					}
				}
			}
		}
	}
	return -1;
}

bool Ground::SetWall(int x, int y, bool value)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return false;
	else map[x * mHeight + y] = value;
	return true;
}

bool Ground::GetWall(int x, int y)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return false;
	else return map[x * mHeight + y];
}

bool Ground::SetFood(int x, int y, int value)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
	{
		return false;
	}
	else
	{
		// if foodstate is not equal to value settle to
		if ((foodMap[x * mHeight + y]) != value && (value == 0 || value == 1))
		{
			if (foodMap[x * mHeight + y] == 0)
			{
				foodNumber++;
			}
			else
			{
				foodNumber--;
			}
		}
		foodMap[x * mHeight + y] = value;
		return true;
	}
}

int Ground::GetFood(int x, int y)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return 0;
	else return foodMap[x * mHeight + y];
}

int Ground::EatSpecialFoodAt(int x, int y)
{
	if (x == specialFoodX && y == specialFoodY && hasSpecialFood)
	{
		hasSpecialFood = false;
		return specialFoodPoint[specialFoodType];
	}
	else
	{
		return 0;
	}
}

void Ground::ClearSpecialFood()
{
	hasSpecialFood = false;
}

bool Ground::SetCorner(int x, int y, int value)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return false;
	else cornerMap[x * mHeight + y] = value;
	return true;
}

int Ground::GetCorner(int x, int y)
{
	if (x < 0 || x >= mWidth || y < 0 || y >= mHeight)
		return -2;
	else return cornerMap[x * mHeight + y];
}