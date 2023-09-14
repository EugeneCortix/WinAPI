#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "burn.h"
LRESULT CALLBACK WindowsProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam);

void PaintGraph(HDC hdc, int left, int top, int right, int bottom);
void PaintGraphTwo(HDC hdc, int left, int top, int right, int bottom);
void PaintGrid(HDC hdc, int left, int top, int right, int bottom);
double dih(double a, double b, double wt);
double f(double U, double id);
double U(double wt);
double finf(double wt, double R);
double id, iobr = 5 * 10e-6, R = 100, al = 32;

double xmin = 0, xmax = 30, xstep = 1;
double ymin = 0, ymax = 7, ystep = 0.5;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;
	MSG msg;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowsProc;
	wc.hInstance = hInstance;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "MainMenu";
	wc.lpszClassName = "SimpleApplication";

	RegisterClass(&wc);

	hWnd = CreateWindow("SimpleApplication", 
		"Зависимость диодного тока от времени", 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		NULL, 
		NULL, 
		hInstance, 
		NULL); 

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL APIENTRY ExampleDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char str[256];
	switch (uMsg)
	{
	case WM_INITDIALOG:
		sprintf(str, "%g", xmax);
		SetDlgItemText(hWnd, IDC_XMAX, str);
		sprintf(str, "%g", xmin);
		SetDlgItemText(hWnd, IDC_XMIN, str);
		sprintf(str, "%g", xstep);
		SetDlgItemText(hWnd, IDC_XSTEP, str);

		sprintf(str, "%g", ymax);
		SetDlgItemText(hWnd, IDC_YMAX, str);
		sprintf(str, "%g", ymin);
		SetDlgItemText(hWnd, IDC_YMIN, str);
		sprintf(str, "%g", ystep);
		SetDlgItemText(hWnd, IDC_YSTEP, str);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hWnd, IDC_XMAX, str, 256);
			xmax = atof(str);
			GetDlgItemText(hWnd, IDC_XMIN, str, 100);
			xmin = atof(str);
			GetDlgItemText(hWnd, IDC_XSTEP, str, 100);
			xstep = atof(str);
			GetDlgItemText(hWnd, IDC_YMAX, str, 100);
			ymax = atof(str);
			GetDlgItemText(hWnd, IDC_YMIN, str, 100);
			ymin = atof(str);
			GetDlgItemText(hWnd, IDC_YSTEP, str, 100);
			ystep = atof(str);

		case IDCANCEL:
			EndDialog(hWnd, wParam);
			return TRUE;
		}

	}
	return FALSE;
}

LRESULT CALLBACK WindowsProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT r;
	static HPEN hpenGrid;
	static HPEN hpenGraph;
	static HPEN hpenGraphTwo;
	static CHOOSECOLOR cc;
	static COLORREF CustColors[16];
	static bShow = MF_CHECKED;
	static tShow = MF_CHECKED;
	switch (uMsg)
	{
	case WM_CREATE:
		hpenGrid = CreatePen(PS_DOT, 1, RGB(170, 170, 170));
		hpenGraph = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
		hpenGraphTwo = CreatePen(PS_SOLID, 3, RGB(252, 102, 0));

		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.lpCustColors = CustColors;
		cc.Flags = CC_RGBINIT;

		

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &r);
		SelectObject(hdc, hpenGrid);
		PaintGrid(hdc, 50, 20, r.right - 20, r.bottom - 20);
		SelectObject(hdc, hpenGraph);
		if (bShow)
			PaintGraph(hdc, 50, 20, r.right - 20, r.bottom - 20);
		SelectObject(hdc, hpenGraphTwo);
		if (tShow)
			PaintGraphTwo(hdc, 50, 20, r.right - 20, r.bottom - 20);
		EndPaint(hWnd, &ps);
		return 0;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_COLOR:
			if (ChooseColor(&cc))
			{
				DeleteObject(&hpenGraph);
				hpenGraph = CreatePen(PS_SOLID, 3, cc.rgbResult);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		case IDM_COLORTWO:
			if (ChooseColor(&cc))
			{
				DeleteObject(&hpenGraphTwo);
				hpenGraphTwo = CreatePen(PS_SOLID, 3, cc.rgbResult);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			break;
		case IDM_SHOW:
			bShow = bShow == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED;
			CheckMenuItem(GetMenu(hWnd), IDM_SHOW, bShow);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_SHOWTWO:
			tShow = tShow == MF_CHECKED ? MF_UNCHECKED : MF_CHECKED;
			CheckMenuItem(GetMenu(hWnd), IDM_SHOWTWO, tShow);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, "Обратный ток 5•10e-6 А\nСопротивление (для зелёного графика) 100 Ом \nСопротивление(для оранжевого графика) 200 Ом \nАльфа 32 1/В", "Условия задачи", MB_OK);
			break;
		case IDM_SETUP:
			DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), "ExampleDialog", hWnd, ExampleDlgProc);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);

		}
		return 0;

	case WM_DESTROY:
		DeleteObject(hpenGrid);
		DeleteObject(hpenGraph);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void PaintGraph(HDC hdc, int left, int top, int right, int bottom)
{
	int px;
	double x, xs = (xmax*10 - xmin*10) / (right - left);
	double ky = (top - bottom) / (ymax - ymin);
	MoveToEx(hdc, left, (finf(xmin*0.1, R) - ymin) * ky + bottom, NULL);
	for (px = left + 1, x = xmin + xs; px <= right; px++, x += xs)
		LineTo(hdc, px, (finf(0.1*x, R) - ymin) * ky + bottom);
}
void PaintGraphTwo(HDC hdc, int left, int top, int right, int bottom)
{
	int px;
	double x, xs = (xmax * 10 - xmin * 10) / (right - left);
	double ky = (top - bottom) / (ymax - ymin);
	MoveToEx(hdc, left, (finf(xmin * 0.1, 2 * R) - ymin) * ky + bottom, NULL);
	for (px = left + 1, x = xmin + xs; px <= right; px++, x += xs)
		LineTo(hdc, px, (finf(0.1 * x, 2 * R) - ymin) * ky + bottom);
}
void PaintGrid(HDC hdc, int left, int top, int right, int bottom)
{
	double p, pstep, mark;
	char str[100];
	pstep = (top - bottom) / (ymax - ymin) * ystep;
	SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
	for (p = bottom, mark = ymin; p >= top - 1; p += pstep, mark += ystep)
	{
		MoveToEx(hdc, left, p, NULL);
		LineTo(hdc, right, p);
		TextOut(hdc, left - 3, p + 5, str, sprintf(str, "%.1f", mark));
	}
	pstep = (right - left) / (xmax - xmin) * xstep;
	SetTextAlign(hdc, TA_CENTER | TA_TOP);
	for (p = left, mark = xmin; p <= right + 1; p += pstep, mark += xstep)
	{
		MoveToEx(hdc, p, top, NULL);
		LineTo(hdc, p, bottom);
		TextOut(hdc, p, bottom + 5, str, sprintf(str, "%.1f", mark));
	}
}
double f(double U, double id)
{
	return U - id * R - log(id / iobr + 1) / al;
}
double U(double wt)
{
	return (6 * R + log(6 / iobr + 1) / al) * sin(wt);

}
double dih(double a, double b, double wt)
{
	double x;
	do
	{
		x = (a + b) / 2;
		if (f(U(wt), a) * f(U(wt), x) > 0)
			a = x;
		else
			b = x;
	} while (fabs(b - a) > 10e-4);
	return x;
}
double finf(double wt, double R)
{
	return dih(0, U(wt) / R, wt);
}
