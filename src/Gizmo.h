#pragma once
#include <SDL.h>
#include <cstdio>
#include <utility>

class Gizmo
{
public:
	Gizmo();
	~Gizmo();

	void SetOrigin(int x, int y);
	void SetUnitSize(int uWidth, int uHeight);
	void SetBrush(SDL_Renderer* mainRenderer);

	std::pair<int, int> ConvertToPosition(int x, int y);

	void SetGizmoColor(int r, int g, int b);
	void DebugLine(int fromX, int fromY, int toX, int toY);
	void DebugRect(int xCoord, int yCoord, int size, bool filled = false);

private:
	SDL_Renderer* mRenderer;

	int originX;
	int originY;
	int uWidth;
	int uHeight;
};