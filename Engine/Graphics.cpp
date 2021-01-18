/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
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
#include "Graphics.h"
#include "Functions.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::ScreenWidth * Graphics::ScreenHeight,16u ) );
}

Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}
	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::ScreenWidth;
	const size_t rowBytes = srcPitch * sizeof( Color );
	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::ScreenHeight; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}
	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer,0u,sizeof( Color ) * Graphics::ScreenHeight * Graphics::ScreenWidth );
}

void Graphics::PutPixel( int x,int y,Color c )
{
	assert( x >= 0 );
	assert( x < int( Graphics::ScreenWidth ) );
	assert( y >= 0 );
	assert( y < int( Graphics::ScreenHeight ) );
	pSysBuffer[Graphics::ScreenWidth * y + x] = c;
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}
/************************************************/
/*              My Stuff                        */
Point::Point(int cx, int cy)
{
	x = cx;
	y = cy;
}
Point::~Point()
{

}
using namespace Utility;
void Graphics::GraphExp(int x1, int x2, int y, float function(float), Color c, float scalingx, float scalingy)
{
	if (x1 > x2)
		std::swap(x1, x2);
	int prevx = -1;
	int prevy = -1;
	for (float i = float(x1); i <= float(x2); i += 0.01f)
	{
		int valx = int(round(i));
		int valy = y - int(round(function((i - Distance(x1,y,x2,y)/2)*scalingx)*scalingy));

		if (IsInBounds(valx, valy) && (valx != prevx || valy != prevy))
		{
			PutPixel(valx, valy, c);
			prevx = valx;
			prevy = valy;
		}
	}
}

void Graphics::Graph(float function(float), Color c, float interval, int offsetX, int offsetY, bool fill)
{
	int prevx = -1;
	int prevy = -1;
	float scaling = 1.f;
	if (centerX > centerY)
		scaling = interval / float(centerX);
	else
		scaling = interval / float(centerY);
	int movex = centerX - offsetX;
	int movey = centerY - offsetY;
	float precision = 0.01f;
	Color dimC(c.GetR() / 2, c.GetG() / 2, c.GetB() / 2);
	for (float x = 0.f; x <= ScreenWidth; x += precision)
	{
		int valx = int(round(x));
		int valy = movey - int(round(function((x - movex) * scaling) / scaling));

		if (fill)
		{
			for (int y = movey; y < valy; y++)
				if (IsInBounds(valx, y) && (valx != prevx || y != prevy))
				{
					PutPixel(valx, y, dimC);
					prevx = valx;
					prevy = y;
				}
			for (int y = movey; y > valy; y--)
				if (IsInBounds(valx, y) && (valx != prevx || y != prevy))
				{
					PutPixel(valx, y, dimC);
					prevx = valx;
					prevy = y;
				}
		}

		if (IsInBounds(valx, valy) && (valx != prevx || valy != prevy))
		{
			PutPixel(valx, valy, c);
			prevx = valx;
			prevy = valy;
		}
	}
}

void Graphics::DrawLine(int x1, int y1, int x2, int y2, Color c)
{
	Utility::ClipToBounds(x1, y1);
	Utility::ClipToBounds(x2, y2);
	if (x1 == x2)
	{
		if (y1 > y2)
			std::swap(y1, y2);
		for (int i = y1; i <= y2; i++)
			PutPixel(x1, i, c);

	}
	else
	{
		if (y1 == y2)
		{
			if (x1 > x2)
				std::swap(x1, x2);
			for (int i = x1; i <= x2; i++)
				PutPixel(i, y1, c);

		}
		else
		{
			if (abs(x1 - x2) >= abs(y1 - y2))
			{

				if (x1 > x2)
				{

					std::swap(x1, x2);
					std::swap(y1, y2);
				}
				const float slope = (float(y2) - float(y1)) / (float(x2) - float(x1));
				for (int i = x1; i <= x2; i++)
				{
					const float trueY = (float(i) - float(x1)) * slope;
					PutPixel(i, y1 + int(round(trueY)), c);
				}
			}
			else
			{
				if (y1 > y2)
				{

					std::swap(x1, x2);
					std::swap(y1, y2);
				}
				const float slope = (float(x2) - float(x1)) / (float(y2) - float(y1));
				for (int i = y1; i <= y2; i++)
				{
					const float trueX = (float(i) - float(y1)) * slope;
					PutPixel(x1 + int(round(trueX)), i, c);
				}
			}
		}
	}
}

void Graphics::DrawAxes(Color c, float interval, int offsetX, int offsetY)
{
	int movex = centerX - offsetX;
	int movey = centerY - offsetY;
	float scaling = 1.f;
	if (centerX > centerY)
		scaling = float(centerX) / interval;
	else
		scaling = float(centerY) / interval;
	float step = 1.f;
	Color dimC(c.GetR() / 2, c.GetG() / 2, c.GetB() / 2);

	if(IsInBounds(0,movey - 1))
		DrawLine(0, movey - 1, ScreenWidth - 1, movey - 1, c);

	if (IsInBounds(0, movey))
		DrawLine(0, movey, ScreenWidth - 1, movey, c);

	if (IsInBounds(0, movey + 1))
		DrawLine(0, movey + 1, ScreenWidth - 1, movey + 1, c);

	if (IsInBounds(movex - 1, 0))
		DrawLine(movex - 1, 0, movex - 1, ScreenHeight - 1, c);

	if (IsInBounds(movex, 0))
		DrawLine(movex, 0, movex, ScreenHeight - 1, c);

	if (IsInBounds(movex + 1, 0))
		DrawLine(movex + 1, 0, movex + 1, ScreenHeight - 1, c);

	for (float i = 0; i <= interval + offsetX; i += step)
	{
		int posx = movex + int(round(i * scaling));
		if (IsInBounds(posx, 0) && posx != movex)
			DrawLine(posx, 0, posx, ScreenHeight - 1, dimC);
		int negx = movex - int(round(i * scaling));
		if (IsInBounds(negx, 0) && negx != movex)
			DrawLine(negx, 0, negx, ScreenHeight - 1, dimC);
	}
	for (float i = 0; i >= -interval + offsetX; i -= step)
	{
		int posx = movex + int(round(i * scaling));
		if (IsInBounds(posx, 0) && posx != movex)
			DrawLine(posx, 0, posx, ScreenHeight - 1, dimC);
		int negx = movex - int(round(i * scaling));
		if (IsInBounds(negx, 0) && negx != movex)
			DrawLine(negx, 0, negx, ScreenHeight - 1, dimC);
	}
	for (float i = 0; i <= interval + offsetY; i += step)
	{
		int posy = movey + int(round(i * scaling));
		if (IsInBounds(0, posy) && posy != movey)
			DrawLine(0, posy, ScreenWidth - 1, posy, dimC);
		int negy = movey - int(round(i * scaling));
		if (IsInBounds(0, negy) && negy != movey)
			DrawLine(0, negy, ScreenWidth - 1, negy, dimC);
	}
	for (float i = 0; i >= -interval + offsetY; i -= step)
	{
		int posy = movey + int(round(i * scaling));
		if (IsInBounds(0, posy) && posy != movey)
			DrawLine(0, posy, ScreenWidth - 1, posy, dimC);
		int negy = movey - int(round(i * scaling));
		if (IsInBounds(0, negy) && negy != movey)
			DrawLine(0, negy, ScreenWidth - 1, negy, dimC);
	}
	
}

void Graphics::DrawPolygon(int n, int vertices[][2], Color c)
{
	for (int i = 0; i < n - 1; i++)
		DrawLine(vertices[i][0], vertices[i][1], vertices[i+1][0], vertices[i+1][1], c);
	DrawLine(vertices[n - 1][0], vertices[n - 1][1], vertices[0][0], vertices[0][1], c);

}


/************************************************/