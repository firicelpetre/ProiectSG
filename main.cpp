#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <conio.h>
#include <iostream>

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "resource.h"
#include <string.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;

HWND                    g_hWnd = NULL;
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DTEXTURE9      g_pTexture[100];
int nTexture = 0;

bool g_bModulate = true;

struct Vertex
{
	float x, y, z;
	float tu1, tv1;
	float tu2, tv2;

	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ | D3DFVF_TEX2
	};
};

Vertex g_quadVertices[50000];
int nVertices = 0;
int WIDTH = 1280;
int HEIGHT = 900;

D3DXVECTOR3 eye_vector = D3DXVECTOR3(110.0f, 13.0f, 110.0f),
lookat_vector = D3DXVECTOR3(400.0f, 13.0f, 400.0f),
up_vector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

float Speed_Move = 0.9;
float Speed_Rotate = 0.002;

POINT cursor_old;
POINT cursor;
DWORD old_time = GetTickCount();//la acelasi interval de milisecunde se verifica processinput() sa nu mai varieze viteza in functie de FPS

int dimensiune_poza = 25;
int nr_poze = 20;

int nrBlocuri = 0;
int nrPomi = 0;
int nrPomi1 = 0;
int nrPomi2 = 0;
int nrFlori = 0;

int nrBlocuri2 = 0;
int nrPrimarie = 0;
int nrPolitie = 0;
int nrBanca= 0;
int nrCase1 = 0;
int nrCase2 = 0;

float bloc_size = 20; //x=z=10
float bloc_height = 40;//y=10;

float bloc2_size = 15; //x=z=10
float bloc2_height = 60;//y=10;
float casa1_size = 15; //x=z=10
float casa1_height = 20;//y=10;
float casa2_size = 10; //x=z=10
float casa2_height = 15;//y=10;
float Primarie_size = 15; //x=z=10
float Primarie_height = 20;//y=10;
float Politie_size = 15; //x=z=10
float Politie_height = 20;//y=10;
float Banca_size = 15; //x=z=10
float Banca_height = 20;//y=10;

LPD3DXFONT m_font;
LPD3DXFONT m_fontenv;
RECT rect;
RECT rectenv;

int temp = 0;

float planeAlfa = 0.0f;

bool tick()
{
	DWORD a = GetTickCount();
	if (a - old_time>5) { old_time = a; return true; } //de 100 de ori pe secunda
	else return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void loadTexture(void);
void init(void);
void shutDown(void);
void render(void);

int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX winClass;
	MSG        uMsg;

	memset(&uMsg, 0, sizeof(uMsg));

	winClass.lpszClassName = L"MY_WINDOWS_CLASS";
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WindowProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if (!RegisterClassEx(&winClass))
		return E_FAIL;

	g_hWnd = CreateWindowEx(NULL, L"MY_WINDOWS_CLASS",
		L"SistemeGrafice ",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

	if (g_hWnd == NULL)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	init();

	while (uMsg.message != WM_QUIT)
	{
		if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
		else
			render();
	}

	shutDown();

	UnregisterClass(L"MY_WINDOWS_CLASS", winClass.hInstance);

	return uMsg.wParam;
}

// Desc: The window's message handler
LRESULT CALLBACK WindowProc(HWND   hWnd, UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case VK_F1:
			g_bModulate = !g_bModulate;
			break;
		}
	}
	break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	break;
	}

	return 0;
}

void addBuilding(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
	
	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = bloc_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addBuilding

void addBuilding2(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
	
	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + bloc2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = bloc2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + bloc2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addBuilding2

void addBuilding3(float x, float z)//casa1
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa1_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = casa1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa1_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addBuilding3

void addBuilding4(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + casa2_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = casa2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + casa2_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addBuilding4

void addPrimarie(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Primarie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = Primarie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Primarie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addPrimarie

void addPolitie(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Politie_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = Politie_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Politie_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addPolitia

void addBanca(float x, float z)
{
	//cub sus 1/5 top
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + Banca_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 5/5 front
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = Banca_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + Banca_size;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addBanca

void addTree(float x, float z)
{
	float tree_size = 30.0f; //pe latime
	float tree_height = 15.0f; //inaltimea

							   // 1/4 right
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z - tree_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + tree_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 2/4 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + tree_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z - tree_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 3/4 front
	g_quadVertices[nVertices].x = x - tree_height / 2;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree_height / 2;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 4/4 back
	g_quadVertices[nVertices].x = x + tree_height / 2;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree_height / 2;
	g_quadVertices[nVertices].y = tree_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addtree

void addTree1(float x, float z)
{
	float tree1_size = 25.0f; //pe latime
	float tree1_height = 15.0f; //inaltimea

							   // 1/4 right
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z - tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 2/4 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z - tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree1_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 3/4 front
	g_quadVertices[nVertices].x = x - tree1_height / 2;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree1_height / 2;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree1_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree1_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 4/4 back
	g_quadVertices[nVertices].x = x + tree1_height / 2;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree1_height / 2;
	g_quadVertices[nVertices].y = tree1_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree1_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree1_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addtree1

void addTree2(float x, float z)
{
	float tree2_size = 30.0f; //pe latime
	float tree2_height = 20.0f; //inaltimea

							   // 1/4 right
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z - tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 2/4 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z - tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - tree2_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 3/4 front
	g_quadVertices[nVertices].x = x - tree2_height / 2;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree2_height / 2;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree2_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree2_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 4/4 back
	g_quadVertices[nVertices].x = x + tree2_height / 2;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree2_height / 2;
	g_quadVertices[nVertices].y = tree2_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + tree2_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - tree2_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addtree2

void addFlori(float x, float z)
{
	float floare_size = 10.0f; //pe latime
	float floare_height = 5.0f; //inaltimea

								// 1/4 right
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z - floare_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + floare_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - floare_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + floare_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 2/4 left
	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + floare_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z - floare_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + floare_height / 2;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z - floare_height / 2;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 3/4 front
	g_quadVertices[nVertices].x = x - floare_height / 2;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + floare_height / 2;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - floare_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + floare_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub 4/4 back
	g_quadVertices[nVertices].x = x + floare_height / 2;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - floare_height / 2;
	g_quadVertices[nVertices].y = floare_height;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + floare_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x - floare_height / 2;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
}//addFlori

void animate_object(int n, int count)
{
	planeAlfa = 0.0001f;
	float poz = 250.0f;//pozitia p x si y

	int X = 0,
		Z = 0;

	for (int i = n; i <= n + count; i++)
	{
		g_quadVertices[i].x = (g_quadVertices[i].z*sin(planeAlfa) + g_quadVertices[i].x*cos(planeAlfa));
		g_quadVertices[i].z = (g_quadVertices[i].z*cos(planeAlfa) - g_quadVertices[i].x*sin(planeAlfa));

		g_quadVertices[i].x += poz;
		g_quadVertices[i].z += poz;
	}

	void *pVertices = NULL;
	g_pVertexBuffer->Lock(0, sizeof(g_quadVertices), (void**)&pVertices, 0);
	memcpy(pVertices, g_quadVertices, sizeof(Vertex)*nVertices);
	g_pVertexBuffer->Unlock();

	for (int i = n; i <= n + count; i++)
	{
		g_quadVertices[i].x = g_quadVertices[i].x - poz;
		g_quadVertices[i].z = g_quadVertices[i].z - poz;
	}
}
void addPlane(int x, int y, int z, int plane_size)
{
	// poza vazuta de jos un fel de top al cubului
	g_quadVertices[nVertices].x = x + plane_size;
	g_quadVertices[nVertices].y = y;
	g_quadVertices[nVertices].z = z + plane_size;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = y;
	g_quadVertices[nVertices].z = z + plane_size;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + plane_size;
	g_quadVertices[nVertices].y = y;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = x + 0;
	g_quadVertices[nVertices].y = y;
	g_quadVertices[nVertices].z = z + 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

}

void incarca_harta_obiecte()
{
	FILE *f = fopen("harta.txt", "r");
	int a = 0;

	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 1)
			{
				addBuilding(i*bloc_size, j*bloc_size);
				nrBlocuri++;
			}
		}
	fclose(f);
	
	f = fopen("harta.txt", "r");
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 3)
			{
				addBuilding2(i*bloc_size, j*bloc_size);
				nrBlocuri2++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 4)
			{
				addBuilding3(i*bloc_size, j*bloc_size);
				nrCase1++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 5)
			{
				addBuilding4(i*bloc_size, j*bloc_size);
				nrCase2++;
			}
		}
	fclose(f);
	f = fopen("harta.txt", "r");//primarie
	for (int i = 0; i<500 / Primarie_size; i++)
		for (int j = 0; j<500 / Primarie_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 6)
			{
				addPrimarie(i*Primarie_size, j*Primarie_size);
				nrPrimarie++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//politie
	for (int i = 0; i<500 / Politie_size; i++)
		for (int j = 0; j<500 / Politie_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 7)
			{
				addPolitie(i*Politie_size, j*Politie_size);
				nrPolitie++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//banca
	for (int i = 0; i<500 / Banca_size; i++)
		for (int j = 0; j<500 / Banca_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 8)
			{
				addBanca(i*Banca_size, j*Banca_size);
				nrBanca++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//trees
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 2)
			{
				addTree(i*bloc_size, j*bloc_size);
				nrPomi++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//trees1
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 9)
			{
				addTree1(i*bloc_size, j*bloc_size);
				nrPomi1++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//trees2
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 10)
			{
				addTree2(i*bloc_size, j*bloc_size);
				nrPomi2++;
			}
		}
	fclose(f);

	f = fopen("harta.txt", "r");//banca stradala
	for (int i = 0; i<500 / bloc_size; i++)
		for (int j = 0; j<500 / bloc_size; j++)
		{
			fscanf(f, "%d", &a);
			if (a == 11)
			{
				addFlori(i*bloc_size, j*bloc_size);
				nrFlori++;
			}
		}
	fclose(f);
}

void create_g_quadVertices()
{
	//pictam ce e jos
	nVertices = 0;
	for (int i = 0; i<nr_poze; i++)
		for (int j = 0; j<nr_poze; j++)
		{
			g_quadVertices[nVertices].x = i*dimensiune_poza + 0;
			g_quadVertices[nVertices].y = 0;
			g_quadVertices[nVertices].z = j*dimensiune_poza + dimensiune_poza;
			g_quadVertices[nVertices].tu1 = 0.0f;
			g_quadVertices[nVertices].tv1 = 0.0f;
			g_quadVertices[nVertices].tu2 = 0.0f;
			g_quadVertices[nVertices].tv2 = 0.0f;
			nVertices++;

			g_quadVertices[nVertices].x = i*dimensiune_poza + dimensiune_poza;
			g_quadVertices[nVertices].y = 0;
			g_quadVertices[nVertices].z = j*dimensiune_poza + dimensiune_poza;
			g_quadVertices[nVertices].tu1 = 1.0f;
			g_quadVertices[nVertices].tv1 = 0.0f;
			g_quadVertices[nVertices].tu2 = 1.0f;
			g_quadVertices[nVertices].tv2 = 0.0f;
			nVertices++;

			g_quadVertices[nVertices].x = i*dimensiune_poza + 0;
			g_quadVertices[nVertices].y = 0;
			g_quadVertices[nVertices].z = j*dimensiune_poza + 0;
			g_quadVertices[nVertices].tu1 = 0.0f;
			g_quadVertices[nVertices].tv1 = 1.0f;
			g_quadVertices[nVertices].tu2 = 0.0f;
			g_quadVertices[nVertices].tv2 = 1.0f;
			nVertices++;

			g_quadVertices[nVertices].x = i*dimensiune_poza + dimensiune_poza;
			g_quadVertices[nVertices].y = 0;
			g_quadVertices[nVertices].z = j*dimensiune_poza + 0;
			g_quadVertices[nVertices].tu1 = 1.0f;
			g_quadVertices[nVertices].tv1 = 1.0f;
			g_quadVertices[nVertices].tu2 = 1.0f;
			g_quadVertices[nVertices].tv2 = 1.0f;
			nVertices++;
		}

	//pictam cele 4 red line de care nu putem trece
	// 1/4 linia de sus
	for (int i = 0; i<15; i++)
	{
		g_quadVertices[nVertices].x = 100 + i * 20 + 0;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 400;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 20;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 400;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 0;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 380;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 20;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 380;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;
	}//end linia de sus
	 //pan la 1660 (1600+15*4)

	 // 2/4 linia de jos
	for (int i = 0; i<15; i++) {
		g_quadVertices[nVertices].x = 100 + i * 20 + 0;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 120;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 20;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 120;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 0;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100 + i * 20 + 20;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;
	}//for15

	 // 3/4
	for (int i = 0; i<15; i++) {
		g_quadVertices[nVertices].x = 100;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 20;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 120;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 20;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 100;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 0;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 120;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 0;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;
	}//for15

	 // 4/4
	for (int i = 0; i<15; i++) {
		g_quadVertices[nVertices].x = 380;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 20;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 400;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 20;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 0.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 0.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 380;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 0;
		g_quadVertices[nVertices].tu1 = 0.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 0.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;

		g_quadVertices[nVertices].x = 400;
		g_quadVertices[nVertices].y = 0.3;
		g_quadVertices[nVertices].z = 100 + i * 20 + 0;
		g_quadVertices[nVertices].tu1 = 1.0f;
		g_quadVertices[nVertices].tv1 = 1.0f;
		g_quadVertices[nVertices].tu2 = 1.0f;
		g_quadVertices[nVertices].tv2 = 1.0f;
		nVertices++;
	}//for15
	 //facem cubul mare in care ne aflam
	 //cub sus 1/5 top

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;


	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 2/5 right
	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 3/5 left
	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 back
	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 500;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	//cub sus 4/5 front
	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 500;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 0.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 0.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 500;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 0.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 0.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;

	g_quadVertices[nVertices].x = 0;
	g_quadVertices[nVertices].y = 0;
	g_quadVertices[nVertices].z = 0;
	g_quadVertices[nVertices].tu1 = 1.0f;
	g_quadVertices[nVertices].tv1 = 1.0f;
	g_quadVertices[nVertices].tu2 = 1.0f;
	g_quadVertices[nVertices].tv2 = 1.0f;
	nVertices++;
	incarca_harta_obiecte();
	addPlane(100, 250, 100, 25);//specificam raza cercului
}

void loadTexture(void)
{
	create_g_quadVertices();
	nTexture = 0;

	D3DXCreateTextureFromFile(g_pd3dDevice, L"texture/Test.bmp", &g_pTexture[nTexture]);//0
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/iarba.jpg", &g_pTexture[nTexture]);//1
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/road.jpg", &g_pTexture[nTexture]);//2
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/road.jpg", &g_pTexture[nTexture]);//3
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/skyup.bmp", &g_pTexture[nTexture]);//4
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/sky.jpg", &g_pTexture[nTexture]);//5
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/sky.jpg", &g_pTexture[nTexture]);//6
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/sky.jpg", &g_pTexture[nTexture]);//7
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/sky.jpg", &g_pTexture[nTexture]);//8
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/cladire1.jpg", &g_pTexture[nTexture]);//9
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/copac.png", &g_pTexture[nTexture]);//10
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/Plane.png", &g_pTexture[nTexture]);//11
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/oldbuilding.jpg", &g_pTexture[nTexture]);//12
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/cladire2.jpg", &g_pTexture[nTexture]);//13
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/cladire3.jpg", &g_pTexture[nTexture]);//14
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/primarie.jpg", &g_pTexture[nTexture]);//15
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/politia.jpg", &g_pTexture[nTexture]);//16
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/bank.jpg", &g_pTexture[nTexture]);//17
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/Tree2.png", &g_pTexture[nTexture]);//18
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/Tree4.png", &g_pTexture[nTexture]);//19
	nTexture++;
	D3DXCreateTextureFromFile(g_pd3dDevice, L"Texture/floare.png", &g_pTexture[nTexture]);//20
	nTexture++;
	
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

}

void init(void)
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DDISPLAYMODE d3ddm;
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);
	loadTexture();
	g_pd3dDevice->CreateVertexBuffer(nVertices*sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF_Flags, D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL);
	void *pVertices = NULL;
	g_pVertexBuffer->Lock(0, sizeof(g_quadVertices), (void**)&pVertices, 0);
	memcpy(pVertices, g_quadVertices, sizeof(Vertex)*nVertices);
	g_pVertexBuffer->Unlock();

	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), WIDTH / HEIGHT, 0.1f, 2000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3DXCreateFont(g_pd3dDevice, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
	D3DXCreateFont(g_pd3dDevice, 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_fontenv);
	rect.left = 20;
	rect.right = 780;
	rect.top = 10;
	rect.bottom = rect.top + 40;
	rectenv.left = 20;
	rectenv.right = 780;
	rectenv.top = 50;
	rectenv.bottom = rectenv.top + 30;
	//Set the colour to come completely from the texture
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);   //Ignored
																			  //Set the alpha to come completely from the diffuse
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);   //Ignored
																			  // g_pd3dDevice->SetTexture(0,g_texture);
																			  //Enable alpha blending
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//Determine how they will be blended
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	// This is how you find out how many texture stages your hardware will support
	D3DCAPS9 caps;
	g_pd3dDevice->GetDeviceCaps(&caps);
	DWORD dwMaxTextureBlendStages = caps.MaxTextureBlendStages;

	GetCursorPos(&cursor_old);
}

void shutDown(void)
{
	for (int i = 0; i<nTexture; i++)
		g_pTexture[i]->Release();

	if (g_pVertexBuffer != NULL)
		g_pVertexBuffer->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();

}

bool allow_walk(float x, float z)
{
	bool ok = false, ok2 = true;

	if (x<100 || z<100 || x>400 || z>400) ok = false;
	else ok = true;

	if (!ok)
	{
		if (eye_vector.x<100)
		{
			if (x >= eye_vector.x) ok = true;
			else ok2 = false;
		}
		if (eye_vector.x>400 && ok2)
		{
			if (x <= eye_vector.x) ok = true;
			else ok2 = false;
		}

		if (eye_vector.z<100 && ok2)
		{
			if (z >= eye_vector.z) ok = true;
			else ok2 = false;
		}
		if (eye_vector.z>400 && ok2)
		{
			if (z <= eye_vector.z) ok = true;
			else ok2 = false;
		}
	}
	return ok&&ok2;
}

void ProcessInput()
{
	D3DXVECTOR3 a = eye_vector;

	// Simple strafing / dolly
	if ((GetKeyState(VK_DOWN) & 0xFF00) || (GetKeyState(83) & 0xFF00))//down sau s
	{
		D3DXVECTOR3 vDirection;
		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector
		a -= vDirection * Speed_Move;
		if (allow_walk(a.x, a.z))
		{
			eye_vector.x -= vDirection.x * Speed_Move;
			eye_vector.z -= vDirection.z * Speed_Move;
			lookat_vector.x -= vDirection.x * Speed_Move;
			lookat_vector.z -= vDirection.z * Speed_Move;
		}
	}
	else if ((GetKeyState(VK_UP) & 0xFF00) || (GetKeyState(87) & 0xFF00))//up sau w
	{
		D3DXVECTOR3 vDirection;
		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector
		a += vDirection * Speed_Move;
		if (allow_walk(a.x, a.z))
			if ((GetKeyState(VK_LSHIFT) & 0xFF00))
			{
				eye_vector.x += vDirection.x * Speed_Move * 3;
				eye_vector.z += vDirection.z * Speed_Move * 3;
				lookat_vector.x += vDirection.x * Speed_Move * 3;
				lookat_vector.z += vDirection.z * Speed_Move * 3;
			}
			else
			{
				eye_vector.x += vDirection.x * Speed_Move;
				eye_vector.z += vDirection.z * Speed_Move;
				lookat_vector.x += vDirection.x * Speed_Move;
				lookat_vector.z += vDirection.z * Speed_Move;
			}
	}


	if ((GetKeyState(VK_RIGHT) & 0xFF00) || (GetKeyState(69) & 0xFF00)) //righ sau e
	{
		D3DXVECTOR3 vDirection, vRotAxis;
		D3DXMATRIX matRotAxis, matRotY;
		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector

		D3DXVec3Cross(&vRotAxis, &vDirection, &up_vector); //strafe vector
		D3DXVec3Normalize(&vRotAxis, &vRotAxis);

		//create rotation matrices
		D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, 0.0f);
		D3DXMatrixRotationY(&matRotY, Speed_Rotate * 10);
		//rotate direction
		D3DXVec3TransformCoord(&vDirection, &vDirection, &(matRotAxis * matRotY));
		//rotate up vector
		D3DXVec3TransformCoord(&up_vector, &up_vector, &(matRotAxis * matRotY));
		//translate up vector
		lookat_vector = vDirection + eye_vector;
	}

	else if ((GetKeyState(VK_LEFT) & 0xFF00) || (GetKeyState(81) & 0xFF00)) //left sau q
	{
		D3DXVECTOR3 vDirection, vRotAxis;
		D3DXMATRIX matRotAxis, matRotY;
		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector

		D3DXVec3Cross(&vRotAxis, &vDirection, &up_vector); //strafe vector
		D3DXVec3Normalize(&vRotAxis, &vRotAxis);

		//create rotation matrices
		D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, 0.0f);
		D3DXMatrixRotationY(&matRotY, -Speed_Rotate * 10);
		//rotate direction
		D3DXVec3TransformCoord(&vDirection, &vDirection, &(matRotAxis * matRotY));
		//rotate up vector
		D3DXVec3TransformCoord(&up_vector, &up_vector, &(matRotAxis * matRotY));
		//translate up vector
		lookat_vector = vDirection + eye_vector;
	}


	if (GetKeyState(65) & 0xFF00) //A
	{
		D3DXVECTOR3 vDirection, vTransAxis;
		D3DXMATRIX matTransAxis, matTransY;

		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector

		D3DXVec3Cross(&vTransAxis, &vDirection, &up_vector); //strafe vector

		a += vTransAxis * Speed_Move;
		if (allow_walk(a.x, a.z))
		{
			eye_vector += vTransAxis * Speed_Move;
			lookat_vector += vTransAxis * Speed_Move;
		}
	}

	if (GetKeyState(68) & 0xFF00) //D
	{
		D3DXVECTOR3 vDirection, vTransAxis;
		D3DXMATRIX matTransAxis, matTransY;

		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector

		D3DXVec3Cross(&vTransAxis, &vDirection, &up_vector); //strafe vector

		a -= vTransAxis * Speed_Move;
		if (allow_walk(a.x, a.z))
		{
			eye_vector -= vTransAxis * Speed_Move;
			lookat_vector -= vTransAxis * Speed_Move;
		}
	}

	if (GetKeyState(VK_RBUTTON) & 0xFF00) //right click
	{
		//cursor_old.x=cursor.x;
		D3DXVECTOR3 vDirection, vRotAxis;
		D3DXMATRIX matRotAxis, matRotY, matRotX;
		D3DXVec3Normalize(&vDirection, &(lookat_vector - eye_vector)); //create direction vector

		D3DXVec3Cross(&vRotAxis, &vDirection, &up_vector); //strafe vector
		D3DXVec3Normalize(&vRotAxis, &vRotAxis);
		//SetCursor( NULL );	

		GetCursorPos(&cursor);
		//create rotation matrices
		D3DXMatrixRotationAxis(&matRotAxis, &vRotAxis, -1.0f*(cursor.y - cursor_old.y)*Speed_Rotate);
		D3DXMatrixRotationY(&matRotY, (1.0f)*(cursor.x - cursor_old.x)*Speed_Rotate);
		//rotate direction
		D3DXVec3TransformCoord(&vDirection, &vDirection, &(matRotAxis * matRotY));
		//rotate up vector
		D3DXVec3TransformCoord(&up_vector, &up_vector, &(matRotAxis * matRotY));

		//translate up vector
		lookat_vector = vDirection + eye_vector;
		//SetCursorPos(cursor.x,cursor.y);
		cursor_old.x = cursor.x;
		cursor_old.y = cursor.y;
	}
	else
	{
		GetCursorPos(&cursor);
		cursor_old.x = cursor.x;
		cursor_old.y = cursor.y;
	}
}

void render(void)
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
	D3DCOLOR_COLORVALUE(240.0f, 240.0f, 240.0f, 240.0f), 1.0f, 0);
	if (tick()) ProcessInput();

	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld, 0, 0, 0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	
	D3DXMATRIX view_matrix;
	D3DXMatrixLookAtLH(&view_matrix, &eye_vector, &lookat_vector, &up_vector);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view_matrix);
	
	g_pd3dDevice->BeginScene();
	
	if (g_bModulate == true)g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE); // Modulate...
	else g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD); // or Add...
	
	g_pd3dDevice->SetFVF(Vertex::FVF_Flags);
	g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex));
	
	//alpha blending enabled
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//source blend factor
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//destination blend factor
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//alpha from texture
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	
	temp = 0;

	//randam terrain
	g_pd3dDevice->SetTexture(0, g_pTexture[1]);
	for (int i = 0; i<nr_poze*nr_poze; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 1600;

	//randam 4 strazi
	g_pd3dDevice->SetTexture(0, g_pTexture[2]);
	for (int i = 0; i<15; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 60;
	for (int i = 0; i<15; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);

	temp += 60;
	g_pd3dDevice->SetTexture(0, g_pTexture[3]);
	for (int i = 0; i<15; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 60;

	for (int i = 0; i<15; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 60;

	//randam cubul in care ne aflam
	//top
	g_pd3dDevice->SetTexture(0, g_pTexture[4]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;
	//right
	g_pd3dDevice->SetTexture(0, g_pTexture[5]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;
	//left
	g_pd3dDevice->SetTexture(0, g_pTexture[6]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;
	//back
	g_pd3dDevice->SetTexture(0, g_pTexture[7]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;
	//front
	g_pd3dDevice->SetTexture(0, g_pTexture[8]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;

	//randam blocurile
	g_pd3dDevice->SetTexture(0, g_pTexture[9]);
	for (int i = 0; i<5 * nrBlocuri; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrBlocuri * 5 * 4;

	//randam building2
	//randam blocurile
	g_pd3dDevice->SetTexture(0, g_pTexture[12]);
	for (int i = 0; i<5 * nrBlocuri2; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrBlocuri2 * 5 * 4;

	//randam casele1
	g_pd3dDevice->SetTexture(0, g_pTexture[13]);
	for (int i = 0; i<5 * nrCase1; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrCase1 * 5 * 4;

	//randam casele2
	g_pd3dDevice->SetTexture(0, g_pTexture[14]);
	for (int i = 0; i<5 * nrCase2; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrCase2 * 5 * 4; //*4*4

	//randam primaria
	g_pd3dDevice->SetTexture(0, g_pTexture[15]);
	for (int i = 0; i<5 * nrPrimarie; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrPrimarie * 5 * 4; //*4*4

	//randam politia
	g_pd3dDevice->SetTexture(0, g_pTexture[16]);
	for (int i = 0; i<5 * nrPolitie; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrPolitie * 5 * 4; //*4*4

	//randam banca
	g_pd3dDevice->SetTexture(0, g_pTexture[17]);
	for (int i = 0; i<5 * nrBanca; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += nrBanca * 5 * 4; //*4*4
	
	//randam trees
	g_pd3dDevice->SetTexture(0, g_pTexture[10]);
	for (int i = 0; i<4 * nrPomi; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 4 * 4 * nrPomi;

	//randam trees1
	g_pd3dDevice->SetTexture(0, g_pTexture[18]);
	for (int i = 0; i<4 * nrPomi1; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 4 * 4 * nrPomi1;

	//randam trees2
	g_pd3dDevice->SetTexture(0, g_pTexture[19]);
	for (int i = 0; i<4 * nrPomi2; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 4 * 4 * nrPomi2;

	//randam banca stradala
	g_pd3dDevice->SetTexture(0, g_pTexture[20]);
	for (int i = 0; i<4 * nrFlori; i++)
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp + i * 4, 2);
	temp += 4 * 4 * nrFlori;

	//randam avion
	animate_object(temp, 4);//de la temp, roteste 4 puncte
	g_pd3dDevice->SetTexture(0, g_pTexture[11]);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, temp, 2);
	temp += 4;
		
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}