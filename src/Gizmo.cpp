#include "Gizmo.h"

Gizmo::Gizmo()
{
	mRenderer = NULL;
	originX = 0;
	originY = 0;
	uWidth = 0;
	uHeight = 0;
}

Gizmo::~Gizmo()
{
	mRenderer = NULL;
	originX = 0;
	originY = 0;
}

void Gizmo::SetOrigin(int x, int y)
{
	originX = x;
	originY = y;
}

void Gizmo::SetUnitSize(int uWidth, int uHeight)
{
	this->uWidth = uWidth;
	this->uHeight = uHeight;
}

void Gizmo::SetBrush(SDL_Renderer* mainRenderer)
{
	mRenderer = mainRenderer;
}

std::pair<int, int> Gizmo::ConvertToPosition(int x, int y)
{
	int centerX = originX + int(x * uWidth);
	int centerY = originY + int(y * uHeight);
	return std::make_pair(centerX, centerY);
}

void Gizmo::SetGizmoColor(int r, int g, int b)
{
	if (mRenderer != NULL)
	{
		SDL_SetRenderDrawColor(mRenderer, r, g, b, 0xFF);
	}
}

void Gizmo::DebugLine(int fromX, int fromY, int toX, int toY)
{
	std::pair<int, int> _tPosition;
	_tPosition = ConvertToPosition(fromX, fromY);
	fromX = _tPosition.first;
	fromY = _tPosition.second;

	_tPosition = ConvertToPosition(toX, toY);
	toX = _tPosition.first;
	toY = _tPosition.second;

	if (mRenderer != NULL)
	{
		SDL_RenderDrawLine(mRenderer, fromX, fromY, toX, toY);
	}
}

void Gizmo::DebugRect(int xCoord, int yCoord, int size, bool filled)
{
	std::pair<int, int> _tPosition;
	_tPosition = ConvertToPosition(xCoord, yCoord);
	xCoord = _tPosition.first;
	yCoord = _tPosition.second;

	if (mRenderer != NULL)
	{
		SDL_Rect _r = { xCoord - size / 2, yCoord - size / 2,
			size, size };
		if (filled)
		{
			SDL_RenderFillRect(mRenderer, &_r);
		}
		else
		{
			SDL_RenderDrawRect(mRenderer, &_r);
		}
	}
}