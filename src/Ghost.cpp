#include "Ghost.h"
#include "Settings.h"

Ghost::Ghost()
{
	sGhost = new Sprite();
	sPlayer = NULL;
	mGround = NULL;

	originX = 0;
	originY = 0;

	rebornX = 0;
	rebornY = 0;

	homeX = 0;
	homeY = 0;

	state = 0;

	motionState = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;

	chasingVelocity = 0;
	fleeingVelocity = 0;
	rebornVelocity = 0;

	lastPositionTime = 0;

	lastPrepareTime = 0;
	preparingTime = 0;

	lastChaseTime = 0;
	chasingTime = 0;

	lastScatterTime = 0;
	scatteringTime = 0;
	
	lastFleeTime = 0;
	fleeingTime = 0;

	lastRebornTime = 0;
	reborningTime = 0;

	towards = 0;
	rebornFlag = false;
}

Ghost::~Ghost()
{
	if (sGhost != NULL)
	{
		delete sGhost;
		sGhost = NULL;
	}
}

bool Ghost::LoadTexture(SDL_Renderer* mainRenderer, Uint8 r, Uint8 g, Uint8 b)
{
	if (sGhost == NULL)
	{
		sGhost = new Sprite();
	}
	if (sGhost->LoadTexture(mainRenderer, "res/image/ghost.png", r, g, b))
	{
		sGhost->SetTileSize(16, 16);
//		sGhost->SetColorModeTo(r, g, b);
		return true;
	}
	else
	{
		return false;
	}
}

bool Ghost::LoadPlayer(Player* player)
{
	if (player != NULL)
	{
		sPlayer = player;
		return true;
	}
	else
	{
		return false;
	}
}

void Ghost::StartAliveAnimation()
{
	state = 0;
	SetTowards(0);
}

void Ghost::InitializeInfo(double chasingVelocity, double fleeingVelocity, double rebornVelocity,
	int fleeingTime, int scatteringTime, int preparingTime)
{
	this->chasingVelocity = chasingVelocity;
	this->fleeingVelocity = fleeingVelocity;
	this->rebornVelocity = rebornVelocity;

	this->fleeingTime = fleeingTime;
	this->scatteringTime = scatteringTime;
	this->preparingTime = preparingTime;
}

void Ghost::StartMove(Ground* mGround, int towards)
{
	motionState = true;
	this->mGround = mGround;

	if (preparingTime >= 0)
	{
		lastPositionX = rebornX;
		lastPositionY = rebornY;
		positionX = rebornX;
		positionY = rebornY;
		this->towards = towards;
		SetState(0);
	}
	else
	{
		lastPositionX = 10;
		lastPositionY = 10;
		positionX = lastPositionX;
		positionY = lastPositionY;
		this->towards = 3;
		SetState(1);
	}

	UpdateNextPosition();
	lastPositionTime = SDL_GetTicks();
}

void Ghost::Freeze()
{
	motionState = false;
}

void Ghost::UpdateState()
{
	Uint32 nowTicks = SDL_GetTicks();

	switch (state)
	{
	case 0:
		if (int(nowTicks - lastPrepareTime) >= preparingTime)
		{
			SetState(1);
		}
		break;
	case 1:
		if (int(nowTicks - lastChaseTime) >= chasingTime)
		{
			SetState(2);
		}
		break;
	case 2:
		if (int(nowTicks - lastScatterTime) >= scatteringTime)
		{
			SetState(1);
		}
		break;
	case 3:
		if (int(nowTicks - lastFleeTime) >= fleeingTime)
		{
			towardsAnimationLock = false;
			SetState(1);
		}
		else if (int(nowTicks - lastFleeTime) >= fleeingTime - 3000)
		{
			towardsAnimationLock = true;
			if ((int(nowTicks - lastFleeTime) - (fleeingTime - 3000)) / 300 % 2 == 0)
			{
				sGhost->SetAnimation(0x0E, 0x0F, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
			}
			else
			{
				sGhost->SetAnimation(0x0C, 0x0D, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
			}
		}
		break;
	case 4:
		if (rebornFlag)
		{
			rebornFlag = false;
			SetState(1);
		}
		break;
	}
	SetTowards(GetTowards());
}

void Ghost::SetState(int state)
{
	towardsAnimationLock = false;
	switch (state)
	{
	case 0:
		lastPrepareTime = SDL_GetTicks();
		break;
	case 1:
		lastChaseTime = SDL_GetTicks();
		break;
	case 2:
		lastScatterTime = SDL_GetTicks();
		break;
	case 3:
		lastFleeTime = SDL_GetTicks();
		break;
	case 4:
		lastRebornTime = SDL_GetTicks();
		break;
	}
	SetTowards(GetTowards());
	this->state = state;
}

void Ghost::SetTowards(int towards)
{
	this->towards = towards;
	if (towardsAnimationLock)
	{
		return;
	}
	if (state == 0 || state == 1 || state == 2)
	{
		if (towards == 0)
		{
			sGhost->SetAnimation(0x00, 0x01, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 1)
		{
			sGhost->SetAnimation(0x02, 0x03, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 2)
		{
			sGhost->SetAnimation(0x04, 0x05, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 3)
		{
			sGhost->SetAnimation(0x06, 0x07, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
	}
	else if (state == 3)
	{
		sGhost->SetAnimation(0x0C, 0x0D, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
	}
	else if (state == 4)
	{
		if (towards == 0)
		{
			sGhost->SetAnimation(0x08, 0x08, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 1)
		{
			sGhost->SetAnimation(0x09, 0x09, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 2)
		{
			sGhost->SetAnimation(0x0A, 0x0A, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
		else if (towards == 3)
		{
			sGhost->SetAnimation(0x0B, 0x0B, Uint32(2000 / GHOST_ANIMATION_SPEED), true);
		}
	}
}

int Ghost::GetRandomTowards()
{
	int dx[] = { 0, -1, 0, 1 };
	int dy[] = { -1, 0, 1, 0 };
	for (int i = 0; i <= 3; i++)
	{
		if (!mGround->GetWall(lastPositionX + dx[i], lastPositionY + dy[i]))
		{
			return i;
		}
	}
	return -1;
}

void Ghost::UpdateNextTowards()
{
	int nextTowards = 0;
	switch (state)
	{
	case 0:
		if (GetTowards() == 0) SetTowards(2);
		else SetTowards(0);
		break;
	case 1:
		SetTowards(GetChasingTowards());
		break;
	case 2:
		nextTowards = mGround->HowToGo(lastPositionX, lastPositionY, homeX, homeY);
		if (nextTowards == -1 || nextTowards == 4)
		{
			SetTowards(GetRandomTowards());
		}
		else
		{
			SetTowards(nextTowards);
		}
		break;
	case 3:
		SetTowards(GetFleeingTowards());
		break;
	case 4:
		nextTowards = mGround->HowToGo(lastPositionX, lastPositionY, rebornX, rebornY);
		if (nextTowards == -1 || nextTowards == 4)
		{
			rebornFlag = true;
			SetTowards(GetRandomTowards());
		}
		else
		{
			SetTowards(nextTowards);
		}
		break;
	default:
		break;
	}
}

int Ghost::GetChasingTowards()
{
	// Implement in derived class
	return -1;
}

int Ghost::GetFleeingTowards()
{
	// Implement in derived class
	return -1;
}

void Ghost::BumpIntoPlayer(bool player_immune)
{
	switch (state)
	{
	case 0: case 1: case 2:
		if (!player_immune)
		{
			sPlayer->Death();
		}
		break;
	case 3:
		ghostEatenStack++;
		SetState(4);
		break;
	case 4:
		break;
	}
}

void Ghost::UpdatePosition()
{
	if (!motionState)
	{
		return;
	}
	Uint32 nowTicks = SDL_GetTicks();

	if (nowTicks - lastPositionTime >= Uint32(1000 / GetVelocity()))
	{
		lastPositionX = nextPositionX;
		lastPositionY = nextPositionY;

		positionX = nextPositionX;
		positionY = nextPositionY;

		UpdateNextPosition();
	}
	else
	{
		double rate = (nowTicks * 1.0 - lastPositionTime) / int(1000.0 / GetVelocity());
		positionX = (1 - rate) * lastPositionX + rate * nextPositionX;
		positionY = (1 - rate) * lastPositionY + rate * nextPositionY;
	}

	std::pair<double, double> pPosition = sPlayer->GetPosition();

	if (abs(pPosition.first - positionX) <= BUMP_CHECK_DISTANCE
		&& abs(pPosition.second - positionY) <= BUMP_CHECK_DISTANCE)
	{
		BumpIntoPlayer();
	}
}

void Ghost::UpdateNextPosition()
{
	// If out of edge
	if (lastPositionX >= MAP_WIDTH - 1) lastPositionX = 0;
	else if (lastPositionX <= 0) lastPositionX = MAP_WIDTH - 1;
	if (lastPositionY >= MAP_HEIGHT - 1) lastPositionY = 0;
	else if (lastPositionY <= 0) lastPositionY = MAP_HEIGHT - 1;

	if (mGround->GetCorner(lastPositionX, lastPositionY) > 2)
	{
		UpdateNextTowards();
	}

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
		int nextdx = 0;
		int nextdy = 0;
		for (int i = 0; i <= 3; i++)
		{
			nextdx = 0;
			nextdy = 0;
			switch (i)
			{
			case 0: nextdy = -1; break;
			case 1: nextdx = -1; break;
			case 2: nextdy = 1; break;
			case 3: nextdx = 1; break;
			}
			if (nextdx == dx && nextdy == dy ||
				nextdx == -dx && nextdy == -dy)
			{
				continue;
			}
			else if (!mGround->GetWall(lastPositionX + nextdx, lastPositionY + nextdy))
			{
				SetTowards(i);
				dx = nextdx;
				dy = nextdy;
				break;
			}
		}
	}

	lastPositionTime += int(1000 / GetVelocity());
	nextPositionX = lastPositionX + dx;
	nextPositionY = lastPositionY + dy;
}

std::pair<double, double> Ghost::GetPosition()
{
	return std::make_pair(positionX, positionY);
}

int Ghost::GetTowards()
{
	return towards;
}

double Ghost::GetVelocity()
{
	switch (state)
	{
	case 0: case 1: case 2:
		return chasingVelocity; break;
	case 3: return fleeingVelocity; break;
	case 4: return rebornVelocity; break;
	default: return 0; break;
	}
}

void Ghost::Render(int centerX, int centerY, double scale, double angle)
{
	UpdateState();
	UpdatePosition();
	if (sPlayer != NULL)
	{
		sGhost->Render(centerX, centerY, scale, angle);
	}
}

void Ghost::RenderAtPoint(double xCoord, double yCoord, double scale)
{
	int centerX = SCREEN_WIDTH / 2 + int(xCoord * WALL_WIDTH * scale);
	int centerY = SCREEN_HEIGHT / 2 + int(yCoord * WALL_HEIGHT * scale);
	Render(centerX, centerY, scale);
}

void Ghost::RenderAtPosition(double scale)
{
	UpdatePosition();

	int centerX = originX + int(positionX * WALL_WIDTH * scale);
	int centerY = originY + int(positionY * WALL_HEIGHT * scale);

	Render(centerX, centerY, scale, 0.0);
}

void Ghost::SetOriginPoint(int x, int y)
{
	originX = x;
	originY = y;
}

int Ghost::GetGhostEaten(bool clear)
{
	int result = ghostEatenStack;
	if (clear) ghostEatenStack = 0;
	return result;
}

void Ghost::DrawGizmos()
{
	// Implement in derives
}
