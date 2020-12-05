#include "Player.h"
#include "Ghost.h"
#include "Settings.h"

Player::Player()
{
	sPlayer = new Sprite();
	mGround = NULL;

	originX = 0;
	originY = 0;

	aliveState = 0;
	deathTime = 0;

	motionState = false;
	motionVelocity = 0;
	blocked = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;
	
	lastPositionTime = 0;
	lastNextControlTime = 0;

	towards = 3;
	nextTowards = -1;
	nextTowardsStack.clear();
	nextTowardsMaxSize = 2;

	changeTowardsAtLastPosition = false;

	foodEatenScoreStack = 0;
	specialFoodEatenScoreStack = 0;
	powerPelletEatenStack = false;
}

Player::~Player()
{
	if (sPlayer != NULL)
	{
		delete sPlayer;
		sPlayer = NULL;
	}
}

bool Player::LoadTexture(SDL_Renderer* mainRenderer)
{
	if (sPlayer == NULL)
	{
		sPlayer = new Sprite();
	}
	if (sPlayer->LoadTexture(mainRenderer, "Assets/pacman.png"))
	{
		sPlayer->SetTileSize(16, 16);
		return true;
	}
	else
	{
		return false;
	}
}

void Player::SetGhosts(std::vector<Ghost*> ghosts)
{
	ghostList.assign(ghosts.begin(), ghosts.end());
}

void Player::StartAliveAnimation()
{
	aliveState = 0;
	sPlayer->SetAnimation(0x00, 0x07, Uint32(8000 / PLAYER_ANIMATION_SPEED), true);
}

void Player::Death()
{
	if (aliveState == 0)
	{
		aliveState = 1;
		motionState = false;
		deathTime = SDL_GetTicks();
		sPlayer->StopAnimation();
		sPlayer->SetStaticTile(0x00);
	}
}

void Player::StartMove(Ground* mGround, int x, int y, int towards)
{
	blocked = false;
	motionState = true;
	this->mGround = mGround;

	lastPositionX = x;
	lastPositionY = y;
	positionX = x;
	positionY = y;
	this->towards = towards;

	UpdateNextPosition();
	lastPositionTime = SDL_GetTicks();
}

void Player::SetVelocity(double velocity)
{
	this->motionVelocity = velocity;
}

void Player::Freeze()
{
	motionState = false;
}

void Player::EatFoodAt(int x, int y)
{
	int specialFoodPoint = mGround->EatSpecialFoodAt(x, y);
	if (specialFoodPoint != 0)
	{
		specialFoodEatenScoreStack += specialFoodPoint;
	}
	if (mGround->GetFood(x, y) == 2)
	{
		foodEatenScoreStack += 50;
		powerPelletEatenStack = true;
		for (auto ghost = ghostList.begin(); ghost != ghostList.end(); ghost++)
		{
			(*ghost)->SetState(3);
		}
	}
	else if (mGround->GetFood(x, y) == 1)
	{
		foodEatenScoreStack += 10;
	}
	mGround->SetFood(x, y, 0);
}

bool Player::TryTowards(int towards)
{
	int dx = 0;
	int dy = 0;
	switch (towards)
	{
	case 0: dy = -1; break;
	case 1: dx = -1; break;
	case 2: dy = 1; break;
	case 3: dx = 1; break;
	}
	if (mGround->GetWall(lastPositionX + dx, lastPositionY + dy))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Player::UpdateState()
{
	Uint32 nowTicks = SDL_GetTicks();

	switch (aliveState)
	{
	case 0:
		break;
	case 1:
		if (nowTicks - deathTime > PLAYER_DELAY_AFTER_DEATH)
		{
			aliveState = 2;
			sPlayer->SetAnimation(0x08, 0x0F, Uint32(8000 / PLAYER_FADING_ANIMATION_SPEED));
		}
		break;
	case 2:
		if (!(sPlayer->GetAnimationState()))
		{
			aliveState = 3;
		}
		break;
	}
}

void Player::UpdatePosition()
{
	if (blocked || !motionState)
	{
		return;
	}
	Uint32 nowTicks = SDL_GetTicks();
	if (nowTicks - lastPositionTime >= Uint32(1000 / motionVelocity))
	{
		lastPositionX = nextPositionX;
		lastPositionY = nextPositionY;

		positionX = nextPositionX;
		positionY = nextPositionY;

		UpdateNextPosition();
	}
	else
	{
		double rate = (nowTicks * 1.0 - lastPositionTime) / int(1000.0 / motionVelocity);
		positionX = (1 - rate) * lastPositionX + rate * nextPositionX;
		positionY = (1 - rate) * lastPositionY + rate * nextPositionY;
	}
}

void Player::UpdateNextPosition(bool inRecursion)
{
	// If out of edge
	if (lastPositionX >= MAP_WIDTH - 1) lastPositionX = 0;
	else if (lastPositionX <= 0) lastPositionX = MAP_WIDTH - 1;
	if (lastPositionY >= MAP_HEIGHT - 1) lastPositionY = 0;
	else if (lastPositionY <= 0) lastPositionY = MAP_HEIGHT - 1;

	EatFoodAt(lastPositionX, lastPositionY);
	// Try stored previous control

	if (SDL_GetTicks() - lastNextControlTime <= Uint32(PLAYER_TURNING_TOLERANCE / motionVelocity))
	{
		if (nextTowards != -1 && !inRecursion)
		{
			int preTowards = towards;
			towards = nextTowards;
			nextTowards = -1;
			UpdateNextPosition(true);

			if (blocked == false)
			{
				nextTowardsStack.erase(nextTowardsStack.begin());
				if (!nextTowardsStack.empty())
				{
					nextTowards = nextTowardsStack.front();
				}
				else
				{
					nextTowards = -1;
				}

				if (towards != preTowards)
				{
					changeTowardsAtLastPosition = true;
				}
				else
				{
					changeTowardsAtLastPosition = false;
				}
				return;
			}
			else
			{
				towards = preTowards;
				nextTowards = nextTowardsStack.front();
			}
		}
	}

	// No previous control / is in previous control
	int dx = 0;
	int dy = 0;
	switch (towards)
	{
	case 0: dy = -1; break;
	case 1: dx = -1; break;
	case 2: dy = 1; break;
	case 3: dx = 1; break;
	}
	if (mGround->GetWall(lastPositionX + dx, lastPositionY + dy))
	{
		// Unwalkable
		blocked = true;
		nextPositionX = lastPositionX;
		nextPositionY = lastPositionY;

		// Not in recursion trying but still get in wall
		if (!inRecursion)
		{
			// Clear previous control
			nextTowards = -1;
			nextTowardsStack.clear();
			changeTowardsAtLastPosition = false;
		}
	}
	else
	{
		blocked = false;
		lastPositionTime += int(1000 / motionVelocity);
		nextPositionX = lastPositionX + dx;
		nextPositionY = lastPositionY + dy;
		if (!inRecursion)
		{
			changeTowardsAtLastPosition = false;
		}
	}
}

std::pair<double, double> Player::GetPosition()
{
	return std::make_pair(positionX, positionY);
}

int Player::GetTowards()
{
	return towards;
}

int Player::GetAliveState()
{
	return aliveState;
}

void Player::Render(int centerX, int centerY, double scale, double angle)
{
	if (sPlayer != NULL)
	{
		UpdateState();
		if (aliveState == 3)
		{
			// Player disappear
		}
		else
		{
			sPlayer->Render(centerX, centerY, scale, angle);
		}
	}
}

void Player::RenderAtPoint(double xCoord, double yCoord, double scale)
{
	int centerX = SCREEN_WIDTH / 2 + int(xCoord * WALL_WIDTH * scale);
	int centerY = SCREEN_HEIGHT / 2 + int(yCoord * WALL_HEIGHT * scale);
	Render(centerX, centerY, scale);
}

void Player::RenderAtPosition(double scale)
{
	UpdatePosition();

	int centerX = originX + int(positionX * WALL_WIDTH * scale);
	int centerY = originY + int(positionY * WALL_HEIGHT * scale);

	double angle = 0.0;

	if (towards == 0x00) angle = 90;
	if (towards == 0x01) angle = 0;
	if (towards == 0x02) angle = -90;
	if (towards == 0x03) angle = 180;

	Render(centerX, centerY, scale, angle);
}

void Player::SetOriginPoint(int x, int y)
{
	originX = x;
	originY = y;
}

void Player::ClearNextTowardsStack()
{
	nextTowardsStack.clear();
}

bool Player::HandleControl(int newTowards)
{
	if (!motionState)
	{
		return false;
	}
	if (blocked)
	{
		towards = newTowards;
		UpdateNextPosition();
		lastPositionTime = SDL_GetTicks();
		changeTowardsAtLastPosition = true;
		return blocked;
	}
	
	else
	{
		// Stores next towards as tolerance
		lastNextControlTime = SDL_GetTicks();
		if (int(nextTowardsStack.size()) < nextTowardsMaxSize)
		{
			nextTowardsStack.push_back(newTowards);
		}
		nextTowards = nextTowardsStack.front();
		return false;
	}
}

int Player::GetFoodEatenScore(bool clear)
{
	int result = foodEatenScoreStack;
	if (clear) foodEatenScoreStack = 0;
	return result;
}

int Player::GetSpecialFoodEatenScore(bool clear)
{
	int result = specialFoodEatenScoreStack;
	if (clear) specialFoodEatenScoreStack = 0;
	return result;
}

bool Player::GetPowerPelletEaten(bool clear)
{
	bool result = powerPelletEatenStack;
	if (clear) powerPelletEatenStack = false;
	return result;
}