#pragma once
#include "Graphics.h"
#include <cmath>
namespace Utility
{
	void ClipToBounds(int& x, int& y);
	void ClipToBounds(Point& p);
	bool IsInBounds(int x, int y);
	bool IsInBounds(Point p);
	float Distance(int x1, int y1, int x2, int y2);
	float Distance(Point p1, Point p2);
}
namespace Math
{
float CustomFunction(float x);
}