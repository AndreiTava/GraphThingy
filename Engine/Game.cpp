/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
}


void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (wnd.kbd.KeyIsPressed(VK_ADD))
	{
		interval -= interval * 0.05f;
	}

	if (wnd.kbd.KeyIsPressed(VK_SUBTRACT))
	{
		interval += interval * 0.05f;
	}
	if (wnd.kbd.KeyIsPressed(VK_LEFT))
		offsetX -= 5;
	if (wnd.kbd.KeyIsPressed(VK_RIGHT))
		offsetX += 5;
	if (wnd.kbd.KeyIsPressed(VK_UP))
		offsetY -= 5;
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
		offsetY += 5;
}

void Game::ComposeFrame()
{
	//gfx.DrawCircle(400, 300, 100, { 255, 0, 0 });
	//gfx.DrawLine(400, 300, wnd.mouse.GetPosX(), wnd.mouse.GetPosY(), { 0, 255, 0 });
	//DrawLine(400, 300, x, y, { 255,255,255 });
	//gfx.DrawPolygon(3, triangle, { 0, 0, 255 });
	//gfx.DrawLine(gfx.ScreenWidth / 2, gfx.ScreenHeight / 2, wnd.mouse.GetPosX(), wnd.mouse.GetPosY(), { 255,255,255 });
	
	/*gfx.DrawAxes(Colors::White, interval, offsetX, offsetY);
	gfx.Graph(Math::CustomFunction, Colors::Red, interval, offsetX, offsetY, false);*/
}

