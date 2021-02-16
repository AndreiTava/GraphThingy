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

	bool LinesIntersecting( Line l1, Line l2)
	{
		if (l1.p1.x > l1.p2.x)
			std::swap(l1.p1, l1.p2);
		if (l2.p1.x > l2.p2.x)
			std::swap(l2.p1, l2.p2);

		float slope1 = float(l1.p2.y - l1.p1.y) / float(l1.p2.x - l1.p1.x);
		float slope2 = float(l2.p2.y - l2.p1.y) / float(l2.p2.x - l2.p1.x);

		if(slope1 == slope2)
			return false;
		else
		{

		}
		return true;
	}
}
namespace Math
{
	float CustomFunction1(float x)
	{
		return sqrt(2 * x * x * x - 3 * x * x + 6 * x - 6 * log(x + 1)) / x;
	}
	float CustomFunction2(float x)
	{
		return 10;
	}

}
