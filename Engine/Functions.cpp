#include "Functions.h"

namespace Utility
{

	void ClipToBounds(int& x, int& y)
	{
		if (x < 0)
			x = 0;
		else if (x >= Graphics::ScreenWidth)
			x = Graphics::ScreenWidth - 1;
		if (y < 0)
			y = 0;
		else if (y >= Graphics::ScreenHeight)
			y = Graphics::ScreenHeight - 1;

	}
	void ClipToBounds(Point& p)
	{
		ClipToBounds(p.x, p.y);
	}
	bool IsInBounds(int x, int y)
	{
		if (x < 0 || x >= Graphics::ScreenWidth)
			return false;
		if (y < 0 || y >= Graphics::ScreenHeight)
			return false;
		return true;
	}
	bool IsInBounds(Point p)
	{
		return IsInBounds(p.x, p.y);
	}
	float Distance(int x1, int y1, int x2, int y2)
	{
		float dx = float(x2) - float(x1);
		float dy = float(y2) - float(y1);
		return sqrt(dx * dx + dy * dy);
	}
	float Distance(Point p1, Point p2)
	{
		return Distance(p1.x, p1.y, p2.x, p2.y);
	}
}
namespace Math
{
	float CustomFunction(float x)
	{
		return x*x;
	}

}