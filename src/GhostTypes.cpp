#include "GhostTypes.h"
#include "Gizmo.h"

extern Gizmo* gizmo;

Shadow::Shadow()
{
	sGhost = new Sprite();
	sPlayer = NULL;
	mGround = NULL;

	originX = 0;
	originY = 0;

	rebornX = 9;
	rebornY = 12;

	homeX = 19;
	homeY = 1;

	state = 0;

	motionState = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;

	lastPositionTime = 0;

	lastPrepareTime = 0;
	preparingTime = 3000;

	lastChaseTime = 0;
	chasingTime = 25000;

	lastScatterTime = 0;
	scatteringTime = 10000;

	lastFleeTime = 0;
	fleeingTime = 8000;

	lastRebornTime = 0;
	reborningTime = 1000;

	towards = 0;
}

int Shadow::GetChasingTowards()
{
	std::pair<double, double> pPosition;
	pPosition = sPlayer->GetPosition();
	int towards = mGround->HowToGo(lastPositionX, lastPositionY,
		int(pPosition.first), int(pPosition.second));

	// impossible to reach || has already reached
	if (towards == -1 || towards == 4)
	{
		return GetRandomTowards();
	}
	else
	{
		return towards;
	}
}

int Shadow::GetFleeingTowards()
{
	int positiveTowards = GetChasingTowards();
	int dx[] = { 0,-1, 0, 1 };
	int dy[] = { -1, 0, 1, 0 };
	int negativeTowards = (positiveTowards + 2) % 4;
	if (mGround->GetWall(lastPositionX + dx[negativeTowards],
		lastPositionY + dy[negativeTowards]))
	{
		return GetRandomTowards();
	}
	else
	{
		return negativeTowards;
	}
}

void Shadow::DrawGizmos()
{
	if (state == 1)
	{
		std::pair<double, double> pPosition;
		pPosition = sPlayer->GetPosition();

		gizmo->SetGizmoColor(0xFF, 0x00, 0x00);
		gizmo->DebugRect(lastPositionX, lastPositionY, 8, false);
		gizmo->DebugRect(int(pPosition.first), int(pPosition.second), 8, true);
		
		gizmo->DebugLine(lastPositionX, lastPositionY, int(pPosition.first), int(pPosition.second));
	}
}

Speedy::Speedy()
{
	sGhost = new Sprite();
	sPlayer = NULL;
	mGround = NULL;

	originX = 0;
	originY = 0;

	rebornX = 9;
	rebornY = 13;

	homeX = 3;
	homeY = 1;

	state = 0;

	motionState = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;

	lastPositionTime = 0;

	lastPrepareTime = 0;
	preparingTime = 2000;

	lastChaseTime = 0;
	chasingTime = 25000;

	lastScatterTime = 0;
	scatteringTime = 10000;

	lastFleeTime = 0;
	fleeingTime = 8000;

	lastRebornTime = 0;
	reborningTime = 1000;

	towards = 0;
}

int Speedy::GetChasingTowards()
{
	std::pair<double, double> pPosition;
	pPosition = sPlayer->GetPosition();
	int pTowards = sPlayer->GetTowards();

	switch (pTowards)
	{
	case 0:
		pPosition.second -= 4; break;
	case 1:
		pPosition.first -= 4; break;
	case 2:
		pPosition.second += 4; break;
	case 3:
		pPosition.first += 4; break;
	}

	int towards = mGround->HowToGo(lastPositionX, lastPositionY,
		int(pPosition.first), int(pPosition.second));

	// impossible to reach || has already reached
	if (towards == -1 || towards == 4)
	{
		return GetRandomTowards();
	}
	else
	{
		return towards;
	}
}

int Speedy::GetFleeingTowards()
{
	int positiveTowards = GetChasingTowards();
	int dx[] = { 0,-1, 0, 1 };
	int dy[] = { -1, 0, 1, 0 };
	int negativeTowards = (positiveTowards + 2) % 4;
	if (mGround->GetWall(lastPositionX + dx[negativeTowards],
		lastPositionY + dy[negativeTowards]))
	{
		return GetRandomTowards();
	}
	else
	{
		return negativeTowards;
	}
}

void Speedy::DrawGizmos()
{
	if (state == 1)
	{
		std::pair<double, double> pPosition;
		int pTowards = sPlayer->GetTowards();

		gizmo->SetGizmoColor(0xFF, 0xEB, 0x00);
		gizmo->DebugRect(lastPositionX, lastPositionY, 8, false);

		for (int i = 1; i <= 4; i++)
		{
			pPosition = sPlayer->GetPosition();
			switch (pTowards)
			{
			case 0:
				pPosition.second -= i; break;
			case 1:
				pPosition.first -= i; break;
			case 2:
				pPosition.second += i; break;
			case 3:
				pPosition.first += i; break;
			}
			if (i == 4)
			{
				gizmo->DebugRect(int(pPosition.first), int(pPosition.second), 8, true);
				gizmo->DebugLine(int(pPosition.first), int(pPosition.second), lastPositionX, lastPositionY);
			}
			else
			{
				gizmo->DebugRect(int(pPosition.first), int(pPosition.second), 5, true);
			}
		}
	}
}

BashFul::BashFul()
{
	sGhost = new Sprite();
	mShadow = NULL;
	sPlayer = NULL;
	mGround = NULL;

	originX = 0;
	originY = 0;

	rebornX = 11;
	rebornY = 13;

	homeX = 2;
	homeY = 25;

	state = 0;

	motionState = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;

	lastPositionTime = 0;

	lastPrepareTime = 0;
	preparingTime = 4000;

	lastChaseTime = 0;
	chasingTime = 25000;

	lastScatterTime = 0;
	scatteringTime = 10000;

	lastFleeTime = 0;
	fleeingTime = 8000;

	lastRebornTime = 0;
	reborningTime = 1000;

	towards = 0;
}

void BashFul::SetShadowGhost(Shadow* shadow)
{
	mShadow = shadow;
}

int BashFul::GetChasingTowards()
{
	std::pair<double, double> pPosition;
	std::pair<double, double> pShadow;
	pPosition = sPlayer->GetPosition();
	pShadow = mShadow->GetPosition();
	int pTowards = sPlayer->GetTowards();

	switch (pTowards)
	{
	case 0:
		pPosition.second -= 2; break;
	case 1:
		pPosition.first -= 2; break;
	case 2:
		pPosition.second += 2; break;
	case 3:
		pPosition.first += 2; break;
	}

	pShadow.first += (pPosition.first - pShadow.first) * 2;
	pShadow.second += (pPosition.second - pShadow.second) * 2;

	int towards = mGround->HowToGo(lastPositionX, lastPositionY,
		int(pPosition.first), int(pPosition.second));

	// impossible to reach || has already reached
	if (towards == -1 || towards == 4)
	{
		return GetRandomTowards();
	}
	else
	{
		return towards;
	}
}

int BashFul::GetFleeingTowards()
{
	int positiveTowards = GetChasingTowards();
	int dx[] = { 0,-1, 0, 1 };
	int dy[] = { -1, 0, 1, 0 };
	int negativeTowards = (positiveTowards + 2) % 4;
	if (mGround->GetWall(lastPositionX + dx[negativeTowards],
		lastPositionY + dy[negativeTowards]))
	{
		return GetRandomTowards();
	}
	else
	{
		return negativeTowards;
	}
}

void BashFul::DrawGizmos()
{
	if (state == 1)
	{
		std::pair<double, double> pPosition;
		std::pair<double, double> pShadow;

		std::pair<double, double> pExtendedPosition;

		gizmo->SetGizmoColor(0x00, 0xCD, 0xFF);

		pPosition = sPlayer->GetPosition();
		pExtendedPosition = sPlayer->GetPosition();
		pShadow = mShadow->GetPosition();
		int pTowards = sPlayer->GetTowards();

		switch (pTowards)
		{
		case 0:
			pExtendedPosition.second -= 2; break;
		case 1:
			pExtendedPosition.first -= 2; break;
		case 2:
			pExtendedPosition.second += 2; break;
		case 3:
			pExtendedPosition.first += 2; break;
		}

		gizmo->DebugLine(int(pPosition.first), int(pPosition.second),
			int(pExtendedPosition.first), int(pExtendedPosition.second));

		gizmo->DebugRect(int(pExtendedPosition.first), int(pExtendedPosition.second), 8, false);

		pPosition.first = pShadow.first + (pExtendedPosition.first - pShadow.first) * 2;
		pPosition.second = pShadow.second + (pExtendedPosition.second - pShadow.second) * 2;

		gizmo->DebugLine(int(pPosition.first), int(pPosition.second), int(pShadow.first), int(pShadow.second));
		
		gizmo->DebugRect(int(pShadow.first), int(pShadow.second), 8, false);
		gizmo->DebugRect(int(pPosition.first), int(pPosition.second), 8, false);
	}
}

Pokey::Pokey()
{
	sGhost = new Sprite();
	sPlayer = NULL;
	mGround = NULL;

	originX = 0;
	originY = 0;

	rebornX = 11;
	rebornY = 12;

	homeX = 19;
	homeY = 25;

	state = 0;

	motionState = false;

	positionX = 0;
	positionY = 0;

	lastPositionX = 0;
	lastPositionY = 0;

	nextPositionX = 0;
	nextPositionY = 0;

	lastPositionTime = 0;

	lastPrepareTime = 0;
	preparingTime = 5000;

	lastChaseTime = 0;
	chasingTime = 25000;

	lastScatterTime = 0;
	scatteringTime = 10000;

	lastFleeTime = 0;
	fleeingTime = 8000;

	lastRebornTime = 0;
	reborningTime = 1000;

	towards = 0;
}

int Pokey::GetChasingTowards()
{
	std::pair<double, double> pPosition;
	pPosition = sPlayer->GetPosition();
	int towards = mGround->HowToGo(lastPositionX, lastPositionY,
		int(pPosition.first), int(pPosition.second));
	if (abs(pPosition.first - lastPositionX) + abs(pPosition.second - lastPositionY) <= 8)
	{
		towards = mGround->HowToGo(lastPositionX, lastPositionY,
			homeX, homeY);
	}
	// impossible to reach || has already reached
	if (towards == -1 || towards == 4)
	{
		return GetRandomTowards();
	}
	else
	{
		return towards;
	}
}

int Pokey::GetFleeingTowards()
{
	int positiveTowards = GetChasingTowards();
	int dx[] = { 0,-1, 0, 1 };
	int dy[] = { -1, 0, 1, 0 };
	int negativeTowards = (positiveTowards + 2) % 4;
	if (mGround->GetWall(lastPositionX + dx[negativeTowards],
		lastPositionY + dy[negativeTowards]))
	{
		return GetRandomTowards();
	}
	else
	{
		return negativeTowards;
	}
}

void Pokey::DrawGizmos()
{
	if (state == 1)
	{
		std::pair<double, double> pPosition;
		pPosition = sPlayer->GetPosition();

		gizmo->SetGizmoColor(0xFF, 0xA5, 0xA5);

		gizmo->DebugRect(lastPositionX, lastPositionY, 8, false);

		if (abs(lastPositionX - pPosition.first) + abs(lastPositionY - pPosition.second) <= 8)
		{
			gizmo->DebugLine(lastPositionX, lastPositionY, homeX, homeY);
			gizmo->DebugRect(homeX, homeY, 8, true);
		}
		else
		{
			gizmo->DebugLine(lastPositionX, lastPositionY, int(pPosition.first), int(pPosition.second));
			gizmo->DebugRect(int(pPosition.first), int(pPosition.second), 8, true);
		}
	}
}